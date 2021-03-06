/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/common.h"
#include "src/bitutils.h"
#include "src/host-x86.h"
#include "src/host-x86/host-x86-internal.h"
#include "src/rtl-internal.h"

/*
 * We use linear scan, as described by Poletto and Sarkar, as the basic
 * algorithm for allocating registers.  We do not maintain an explicit
 * list of live intervals, but we achieve the same effect by iterating
 * over instructions in code-stream order and checking for newly live
 * registers at each instruction.  This is not significantly slower than
 * iterating over a live interval list because SSA implies that most
 * instructions create a new register, so the number of registers -- and
 * thus the number of live ranges -- is of roughly the same order as the
 * number of instructions; by iterating over instructions, we also save
 * the expense of creating a separate list of live ranges.  We also do not
 * keep a separate list of active live ranges, since the cost of
 * maintaining such a list is likely to be greater than the cost of simply
 * iterating over the 16 hardware registers to find a register to spill.
 *
 * Register spilling follows the recommendation of Poletto and Sarkar to
 * spill the register whose live interval ends latest.  However, we track
 * the point in the code stream (RTL instruction index) at which a register
 * is spilled, and we allow the register to live in a host register until
 * that instruction is reached -- thus effectively splitting the live range
 * at the spill point.  This simplifies code generation since instructions
 * will always be able to write to a register, rather than each instruction
 * needing to check whether to use the write-to-register or write-to-memory
 * variant.  (As a corollary, we never spill the new register during a
 * spill check, even if it has the latest death.)
 *
 * We also never spill registers which are used as alias bases to avoid the
 * risk of spilling an alias base and reallocating the same host register
 * to a value which would eventually need to be stored through that alias
 * base.  This is not an unsolvable problem, but solving it adds a fair
 * amount of complexity which can be easily avoided by just locking alias
 * bases into the host register file.  Normally at most one alias base will
 * be required, so this does not add undue register pressure.
 *
 * Before the register allocation pass itself, we perform a more
 * lightweight scan of the code (see first_pass_for_block()) for three
 * primary purposes:
 *
 * - We record which RTL registers are stored in which aliases at the end
 *   of each block; this information is used to try and allocate the same
 *   hardware register to RTL registers linked to the same alias.
 *
 * - We record a list of all non-tail CALL instructions, linked through
 *   the host_data_32 field of RTLInsn, to inform the register allocator
 *   when it should avoid allocating caller-saved registers.  Note that
 *   host_data_32 is repurposed during the actual allocation pass to store
 *   a bitmask of host registers which should be saved and restored around
 *   the call by the code generator.
 *
 * - We modify instructions or operands where necessary for optimal code
 *   generation (for example, inverting the operands and comparison sense
 *   for an FCMP instruction with a less-than comparison).
 *
 * During the first pass, we also perform any enabled optimizations on the
 * RTL instruction stream; we do this as part of the same pass to avoid the
 * cost of looping over the entire unit an additional time.
 *
 * Resolution of alias references occurs in several stages, depending on
 * the nature of the reference:
 *
 * - Multiple loads (GET_ALIAS) and stores (SET_ALIAS) to the same alias
 *   within the same basic block are resolved during the first pass by
 *   converting load-after-load and load-after-store to MOVE.
 *   Store-after-store is handled by killing the earlier (dead) store
 *   during the first pass, except that if there is a CALL or
 *   CALL_TRANSPARENT instruction between the two stores and the alias has
 *   bound storage, the earlier store is not killed.
 *
 * - After the first pass, the source registers for all remaining alias
 *   stores have their live ranges extended to the end of the block
 *   containing the store if a successor block loads the same alias (see
 *   update_alias_live_ranges()).
 *
 * - For loads not rewritten in the first pass, if at least one predecessor
 *   block (that is, a block with a flow graph edge entering the current
 *   block) stores to the alias, the register allocator will attempt to
 *   allocate a host register which has not yet been used in the current
 *   block, giving preference to the register used to store to the alias
 *   in the predecessor block.  (If this applies to multiple predecessor
 *   blocks, first preference is given to the immediately previous block
 *   in code stream order if that block is a predecessor block.)  If this
 *   is successful, the corresponding RTL register is marked as valid for
 *   alias merging during code generation.  Otherwise, a host register is
 *   allocated at the point of the load as usual.
 *
 * - At code generation time, stores and loads of aliases marked as valid
 *   for merging are ignored.  Instead, at the exit point of the block
 *   containing the store (immediately before the branch instruction, if
 *   one is to be generated), the target host register is loaded with the
 *   proper value, so it will be valid on entry to the block containing
 *   the load.  ("Ignored" loads may still generate a MOV instruction if
 *   the register used to carry the alias value between blocks can't be
 *   reused in subsequent instructions due to operand constraints.)
 *
 * - All other loads and stores are replaced by memory load and store
 *   instructions referencing the alias's storage.  Merged stores are also
 *   stored to memory if the successor block does not store the alias
 *   itself.
 */

/*************************************************************************/
/****************************** Local data *******************************/
/*************************************************************************/

/**
 * RESERVED_REGS:  Set of registers (as a bitmask) we exclude from register
 * allocation.  RSP is off-limits for obvious reasons; we reserve R15 as a
 * last-resort register for instructions which need a temporary register
 * in case all other registers are in use, since it simplifies code
 * generation logic if only the destination register can cause a spill.
 * We also reserve both R15 and XMM15 as temporaries for reloading spilled
 * values in SET_ALIAS instructions.
 */
#define RESERVED_REGS  (1<<X86_SP | 1<<X86_R15 | 1<<X86_XMM15)

/*************************************************************************/
/***************** Register allocation utility routines ******************/
/*************************************************************************/

/**
 * get_gpr:  Return the index (X86Register) of the next available GPR, or
 * -1 if no GPRs are available.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     avoid_regs: Bitmask of registers to not use even if free.
 * [Return value]
 *     Next free GPR, or -1 if no GPRs are free.
 */
static inline int get_gpr(HostX86Context *ctx, uint32_t avoid_regs)
{
    const uint32_t regs_free = ctx->regs_free & 0xFFFF & ~avoid_regs;

    /* Give preference to caller-saved registers, so we don't need to
     * unnecessarily save and restore registers ourselves. */
    int host_reg = ctz32(regs_free & ~ctx->callee_saved_regs);
    if (host_reg < 16) {
        return host_reg;
    } else {
        return regs_free ? ctz32(regs_free) : -1;
    }
}

/*-----------------------------------------------------------------------*/

/**
 * get_xmm:  Return the index (X86Register) of the next available XMM
 * register, or -1 if no XMM registers are available.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     avoid_regs: Bitmask of registers to not use even if free.
 * [Return value]
 *     Next free XMM register, or -1 if no XMM registers are free.
 */
static inline int get_xmm(HostX86Context *ctx, uint32_t avoid_regs)
{
    const uint32_t regs_free = ctx->regs_free & 0xFFFF0000 & ~avoid_regs;
    return regs_free ? ctz32(regs_free) : -1;
}

/*-----------------------------------------------------------------------*/

/**
 * assign_register:  Assign the given host register to the given RTL
 * register.  Assumes the register is currently unused.  Updates
 * ctx->reg_map, ctx->regs_free, and ctx->block_regs_touched, but does not
 * modify the HostX86RegInfo record for the register.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     reg_index: RTL register number.
 *     host_reg: Host register to assign.
 * [Return value]
 *     Allocated register index.
 */
static void assign_register(HostX86Context *ctx, int reg_index,
                            X86Register host_reg)
{
    ASSERT(ctx);

    ASSERT(!ctx->reg_map[host_reg]);
    ctx->reg_map[host_reg] = reg_index;
    ASSERT(ctx->regs_free & (1 << host_reg));
    ctx->regs_free ^= 1 << host_reg;
    ctx->block_regs_touched |= 1 << host_reg;
}

/*-----------------------------------------------------------------------*/

/**
 * allocate_frame_slot:  Allocate a stack frame slot for a value of the
 * given type.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     type: Data type of value (RTLTYPE_*).
 * [Return value]
 *     Stack offset of allocated slot.
 */
static int allocate_frame_slot(HostX86Context *ctx, RTLDataType type)
{
    ASSERT(ctx);

    static const uint8_t type_sizes[] = {
        [RTLTYPE_INT32     ] = 4,
        [RTLTYPE_INT64     ] = 8,
        [RTLTYPE_ADDRESS   ] = 8,
        [RTLTYPE_FLOAT32   ] = 4,
        [RTLTYPE_FLOAT64   ] = 8,
        [RTLTYPE_V2_FLOAT32] = 16,  // So we can use it as a memory operand.
        [RTLTYPE_V2_FLOAT64] = 16,
        [RTLTYPE_FPSTATE   ] = 4,
    };
    ASSERT(type > 0 && type < lenof(type_sizes));
    ASSERT(type_sizes[type]);
    const int size = type_sizes[type];

    /* For simplicity, we just add the new slot at the end of the stack
     * frame.  This can leave holes in the stack frame depending on the
     * sizes of values stored, but for our purpose it's probably not worth
     * the extra expense of managing a bitmap of free slots. */
    ctx->frame_size = align_up(ctx->frame_size, size);
    const int offset = ctx->frame_size;
    ctx->frame_size += size;

    return offset;
}

/*-----------------------------------------------------------------------*/

/**
 * cancel_early_merge:  Cancel a pending merge from the MERGE_REGS
 * optimization.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     block_index: Index of basic block containing current instruction.
 *     host_reg: X86Register whose merge is to be cancelled.
 */
static void cancel_early_merge(HostX86Context *ctx, int block_index,
                               X86Register host_reg)
{
    ASSERT(ctx->early_merge_regs & (1 << host_reg));
    ctx->early_merge_regs ^= 1 << host_reg;

    const RTLUnit * const unit = ctx->unit;
    for (int alias = 1; ; alias++) {
        ASSERT(alias < unit->next_alias);
        const int load_reg = ctx->blocks[block_index].alias_load[alias];
        if (load_reg && ctx->regs[load_reg].merge_alias
         && ctx->regs[load_reg].host_merge == host_reg) {
            ctx->regs[load_reg].merge_alias = false;
            break;
        }
    }
}

/*-----------------------------------------------------------------------*/

/**
 * allocate_register:  Allocate a host register for the given RTL register.
 * The HostX86RegInfo.host_allocated flag is not modified.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     block_index: Index of basic block containing current instruction.
 *     insn_index: Index of current instruction in ctx->unit->insns[].
 *     reg_index: RTL register number.
 *     reg: RTLRegister structure for the register.
 *     avoid_regs: Bitmask of registers to not use even if free.
 *     soft_avoid: Bitmask of registers to use only if no other register
 *         is free.
 * [Return value]
 *     Allocated register index.
 */
static X86Register allocate_register(
    HostX86Context *ctx, int block_index, int insn_index, int reg_index,
    const RTLRegister *reg, uint32_t avoid_regs, uint32_t soft_avoid)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);
    ASSERT(reg_index > 0);
    ASSERT(reg_index < ctx->unit->next_reg);

    const bool is_gpr = (rtl_register_is_int(reg)
                         || reg->type == RTLTYPE_FPSTATE);
    const uint32_t reg_type_mask = is_gpr ? 0x0000FFFF : 0xFFFF0000;
    const bool live_across_call = (ctx->nontail_call_list >= 0
                                   && reg->death > ctx->nontail_call_list);

    int host_reg = -1;
    if (live_across_call) {
        /* Try to allocate a callee-saved register so we don't have to
         * save and restore this value across the call. */
        const uint32_t avoid_caller_saved =
            avoid_regs | soft_avoid | ctx->early_merge_regs
            | ~ctx->callee_saved_regs;
        if (is_gpr) {
            host_reg = get_gpr(ctx, avoid_caller_saved);
        } else {
            host_reg = get_xmm(ctx, avoid_caller_saved);
        }
    }
    if (host_reg < 0) {
        if (is_gpr) {
            host_reg =
                get_gpr(ctx, avoid_regs | soft_avoid | ctx->early_merge_regs);
        } else {
            host_reg =
                get_xmm(ctx, avoid_regs | soft_avoid | ctx->early_merge_regs);
        }
    }
    if (host_reg < 0 && soft_avoid != 0) {
        /* There are currently no cases in which we soft-avoid XMM regs. */
        ASSERT(is_gpr);
        host_reg = get_gpr(ctx, avoid_regs | ctx->early_merge_regs);
    }
    if (host_reg >= 0) {
        assign_register(ctx, reg_index, host_reg);
        return host_reg;
    }

    const RTLUnit * const unit = ctx->unit;

    /* If there are any pending merges from the MERGE_REGS optimization,
     * cancel the first one to make room for this register, in preference
     * to spilling a register already live in this unit. */
    if (ctx->early_merge_regs & reg_type_mask & ~avoid_regs) {
        const X86Register merge_to_cancel =
            ctz32(ctx->early_merge_regs & reg_type_mask & ~avoid_regs);
        cancel_early_merge(ctx, block_index, merge_to_cancel);
        assign_register(ctx, reg_index, merge_to_cancel);
        return merge_to_cancel;
    }

    int spill_reg = -1;
    int spill_index = 0;
    int32_t spill_death = -1;
    const X86Register spill_reg_base = is_gpr ? X86_AX : X86_XMM0;
    for (X86Register i = spill_reg_base; i < spill_reg_base + 16; i++) {
        if (!(avoid_regs & (1 << i))) {
            const int index = ctx->reg_map[i];
            if (index
             && !unit->regs[index].unspillable
             && unit->regs[index].death >= spill_death) {
                spill_reg = i;
                spill_index = index;
                spill_death = unit->regs[index].death;
            }
        }
    }
    ASSERT(spill_reg >= 0);
    ASSERT(spill_index);

    HostX86RegInfo *spill_info = &ctx->regs[spill_index];
    ASSERT(!spill_info->spilled);
    spill_info->spilled = true;
    spill_info->spill_offset =
        allocate_frame_slot(ctx, unit->regs[spill_index].type);
    spill_info->spill_insn = insn_index;

    ctx->reg_map[spill_reg] = reg_index;
    return spill_reg;
}

