/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/common.h"
#include "src/endian.h"
#include "src/guest-ppc/guest-ppc-decode.h"
#include "src/guest-ppc/guest-ppc-internal.h"

/*************************************************************************/
/************************ Configuration settings *************************/
/*************************************************************************/

/**
 * BLOCKS_EXPAND_SIZE:  Number of entries by which to expand the block
 * table when full.
 */
#ifndef BLOCKS_EXPAND_SIZE
    #define BLOCKS_EXPAND_SIZE  32
#endif

/*************************************************************************/
/**************************** Local routines *****************************/
/*************************************************************************/

/**
 * get_block:  Return the basic block entry for the given block start
 * address, creating a new entry if create_new is true and one does not
 * yet exist.  Returns -1 if a new entry must be created but memory
 * allocation fails, or if create_new is false and the block is not found.
 *
 * The returned index is only valid until the next successful get_block()
 * call.
 */
static int get_block(GuestPPCContext *ctx, uint32_t address, bool create_new)
{
    ASSERT(ctx);

    int low = 0, high = ctx->num_blocks - 1;  // Binary search
    while (low <= high) {
        const int mid = (low + high) / 2;
        if (address == ctx->blocks[mid].start) {
            return mid;
        } else if (address < ctx->blocks[mid].start) {
            high = mid - 1;
        } else {
            low = mid + 1;
        }
    }

    if (!create_new) {
        return -1;
    }

    if (ctx->num_blocks >= ctx->blocks_size) {
        const int new_size = ctx->blocks_size + BLOCKS_EXPAND_SIZE;
        GuestPPCBlockInfo *new_blocks = binrec_realloc(
            ctx->handle, ctx->blocks, sizeof(*ctx->blocks) * new_size);
        if (UNLIKELY(!new_blocks)) {
            return -1;
        }
        ctx->blocks = new_blocks;
        ctx->blocks_size = new_size;
    }

    const int index = low;
    GuestPPCBlockInfo *block = &ctx->blocks[index];
    memmove(block+1, block, sizeof(*ctx->blocks) * (ctx->num_blocks - index));
    ctx->num_blocks++;
    memset(block, 0, sizeof(*block));
    block->start = address;
    block->branch_block = -1;
    block->next_block = -1;
    return index;
}

/*-----------------------------------------------------------------------*/

/*
 * Helper functions for update_used_changed() to set register used/changed
 * bits.  The mark_*_used functions mark a register as used only if the
 * same register has not been set in the block (since if it has been set,
 * the value being read is that value, not the value on entry to the block).
 *
 * For FPR functions, fpr_type gives the register access type (FPR_*).
 */

static inline void mark_gpr_used(GuestPPCBlockInfo *block, const int index) {
    if (!(block->gpr_changed & (1 << index))) block->gpr_used |= 1 << index;
}

static inline void mark_gpr_changed(GuestPPCBlockInfo *block, const int index) {
    block->gpr_changed |= 1 << index;
}

static inline void mark_fpr_used(GuestPPCBlockInfo *block, const int index,
                                 int fpr_type) {
    if (!(block->fpr_changed & (1 << index))) block->fpr_used |= 1 << index;
    if (!block->fpr_type_first[index]) block->fpr_type_first[index] = fpr_type;
    block->fpr_type_last[index] = fpr_type;
}

static inline void mark_fpr_changed(GuestPPCBlockInfo *block, const int index,
                                    int fpr_type) {
    block->fpr_changed |= 1 << index;
    if (!block->fpr_type_first[index]) block->fpr_type_first[index] = fpr_type;
    block->fpr_type_last[index] = fpr_type;
}

static inline void mark_crb_used(GuestPPCBlockInfo *block, const int index) {
    if (!(block->crb_changed & (1 << index))) block->crb_used |= 1 << index;
}

static inline void mark_crf_used(GuestPPCBlockInfo *block, const int index) {
    const uint32_t mask = 0xF << (4*index);
    if ((block->crb_changed & mask) != mask) {
        block->crb_used |= 0xF << (4*index);
    }
}

static inline void mark_cr_used(GuestPPCBlockInfo *block) {
    if (!block->cr_changed) block->cr_used = 1;
}

static inline void mark_crb_changed(GuestPPCBlockInfo *block, const int index) {
    block->crb_changed |= 1 << index;
}

static inline void mark_crf_changed(GuestPPCBlockInfo *block, const int index) {
    block->crb_changed |= 0xF << (4*index);
}

static inline void mark_cr_changed(GuestPPCBlockInfo *block) {
    block->cr_changed = 1;
}

static inline void mark_lr_used(GuestPPCBlockInfo *block) {
    if (!block->lr_changed) block->lr_used = 1;
}

static inline void mark_lr_changed(GuestPPCBlockInfo *block) {
    block->lr_changed = 1;
}

static inline void mark_ctr_used(GuestPPCBlockInfo *block) {
    if (!block->ctr_changed) block->ctr_used = 1;
}

static inline void mark_ctr_changed(GuestPPCBlockInfo *block) {
    block->ctr_changed = 1;
}

static inline void mark_xer_used(GuestPPCBlockInfo *block) {
    if (!block->xer_changed) block->xer_used = 1;
}

static inline void mark_xer_changed(GuestPPCBlockInfo *block) {
    block->xer_changed = 1;
}

static inline void mark_fpscr_used(GuestPPCBlockInfo *block) {
    if (!block->fpscr_changed) block->fpscr_used = 1;
}