/*-----------------------------------------------------------------------*/

/**
 * unassign_register:  Remove the given RTL register from the current
 * register map if it's present in its host register's slot.  (It might
 * not be, if it was spilled.)
 *
 * [Parameters]
 *     ctx: Translation context.
 *     reg_index: RTL register number.
 *     reg_info: HostX86RegInfo structure for the register.
 */
static void unassign_register(HostX86Context *ctx, int reg_index,
                              const HostX86RegInfo *reg_info)
{
    ASSERT(reg_info->host_allocated);
    const X86Register host_reg = reg_info->host_reg;

    if (ctx->reg_map[host_reg] == reg_index) {
        ctx->regs_free |= 1 << host_reg;
        ctx->reg_map[host_reg] = 0;
    }
}

/*-----------------------------------------------------------------------*/

/**
 * allocate_callsave_slots:  Allocate frame slots as necessary to save each
 * register in the given register set.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     regset: Set of registers to save (X86Register bitmask).
 */
static void allocate_callsave_slots(HostX86Context *ctx, uint32_t regset)
{
    while (regset) {
        const int reg = ctz32(regset);
        regset ^= 1 << reg;
        if (ctx->stack_callsave[reg] < 0) {
            const RTLDataType type =
                reg >= X86_XMM0 ? RTLTYPE_V2_FLOAT64 : RTLTYPE_INT64;
            ctx->stack_callsave[reg] = allocate_frame_slot(ctx, type);
        }
    }
}

/*************************************************************************/
/*********************** Register allocation core ************************/
/*************************************************************************/

/**
 * allocate_regs_for_insn:  Allocate host registers for the given RTL
 * instruction.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     insn_index: Index of instruction in ctx->unit->insns[].
 *     block_index: Index of basic block containing instruction.
 * [Return value]
 *     True on success, false on error.
 */