static inline void mark_fpscr_changed(GuestPPCBlockInfo *block) {
    block->fpscr_changed = 1;
}

static inline void mark_fr_fi_fprf_used(GuestPPCBlockInfo *block) {
    if (!block->fr_fi_fprf_changed) block->fr_fi_fprf_used = 1;
}

static inline void mark_fr_fi_fprf_changed(GuestPPCBlockInfo *block) {
    block->fr_fi_fprf_changed = 1;
}

/*-----------------------------------------------------------------------*/

/**
 * update_used_changed:  Update the register used and changed data in the
 * given block based on the given instruction.  The instruction is assumed
 * to be valid.
 */
static void update_used_changed(GuestPPCContext *ctx, GuestPPCBlockInfo *block,
                                const uint32_t insn)
{
    switch (insn_OPCD(insn)) {
      case OPCD_SC:
        /* No registers touched. */
        break;

      case OPCD_TWI:
        mark_gpr_used(block, insn_rA(insn));
        break;

      case OPCD_BC:
        if (!(insn_BO(insn) & 0x04)) {
            mark_ctr_used(block);
            mark_ctr_changed(block);
        }
        if (!(insn_BO(insn) & 0x10)) {
            mark_crb_used(block, insn_BI(insn));
        }
        /* fall through */
      case OPCD_B:
        if (insn_LK(insn)) {
            mark_lr_changed(block);
        }
        break;

      case OPCD_CMPLI:
      case OPCD_CMPI:
        mark_gpr_used(block, insn_rA(insn));
        mark_xer_used(block);
        mark_crf_changed(block, insn_crfD(insn));
        break;

      case OPCD_ADDIC_:
        mark_crf_changed(block, 0);
        /* fall through */
      case OPCD_SUBFIC:
      case OPCD_ADDIC:
        mark_xer_used(block);
        mark_xer_changed(block);
        /* fall through */
      case OPCD_MULLI:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rD(insn));
        break;

      case OPCD_ADDIS:
        if (insn_rA(insn) != 0) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_gpr_changed(block, insn_rD(insn));
        break;

      case OPCD_ORI:
        if (insn == 0x60000000) {  // nop
            break;
        }
        /* fall through */
      case OPCD_ORIS:
      case OPCD_XORI:
      case OPCD_XORIS:
        mark_gpr_used(block, insn_rS(insn));
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_ANDI_:
      case OPCD_ANDIS_:
        mark_xer_used(block);
        mark_crf_changed(block, 0);
        mark_gpr_used(block, insn_rS(insn));
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_ADDI:
      case OPCD_LWZ:
      case OPCD_LBZ:
      case OPCD_LHZ:
      case OPCD_LHA:
        if (insn_rA(insn) != 0) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_gpr_changed(block, insn_rD(insn));
        break;

      case OPCD_RLWIMI:
        mark_gpr_used(block, insn_rA(insn));
        /* fall through */
      case OPCD_RLWINM:
        mark_gpr_used(block, insn_rS(insn));
        mark_gpr_changed(block, insn_rA(insn));
        if (insn_Rc(insn)) {
            mark_xer_used(block);
            mark_crf_changed(block, 0);
        }
        break;

      case OPCD_LWZU:
      case OPCD_LBZU:
      case OPCD_LHZU:
      case OPCD_LHAU:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rD(insn));
        break;

      case OPCD_STW:
      case OPCD_STB:
      case OPCD_STH:
        if (insn_rA(insn) != 0) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_gpr_used(block, insn_rD(insn));
        break;

      case OPCD_STWU:
      case OPCD_STBU:
      case OPCD_STHU:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_used(block, insn_rD(insn));
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_RLWNM:
        mark_gpr_used(block, insn_rS(insn));
        mark_gpr_used(block, insn_rB(insn));
        mark_gpr_changed(block, insn_rA(insn));
        if (insn_Rc(insn)) {
            mark_xer_used(block);
            mark_crf_changed(block, 0);
        }
        break;

      case OPCD_LMW:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        for (int i = insn_rD(insn); i < 32; i++) {
            mark_gpr_changed(block, i);
        }
        break;

      case OPCD_STMW:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        for (int i = insn_rD(insn); i < 32; i++) {
            mark_gpr_used(block, i);
        }
        break;

      case OPCD_LFS:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_fpr_changed(block, insn_frD(insn), FPR_SINGLE);
        break;

      case OPCD_LFD:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
        break;

      case OPCD_PSQ_L:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
        ctx->fpr_is_ps |= 1 << insn_frD(insn);
        break;

      case OPCD_LFSU:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rA(insn));
        mark_fpr_changed(block, insn_frD(insn), FPR_SINGLE);
        break;

      case OPCD_LFDU:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rA(insn));
        mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
        break;

      case OPCD_PSQ_LU:
        mark_gpr_used(block, insn_rA(insn));
        mark_gpr_changed(block, insn_rA(insn));
        mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
        ctx->fpr_is_ps |= 1 << insn_frD(insn);
        break;

      case OPCD_STFS:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        // FIXME: this shouldn't cause the ps0 value to get copied to ps1
        mark_fpr_used(block, insn_frD(insn), FPR_SINGLE);
        break;

      case OPCD_STFD:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_fpr_used(block, insn_frD(insn), FPR_DOUBLE);
        break;

      case OPCD_PSQ_ST:
        if (insn_rA(insn)) {
            mark_gpr_used(block, insn_rA(insn));
        }
        mark_fpr_used(block, insn_frD(insn), FPR_PAIRED);
        ctx->fpr_is_ps |= 1 << insn_frD(insn);
        break;

      case OPCD_STFSU:
        mark_gpr_used(block, insn_rA(insn));
        mark_fpr_used(block, insn_frD(insn), FPR_SINGLE);
        // FIXME: as above
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_STFDU:
        mark_gpr_used(block, insn_rA(insn));
        mark_fpr_used(block, insn_frD(insn), FPR_DOUBLE);
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_PSQ_STU:
        mark_gpr_used(block, insn_rA(insn));
        mark_fpr_used(block, insn_frD(insn), FPR_PAIRED);
        ctx->fpr_is_ps |= 1 << insn_frD(insn);
        mark_gpr_changed(block, insn_rA(insn));
        break;

      case OPCD_x04:
        switch ((PPCExtendedOpcode04_750CL_5)insn_XO_5(insn)) {
          case XO_PS_CMP:
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_used(block);
            mark_fr_fi_fprf_changed(block);
            mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frA(insn);
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_crf_used(block, insn_crfD(insn));  // FIXME: temp until these insns are implemented, to avoid uninitialized values in CR merging
            mark_crf_changed(block, insn_crfD(insn));
            break;
          case XO_PSQ_LX:
            if (insn_rA(insn)) {
                mark_gpr_used(block, insn_rA(insn));
            }
            mark_gpr_used(block, insn_rB(insn));
            if (insn_XO_10(insn) & 0x020) {
                mark_gpr_changed(block, insn_rA(insn));
            }
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            break;
          case XO_PSQ_STX:
            if (insn_rA(insn)) {
                mark_gpr_used(block, insn_rA(insn));
            }
            mark_gpr_used(block, insn_rB(insn));
            mark_fpr_used(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            if (insn_XO_10(insn) & 0x020) {
                mark_gpr_changed(block, insn_rA(insn));
            }
            break;
          case XO_PS_MOVE:
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            if (insn_Rc(insn)) {
                mark_fpscr_used(block);
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_MERGE:
            if (insn_XO_10(insn) == XO_PS_MERGE10
             && insn_frB(insn) == insn_frA(insn)
             && insn_frD(insn) == insn_frA(insn)) {
                /* This is the IBM-recommended way to switch a register
                 * from double to single precision without touching the
                 * value in ps1, so it doesn't require a paired-mode alias. */
                mark_fpr_used(block, insn_frD(insn), FPR_SINGLE);
                mark_fpr_changed(block, insn_frD(insn), FPR_SINGLE);
            } else {
                mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
                ctx->fpr_is_ps |= 1 << insn_frA(insn);
                mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
                ctx->fpr_is_ps |= 1 << insn_frB(insn);
                mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
                ctx->fpr_is_ps |= 1 << insn_frD(insn);
            }
            if (insn_Rc(insn)) {
                mark_fpscr_used(block);
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_MISC:  // dcbz_l
            if (insn_rA(insn) != 0) {
                mark_gpr_used(block, insn_rA(insn));
            }
            mark_gpr_used(block, insn_rB(insn));
            break;
          case XO_PS_DIV:
          case XO_PS_SUB:
          case XO_PS_ADD:
            mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frA(insn);
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_SEL:
            mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frA(insn);
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_fpr_used(block, insn_frC(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frC(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            if (insn_Rc(insn)) {
                mark_fpscr_used(block);
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_RES:
          case XO_PS_RSQRTE:
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_MULS0:
          case XO_PS_MULS1:
          case XO_PS_MUL:
            mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frA(insn);
            mark_fpr_used(block, insn_frC(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frC(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;
          case XO_PS_SUM0:
          case XO_PS_SUM1:
          case XO_PS_MADDS0:
          case XO_PS_MADDS1:
          case XO_PS_MSUB:
          case XO_PS_MADD:
          case XO_PS_NMSUB:
          case XO_PS_NMADD:
            mark_fpr_used(block, insn_frA(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frA(insn);
            mark_fpr_used(block, insn_frB(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frB(insn);
            mark_fpr_used(block, insn_frC(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frC(insn);
            mark_fpr_changed(block, insn_frD(insn), FPR_PAIRED);
            ctx->fpr_is_ps |= 1 << insn_frD(insn);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;
          default:
            ASSERT(!"impossible");
        }
        break;

      case OPCD_x13:
        switch (insn_XO_5(insn)) {
          case 0x00:  // mcrf
            mark_crf_used(block, insn_crfS(insn));
            mark_crf_changed(block, insn_crfD(insn));
            break;

          case 0x01:  // crand, etc.
            /* No dependency on crbA/crbB for crclr and crset. */
            if (!((insn_XO_10(insn) == XO_CRXOR || insn_XO_10(insn) == XO_CREQV)
                  && insn_crbA(insn) == insn_crbB(insn))) {
                mark_crb_used(block, insn_crbA(insn));
                mark_crb_used(block, insn_crbB(insn));
            }
            mark_crb_changed(block, insn_crbD(insn));
            break;

          case 0x10:  // bclr/bcctr
            if (insn_XO_10(insn) & 0x200) {  // bcctr
                mark_ctr_used(block);
                // FIXME: try to detect jump tables
            } else {  // bclr
                mark_lr_used(block);
                if (!(insn_BO(insn) & 0x04)) {
                    mark_ctr_used(block);
                    mark_ctr_changed(block);
                }
            }
            if (!(insn_BO(insn) & 0x10)) {
                mark_crb_used(block, insn_BI(insn));
            }
            if (insn_LK(insn)) {
                mark_lr_changed(block);
            }
            break;

          case 0x12:  // rfi
          case 0x16:  // isync
            break;  // No user-level registers modified

          default:
            ASSERT(!"impossible");
        }
        break;

      case OPCD_x1F:
        switch (insn_XO_5(insn)) {
          case 0x00:  // cmp, cmpl, mcrxr
            mark_xer_used(block);
            if (insn_XO_10(insn) == XO_MCRXR) {
                mark_xer_changed(block);
            } else {
                mark_gpr_used(block, insn_rA(insn));
                mark_gpr_used(block, insn_rB(insn));
            }
            mark_crf_changed(block, insn_crfD(insn));
            break;

          case 0x04:  // tw
            mark_gpr_used(block, insn_rA(insn));
            mark_gpr_used(block, insn_rB(insn));
            break;

          case 0x08:  // sub*
          case 0x0A:  // add*
            mark_gpr_used(block, insn_rA(insn));
            if ((insn_XO_10(insn) & 0x1C0) != 0x0C0  // sub/addze, sub/addme
             && (insn_XO_10(insn) & 0x1FF) != XO_NEG) {
                mark_gpr_used(block, insn_rB(insn));
            }
            mark_gpr_changed(block, insn_rD(insn));
            if (insn_OE(insn) || !(insn_XO_10(insn) == XO_SUBF
                               || insn_XO_10(insn) == XO_NEG
                               || insn_XO_10(insn) == XO_ADD)) {
                mark_xer_used(block);
                mark_xer_changed(block);
            }
            if (insn_Rc(insn)) {
                mark_xer_used(block);
                mark_crf_changed(block, 0);
            }
            break;

          case 0x0B:  // mul*, div*
            mark_gpr_used(block, insn_rA(insn));
            mark_gpr_used(block, insn_rB(insn));
            mark_gpr_changed(block, insn_rD(insn));
            if (insn_OE(insn)) {
                mark_xer_used(block);
                mark_xer_changed(block);
            }
            if (insn_Rc(insn)) {
                mark_xer_used(block);
                mark_crf_changed(block, 0);
            }
            break;

          case 0x10:  // mtcrf
            mark_gpr_used(block, insn_rS(insn));
            /* CRM==255 (mtcr) is handled specially, without use of the CR
             * bit aliases. */
            if (insn_CRM(insn) == 255) {
                mark_cr_changed(block);
            } else {
                for (int crf = 0; crf < 8; crf++) {
                    if (insn_CRM(insn) & (0x80 >> crf)) {
                        mark_crf_changed(block, crf);
                    }
                }
            }
            break;

          case 0x12:  // mtmsr, mtsr, mtsrin, tlbie
            if (insn_XO_10(insn) != XO_TLBIE) {
                mark_gpr_used(block, insn_rS(insn));
            }
            if (insn_XO_10(insn) == XO_MTSRIN || insn_XO_10(insn) == XO_TLBIE) {
                mark_gpr_used(block, insn_rB(insn));
            }
            break;

          case 0x13:  // mf*r, mtspr
            if (insn_XO_10(insn) == XO_MTSPR) {
                mark_gpr_used(block, insn_rS(insn));
            } else {
                if (insn_XO_10(insn) == XO_MFCR) {
                    mark_cr_used(block);
                } else if (insn_XO_10(insn) == XO_MFSRIN) {
                    mark_gpr_used(block, insn_rB(insn));
                }
                mark_gpr_changed(block, insn_rD(insn));
            }
            if (insn_XO_10(insn) == XO_MFSPR) {
                const int spr = insn_spr(insn);
                if (spr == SPR_XER) {
                    mark_xer_used(block);
                } else if (spr == SPR_LR) {
                    mark_lr_used(block);
                } else if (spr == SPR_CTR) {
                    mark_ctr_used(block);
                }
            } else if (insn_XO_10(insn) == XO_MTSPR) {
                const int spr = insn_spr(insn);
                if (spr == SPR_XER) {
                    mark_xer_changed(block);
                } else if (spr == SPR_LR) {
                    mark_lr_changed(block);
                } else if (spr == SPR_CTR) {
                    mark_ctr_changed(block);
                }
            }
            break;

          case 0x14:  // lwarx
            if (insn_rA(insn) != 0) {
                mark_gpr_used(block, insn_rA(insn));
            }
            mark_gpr_used(block, insn_rB(insn));
            mark_gpr_changed(block, insn_rD(insn));
            break;

          case 0x15: {  // lsw*, stsw*
            const bool is_immediate = (insn_XO_10(insn) & 0x040) != 0;
            const bool is_store = (insn_XO_10(insn) & 0x080) != 0;
            if (insn_rA(insn)) {
                mark_gpr_used(block, insn_rA(insn));
            }
            if (is_immediate) {
                const int n = insn_NB(insn) ? insn_NB(insn) : 32;
                int rD = insn_rD(insn);
                for (int i = 0; i < n; i += 4, rD = (rD + 1) & 31) {
                    if (is_store) {
                        mark_gpr_used(block, rD);
                    } else {
                        mark_gpr_changed(block, rD);
                    }
                }
            } else {
                mark_gpr_used(block, insn_rB(insn));
                mark_xer_used(block);
                /* We have no way to tell at this point how many bytes
                 * will be transferred, so we essentially have to say
                 * "we no longer have any idea about GPRs in this block".
                 * We mark all GPRs as used so their values are available
                 * if needed, then for lswx we mark all GPRs as changed to
                 * ensure that whatever registers were modified get written
                 * back to the processor state block.  RTL optimization
                 * should be able to clear out some of the unnecessary
                 * loads and stores, but this is probably an example of
                 * how (from the PowerPC manual) "this instruction is
                 * likely to ... take longer to execute, perhaps much
                 * longer, than a sequence of individual load or store
                 * instructions that produce the same results." */
                for (int i = 0; i < 32; i++) {
                    mark_gpr_used(block, i);
                    if (!is_store) {
                        mark_gpr_changed(block, i);
                    }
                }
            }
            break;
          }

          case 0x16:  // Miscellaneous instructions
            switch (insn_XO_10(insn)) {
              case XO_DCBST:
              case XO_DCBF:
              case XO_DCBTST:
              case XO_DCBT:
              case XO_DCBI:
              case XO_ICBI:
              case XO_DCBZ:
                if (insn_rA(insn) != 0) {
                    mark_gpr_used(block, insn_rA(insn));
                }
                mark_gpr_used(block, insn_rB(insn));
                break;
              case XO_STWCX_:
                if (insn_rA(insn) != 0) {
                    mark_gpr_used(block, insn_rA(insn));
                }
                mark_gpr_used(block, insn_rB(insn));
                mark_gpr_used(block, insn_rS(insn));
                mark_xer_used(block);
                mark_crf_changed(block, 0);
                break;
              case XO_ECIWX:
              case XO_LWBRX:
              case XO_LHBRX:
                if (insn_rA(insn) != 0) {
                    mark_gpr_used(block, insn_rA(insn));
                }
                mark_gpr_used(block, insn_rB(insn));
                mark_gpr_changed(block, insn_rD(insn));
                break;
              case XO_ECOWX:
              case XO_STWBRX:
              case XO_STHBRX:
                if (insn_rA(insn) != 0) {
                    mark_gpr_used(block, insn_rA(insn));
                }
                mark_gpr_used(block, insn_rB(insn));
                mark_gpr_used(block, insn_rS(insn));
                break;
              case XO_TLBSYNC:
              case XO_SYNC:
              case XO_EIEIO:  // And on that farm he had a duck...
                break;
              default:
                ASSERT(!"impossible");
            }
            break;

          case 0x17: {  // Indexed load/store
            const bool is_fp = (insn_XO_10(insn) & 0x200) != 0;
            const bool is_store = (insn_XO_10(insn) & 0x080) != 0;
            const bool is_update = (insn_XO_10(insn) & 0x020) != 0;
            if (insn_rA(insn) != 0) {
                mark_gpr_used(block, insn_rA(insn));
            }
            mark_gpr_used(block, insn_rB(insn));
            if (is_fp) {
                const bool fp_type =
                    (insn_XO_10(insn) & 0x040) ? FPR_DOUBLE : FPR_SINGLE;
                if (is_store) {
                    mark_fpr_used(block, insn_frD(insn), fp_type);
                } else {
                    mark_fpr_changed(block, insn_frD(insn), fp_type);
                }
            } else {
                if (is_store) {
                    mark_gpr_used(block, insn_rD(insn));
                } else {
                    mark_gpr_changed(block, insn_rD(insn));
                }
            }
            if (is_update) {
                mark_gpr_changed(block, insn_rA(insn));
            }
            break;
          }

          case 0x18:  // Shift instructions
            mark_gpr_used(block, insn_rS(insn));
            if (insn_XO_10(insn) != XO_SRAWI) {
                mark_gpr_used(block, insn_rB(insn));
            }
            mark_gpr_changed(block, insn_rA(insn));
            if (insn_XO_10(insn) == XO_SRAW || insn_XO_10(insn) == XO_SRAWI) {
                mark_xer_used(block);
                mark_xer_changed(block);
            }
            if (insn_Rc(insn)) {
                mark_xer_used(block);
                mark_crf_changed(block, 0);
            }
            break;

          case 0x1A:  // cntlzw, extsb, extsh
            mark_gpr_used(block, insn_rS(insn));
            mark_gpr_changed(block, insn_rA(insn));
            if (insn_Rc(insn)) {
                mark_xer_used(block);
                mark_crf_changed(block, 0);
            }
            break;

          case 0x1C:  // Logical instructions
            mark_gpr_used(block, insn_rS(insn));
            mark_gpr_used(block, insn_rB(insn));
            mark_gpr_changed(block, insn_rA(insn));
            if (insn_Rc(insn)) {
                mark_xer_used(block);
                mark_crf_changed(block, 0);
            }
            break;

          default:
            ASSERT(!"impossible");
        }
        break;

      case OPCD_x3B:
        switch (insn_XO_5(insn)) {
          case XO_FDIVS:
          case XO_FSUBS:
          case XO_FADDS:
            mark_fpr_used(block, insn_frA(insn), FPR_SINGLE);
            mark_fpr_used(block, insn_frB(insn), FPR_SINGLE);
            break;

          case XO_FRES:
            mark_fpr_used(block, insn_frB(insn), FPR_SINGLE);
            break;

          case XO_FMULS:
            mark_fpr_used(block, insn_frA(insn), FPR_SINGLE);
            mark_fpr_used(block, insn_frC(insn), FPR_SINGLE);
            break;

          case XO_FMSUBS:
          case XO_FMADDS:
          case XO_FNMSUBS:
          case XO_FNMADDS:
            mark_fpr_used(block, insn_frA(insn), FPR_SINGLE);
            mark_fpr_used(block, insn_frB(insn), FPR_SINGLE);
            mark_fpr_used(block, insn_frC(insn), FPR_SINGLE);
            break;

          default:
            ASSERT(!"impossible");
        }
        mark_fpr_changed(block, insn_frD(insn), FPR_SINGLE);
        mark_fpscr_used(block);
        mark_fpscr_changed(block);
        mark_fr_fi_fprf_changed(block);
        if (insn_Rc(insn)) {
            mark_crf_changed(block, 1);
        }
        break;

      case OPCD_x3F:
        switch (insn_XO_5(insn)) {
          case 0x00:  // fcmpu, fcmpo, mcrfs
            mark_fpscr_used(block);
            if (insn_XO_10(insn) == XO_MCRFS) {
                if (insn_crfS(insn) <= 3 || insn_crfS(insn) == 5) {
                    mark_fpscr_changed(block);
                }
                if (insn_crfS(insn) == 3 || insn_crfS(insn) == 4) {
                    mark_fr_fi_fprf_used(block);
                    mark_fr_fi_fprf_changed(block);
                }
            } else {  // fcmpu, fcmpo
                mark_fpr_used(block, insn_frA(insn), FPR_DOUBLE);
                mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
                mark_fpscr_changed(block);
                mark_fr_fi_fprf_used(block);
                mark_fr_fi_fprf_changed(block);
            }
            mark_crf_changed(block, insn_crfD(insn));
            break;

          case 0x06:  // mtfsb0, mtfsb1, mtfsfi
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            if (insn_XO_10(insn) == XO_MTFSFI
                ? (insn_crfD(insn) == 3 || insn_crfD(insn) == 4)
                : (insn_crbD(insn) >= 13 && insn_crbD(insn) <= 19))
            {
                mark_fr_fi_fprf_used(block);
                mark_fr_fi_fprf_changed(block);
            }
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case 0x07:  // mffs, mtfsf
            if (!(insn_XO_10(insn) == XO_MTFSF && insn_FM(insn) == 0xFF)) {
                mark_fpscr_used(block);
                mark_fr_fi_fprf_used(block);
            }
            if (insn_XO_10(insn) == XO_MFFS) {
                mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            } else {
                mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
                mark_fpscr_changed(block);
                mark_fr_fi_fprf_changed(block);
            }
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case 0x08:  // fmr, etc.
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            if (insn_Rc(insn)) {
                mark_fpscr_used(block);
                mark_crf_changed(block, 1);
            }
            break;

          case 0x0C:  // frsp
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_SINGLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case 0x0E:  // fctiw
          case 0x0F:  // fctiwz
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case XO_FDIV:
          case XO_FSUB:
          case XO_FADD:
            mark_fpr_used(block, insn_frA(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case XO_FSEL:
            mark_fpr_used(block, insn_frA(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frC(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            if (insn_Rc(insn)) {
                mark_fpscr_used(block);
                mark_crf_changed(block, 1);
            }
            break;

          case XO_FMUL:
            mark_fpr_used(block, insn_frA(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frC(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case XO_FRSQRTE:
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          case XO_FMSUB:
          case XO_FMADD:
          case XO_FNMSUB:
          case XO_FNMADD:
            mark_fpr_used(block, insn_frA(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frB(insn), FPR_DOUBLE);
            mark_fpr_used(block, insn_frC(insn), FPR_DOUBLE);
            mark_fpr_changed(block, insn_frD(insn), FPR_DOUBLE);
            mark_fpscr_used(block);
            mark_fpscr_changed(block);
            mark_fr_fi_fprf_changed(block);
            if (insn_Rc(insn)) {
                mark_crf_changed(block, 1);
            }
            break;

          default:
            ASSERT(!"impossible");
        }
        break;

      default:
        ASSERT(!"impossible");
    }  // switch (insn_OPCD(insn))
}

/*-----------------------------------------------------------------------*/

/**
 * scan_cr_bits:  Set the given block's crb_changed_recursive field to the
 * set of CR bits which are written at least once on every control flow
 * path out of the unit.
 *
 * [Parameters]
 *     ctx: Translation context.
 *     visited: Array of visited flags (1 byte per block).
 *     block_index: Index of block to scan.
 */
static void scan_cr_bits(GuestPPCContext *ctx, uint8_t *visited,
                         int block_index)
{
    ASSERT(ctx);
    ASSERT(visited);
    ASSERT(block_index >= 0);
    ASSERT(block_index < ctx->num_blocks);
    ASSERT(!visited[block_index]);

    GuestPPCBlockInfo * const block = &ctx->blocks[block_index];

    /* Mark the current block as currently being scanned, so we can detect
     * cycles. */
    visited[block_index] = 1;

    /* Look up the control flow edges out of this block, and recursively
     * scan them if they haven't been seen yet.  If either edge is to a
     * block we're recursing from, implying that we've found a cycle in
     * the flow graph, we simply drop it.  A full analysis would determine
     * the set of bits written by the cycle as a whole and propagate that
     * to all edges entering the cycle, but for our purposes it should be
     * good enough (and is faster) to just stop recursing when we find a
     * cycle; for typical cases in which we fall into the top of a loop,
     * we'll still pick up the full set of changed bits. */
    bool have_second_exit = false;
    int exit0 = block->branch_block;
    if (exit0 >= 0 && !visited[exit0]) {
        scan_cr_bits(ctx, visited, exit0);
    }
    int exit1;
    if (block->is_conditional_branch) {
        exit1 = block->next_block;
        if (exit1 >= 0 && !visited[exit1]) {
            scan_cr_bits(ctx, visited, exit1);
        }
        if (exit1 >= 0 && visited[exit1] == 1) {
            /* The fall-through edge is a cycle.  In this case we can just
             * treat the branch as an unconditional branch to the target. */
            exit1 = -1;
        } else if (exit0 >= 0 && visited[exit0] == 1) {
            /* The branch target edge is a cycle.  Ignore it and use the
             * fall-through edge for CR bit checking. */
            exit0 = block->next_block;
        } else {
            /* Neither edge is a cycle, so we need to look at both edges.
             * If either edge is terminal, we won't forward any CR bits
             * through this block (even if the other edge is non-terminal). */
            have_second_exit = true;
        }
    }

    /* Determine which CR bits are changed on all outgoing paths (modulo
     * any cycle-entering edges we dropped). */
    uint32_t successor_changed;
    if (exit0 >= 0) {
        successor_changed = ctx->blocks[exit0].crb_changed_recursive;
    } else {
        successor_changed = 0;
    }
    if (have_second_exit) {
        if (exit1 >= 0) {
            successor_changed &= ctx->blocks[exit1].crb_changed_recursive;
        } else {
            successor_changed = 0;
        }
    }

    /* If the block contains a trap, we have to ensure flags are properly
     * stored before calling the trap handler, so we can't pass any bits
     * through from successor blocks. */
    if (block->has_trap) {
        successor_changed = 0;
    }

    /* Record the final set of changed CR bits. */
    block->crb_changed_recursive =
        (block->crb_changed | successor_changed) & ~block->crb_used;

    /* Mark the current block as completely scanned so that parent blocks
     * can pick up its full bit set. */
    visited[block_index] = 2;
}

/*************************************************************************/
/************************* Scanning entry point **************************/
/*************************************************************************/

bool guest_ppc_scan(GuestPPCContext *ctx, uint32_t limit)
{
    ASSERT(ctx);

    ASSERT((ctx->start & 3) == 0);
    ASSERT(limit >= ctx->start + 3);
    const uint32_t start = ctx->start;
    /* max_insns is calculated in this manner to correctly handle the
     * pathological case where start == 0 and limit == -1. */
    const uint32_t aligned_limit = min(limit, ctx->handle->code_range_end) - 3;
    const uint32_t max_insns = (aligned_limit - ctx->start) / 4 + 1;
    const uint32_t *memory_base =
        (const uint32_t *)ctx->handle->setup.guest_memory_base;

    GuestPPCBlockInfo *block = NULL;

    /* First scan instructions starting from the given address to
     * identify basic blocks in the code stream. */
    uint32_t insn_count;
    for (insn_count = 0; insn_count < max_insns; insn_count++) {
        const uint32_t address = start + insn_count*4;
        const uint32_t insn = bswap_be32(memory_base[address/4]);
        const bool is_invalid = !is_valid_insn(insn);
        const PPCOpcode opcd = insn_OPCD(insn);

        /* Start a new block if the previous one was terminated (or if
         * this is the first instruction scanned). */
        if (!block) {
            /* If this opcode is invalid, we may be passing over data for
             * a jump table, so don't treat this as part of a block.  But
             * always translate invalid instructions at the beginning of
             * a block. */
            if (address > start && UNLIKELY(is_invalid)) {
                continue;
            }
            /* Otherwise, start a new block at this address.  Do this even
             * if the current address is not a branch target, since the
             * previous instruction may have (for example) branched to loop
             * termination code which will in turn branch back here. */
            const int block_index = get_block(ctx, address, true);
            if (UNLIKELY(block_index < 0)) {
                log_error(ctx->handle, "Out of memory expanding basic block"
                          " table at 0x%X", address);
                return false;
            }
            block = &ctx->blocks[block_index];
        }

        /* Check for trap instructions (used by the TRIM_CR_STORES
         * optimization). */
        if (opcd==OPCD_TWI || (opcd==OPCD_x1F && insn_XO_10(insn)==XO_TW)) {
            block->has_trap = true;
        }

        /* Terminate the block if this is a branch, trap, or invalid
         * instruction, or at icbi.  (We terminate at trap instructions
         * to help out data flow analysis, since we need to be able to
         * store all live register values to the state block if a trap is
         * taken.)  Also terminate the entire unit if this looks like the
         * end of a function.  But make sure not to stop at an sc before
         * a blr so we can optimize the sc+blr case properly. */
        const bool is_direct_branch = ((opcd & ~0x02) == OPCD_BC);
        const bool is_indirect_branch =
            (opcd == OPCD_x13 && (insn_XO_10(insn) == XO_BCLR
                                  || insn_XO_10(insn) == XO_BCCTR));
        const int is_unconditional_branch =
            (opcd == OPCD_B
             || ((opcd == OPCD_BC || is_indirect_branch)
                 && (insn_BO(insn) & 0x14) == 0x14));
        const bool is_icbi = (opcd == OPCD_x1F && insn_XO_10(insn) == XO_ICBI);
        const bool is_sc =
            (opcd == OPCD_SC
             && (insn_count == max_insns - 1
                 || bswap_be32(memory_base[(address+4)/4]) != 0x4E800020));
        if (is_direct_branch || is_indirect_branch || block->has_trap
         || is_invalid || is_icbi || is_sc) {
            block->len = (address + 4) - block->start;
            block->is_conditional_branch =
                ((is_direct_branch || is_indirect_branch)
                 && !is_unconditional_branch);
            block = NULL;

            /* If this is a non-subroutine (LK=0) direct (not bclr/bcctr)
             * branch and the target address is within our scanning range,
             * add it to the basic block list. */
            if (is_direct_branch && !insn_LK(insn)) {
                const int32_t disp =
                    (opcd == OPCD_B) ? insn_LI(insn) : insn_BD(insn);
                uint32_t target;
                if (insn_AA(insn)) {
                    target = (uint32_t)disp;
                } else {
                    target = address + disp;
                }
                if (target >= start && target <= aligned_limit) {
                    const int target_index = get_block(ctx, target, true);
                    if (UNLIKELY(target_index < 0)) {
                        log_error(ctx->handle, "Out of memory expanding basic"
                                  " block table for branch target 0x%X at"
                                  " 0x%X", target, address);
                        return false;
                    }
                    ctx->blocks[target_index].is_branch_target = true;
                }
            }

            /*
             * Functions typically end with an unconditional branch of some
             * sort, most often blr but possibly a branch to the entry
             * point of some other function (a tail call).  However, a
             * function might have several copies of its epilogue as a
             * result of optimization, so we only treat an unconditional
             * branch (or invalid instruction) as end-of-unit if there are
             * no branch targets we haven't yet reached.
             *
             * Note that we don't check LK here because we currently return
             * from translated code on a subroutine branch.  If we add
             * support for translating LK=1 branches to native calls, those
             * should not be treated as terminal here.
             *
             * icbi and sc instructions always cause a return from
             * translated code, so they can potentially end the unit as well.
             */
            if (is_invalid || is_icbi || is_sc || is_unconditional_branch) {
                ASSERT(ctx->num_blocks > 0);
                if (ctx->blocks[ctx->num_blocks-1].start <= address) {
                    /* No more blocks follow this one, so we've reached
                     * the end of the function. */
                    break;
                }
            }
        }
    }

    if (insn_count < max_insns) {
        ASSERT(!block);
    } else {
        if (block) {
            block->len = (aligned_limit + 4) - block->start;
        }
        if (aligned_limit + 3 >= limit) {
            log_info(ctx->handle, "Scanning terminated at requested limit"
                     " 0x%X", limit);
        } else {
            log_warning(ctx->handle, "Scanning terminated at 0x%X due to code"
                        " range bounds", aligned_limit + 4);
        }
    }

    /* Backward branches may have inserted basic blocks in the block list
     * which were never updated because we had already scanned past them.
     * Look for such blocks and split their predecessors. */
    for (int i = 1; i < ctx->num_blocks; i++) {
        block = &ctx->blocks[i];
        if (block->len == 0) {
            GuestPPCBlockInfo *prev = &ctx->blocks[i-1];
            /* Careful calculation here to correctly handle the case of
             * prev->start + prev->len == 0 (top of address space). */
            if (block->start - prev->start < prev->len) {
                block->len = prev->len - (block->start - prev->start);
                prev->len = block->start - prev->start;
            }
        }
    }

    /* Scan each block to find registers used and changed in the block. */
    for (int i = 0; i < ctx->num_blocks; i++) {
        block = &ctx->blocks[i];
        const uint32_t *block_base = &memory_base[block->start/4];
        for (uint32_t j = 0; j < block->len; j += 4) {
            const uint32_t insn = bswap_be32(block_base[j/4]);
            if (is_valid_insn(insn)) {
                update_used_changed(ctx, block, insn);
            }
        }
    }

    /* If optimizing CR bits, traverse all blocks in control flow order to
     * find which CR bits don't need to be stored. */
    if (ctx->handle->guest_opt & BINREC_OPT_G_PPC_TRIM_CR_STORES) {
        /* Fill in exit edges for each block to generate the control flow
         * graph. */
        for (int i = 0; i < ctx->num_blocks; i++) {
            block = &ctx->blocks[i];
            ASSERT(block->len > 0);
            const uint32_t address = block->start + block->len - 4;
            const uint32_t insn = bswap_be32(memory_base[address/4]);
            if (insn_OPCD(insn) == OPCD_BC) {
                block->next_block =
                    get_block(ctx, block->start + block->len, false);
                const int32_t disp = insn_BD(insn);
                uint32_t target;
                if (insn_AA(insn)) {
                    target = (uint32_t)disp;
                } else {
                    target = address + disp;
                }
                block->branch_block = get_block(ctx, target, false);
            } else if (insn_OPCD(insn) == OPCD_B) {
                const int32_t disp = insn_LI(insn);
                uint32_t target;
                if (insn_AA(insn)) {
                    target = (uint32_t)disp;
                } else {
                    target = address + disp;
                }
                block->branch_block = get_block(ctx, target, false);
            } else if (block->has_trap) {
                /* Treat this as an unconditional branch to the next
                 * instruction, to simplify scan_cr_bits() logic. */
                block->branch_block =
                    get_block(ctx, block->start + block->len, false);
            }
        }

        uint8_t *visited = binrec_malloc(ctx->handle, ctx->num_blocks);
        if (UNLIKELY(!visited)){ 
            log_warning(ctx->handle, "No memory for block visited flags"
                        " (%d bytes), skipping TRIM_CR_STORES optimization",
                        ctx->num_blocks);
        } else {
            memset(visited, 0, ctx->num_blocks);
            scan_cr_bits(ctx, visited, 0);
            binrec_free(ctx->handle, visited);
        }
    }

    return true;
}

/*************************************************************************/
/*************************************************************************/