static bool allocate_regs_for_insn(HostX86Context *ctx, int insn_index,
                                   int block_index)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);
    ASSERT(insn_index >= 0 && (uint32_t)insn_index < ctx->unit->num_insns);
    ASSERT(block_index >= 0);
    ASSERT((unsigned int)block_index < ctx->unit->num_blocks);

    const RTLUnit * const unit = ctx->unit;
    RTLInsn * const insn = &unit->insns[insn_index];

    const int dest = insn->dest;
    ASSERT(dest >= 0 && dest < unit->next_reg);
    const RTLRegister * const dest_reg = &unit->regs[dest];
    HostX86RegInfo * const dest_info = &ctx->regs[dest];

    const int src1 = insn->src1;
    ASSERT(src1 >= 0 && src1 < unit->next_reg);
    const RTLRegister * const src1_reg = &unit->regs[src1];
    const HostX86RegInfo * const src1_info = &ctx->regs[src1];

    const int src2 = insn->src2;
    ASSERT(src2 >= 0 && src2 < unit->next_reg);
    const RTLRegister * const src2_reg = &unit->regs[src2];
    const HostX86RegInfo * const src2_info = &ctx->regs[src2];

    /* Bitmap of registers we need to avoid for output or temporary
     * register allocation. */
    uint32_t avoid_regs = dest_info->avoid_regs;

    /* Special cases for store-type instructions.  These don't have
     * destination register operands, and (except for SET_ALIAS) the
     * host_data fields of the instruction are already used for address
     * optimization, so we don't have a place to record an arbitrary
     * temporary register for reloading a spilled value.  Instead, we
     * co-opt the instruction's src3 field to hold a temporary register
     * for reloading the store source value, and we unconditionally use
     * R15 as a temporary for reloading the address if necessary.  (For
     * SET_ALIAS, we unconditionally use R15 or XMM15 to store the
     * reloaded value, since SET_ALIAS source values should generally not
     * be spilled in the first place and thus it's not worth the effort
     * to add special handling.) */
    switch (insn->opcode) {
      case RTLOP_SET_ALIAS:
        if (src1_info->spilled) {
            const X86Register temp_reg =
                (rtl_register_is_int(src1_reg) ? X86_R15 : X86_XMM15);
            ctx->block_regs_touched |= 1 << temp_reg;
        }
        break;

      case RTLOP_STORE:
      case RTLOP_STORE_I8:
      case RTLOP_STORE_I16:
      case RTLOP_STORE_BR:
      case RTLOP_STORE_I16_BR: {
        if (insn->host_data_32 == 0) {
            insn->host_data_32 = insn->offset;
        }
        insn->src3 = 0;
        bool used_r15 = false;
        const int src3 = insn->host_data_16;
        if (src1_info->spilled || (src3 && ctx->regs[src3].spilled)) {
            ctx->block_regs_touched |= 1 << X86_R15;
            used_r15 = true;
        }
        bool need_src2_temp = false;
        if (src2_info->spilled) {
            need_src2_temp = true;
        } else if (insn->opcode == RTLOP_STORE_BR
                || insn->opcode == RTLOP_STORE_I16_BR) {
            /* For byte-reversed stores, we need to have the value in a GPR
             * whether we're using MOVBE or BSWAP, so we need a temporary
             * to hold the value of a non-spilled float.  If not using
             * MOVBE, we also need a temporary if src2 is the same register
             * as src1 or the index (if any), so we don't clobber the
             * register's value before using it as an address component. */
            if (rtl_register_is_float(src2_reg)) {
                need_src2_temp = true;
            } else if (!(ctx->handle->setup.host_features & BINREC_FEATURE_X86_MOVBE)) {
                if (src2 == src1) {
                    need_src2_temp = true;
                } else if (src3 && src2 == src3) {
                    need_src2_temp = true;
                }
            }
        }
        if (need_src2_temp) {
            int value_temp;
            if (rtl_register_is_int(src2_reg)
             || insn->opcode == RTLOP_STORE_BR) {
                value_temp = get_gpr(ctx, avoid_regs | ctx->early_merge_regs);
                if (value_temp < 0) {
                    if (insn->opcode == RTLOP_STORE) {
                        /* For a plain store, we can just reload into and
                         * store from an XMM register. */
                        value_temp = X86_XMM15;
                    } else {
                        value_temp = X86_R15;
                        /* If the address also used R15, we have nowhere to
                         * load the value into.  The code generator will
                         * detect this case and save some other GPR to
                         * XMM15, so mark XMM15 as touched. */
                        if (used_r15) {
                            ctx->block_regs_touched |= 1 << X86_XMM15;
                        }
                    }
                }
            } else {  // non-integer, non-byte-reversed
                value_temp = get_xmm(ctx, avoid_regs | ctx->early_merge_regs);
                if (value_temp < 0) {
                    value_temp = X86_XMM15;
                }
            }
            insn->src3 = (uint16_t)value_temp;
            ctx->block_regs_touched |= 1 << value_temp;
        }
        break;
      }  // case RTLOP_STORE*

      case RTLOP_CHAIN:
        /* R15 used as temporary for chain target load. */
        ctx->block_regs_touched |= 1 << X86_R15;
        break;
    }  // switch (insn->opcode)

    if (src1) {
        /* Source registers must have already had a host register allocated,
         * unless they're undefined (which is invalid in the first place
         * and is prevented by operand assertions if enabled) or not live
         * (which can potentially occur if the value is optimized out, such
         * as for immediate stores).  Note that if the register index is 0
         * (likewise invalid), the associated RTLRegister record has type
         * INVALID, source UNDEFINED, and !live, so we end up treating it
         * just like a nonzero-index nonlive or undefined register. */
        ASSERT((src1_reg->live && src1_reg->source != RTLREG_UNDEFINED)
               == src1_info->host_allocated);
        if (src1_reg->death == insn_index) {
            unassign_register(ctx, src1, src1_info);
        }
    }

    if (src2) {
        ASSERT((src2_reg->live && src2_reg->source != RTLREG_UNDEFINED)
               == src2_info->host_allocated);
        if (src2_reg->death == insn_index) {
            unassign_register(ctx, src2, src2_info);
        }
    }

    if (rtl_opcode_has_src3(insn->opcode) && insn->src3) {
        ASSERT(insn->src3 < unit->next_reg);
        const RTLRegister * const src3_reg = &unit->regs[insn->src3];
        const HostX86RegInfo * const src3_info = &ctx->regs[insn->src3];
        ASSERT((src3_reg->live && src3_reg->source != RTLREG_UNDEFINED)
               == src3_info->host_allocated);
        if (src3_reg->death == insn_index) {
            unassign_register(ctx, insn->src3, src3_info);
        }
    }

    if (insn->opcode >= RTLOP_LOAD && insn->opcode <= RTLOP_CMPXCHG
     && insn->host_data_16 != 0) {
        ASSERT(insn->host_data_16 < unit->next_reg);
        const int index = insn->host_data_16;
        const RTLRegister * const index_reg = &unit->regs[index];
        const HostX86RegInfo * const index_info = &ctx->regs[index];
        ASSERT((index_reg->source != RTLREG_UNDEFINED)
               == index_info->host_allocated);
        if (index_reg->death == insn_index) {
            unassign_register(ctx, index, index_info);
        }
    }

    /* If this is a non-tail or transparent call, record the set of live
     * caller-saved registers so the translator knows what to save and
     * restore.  For regular (non-transparent) calls, also advance the
     * list pointer here so the return-value register (if any) isn't
     * unnecessarily forced to a callee-saved register. */
    if ((insn->opcode == RTLOP_CALL && !insn->host_data_16)
     || insn->opcode == RTLOP_CALL_TRANSPARENT) {
        if (insn->opcode == RTLOP_CALL) {
            ASSERT(ctx->nontail_call_list == insn_index);
            ctx->nontail_call_list = insn->host_data_32;
        }

        const uint32_t caller_saved_regs =
            ~ctx->callee_saved_regs & ~RESERVED_REGS;
        const uint32_t regs_to_save =
            caller_saved_regs & (~ctx->regs_free | ctx->early_merge_regs);
        allocate_callsave_slots(ctx, regs_to_save);
        insn->host_data_32 = regs_to_save;

        /* A function call effectively touches all caller-saved registers,
         * so block alias merging across a call through any such registers.
         * However, we honor the semantic that CALL_TRANSPARENT should have
         * minimal effect on register allocation, at the cost of having to
         * scan backward from an alias merge to update all CALL_TRANSPARENT
         * save masks in the same block when the merge register is chosen. */
        if (insn->opcode == RTLOP_CALL_TRANSPARENT) {
            ctx->have_call_transparent = true;
        } else {
            ctx->block_regs_touched |= caller_saved_regs;
        }

        /* Always allocate a frame slot for MXCSR (if one hasn't been
         * allocated yet) so we can clear exceptions on return. */
        if (ctx->stack_mxcsr < 0) {
            ctx->stack_mxcsr = allocate_frame_slot(ctx, RTLTYPE_INT32);
        }

        /* Windows requires shadow space at the bottom of the frame (to be
         * used by the callee) for the first four arguments to a called
         * function. */
        if (ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS
         || ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS_SEH) {
            if (ctx->frame_callee_reserve < 32) {
                ctx->frame_callee_reserve = 32;
            }
        }
    }

    if (dest) {
        ASSERT(dest_reg->birth == insn_index);  // Guaranteed by SSA.

        uint32_t soft_avoid = 0;  // Bitmap of last-choice registers.

        /* For GET_ALIAS handling -- this has to be set before we allocate
         * a preassigned register, or we'll undesirably avoid the register
         * when choosing a merge target. */
        uint32_t usable_regs = ctx->regs_free & ~(ctx->block_regs_touched
                                                  | ctx->early_merge_regs);

        /* Check whether a host register was reserved by the FIXED_REGS
         * optimization.  Even if it was, we don't allocate it immediately;
         * we need to check whether the register is still valid.  (For
         * example, if using the reserved register would cause dest to
         * overlap src2 in a SUB instruction, we have to discard the
         * reserved register because we can't use src2 as the RMW operand
         * to a subtract operation.) */
        int fixed_reg = -1;  // -1 (none) or an X86Register constant.
        if (dest_info->host_allocated) {
            const X86Register host_reg = dest_info->host_reg;
            /* Even though we check for collisions during the fixed-regs
             * pass, collisions could be introduced by optimization, such
             * as when extending the live range of ADD operands when the
             * ADD is eliminated by address operand optimization. */
            if (!ctx->reg_map[host_reg]) {
                fixed_reg = host_reg;
            }
            /* This must have been the first register on the fixed-regs
             * list.  Advance the list pointer so we don't have to scan
             * over this register again. */
            ASSERT(ctx->fixed_reg_list == dest);
            ctx->fixed_reg_list = dest_info->next_fixed;
            /* Clear the allocation flag until we finalize the allocation. */
            dest_info->host_allocated = false;
        }

        /* Make sure not to choose any registers that have already been
         * reserved.  But don't avoid a register which has been reserved
         * for the current RTL register; that case can arise if the current
         * register's live range was extended by the ADDRESS_OPERANDS
         * optimization (see tests/host-x86/opt/address-fixed-regs-overlap.c
         * for an example). */
        for (int r = ctx->fixed_reg_list; r; r = ctx->regs[r].next_fixed) {
            if (unit->regs[r].birth >= dest_reg->death) {
                break;
            }
            ASSERT(ctx->regs[r].host_allocated);
            if ((int)ctx->regs[r].host_reg != fixed_reg) {
                avoid_regs |= 1 << ctx->regs[r].host_reg;
            }
        }

        /* If loading from an alias which was written by a predecessor
         * block, try to keep the value live in a host register.  We check
         * for this even if a host register was allocated for dest during
         * optimization, since we may still be able to reserve that
         * register as a block input or find another register to serve as
         * an intermediary to avoid the memory store/load. */
        if (insn->opcode == RTLOP_GET_ALIAS) {
            const int alias = insn->alias;
            ASSERT(ctx->blocks[block_index].alias_load[alias] == dest);
            bool have_preceding_store = false;

            const int prev_block = unit->blocks[block_index].prev_block;
            int prev_store_reg;
            if (prev_block >= 0
             && unit->blocks[prev_block].exits[0] == block_index) {
                prev_store_reg = ctx->blocks[prev_block].alias_store[alias];
            } else {
                prev_store_reg = 0;
            }

            /* If we've already reserved a register with MERGE_REGS, take
             * it out of the reserved set so we can properly allocate it
             * below.  If the reserved register is not actually available
             * here (because it got clobbered by a fixed instruction input,
             * like rAX/rDX in multiply/divide instructions), clear the
             * merge_alias flag and select a merge register normally. */
            if (dest_info->merge_alias) {
                const X86Register host_merge = dest_info->host_merge;
                ASSERT(ctx->early_merge_regs & (1 << host_merge));
                ctx->early_merge_regs ^= 1 << host_merge;
                if (ctx->block_regs_touched & (1 << host_merge)) {
                    /* Can't use this register!  Oh well. */
                    dest_info->merge_alias = false;
                } else {
                    avoid_regs ^= 1 << host_merge;
                }
            }

            /* First priority: register used by SET_ALIAS in previous block
             * (if any, and if it has an edge to this block).  We only need
             * to check exits[0] because conditional branches (the only
             * instructions that can generate multiple exit edges) always
             * put the fall-through edge in exits[0]. */
            if (!dest_info->merge_alias && prev_store_reg) {
                have_preceding_store = true;
                const X86Register host_reg = ctx->regs[prev_store_reg].host_reg;
                if (usable_regs & (1 << host_reg)) {
                    dest_info->merge_alias = true;
                    dest_info->host_merge = host_reg;
                }
            }

            /* Second priority: register used by SET_ALIAS in any
             * predecessor block. */
            if (!dest_info->merge_alias) {
                for (int entry_index = block_index; entry_index >= 0;
                     entry_index = unit->blocks[entry_index].entry_overflow)
                {
                    RTLBlock *block = &unit->blocks[entry_index];
                    for (int i = 0; (i < lenof(block->entries)
                                     && block->entries[i] >= 0); i++) {
                        if (block->entries[i] == block_index - 1) {
                            continue;  // Already checked this block above.
                        }
                        const HostX86BlockInfo *predecessor =
                            &ctx->blocks[block->entries[i]];
                        const int store_reg = predecessor->alias_store[alias];
                        /* Don't try to use the register if it hasn't been
                         * allocated (because it comes later in the code
                         * stream). */
                        if (store_reg) {
                            have_preceding_store = true;
                            if (ctx->regs[store_reg].host_allocated) {
                                const X86Register host_reg =
                                    ctx->regs[store_reg].host_reg;
                                if (usable_regs & (1 << host_reg)) {
                                    dest_info->merge_alias = true;
                                    dest_info->host_merge = host_reg;
                                    goto found_merge_pri2;  // break 2 loops
                                }
                            }
                        }
                    }
                }
              found_merge_pri2:;
            }

            /* Zeroth priority: If we already have a register allocated
             * (from optimization) and that register is free back to the
             * beginning of the block, use it.  We delay this check until
             * now since we need to know whether to allocate a register in
             * the first place (i.e., have_preceding_store). */
            if (have_preceding_store && fixed_reg >= 0
             && usable_regs & (1 << dest_info->host_reg)) {
                /* The register is available!  Claim it for our own. */
                dest_info->merge_alias = true;
                dest_info->host_merge = fixed_reg;
            }

            /* Lowest priority: any register not yet used in this block.
             * But if there are no predecessor blocks which set this alias,
             * there's nothing to merge, so don't even try to allocate a
             * register. */
            if (!dest_info->merge_alias && have_preceding_store) {
                const uint32_t saved_free = ctx->regs_free;
                ctx->regs_free = usable_regs;
                const bool is_gpr = rtl_register_is_int(dest_reg);
                int host_reg;
                if (is_gpr) {
                    host_reg = get_gpr(ctx, avoid_regs | ctx->early_merge_regs);
                } else {
                    host_reg = get_xmm(ctx, avoid_regs | ctx->early_merge_regs);
                }
                ctx->regs_free = saved_free;
                if (host_reg >= 0) {
                    dest_info->merge_alias = true;
                    dest_info->host_merge = host_reg;
                }
            }

            /* If we found a usable register, assign it to dest.  But if
             * dest already had a register (from optimization) or if the
             * chosen register is masked out by avoid_regs, just mark the
             * register as touched so subsequent alias loads don't also try
             * to use it; the code generator will construct a MOV to get
             * the value to its proper location. */
            if (dest_info->merge_alias) {
                const X86Register host_merge = dest_info->host_merge;
                if (fixed_reg >= 0 && (X86Register)fixed_reg != host_merge) {
                    ctx->block_regs_touched |= 1 << host_merge;
                } else {
                    dest_info->host_allocated = true;
                    dest_info->host_reg = host_merge;
                    assign_register(ctx, dest, host_merge);
                }
                /* If this alias is merged from the immediately previous
                 * block in code order, include its register in the set of
                 * registers live at the end of that block, so that
                 * conditional branches properly detect reload conflicts. */
                if (prev_store_reg) {
                    ASSERT(ctx->regs[prev_store_reg].host_allocated);
                    ctx->blocks[prev_block].end_live |=
                        1 << ctx->regs[prev_store_reg].host_reg;
                }
                /* If we're merging through a caller-saved register and
                 * we've seen any CALL_TRANSPARENT instructions in this
                 * block, scan back and add this register to all such
                 * instructions' save bitmasks.  (We don't need to check
                 * CALL instructions because those block merging through
                 * all caller-saved registers.) */
                if (ctx->have_call_transparent) {
                    const uint32_t caller_saved_regs =
                        ~ctx->callee_saved_regs & ~RESERVED_REGS;
                    if (caller_saved_regs & (1 << host_merge)) {
                        for (int i = insn_index - 1;
                             i >= unit->blocks[block_index].first_insn; i--)
                        {
                            if (unit->insns[i].opcode==RTLOP_CALL_TRANSPARENT) {
                                unit->insns[i].host_data_32 |= 1 << host_merge;
                                allocate_callsave_slots(
                                    ctx, unit->insns[i].host_data_32);
                            }
                        }
                    }
                }
            }
        }  // if (insn->opcode == RTLOP_GET_ALIAS)

        /*
         * If loading a function argument, try to reuse the same register
         * the argument is passed in, unless the result register is live
         * across a CALL instruction.  If we can't reuse the initial
         * register, make sure that whichever register we do choose doesn't
         * clobber another argument which has not yet been retrieved.
         */
        if (insn->opcode == RTLOP_LOAD_ARG && fixed_reg < 0
            && (ctx->nontail_call_list < 0
                || dest_reg->death <= ctx->nontail_call_list)) {
            const int target_reg = host_x86_int_arg_register(
                ctx, insn->arg_index);
            if (target_reg < 0) {
                log_error(ctx->handle, "LOAD_ARG %d not supported (argument"
                          " is not in a register)", insn->arg_index);
                return false;
            } else if (!ctx->reg_map[target_reg]
                       && !(avoid_regs & (1 << target_reg))) {
                dest_info->host_allocated = true;
                dest_info->host_reg = target_reg;
                assign_register(ctx, dest, target_reg);
            } else {
                /* We assume here that (as specified by the LOAD_ARG
                 * instruction description) all LOAD_ARG instructions
                 * appear before any other instructions in the unit. */
                for (int i = insn_index + 1;
                     i < (int)unit->num_insns
                         && unit->insns[i].opcode == RTLOP_LOAD_ARG;
                     i++)
                {
                    const int next_target = host_x86_int_arg_register(
                        ctx, unit->insns[i].arg_index);
                    if (next_target >= 0) {
                        avoid_regs |= 1 << next_target;
                    }
                }
            }
        }

        /*
         * Look for preferred or impermissible registers based on the
         * instruction and operands, and allocate the preferred or
         * reserved register if possible.
         *
         * x86 doesn't have separate destination operands for most
         * instructions, so if one of the source operands (if any) dies at
         * this instruction, reuse its host register for the destination
         * to avoid an unnecessary register move.  However, for some
         * complex instructions we need to write to the destination
         * register before we've consumed all the input operands, so we
         * explicitly avoid reusing source registers in those cases.
         */
        if (!dest_info->host_allocated) {
            int preferred_reg = -1;  // -1 (none) or an X86Register constant.

            switch (insn->opcode) {
              case RTLOP_MULHU:
              case RTLOP_MULHS:
                /* Long multiply always stores the high word to rDX, so
                 * allocate that register if it's available, and avoid rAX
                 * (which is clobbered by both input and output) in any
                 * case. */
                if (!ctx->reg_map[X86_DX]) {
                    /* Currently there are no cases in which a MULH/DIV/MOD
                     * output register can be forced to avoid its natural
                     * location.  Assert on that just to be safe. */
                    ASSERT(!(avoid_regs & (1 << X86_DX)));
                    preferred_reg = X86_DX;
                    /* This looks like a contradiction to preferred_reg,
                     * but it's needed in case preferred_reg is rejected
                     * below due to early alias merges. */
                    avoid_regs |= 1 << X86_DX;
                }
                avoid_regs |= 1 << X86_AX;
                break;

              case RTLOP_DIVU:
              case RTLOP_DIVS:
                /* Similar logic to MULHU/MULHS, but always avoid reusing
                 * src2 since we use dest to store src2 if src2 is in the
                 * way of the fixed input registers. */
                if (!ctx->reg_map[X86_AX] && src2_info->host_reg != X86_AX) {
                    ASSERT(!(avoid_regs & (1 << X86_AX)));
                    preferred_reg = X86_AX;
                }
                avoid_regs |= 1 << X86_AX
                            | 1 << X86_DX
                            | 1 << src2_info->host_reg;
                break;

              case RTLOP_MODU:
              case RTLOP_MODS:
                /* As for DIVU/DIVS. */
                if (!ctx->reg_map[X86_DX] && src2_info->host_reg != X86_DX) {
                    ASSERT(!(avoid_regs & (1 << X86_DX)));
                    preferred_reg = X86_DX;
                }
                avoid_regs |= 1 << X86_AX
                            | 1 << X86_DX
                            | 1 << src2_info->host_reg;
                break;

              case RTLOP_SEQ:
              case RTLOP_SLTU:
              case RTLOP_SLTS:
              case RTLOP_SGTU:
              case RTLOP_SGTS:
                /* Try to avoid reusing src1 or src2 so the comparison
                 * result can be implemented as XOR+SETcc rather than the
                 * slower SETcc+MOVZX.  But SETcc+MOVZX is still faster
                 * than spilling a register, so use soft_avoid rather than
                 * avoid_regs. */
                if (!src1_info->spilled) {
                    soft_avoid |= 1 << src1_info->host_reg;
                }
                if (!src2_info->spilled) {
                    /* However, we need to completely avoid src2 if src1 is
                     * spilled so that the destination register is safe to
                     * use as a temporary for reloading src1. */
                    if (src1_info->spilled) {
                        avoid_regs |= 1 << src2_info->host_reg;
                    } else {
                        soft_avoid |= 1 << src2_info->host_reg;
                    }
                }
                break;

              case RTLOP_SEQI:
              case RTLOP_SLTUI:
              case RTLOP_SLTSI:
              case RTLOP_SGTUI:
              case RTLOP_SGTSI:
                /* As for the register-register versions. */
                if (!src1_info->spilled) {
                    soft_avoid |= 1 << src1_info->host_reg;
                }
                break;

              case RTLOP_BITCAST:
              case RTLOP_FSCAST:
              case RTLOP_FZCAST:
              case RTLOP_FROUNDI:
              case RTLOP_FTRUNCI:
              case RTLOP_FCMP:
              case RTLOP_VFCMP:
                /* These instructions operate between registers of
                 * different types, so we can never reuse src1.  The
                 * standard algorithm will never select src1, but we need
                 * this case to avoid explicitly choosing it as an
                 * optimization. */
                break;

              case RTLOP_FMADD:
              case RTLOP_FMSUB:
              case RTLOP_FNMADD:
              case RTLOP_FNMSUB:
                if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_FMA) {
                    /* If using FMA instructions, the overwritten operand
                     * depends on the specific instruction variant chosen,
                     * so avoid clobbering any other operands.  This logic
                     * needs to be kept in sync with translate_fma() in
                     * host-x86-translate.c.  If the NATIVE_IEEE_NAN
                     * optimization is enabled, we could potentially
                     * reverse the order of multiplicands if that reduced
                     * the number of reloads, but we currently don't
                     * attempt to do that. */
                    const bool spilled1 = ctx->regs[src1].spilled;
                    const bool spilled2 = ctx->regs[src2].spilled;
                    const bool spilled3 = ctx->regs[insn->src3].spilled;
                    const X86Register host_src3 =
                        ctx->regs[insn->src3].host_reg;
                    if (spilled3) {
                        /* 213 variant. */
                        if (!spilled2 && !ctx->reg_map[src2_info->host_reg]) {
                            ASSERT(!(avoid_regs & (1 << src2_info->host_reg)));
                            preferred_reg = src2_info->host_reg;
                        }
                        if (!spilled1) {
                            avoid_regs |= 1 << src1_info->host_reg;
                        }
                    } else if (!ctx->reg_map[host_src3]) {
                        /* 231 variant. */
                        ASSERT(!(avoid_regs & (1 << host_src3)));
                        preferred_reg = host_src3;
                    } else {
                        /* 132 variant. */
                        if (!spilled1 && !ctx->reg_map[src1_info->host_reg]) {
                            ASSERT(!(avoid_regs & (1 << src1_info->host_reg)));
                            preferred_reg = src1_info->host_reg;
                        }
                        if (!spilled2) {
                            avoid_regs |= 1 << src2_info->host_reg;
                        }
                    }
                } else {
                    /* For non-FMA, avoid src3 since we'd have to exchange
                     * it with src1 or src2 and there's no XMM equivalent
                     * to XCHG.  Also avoid src2 if NaN order is important. */
                    if (!ctx->regs[insn->src3].spilled) {
                        avoid_regs |= 1 << ctx->regs[insn->src3].host_reg;
                    }
                    if (!(ctx->handle->common_opt & BINREC_OPT_NATIVE_IEEE_NAN)
                     && !ctx->regs[src2].spilled) {
                        avoid_regs |= 1 << ctx->regs[src2].host_reg;
                    }
                }
                break;

              case RTLOP_FTESTEXC:
                /* As for SEQ, etc.  But if the exception is INVALID, we
                 * can use a simple AND-immediate, so we don't need to
                 * avoid anything. */
                if (!src1_info->spilled && insn->src_imm != RTLFEXC_INVALID) {
                    soft_avoid |= 1 << src1_info->host_reg;
                }
                break;

              case RTLOP_ATOMIC_INC:
                /* Avoid reusing src1 (if it's not spilled) since we need
                 * to write the second XADD operand (constant 1) to dest
                 * before executing the XADD instruction itself. */
                if (!src1_info->spilled) {
                    avoid_regs |= 1 << src1_info->host_reg;
                }
                break;

              case RTLOP_CMPXCHG:
                /* In the pathological case of all three input operands
                 * spilled and all other GPRs except rAX in use, we need a
                 * total of three temporary GPRs; in that case, we need the
                 * output to _not_ be in rAX so we have two distinct
                 * registers available for reloading the spilled operands
                 * and thus only need to allocate one additional temporary
                 * register (which will be R15).  For this reason, we
                 * deliberately avoid rAX for the output register even
                 * though doing so requires an additional MOV after the
                 * CMPXCHG completes.  On the other hand, if the input
                 * operands are not spilled then we may have to contort
                 * ourselves to avoid clobbering input values as we move
                 * src2 to rAX; to avoid that complexity, we require that
                 * dest not be allocated to the same register as any
                 * (non-spilled) input operand. */
                avoid_regs |= 1 << X86_AX;
                if (!src1_info->spilled) {
                    avoid_regs |= 1 << src1_info->host_reg;
                }
                if (!src2_info->spilled) {
                    avoid_regs |= 1 << src2_info->host_reg;
                }
                if (!ctx->regs[insn->src3].spilled) {
                    avoid_regs |= 1 << ctx->regs[insn->src3].host_reg;
                }
                if (insn->host_data_16
                 && !ctx->regs[insn->host_data_16].spilled) {
                    avoid_regs |= 1 << ctx->regs[insn->host_data_16].host_reg;
                }
                break;

              case RTLOP_CALL:
              case RTLOP_CALL_TRANSPARENT:
                /* There's no relationship between the inputs and output,
                 * so just use the normal allocation algorithm. */
                break;

              default:
                if (src1 && !src1_info->spilled
                 && src1_reg->death == insn_index) {
                    /* The first operand's register can usually be reused
                     * for the destination, except for the cases below. */
                    bool src1_ok;
                    switch (insn->opcode) {
                      case RTLOP_SLL:
                      case RTLOP_SRL:
                      case RTLOP_SRA:
                        /* We can reuse src1 for BMI2-enabled shifts unless
                         * src2 is spilled, in which case we need dest as a
                         * temporary. */
                        if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_BMI2) {
                            src1_ok = !src2_info->spilled;
                            break;
                        }
                        /* Otherwise fall through to non-BMI2 logic. */
                      case RTLOP_ROL:
                      case RTLOP_ROR:
                        /* Can't reuse a src1 in rCX because we need CL for
                         * the shift count.  src1==src2 should normally
                         * never happen (unless the input is doing
                         * something bizarre), but if it does and CX is in
                         * use, the code generator will get confused, so
                         * avoid that case as well. */
                        src1_ok = (src1_info->host_reg != X86_CX
                                   && !(src1 == src2
                                        && ctx->reg_map[X86_CX] != 0));
                        break;
                      case RTLOP_BFEXT: {
                        /* Can't reuse src1 when using the BEXTR instruction
                         * because we store the BEXTR control word in dest. */
                        const unsigned int features =
                            ctx->handle->setup.host_features;
                        const int operand_size =
                            int_type_is_64(src1_reg->type) ? 64 : 32;
                        const int start = insn->bitfield.start;
                        const int count = insn->bitfield.count;
                        src1_ok = !((features & BINREC_FEATURE_X86_BMI1)
                                    && start != 0
                                    && start + count < operand_size);
                        break;
                      }
                      case RTLOP_BFINS:
                        /* Can't reuse src1 if it's the same as src2 because
                         * we need to write dest before reading src2. */
                        src1_ok = (src1_info->host_reg != src2_info->host_reg);
                        break;
                      case RTLOP_LOAD:
                      case RTLOP_LOAD_BR:
                        /* Can't reuse src1 if dest is an XMM register
                         * (this is obvious, but we need to explicitly set
                         * src1_ok=false because there's no register type
                         * check on preferred_reg).  This will also cause
                         * the temporary to unnecessarily avoid src1, but
                         * since it's only a temporary we let it slide. */
                        src1_ok = rtl_register_is_int(dest_reg);
                        break;
                      default:
                        src1_ok = true;
                        break;
                    }
                    if (src1_ok
                     && !(avoid_regs & (1 << src1_info->host_reg))) {
                        preferred_reg = src1_info->host_reg;
                    } else {
                        /* Make sure it's also not chosen by the regular
                         * allocator. */
                        avoid_regs |= 1 << src1_info->host_reg;
                    }
                }  // if (src1 is available for reuse)

                /* Check whether src2 needs to be avoided even if src1 is
                 * available, because we might override preferred_reg with
                 * fixed_reg. */
                if (src2 && !src2_info->spilled
                 && src2_reg->death == insn_index) {
                    /* The second operand's register can only be reused for
                     * commutative operations.  However, RTL SLT/SGT
                     * instructions translate to multiple x86 instructions,
                     * so we can safely reuse src2 for those cases.  DIV/MOD
                     * instructions aren't included here since they don't
                     * reach this test. */
                    static const uint8_t non_commutative[RTLOP__LAST/8 + 1] = {
                        /* Note that opcodes will need to be shifted around
                         * if their numeric values are changed such that
                         * they move to different bytes.  The compiler
                         * should normally warn in this case ("shift count
                         * is negative" or "large constant truncated"). */
                        0,
                        1<<(RTLOP_SUB-8),
                        0,
                        1<<(RTLOP_SLL-24) | 1<<(RTLOP_SRL-24)
                            | 1<<(RTLOP_SRA-24) | 1<<(RTLOP_ROL-24)
                            | 1<<(RTLOP_ROR-24),
                        1<<(RTLOP_BFINS-32),
                        0,
                        0,
                        1<<(RTLOP_FSUB-56),
                        1<<(RTLOP_FDIV-64),
                        1<<(RTLOP_VBUILD2-72),
                        1<<(RTLOP_VINSERT-80),
                    };
                    ASSERT(insn->opcode >= RTLOP__FIRST
                           && insn->opcode <= RTLOP__LAST);
                    bool src2_ok = !(non_commutative[insn->opcode / 8]
                                     & (1 << (insn->opcode % 8)));
                    if ((ctx->handle->setup.host_features & BINREC_FEATURE_X86_BMI2)
                     && (insn->opcode == RTLOP_SLL
                      || insn->opcode == RTLOP_SRL
                      || insn->opcode == RTLOP_SRA)) {
                        /* The BMI2 shift instructions are non-destructive,
                         * so we can reuse either operand safely. */
                        src2_ok = true;
                    }
                    if (!(ctx->handle->common_opt & BINREC_OPT_NATIVE_IEEE_NAN)
                     && (insn->opcode == RTLOP_FADD
                      || insn->opcode == RTLOP_FMUL)) {
                        /* FADD and FMUL are commutative in a mathematical
                         * sense, but we need to preserve operand order for
                         * correct NaN output when both operands are NaNs. */
                        src2_ok = false;
                    }
                    if (src2_ok
                     && !(avoid_regs & (1 << src2_info->host_reg))) {
                        if (preferred_reg < 0) {
                            preferred_reg = src2_info->host_reg;
                        }
                    } else {
                        avoid_regs |= 1 << src2_info->host_reg;
                    }
                }  // if (src2 is available for reuse)

                break;
            }  // switch (insn->opcode)

            /* At this point, we know what registers we need to avoid, so
             * if we have a reserved register from FIXED_REGS and it's
             * usable for this instruction, go ahead and choose it over
             * whatever the instruction may have preferred.  If the fixed
             * register matches preferred_reg, we accept it regardless of
             * avoid_regs since avoid_regs may intentionally include the
             * preferred register (see MULHU, for example). */
            if (fixed_reg >= 0 && (fixed_reg == preferred_reg
                                   || !(avoid_regs & (1 << fixed_reg)))) {
                preferred_reg = fixed_reg;
                /* If there's also an early merge on this register, cancel
                 * it; the cost of merging on a different register will
                 * probably be less than the cost of swapping registers
                 * around for the fixed operand. */
                if (ctx->early_merge_regs & (1 << fixed_reg)) {
                    cancel_early_merge(ctx, block_index, fixed_reg);
                }
            } else if (preferred_reg >= 0
                       && (ctx->early_merge_regs & (1 << preferred_reg))) {
                /* If the chosen register is only a preference, give early
                 * merges priority. */
                preferred_reg = -1;
            }

            if (preferred_reg >= 0) {
                dest_info->host_allocated = true;
                dest_info->host_reg = preferred_reg;
                assign_register(ctx, dest, preferred_reg);
            }
        }  // if (!dest_info->host_allocated)

        /* If none of the special cases apply, allocate a register normally. */
        if (!dest_info->host_allocated) {
            /* Be careful not to allocate an unclobberable input register.
             * Currently this is just rCX (the shift count register) for
             * non-BMI2 shift-type instructions. */
            switch (insn->opcode) {
              case RTLOP_SLL:
              case RTLOP_SRL:
              case RTLOP_SRA:
                if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_BMI2) {
                    break;
                }
                /* Fall through. */
              case RTLOP_ROL:
              case RTLOP_ROR:
                avoid_regs |= 1 << X86_CX;
                break;
            }
            dest_info->host_reg = allocate_register(
                ctx, block_index, insn_index, dest, dest_reg, avoid_regs,
                soft_avoid);
            dest_info->host_allocated = true;
        }

        /* Find a temporary register for instructions which need it. */
        bool need_temp;
        bool temp_is_fpr = false;
        uint32_t temp_avoid = avoid_regs | ctx->early_merge_regs;
        switch (insn->opcode) {
          case RTLOP_MULHU:
          case RTLOP_MULHS:
            /* Temporary needed to save RAX if it's live across this insn. */
            need_temp = (ctx->reg_map[X86_AX] != 0);
            /* These technically don't need to be avoided if they're
             * spilled, but it's not worth the extra cycles to check.
             * Likewise below. */
            temp_avoid |= 1 << src1_info->host_reg
                        | 1 << src2_info->host_reg;
            break;

          case RTLOP_DIVU:
          case RTLOP_DIVS:
            /* Temporary needed to save RDX if it's live across this insn
             * or if it's allocated to src2 (even if src2 dies here). */
            need_temp = (ctx->reg_map[X86_DX] != 0
                         || ctx->regs[src2].host_reg == X86_DX);
            temp_avoid |= 1 << src1_info->host_reg
                        | 1 << src2_info->host_reg;
            break;

          case RTLOP_MODU:
          case RTLOP_MODS:
            /* Temporary needed to save RAX if it's live across this insn
             * or if it's allocated to src2 (even if src2 dies here). */
            need_temp = (ctx->reg_map[X86_AX] != 0
                         || src2_info->host_reg == X86_AX);
            temp_avoid |= 1 << src1_info->host_reg
                        | 1 << src2_info->host_reg;
            break;

          case RTLOP_SLL:
          case RTLOP_SRL:
          case RTLOP_SRA:
            if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_BMI2) {
                need_temp = false;
                break;
            }
            /* Fall through. */
          case RTLOP_ROL:
          case RTLOP_ROR:
            /* Temporary needed for non-BMI2 if rCX is live and src2 is
             * spilled. */
            need_temp = (ctx->reg_map[X86_CX] != 0 && src2_info->spilled);
            break;

          case RTLOP_CLZ:
            /* Temporary needed if using BSR instead of LZCNT to count bits.
             * The temporary can never overlap the input for most
             * single-input instructions (since either the input is still
             * live or its register is reused for the output), so we don't
             * need to explicitly avoid it. */
            need_temp = !(ctx->handle->setup.host_features
                          & BINREC_FEATURE_X86_LZCNT);
            break;

          case RTLOP_BFEXT:
            /* Temporary needed for mask if extracting from the high half
             * of a 64-bit value (but not the very top, since that's
             * implemented with a simple shift). */
            need_temp = (dest_reg->type != RTLTYPE_INT32
                         && insn->bitfield.start + insn->bitfield.count < 64
                         && insn->bitfield.count > 32);
            break;

          case RTLOP_BFINS:
            /* Temporary needed if inserting into a 64-bit src1 whose
             * register is reused as the destination (so we have somewhere
             * to put the mask), or if src2 is spilled or remains live past
             * this instruction (so we can't mask and shift it in place). */
            need_temp = ((dest_reg->type != RTLTYPE_INT32
                          && dest_info->host_reg == src1_info->host_reg)
                         || src2_info->spilled
                         || src2_reg->death > insn_index);
            temp_avoid |= 1 << src1_info->host_reg
                        | 1 << src2_info->host_reg;
            break;

          case RTLOP_FZCAST:
            /* Temporary needed if converting a 64-bit or spilled 32-bit
             * value. */
            need_temp = (int_type_is_64(src1_reg->type) || src1_info->spilled);
            break;

          case RTLOP_FDIV:
            /* Temporary needed for type V2_FLOAT32 to load 1's into the
             * high elements (see translation logic for explanation). */
            need_temp = (dest_reg->type == RTLTYPE_V2_FLOAT32);
            temp_is_fpr = true;
            break;

          case RTLOP_FMADD:
          case RTLOP_FMSUB:
          case RTLOP_FNMADD:
          case RTLOP_FNMSUB:
            if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_FMA) {
                /* Temporary used in various spill cases.  Deciding whether
                 * a temporary is actually needed is non-trivial, so just
                 * unconditionally allocate it. */
                need_temp = true;
                temp_is_fpr = true;
                temp_avoid |= 1 << src1_info->host_reg
                            | 1 << src2_info->host_reg
                            | 1 << ctx->regs[insn->src3].host_reg;
            } else {
                need_temp = false;
            }
            break;

          case RTLOP_FCMP:
            /* Temporary needed if the first operand is spilled. */
            need_temp = src1_info->spilled;
            temp_is_fpr = true;
            break;

          case RTLOP_FCOPYROUND:
            /* Temporary needed for bitfield merging. */
            need_temp = true;
            break;

          case RTLOP_VBUILD2:
          case RTLOP_VINSERT:
            /* Temporary needed for inserting a spilled 32-bit float. */
            if (unit->regs[dest].type == RTLTYPE_V2_FLOAT32) {
                need_temp = src2_info->spilled;
                temp_is_fpr = true;
            } else {
                need_temp = false;
            }
            break;

          case RTLOP_VFCMP:
            /* Temporary needed to hold the compare result. */
            need_temp = true;
            temp_is_fpr = true;
            if (!src2_info->spilled) {
                temp_avoid |= 1 << src2_info->host_reg;
            }
            break;

          case RTLOP_LOAD_IMM:
            /* Temporary needed if loading a nonzero floating-point value. */
            need_temp = (!rtl_register_is_int(dest_reg) && insn->src_imm != 0);
            break;

          case RTLOP_LOAD:
          case RTLOP_LOAD_U8:
          case RTLOP_LOAD_S8:
          case RTLOP_LOAD_U16:
          case RTLOP_LOAD_S16:
          case RTLOP_LOAD_BR:
          case RTLOP_LOAD_U16_BR:
          case RTLOP_LOAD_S16_BR:
            /* Temporary needed if indexed addressing is used and the base
             * or index is spilled. */
            if (insn->host_data_16) {
                need_temp = src1_info->spilled
                          | ctx->regs[insn->host_data_16].spilled;
                temp_avoid |= 1 << src1_info->host_reg
                            | 1 << ctx->regs[insn->host_data_16].host_reg;
            } else {
                need_temp = false;
            }
            /* Temporary also needed if loading a byte-reversed float.
             * In this case, we don't have to avoid anything. */
            if (!need_temp && insn->opcode == RTLOP_LOAD_BR) {
                need_temp = rtl_register_is_float(dest_reg);
            }
            break;

          case RTLOP_ATOMIC_INC:
            /* Temporary needed if the base or index is spilled. */
            need_temp = src1_info->spilled;
            temp_avoid |= 1 << src1_info->host_reg;
            if (insn->host_data_16) {
                need_temp |= ctx->regs[insn->host_data_16].spilled;
                temp_avoid |= 1 << ctx->regs[insn->host_data_16].host_reg;
            }
            break;

          case RTLOP_CMPXCHG:
            /* CMPXCHG can require up to two temporaries.  It can be tricky
             * to work out the proper number, so we play it safe: we ensure
             * we have at least one temporary (not in rAX), and we allocate
             * another to save rAX if it's live across this instruction,
             * which (along with rAX and the output register) gives us the
             * GPRs we need for the instruction.  We use the temp_allocated
             * flag to indicate to the translator whether to save rAX, but
             * we actually save rAX in R15 or XMM15, and we use R15 as the
             * temporary if not saving rAX; see the translation logic for
             * details. */
            need_temp = (ctx->reg_map[X86_AX] != 0);
            temp_avoid |= 1 << X86_AX
                        | 1 << src1_info->host_reg
                        | 1 << src2_info->host_reg
                        | 1 << ctx->regs[insn->src3].host_reg;
            if (insn->host_data_16) {
                temp_avoid |= 1 << ctx->regs[insn->host_data_16].host_reg;
            }
            break;

          default:
            need_temp = false;
            break;
        }
        if (need_temp) {
            int temp_reg;
            if (temp_is_fpr) {
                temp_reg = get_xmm(ctx, temp_avoid);
                if (temp_reg < 0) {
                    temp_reg = X86_XMM15;
                }
            } else {
                temp_reg = get_gpr(ctx, temp_avoid);
                if (temp_reg < 0) {
                    temp_reg = X86_R15;
                }
            }
            dest_info->host_temp = (uint8_t)temp_reg;
            dest_info->temp_allocated = true;
            ctx->block_regs_touched |= 1 << temp_reg;
        }

        /* Mark additional touched registers for specific instructions. */
        if (insn->opcode == RTLOP_CMPXCHG) {
            if (dest_info->temp_allocated && dest_info->host_temp == X86_R15) {
                ctx->block_regs_touched |= 1 << X86_XMM15;
            } else {
                /* We still (potentially) use R15 even if we don't allocate
                 * a separate temporary. */
                ctx->block_regs_touched |= 1 << X86_R15;
            }
        }

        /* If the register isn't referenced again, immediately free it.
         * Normally such instructions will be optimized out, but if
         * optimization is disabled, for example, we shouldn't leave the
         * register allocated forever. */
        if (UNLIKELY(dest_reg->death == insn_index)) {
            unassign_register(ctx, dest, dest_info);
        }
    }  // if (dest)

    /* For conditional instructions with forwarded conditions, we need a
     * temporary if the condition is a register-register compare and the
     * first comparand is spilled.  We handle this separately from
     * temporary allocation above because GOTO_IF_* have no dest register
     * and we thus need to squeeze the register into bits 8-12 of the
     * host_data_16 field.  (We could potentially use dest_info->host_temp
     * for SELECT, but we don't so that we can handle all three of these
     * instructions with the same logic.) */
    if (insn->opcode == RTLOP_SELECT
     || insn->opcode == RTLOP_GOTO_IF_Z
     || insn->opcode == RTLOP_GOTO_IF_NZ) {
        if (insn->host_data_16 && !(insn->host_data_16 & 0x10)) {
            const bool is_select = (insn->opcode == RTLOP_SELECT);
            const int cmp1 = is_select ? insn->src3 : insn->src1;
            const int cmp2 = is_select ? insn->host_data_32 : insn->src2;
            if (ctx->regs[cmp1].spilled) {
                int cmp1_temp;
                /* Avoid clobbering the second comparand.  This is
                 * technically unnecessary if cmp2 is also spilled, but
                 * the case of both registers being spilled should be
                 * sufficiently uncommon that it's not worth the extra
                 * effort to handle that case. */
                if (is_select
                 && dest_info->host_reg != ctx->regs[cmp2].host_reg) {
                    /* We can use the SELECT output as the temporary if it
                     * doesn't overlap cmp2. */
                    cmp1_temp = dest_info->host_reg;
                } else {
                    const uint32_t cmp1_avoid_regs =
                        (1 << ctx->regs[cmp2].host_reg
                         | avoid_regs | ctx->early_merge_regs);
                    if (rtl_register_is_int(&unit->regs[cmp1])) {
                        cmp1_temp = get_gpr(ctx, cmp1_avoid_regs);
                        if (cmp1_temp < 0) {
                            cmp1_temp = X86_R15;
                        }
                    } else {  // non-integer
                        cmp1_temp = get_xmm(ctx, cmp1_avoid_regs);
                        if (cmp1_temp < 0) {
                            cmp1_temp = X86_XMM15;
                        }
                    }
                }
                insn->host_data_16 |= cmp1_temp << 8;
                ctx->block_regs_touched |= 1 << cmp1_temp;
            }
        }
    }

    /*
     * Perform any final instruction-specific handling required:
     *
     * - Mark as touched any fixed registers used by the instruction, since
     *   they may not have any associated RTL registers but we still need
     *   to prevent merging through them.
     *
     * - Record any constant data used by the instruction, so it can be
     *   inserted into the function prologue.
     *
     * - Allocate a stack frame slot for MXCSR if the instruction needs it
     *   (and if a slot isn't already allocated for that purpose).
     *
     * - Record labels which are targeted by backward branches.
     */
    switch (insn->opcode) {
      case RTLOP_MULHU:
      case RTLOP_MULHS:
      case RTLOP_DIVU:
      case RTLOP_DIVS:
      case RTLOP_MODU:
      case RTLOP_MODS:
        /* At least one of rAX and rDX will always be occupied here, either
         * by dest or by an existing register which blocked dest from its
         * preferred allocation, but we mark both as touched just to be
         * safe (and also so we can handle all these instructions with a
         * single case). */
        ctx->block_regs_touched |= 1 << X86_AX | 1 << X86_DX;
        break;

      case RTLOP_SLL:
      case RTLOP_SRL:
      case RTLOP_SRA:
      case RTLOP_ROL:
      case RTLOP_ROR:
        ctx->block_regs_touched |= 1 << X86_CX;
        break;

      case RTLOP_FNEG:
      case RTLOP_FNABS:
        switch ((RTLDataType)unit->regs[insn->dest].type) {
          case RTLTYPE_FLOAT32:
            ctx->const_loc[LC_FLOAT32_SIGNBIT] = 1;
            break;
          case RTLTYPE_FLOAT64:
            ctx->const_loc[LC_FLOAT64_SIGNBIT] = 1;
            break;
          case RTLTYPE_V2_FLOAT32:
            ctx->const_loc[LC_V2_FLOAT32_SIGNBIT] = 1;
            break;
          default:
            ASSERT(unit->regs[insn->dest].type == RTLTYPE_V2_FLOAT64);
            ctx->const_loc[LC_V2_FLOAT64_SIGNBIT] = 1;
            break;
        }
        break;

      case RTLOP_FABS:
        switch ((RTLDataType)unit->regs[insn->dest].type) {
          case RTLTYPE_FLOAT32:
            ctx->const_loc[LC_FLOAT32_INV_SIGNBIT] = 1;
            break;
          case RTLTYPE_FLOAT64:
            ctx->const_loc[LC_FLOAT64_INV_SIGNBIT] = 1;
            break;
          case RTLTYPE_V2_FLOAT32:
            ctx->const_loc[LC_V2_FLOAT32_INV_SIGNBIT] = 1;
            break;
          default:
            ASSERT(unit->regs[insn->dest].type == RTLTYPE_V2_FLOAT64);
            ctx->const_loc[LC_V2_FLOAT64_INV_SIGNBIT] = 1;
            break;
        }
        break;

      case RTLOP_FDIV:
        if (unit->regs[insn->dest].type == RTLTYPE_V2_FLOAT32) {
            ctx->const_loc[LC_V2_FLOAT32_HIGH_ONES] = 1;
        }
        break;

      case RTLOP_FNMADD:
      case RTLOP_FNMSUB:
        if (!(ctx->handle->setup.host_features & BINREC_FEATURE_X86_FMA)) {
            switch ((RTLDataType)unit->regs[insn->dest].type) {
              case RTLTYPE_FLOAT32:
                ctx->const_loc[LC_FLOAT32_SIGNBIT] = 1;
                break;
              case RTLTYPE_FLOAT64:
                ctx->const_loc[LC_FLOAT64_SIGNBIT] = 1;
                break;
              case RTLTYPE_V2_FLOAT32:
                ctx->const_loc[LC_V2_FLOAT32_SIGNBIT] = 1;
                break;
              default:
                ASSERT(unit->regs[insn->dest].type == RTLTYPE_V2_FLOAT64);
                ctx->const_loc[LC_V2_FLOAT64_SIGNBIT] = 1;
                break;
            }
        }
        break;

      case RTLOP_FZCAST:
        if (!int_type_is_64(unit->regs[insn->src1].type)) {
            break;  // MXCSR not needed for converting from INT32. */
        }
        /* Fall through to MXCSR frame slot allocation. */
      case RTLOP_FGETSTATE:
      case RTLOP_FSETSTATE:
        /* These instructions touch MXCSR, which requires a memory
         * location rather than a register, so ensure that we have a
         * frame slot allocated. */
        if (ctx->stack_mxcsr < 0) {
            ctx->stack_mxcsr = allocate_frame_slot(ctx, RTLTYPE_INT32);
        }
        break;

      case RTLOP_CMPXCHG:
        ctx->block_regs_touched |= 1 << X86_AX;
        break;

      case RTLOP_GOTO:
      case RTLOP_GOTO_IF_Z:
      case RTLOP_GOTO_IF_NZ:
        if (unit->label_blockmap[insn->label] <= block_index) {
            const RTLBlock *target_block =
                &unit->blocks[unit->label_blockmap[insn->label]];
            RTLInsn *target_insn = &unit->insns[target_block->first_insn];
            ASSERT(target_insn->opcode == RTLOP_LABEL);
            ASSERT(target_insn->label == insn->label);
            target_insn->host_data_16 = 1;
        }
        break;
    }

    return true;
}

/*-----------------------------------------------------------------------*/

/**
 * allocate_regs_for_block:  Allocate host registers for RTL registers in
 * the given basic block.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     block_index: Index of basic block in ctx->unit->blocks[].
 * [Return value]
 *     True on success, false on error.
 */
static bool allocate_regs_for_block(HostX86Context *ctx, int block_index)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);
    ASSERT(block_index >= 0);
    ASSERT(block_index < ctx->unit->num_blocks);

    const RTLUnit * const unit = ctx->unit;
    const RTLBlock * const block = &unit->blocks[block_index];
    HostX86BlockInfo *block_info = &ctx->blocks[block_index];

#ifdef ENABLE_OPERAND_SANITY_CHECKS
    for (int i = 0; i < 32; i++) {
        const int reg = ctx->reg_map[i];
        if (reg && unit->regs[reg].death < block->first_insn) {
            log_ice(ctx->handle, "Register %d in live set but not live on"
                    " entry to block %d", reg, block_index);
            return false;
        }
    }
#endif
    STATIC_ASSERT(sizeof(block_info->initial_reg_map) == sizeof(ctx->reg_map),
                  "Register map size mismatch");
    memcpy(block_info->initial_reg_map, ctx->reg_map, sizeof(ctx->reg_map));

    /* Reserved registers are excluded from the free set but don't count
     * as touched. */
    ctx->block_regs_touched = ~(ctx->regs_free | (RESERVED_REGS));

    /* If smart merging is enabled, first give each GET_ALIAS a chance at
     * the host register allocated to its merge source, if any (and if the
     * merge source isn't spilled). */
    ctx->early_merge_regs = 0;
    if (ctx->handle->host_opt & BINREC_OPT_H_X86_MERGE_REGS) {
        uint32_t usable_regs = ctx->regs_free;
        for (int insn_index = block->first_insn;
             insn_index <= block->last_insn; insn_index++)
        {
            const RTLInsn * const insn = &unit->insns[insn_index];
            if (insn->opcode == RTLOP_GET_ALIAS) {
                HostX86RegInfo * const dest_info = &ctx->regs[insn->dest];
                const int alias = insn->alias;
                for (int entry_index = block_index; entry_index >= 0;
                     entry_index = unit->blocks[entry_index].entry_overflow)
                {
                    RTLBlock *entry_block = &unit->blocks[entry_index];
                    for (int i = 0; (i < lenof(entry_block->entries)
                                     && entry_block->entries[i] >= 0); i++) {
                        const HostX86BlockInfo *predecessor =
                            &ctx->blocks[entry_block->entries[i]];
                        const int store_reg = predecessor->alias_store[alias];
                        if (store_reg && ctx->regs[store_reg].host_allocated
                         && !ctx->regs[store_reg].spilled) {
                            const X86Register host_reg =
                                ctx->regs[store_reg].host_reg;
                            if (usable_regs & (1 << host_reg)) {
                                dest_info->merge_alias = true;
                                dest_info->host_merge = host_reg;
                                usable_regs ^= 1 << host_reg;
                                ctx->early_merge_regs |= 1 << host_reg;
                                goto found_merge;  // i.e., break 2 loops
                            }
                        }
                    }
                }
              found_merge:;
            }  // if GET_ALIAS
        }   // for each instruction
    }  // if MERGE_REGS

    ctx->have_call_transparent = false;
    for (int insn_index = block->first_insn; insn_index <= block->last_insn;
         insn_index++)
    {
        if (!allocate_regs_for_insn(ctx, insn_index, block_index)) {
            return false;
        }
    }

    /* Registers used in SET_ALIAS instructions, or registers which died
     * in this block but have a control flow edge returning to a block in
     * which they were live, may have had their live ranges extended past
     * the last instruction that referenced them.  Make sure to properly
     * free their host registers. */
    const int32_t last_insn = block->last_insn;
    for (int i = 0; i < 32; i++) {
        const int reg = ctx->reg_map[i];
        if (reg && unit->regs[reg].death <= last_insn) {
            const HostX86RegInfo *reg_info = &ctx->regs[reg];
            ASSERT(reg_info->host_allocated);
            unassign_register(ctx, reg, reg_info);
        }
    }

    block_info->end_live = ~(ctx->regs_free | RESERVED_REGS);
    ctx->regs_touched |= ctx->block_regs_touched;
    return true;
}

/*************************************************************************/
/************** First-pass scanner and other local routines **************/
/*************************************************************************/

/**
 * maybe_optimize_call_immediate:  Check whether the given register is a
 * constant valid for CALL operand optimization, and kill it if so.
 * Helper for optimize_call_immediates().
 */
static void maybe_optimize_call_immediate(RTLUnit *unit, int insn_index,
                                          int reg_index)
{
    RTLRegister * const reg = &unit->regs[reg_index];
    if (reg->source == RTLREG_CONSTANT && reg->death == insn_index) {
        const int prev_use = rtl_opt_prev_reg_use(unit, reg_index, insn_index);
        if (prev_use == reg->birth) {
            reg->death = reg->birth;
            rtl_opt_kill_insn(unit, reg->birth, true, false);
            /* The translator relies on the live flag being clear to know
             * to load the constant directly. */
            ASSERT(!reg->live);
        }
    }
}

/*-----------------------------------------------------------------------*/

/**
 * optimize_call_immediates:  Look for operands to the given CALL or
 * CALL_TRANSPARENT instruction which are constants not used elsewhere,
 * and kill the associated LOAD_IMM instructions.  This allows the value
 * to be loaded right before the call, avoiding the need to allocate a
 * separate register for it.
 */
static void optimize_call_immediates(RTLUnit *unit, int insn_index)
{
    const RTLInsn * const insn = &unit->insns[insn_index];
    const int src1 = insn->src1;
    const int src2 = insn->src2;
    const int src3 = insn->src3;

    maybe_optimize_call_immediate(unit, insn_index, src1);
    if (src2 && src2 != src1) {
        maybe_optimize_call_immediate(unit, insn_index, src2);
        if (src3 && src3 != src1 && src3 != src2) {
            maybe_optimize_call_immediate(unit, insn_index, src3);
        }
    }
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_mulh:  Allocate fixed registers for a MULHU or MULHS
 * instruction.
 */
static void alloc_fixed_regs_mulh(HostX86Context *ctx, RTLUnit *unit,
                                  int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];
    const RTLRegister *dest_reg = &unit->regs[insn->dest];
    HostX86RegInfo *dest_info = &ctx->regs[insn->dest];
    const RTLRegister *src1_reg = &unit->regs[insn->src1];
    HostX86RegInfo *src1_info = &ctx->regs[insn->src1];
    const RTLRegister *src2_reg = &unit->regs[insn->src2];
    HostX86RegInfo *src2_info = &ctx->regs[insn->src2];

    /* MUL and IMUL read rAX and write rDX:rAX, so ideally we want one
     * input operand in rAX, the other in rDX if it dies at this
     * instruction, and the result in rDX since these instructions return
     * the high word of the result. */

    const int prev_dx_death = ctx->last_dx_death;
    if (prev_dx_death <= insn_index) {
        ASSERT(dest_reg->birth == insn_index);
        /* This assignment is done in the first pass, so a newly born RTL
         * register can't have any host registers in its avoid set, but
         * assert on that in case future additions to the program break
         * that assumption. */
        ASSERT(!dest_info->avoid_regs);
        dest_info->host_allocated = true;
        dest_info->host_reg = X86_DX;
        ctx->last_dx_death = dest_reg->death;
    }

    /* Check which of rAX and rDX we can allocate to src1 and src2.
     * We prioritize putting at least one operand in rAX to save a move
     * operation; the only benefit to using rDX is that if the operand
     * dies here, assigning it to rDX ensures there are no "holes" where
     * rDX is not in use but can't be allocated due to dest having it
     * reserved.  Naturally, if we couldn't allocate rDX for dest then we
     * won't be able to allocate it for either source operand, so we skip
     * the rDX tests in that case. */

    const bool src1_ax_ok = (!src1_info->host_allocated
                             && !(src1_info->avoid_regs & (1 << X86_AX))
                             && src1_reg->birth >= ctx->last_ax_death);
    const bool src2_ax_ok = (insn->src2 != insn->src1
                             && !src2_info->host_allocated
                             && !(src2_info->avoid_regs & (1 << X86_AX))
                             && src2_reg->birth >= ctx->last_ax_death);
    const bool src1_dx_ok = (dest_info->host_allocated
                             && !src1_info->host_allocated
                             && !(src1_info->avoid_regs & (1 << X86_DX))
                             && src1_reg->death == insn_index
                             && src1_reg->birth >= prev_dx_death);
    const bool src2_dx_ok = (insn->src2 != insn->src1
                             && dest_info->host_allocated
                             && !src2_info->host_allocated
                             && !(src2_info->avoid_regs & (1 << X86_DX))
                             && src2_reg->death == insn_index
                             && src2_reg->birth >= prev_dx_death);

    int ax_src = 0, dx_src = 0;  // 1 (src1), 2 (src2), or 0 (none)
    if (src1_ax_ok && src2_ax_ok) {
        /* Either operand can go in rAX, so choose the one (if any) that
         * can't get rDX. */
        if (src1_dx_ok) {
            dx_src = 1;
            ax_src = 2;
        } else {
            ax_src = 1;
            if (src2_dx_ok) {
                dx_src = 2;
            }
        }
    } else if (src1_ax_ok) {
        ax_src = 1;
        if (src2_dx_ok) {
            dx_src = 2;
        }
    } else if (src2_ax_ok) {
        ax_src = 2;
        if (src1_dx_ok) {
            dx_src = 1;
        }
    } else {
        if (src1_dx_ok) {
            dx_src = 1;
        } else if (src2_dx_ok) {
            dx_src = 2;
        }
    }

    if (ax_src == 1) {
        src1_info->host_allocated = true;
        src1_info->host_reg = X86_AX;
        ctx->last_ax_death = src1_reg->death;
    } else if (ax_src == 2) {
        src2_info->host_allocated = true;
        src2_info->host_reg = X86_AX;
        ctx->last_ax_death = src2_reg->death;
    }

    if (dx_src == 1) {
        src1_info->host_allocated = true;
        src1_info->host_reg = X86_DX;
    } else if (dx_src == 2) {
        src2_info->host_allocated = true;
        src2_info->host_reg = X86_DX;
    }
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_div:  Allocate fixed registers for a DIVU or DIVS
 * instruction.
 */
static void alloc_fixed_regs_div(HostX86Context *ctx, RTLUnit *unit,
                                 int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];
    const RTLRegister *dest_reg = &unit->regs[insn->dest];
    HostX86RegInfo *dest_info = &ctx->regs[insn->dest];
    const RTLRegister *src1_reg = &unit->regs[insn->src1];
    HostX86RegInfo *src1_info = &ctx->regs[insn->src1];
    HostX86RegInfo *src2_info = &ctx->regs[insn->src2];

    /* For div/mod, we only care about putting the result in rAX or rDX (as
     * appropriate) and the dividend in rAX.  Putting the divisor in either
     * rAX or rDX would just force us to move it out of the way later. */
    if (ctx->last_ax_death <= insn_index) {
        ASSERT(dest_reg->birth == insn_index);
        /* We require that dest and src2 not share the same
         * register, so if src2 is already in rAX, we can't assign
         * it to dest here. */
        if (!(src2_info->host_allocated && src2_info->host_reg == X86_AX)) {
            ASSERT(!dest_info->avoid_regs);
            dest_info->host_allocated = true;
            dest_info->host_reg = X86_AX;
            if (!src1_info->host_allocated
             && !(src1_info->avoid_regs & (1 << X86_AX))
             && src1_reg->death == insn_index
             && src1_reg->birth >= ctx->last_ax_death) {
                src1_info->host_allocated = true;
                src1_info->host_reg = X86_AX;
            }
            ctx->last_ax_death = dest_reg->death;
        }
    }

    if (!src2_info->host_allocated) {
        src2_info->avoid_regs |= 1<<X86_AX | 1<<X86_DX;
    }
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_mod:  Allocate fixed registers for a MODU or MODS
 * instruction.
 */
static void alloc_fixed_regs_mod(HostX86Context *ctx, RTLUnit *unit,
                                 int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];
    const RTLRegister *dest_reg = &unit->regs[insn->dest];
    HostX86RegInfo *dest_info = &ctx->regs[insn->dest];
    const RTLRegister *src1_reg = &unit->regs[insn->src1];
    HostX86RegInfo *src1_info = &ctx->regs[insn->src1];
    HostX86RegInfo *src2_info = &ctx->regs[insn->src2];

    if (ctx->last_dx_death <= insn_index) {
        ASSERT(dest_reg->birth == insn_index);
        if (!(src2_info->host_allocated && src2_info->host_reg == X86_DX)) {
            ASSERT(!dest_info->avoid_regs);
            dest_info->host_allocated = true;
            dest_info->host_reg = X86_DX;
            ctx->last_dx_death = dest_reg->death;
        }
    }

    if (!src1_info->host_allocated
     && !(src1_info->avoid_regs & (1 << X86_AX))
     && src1_reg->birth > ctx->last_ax_death) {
        src1_info->host_allocated = true;
        src1_info->host_reg = X86_AX;
        ctx->last_ax_death = src1_reg->death;
    }

    if (!src2_info->host_allocated) {
        src2_info->avoid_regs |= 1<<X86_AX | 1<<X86_DX;
    }
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_shift:  Allocate fixed registers for a variable-count
 * shift or rotate instruction.
 */
static void alloc_fixed_regs_shift(HostX86Context *ctx, RTLUnit *unit,
                                   int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];
    const RTLRegister *src2_reg = &unit->regs[insn->src2];
    HostX86RegInfo *src2_info = &ctx->regs[insn->src2];

    if (!src2_info->host_allocated
     && !(src2_info->avoid_regs & (1 << X86_CX))
     && src2_reg->birth >= ctx->last_cx_death) {
        src2_info->host_allocated = true;
        src2_info->host_reg = X86_CX;
        ctx->last_cx_death = src2_reg->death;
    }

    /* Make sure rCX isn't allocated to the destination register even if
     * it's later used as a shift count, since the translator doesn't
     * support rCX as a shift destination. */
    ctx->regs[insn->dest].avoid_regs |= 1 << X86_CX;
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_cmpxchg:  Allocate fixed registers for a CMPXCHG
 * instruction.
 */
static void alloc_fixed_regs_cmpxchg(HostX86Context *ctx, RTLUnit *unit,
                                     int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];
    const RTLRegister *src2_reg = &unit->regs[insn->src2];
    HostX86RegInfo *src2_info = &ctx->regs[insn->src2];

    if (!src2_info->host_allocated
     && !(src2_info->avoid_regs & (1 << X86_AX))
     && src2_reg->birth >= ctx->last_ax_death) {
        src2_info->host_allocated = true;
        src2_info->host_reg = X86_AX;
        ctx->last_ax_death = src2_reg->death;
    }

    /* We never allocate CMPXCHG ouptuts in rAX (see notes in the primary
     * allocator). */
    ctx->regs[insn->dest].avoid_regs |= 1 << X86_AX;
}

/*-----------------------------------------------------------------------*/

/**
 * alloc_fixed_regs_return:  Allocate fixed registers for a RETURN
 * instruction.
 */
static void alloc_fixed_regs_return(HostX86Context *ctx, RTLUnit *unit,
                                     int insn_index)
{
    RTLInsn * const insn = &unit->insns[insn_index];

    if (insn->src1) {
        const RTLRegister *src1_reg = &unit->regs[insn->src1];
        HostX86RegInfo *src1_info = &ctx->regs[insn->src1];
        if (!src1_info->host_allocated
         && !(src1_info->avoid_regs & (1 << X86_AX))
         && src1_reg->birth >= ctx->last_ax_death) {
            src1_info->host_allocated = true;
            src1_info->host_reg = X86_AX;
            ctx->last_ax_death = src1_reg->death;
        }
    }
}

/*-----------------------------------------------------------------------*/

/**
 * first_pass_for_block:  Run an initial analysis pass on the given basic
 * block, and perform any enabled RTL-level optimizations.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     block_index: Index of basic block in ctx->unit->blocks[].
 */
static void first_pass_for_block(HostX86Context *ctx, int block_index)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);
    ASSERT(block_index >= 0);
    ASSERT(block_index < ctx->unit->num_blocks);

    RTLUnit * const unit = ctx->unit;
    const RTLBlock * const block = &unit->blocks[block_index];
    HostX86BlockInfo * const block_info = &ctx->blocks[block_index];
    const bool do_fixed_regs =
        (ctx->handle->host_opt & BINREC_OPT_H_X86_FIXED_REGS) != 0;

    const int num_aliases = unit->next_alias;
    block_info->alias_load = (uint16_t *)((uintptr_t)ctx->alias_buffer
                                          + block_index * (4 * num_aliases));
    block_info->alias_store = (uint16_t *)((uintptr_t)ctx->alias_buffer
                                           + block_index * (4 * num_aliases)
                                           + (2 * num_aliases));

    /* If this block has exactly one entering edge and that edge comes from
     * a block we've already seen, carry alias-store data over from that
     * block to try and keep the alias values in host registers. */
    const bool forward_alias_store = (
        block->entries[0] >= 0 && block->entries[0] < block_index
        && block->entries[1] < 0 && block->entry_overflow < 0);
    const uint16_t *predecessor_store = NULL;
    if (forward_alias_store) {
        predecessor_store = ctx->blocks[block->entries[0]].alias_store;
    }

    for (int insn_index = block->first_insn; insn_index <= block->last_insn;
         insn_index++)
    {
        RTLInsn * const insn = &unit->insns[insn_index];

        switch (insn->opcode) {
          case RTLOP_GET_ALIAS:
            if (block_info->alias_store[insn->alias]) {
                /* We already stored the alias in this block!  Probably a
                 * lazy guest translator.  Just reuse the register. */
                insn->src1 = block_info->alias_store[insn->alias];
                insn->opcode = RTLOP_MOVE;
                if (unit->regs[insn->src1].death < insn_index) {
                    unit->regs[insn->src1].death = insn_index;
                }
            } else if (block_info->alias_load[insn->alias]) {
                /* We already loaded the alias once!  Reuse the register. */
                insn->src1 = block_info->alias_load[insn->alias];
                insn->opcode = RTLOP_MOVE;
                if (unit->regs[insn->src1].death < insn_index) {
                    unit->regs[insn->src1].death = insn_index;
                }
            } else {
                block_info->alias_load[insn->alias] = insn->dest;
                /* We don't convert forwarded stores to MOVE in order to
                 * give the register allocator leeway to use a different
                 * register between the beginning of the block and this
                 * instruction. */
            }
            break;

          case RTLOP_SET_ALIAS:
            block_info->alias_store[insn->alias] = insn->src1;
            break;

          case RTLOP_SELECT:
            if (ctx->handle->host_opt & BINREC_OPT_H_X86_FORWARD_CONDITIONS) {
                host_x86_optimize_conditional_move(ctx, insn_index);
            }
            break;

          case RTLOP_MULHU:
          case RTLOP_MULHS:
            if (do_fixed_regs) {
                alloc_fixed_regs_mulh(ctx, unit, insn_index);
            }
            break;

          case RTLOP_DIVU:
          case RTLOP_DIVS:
            if (do_fixed_regs) {
                alloc_fixed_regs_div(ctx, unit, insn_index);
            }
            break;

          case RTLOP_MODU:
          case RTLOP_MODS:
            if (do_fixed_regs) {
                alloc_fixed_regs_mod(ctx, unit, insn_index);
            }
            break;

          case RTLOP_SLL:
          case RTLOP_SRL:
          case RTLOP_SRA:
            if (ctx->handle->setup.host_features & BINREC_FEATURE_X86_BMI2) {
                break;  // SHLX/SHRX/SARX have no fixed registers.
            }
            /* Otherwise fall through. */

          case RTLOP_ROL:
          case RTLOP_ROR:
            if (do_fixed_regs) {
                alloc_fixed_regs_shift(ctx, unit, insn_index);
            }
            break;

          case RTLOP_FCMP:
          case RTLOP_VFCMP: {
            /* For less-than comparisons, it's faster to swap the operands
             * and the sense of the comparison so we don't need to check
             * the parity flag. */
            RTLFloatCompare cmpsel = insn->fcmp & 7;
            if (cmpsel == RTLFCMP_LT || cmpsel == RTLFCMP_LE) {
                insn->fcmp += RTLFCMP_GT - RTLFCMP_LT;
                int temp = insn->src1;
                insn->src1 = insn->src2;
                insn->src2 = temp;
                RTLRegister *dest_reg = &unit->regs[insn->dest];
                dest_reg->result.src1 = insn->src1;
                dest_reg->result.src2 = insn->src2;
                dest_reg->result.fcmp = insn->fcmp;
            }
            break;
          }  // case RTLOP_FCMP, RTLOP_VFCMP

          case RTLOP_VBROADCAST:
            if (ctx->handle->common_opt & BINREC_OPT_DSE) {
                const int src1 = insn->src1;
                RTLRegister * const src1_reg = &unit->regs[src1];
                if (src1_reg->source == RTLREG_CONSTANT
                 && src1_reg->value.i64 == 0
                 && src1_reg->death == insn_index) {
                    /* As in host-x86-optimize.c:kill_reg(). */
                    ASSERT(!ctx->regs[src1].host_allocated);
                    src1_reg->death =
                        rtl_opt_prev_reg_use(unit, src1, insn_index);
                    if (src1_reg->death == src1_reg->birth) {
                        const bool ignore_fexc =
                            (ctx->handle->common_opt & BINREC_OPT_DSE_FP) != 0;
                        rtl_opt_kill_insn(unit, src1_reg->birth, ignore_fexc,
                                          true);
                    }
                }
            }
            break;

          case RTLOP_STORE:
          case RTLOP_STORE_I8:
          case RTLOP_STORE_I16:
          case RTLOP_STORE_BR:
          case RTLOP_STORE_I16_BR:
            if ((ctx->handle->host_opt & BINREC_OPT_H_X86_STORE_IMMEDIATE)
             && unit->regs[insn->src2].source == RTLREG_CONSTANT) {
                host_x86_optimize_immediate_store(ctx, insn_index);
            }
            /* Fall through to address optimization. */
          case RTLOP_LOAD:
          case RTLOP_LOAD_U8:
          case RTLOP_LOAD_S8:
          case RTLOP_LOAD_U16:
          case RTLOP_LOAD_S16:
          case RTLOP_LOAD_BR:
          case RTLOP_LOAD_U16_BR:
          case RTLOP_LOAD_S16_BR:
          case RTLOP_ATOMIC_INC:
            insn->host_data_16 = 0;
            insn->host_data_32 = 0;
            if (ctx->handle->host_opt & BINREC_OPT_H_X86_ADDRESS_OPERANDS) {
                /* This could potentially kill a register to which we've
                 * already assigned a fixed address, but at worst that
                 * will just result in suboptimal register allocation, so
                 * we don't worry about it. */
                host_x86_optimize_address(ctx, insn_index);
            }
            break;

          case RTLOP_CMPXCHG:
            insn->host_data_16 = 0;  // Address optimization, as above.
            insn->host_data_32 = 0;
            if (ctx->handle->host_opt & BINREC_OPT_H_X86_ADDRESS_OPERANDS) {
                host_x86_optimize_address(ctx, insn_index);
            }
            if (do_fixed_regs) {
                alloc_fixed_regs_cmpxchg(ctx, unit, insn_index);
            }
            break;

          case RTLOP_GOTO_IF_Z:
          case RTLOP_GOTO_IF_NZ:
            if (ctx->handle->host_opt & BINREC_OPT_H_X86_FORWARD_CONDITIONS) {
                host_x86_optimize_conditional_branch(ctx, insn_index);
            }
            break;

          case RTLOP_CALL:
            /* Check for constants whose loads can be moved to right
             * before the call. */
            optimize_call_immediates(unit, insn_index);
            /* Non-tail calls require special handling to preserve
             * caller-saved registers around the call, and to avoid
             * allocating callee-saved registers to values which are
             * live across the call.  Record the tailness of the call in
             * host_data_16 so the translator doesn't have to repeat the
             * same check. */
            if ((uint32_t)insn_index == unit->num_insns - 1
                || (unit->insns[insn_index+1].opcode == RTLOP_RETURN
                    && (unit->insns[insn_index+1].src1 == 0
                        || unit->insns[insn_index+1].src1 == insn->dest))) {
                insn->host_data_16 = 1;
            }
            /* Tail calls violate Windows x64 structured exception handling
             * rules, so we have to disable them for that ABI. */
            if (ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS_SEH) {
                insn->host_data_16 = 0;
            }
            if (!insn->host_data_16) {
                insn->host_data_16 = 0;
                insn->host_data_32 = -1;
                if (ctx->last_nontail_call >= 0) {
                    unit->insns[ctx->last_nontail_call].host_data_32 =
                        insn_index;
                } else {
                    ctx->nontail_call_list = insn_index;
                }
                ctx->last_nontail_call = insn_index;
                block_info->has_nontail_call = true;
            }
            /*
             * We could potentially allocate argument and return value
             * registers for the fixed-regs optimization, but we currently
             * don't bother because:
             *
             * - CALL instructions are currently fairly uncommon.
             *
             * - Return values will get the correct register (rAX) if it's
             *   not used by a value live across the CALL, and such values
             *   (which should be uncommon to begin with -- see below) will
             *   prefer callee-saved registers.
             *
             * - Argument registers differ between ABIs, and in the case of
             *   the Windows ABI they overlap fixed registers used by other
             *   instructions (rCX/rDX) so we can't just have separate
             *   "last_argN_death" fields to track register allocation state.
             *
             * We also don't worry about the live-across-a-CALL case when
             * allocating fixed registers for other instructions' operands,
             * since ALU results generally won't be live across a control
             * transfer instruction for the usage patterns of this library.
             */
            break;

          case RTLOP_CALL_TRANSPARENT:
            /* We treat all CALL_TRANSPARENT instructions as non-tail. */
            insn->host_data_16 = 0;
            block_info->has_nontail_call = true;
            optimize_call_immediates(unit, insn_index);
            break;

          case RTLOP_CHAIN_RESOLVE: {
            /* Make sure the function arguments passed to the chained code
             * are live through this instruction, so we can jump directly
             * back to the CHAIN after modifying it. */
            const RTLInsn *chain_insn = &unit->insns[insn->src_imm];
            const int arg0 = chain_insn->src1;
            const int arg1 = chain_insn->src2;
            if (arg0 && unit->regs[arg0].death < insn_index) {
                unit->regs[arg0].death = insn_index;
            }
            if (arg1 && unit->regs[arg1].death < insn_index) {
                unit->regs[arg1].death = insn_index;
            }
            break;
          }  // case RTLOP_CHAIN_RESOLVE

          case RTLOP_RETURN:
            if (do_fixed_regs) {
                alloc_fixed_regs_return(ctx, unit, insn_index);
            }
            break;

          default:
            break;  // Nothing to do in this pass.
        }
    }

    /* Forward alias store data if appropriate.  Aliases which were loaded
     * but not stored in this block are _not_ forwarded, so the code
     * generator knows that it needs to generate a memory store at the
     * earlier SET_ALIAS instruction. */
    if (forward_alias_store) {
        for (int i = 1; i < unit->next_alias; i++) {
            if (!block_info->alias_load[i] && !block_info->alias_store[i]) {
                block_info->alias_store[i] = predecessor_store[i];
            }
        }
    }
}

/*-----------------------------------------------------------------------*/

/**
 * update_alias_live_ranges:  Extend live ranges of registers associated
 * with aliases read by the given basic block so they are live when
 * entering this block.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     block_index: Index of basic block in ctx->unit->blocks[].
 */
static void update_alias_live_ranges(HostX86Context *ctx, int block_index)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);
    ASSERT(block_index >= 0);
    ASSERT(block_index < ctx->unit->num_blocks);

    const RTLUnit * const unit = ctx->unit;
    const int num_aliases = unit->next_alias;
    const RTLBlock * const block = &unit->blocks[block_index];
    HostX86BlockInfo * const block_info = &ctx->blocks[block_index];

    if (block->entries[0] < 0 && block->entry_overflow < 0) {
        return;  // Nothing to do for the initial block.
    }

    for (int alias = 1; alias < num_aliases; alias++) {
        if (!block_info->alias_load[alias]) {
            continue;
        }

        for (int entry_index = block_index; entry_index >= 0;
             entry_index = unit->blocks[entry_index].entry_overflow)
        {
            const RTLBlock * const entry_block = &unit->blocks[entry_index];
            for (int i = 0; (i < lenof(entry_block->entries)
                             && entry_block->entries[i] >= 0); i++) {
                const int predecessor = entry_block->entries[i];
                const int reg = ctx->blocks[predecessor].alias_store[alias];
                if (reg) {
                    const int32_t last_insn =
                        unit->blocks[predecessor].last_insn;
                    if (unit->regs[reg].death < last_insn) {
                        /* If we extend the live range through any blocks
                         * with control flow edges entering from later
                         * blocks, we also need to extend the live range
                         * through the latest of those blocks (and so on). */
                        unit->regs[reg].death = last_insn;
                        for (int j = 0;
                             j != -1 && unit->blocks[j].first_insn <= unit->regs[reg].death;
                             j = unit->blocks[j].next_block)
                        {
                            if (unit->regs[reg].death < unit->blocks[j].min_death) {
                                unit->regs[reg].death = unit->blocks[j].min_death;
                            }
                        }
                    }
                }
            }
        }
    }
    /* Dim lights danced briefly in the black eyes of the bird as, deep in
     * its instructional address space, bracket after bracket was finally
     * closing, if clauses were finally ending, repeat loops halting... */
}

/*************************************************************************/
/********************** Internal interface routines **********************/
/*************************************************************************/

bool host_x86_allocate_registers(HostX86Context *ctx)
{
    ASSERT(ctx);
    ASSERT(ctx->unit);

    RTLUnit * const unit = ctx->unit;

    if (ctx->handle->setup.host == BINREC_ARCH_X86_64_SYSV) {
        ctx->callee_saved_regs =
            1<<X86_BX | 1<<X86_BP | 1<<X86_R12 | 1<<X86_R13
            | 1<<X86_R14 | 1<<X86_R15;
    } else {
        ASSERT(ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS
            || ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS_SEH);
        ctx->callee_saved_regs =
            1<<X86_BX | 1<<X86_BP | 1<<X86_SI | 1<<X86_DI
            | 1<<X86_R12 | 1<<X86_R13 | 1<<X86_R14 | 1<<X86_R15
            | 1<<X86_XMM6 | 1<<X86_XMM7 | 1<<X86_XMM8 | 1<<X86_XMM9
            | 1<<X86_XMM10 | 1<<X86_XMM11 | 1<<X86_XMM12 | 1<<X86_XMM13
            | 1<<X86_XMM14 | 1<<X86_XMM15;
    }

    /* Reserve stack space for any aliases without bound storage. */
    for (int i = 1; i < unit->next_alias; i++) {
        RTLAlias *alias = &unit->aliases[i];
        if (!alias->base) {
            alias->offset = allocate_frame_slot(ctx, alias->type);
        }
    }

    /* Update live ranges for registers live at backward branch targets. */
    rtl_update_live_ranges(unit);

    /* First pass: record alias info, and allocate fixed regs if enabled. */
    ctx->nontail_call_list = -1;
    ctx->last_nontail_call = -1;
    ctx->fixed_reg_list = 0;
    ctx->last_ax_death = -1;
    ctx->last_cx_death = -1;
    ctx->last_dx_death = -1;
    for (int block_index = 0; block_index >= 0;
         block_index = unit->blocks[block_index].next_block)
    {
        first_pass_for_block(ctx, block_index);
    }

    /* Generate sorted list of registers allocated by fixed-regs allocation.
     * We do this in a separate pass since it can be done in linear time,
     * as opposed to ordered insertion during allocation (which won't
     * always be appending to the end of the list because we allocate some
     * registers at point of use rather than definition). */
    if (ctx->handle->host_opt & BINREC_OPT_H_X86_FIXED_REGS) {
        int last_fixed_reg = 0;
        for (int block_index = 0; block_index >= 0;
             block_index = unit->blocks[block_index].next_block)
        {
            const RTLBlock * const block = &unit->blocks[block_index];
            for (int insn_index = block->first_insn;
                 insn_index <= block->last_insn; insn_index++)
            {
                const RTLInsn * const insn = &unit->insns[insn_index];
                if (insn->dest && ctx->regs[insn->dest].host_allocated) {
                    if (last_fixed_reg) {
                        ctx->regs[last_fixed_reg].next_fixed = insn->dest;
                    } else {
                        ctx->fixed_reg_list = insn->dest;
                    }
                    last_fixed_reg = insn->dest;
                }
            }
        }
    }

    /* Extend live ranges for registers linked through aliases. */
    for (int block_index = 0; block_index >= 0;
         block_index = unit->blocks[block_index].next_block)
    {
        update_alias_live_ranges(ctx, block_index);
    }

    /* Second pass: allocate hardware registers to all (remaining) RTL
     * registers. */
    memset(ctx->reg_map, 0, sizeof(ctx->reg_map));
    ctx->regs_free = ~(uint32_t)RESERVED_REGS;
    ctx->regs_touched = 0;
    for (int block_index = 0; block_index >= 0;
         block_index = unit->blocks[block_index].next_block)
    {
        if (!allocate_regs_for_block(ctx, block_index)) {
            return false;
        }
    }

    return true;
}

/*-----------------------------------------------------------------------*/

int host_x86_int_arg_register(const HostX86Context *ctx, int index)
{
    ASSERT(ctx);
    ASSERT(ctx->handle);
    ASSERT(index >= 0);

    if (ctx->handle->setup.host == BINREC_ARCH_X86_64_SYSV) {
        static const uint8_t regs[6] =
            {X86_DI, X86_SI, X86_DX, X86_CX, X86_R8, X86_R9};
        if (index < lenof(regs)) {
            return regs[index];
        } else {
            return -1;
        }
    } else {
        ASSERT(ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS
            || ctx->handle->setup.host == BINREC_ARCH_X86_64_WINDOWS_SEH);
        static const uint8_t regs[4] = {X86_CX, X86_DX, X86_R8, X86_R9};
        if (index < lenof(regs)) {
            return regs[index];
        } else {
            return -1;
        }
    }
}

/*************************************************************************/
/*************************************************************************/
