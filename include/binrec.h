/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#ifndef BINREC_H
#define BINREC_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*
 * In this header (and the library source code in general), "guest" refers
 * to the source CPU or architecture, i.e. the input to the translator, and
 * "host" or "native" refers to the target CPU or architecture, i.e. the
 * output of the translator.
 *
 * Note that the numeric values of constants in this header are not part of
 * the public API; always use the symbolic names rather than the value.
 */

/*************************************************************************/
/*********************** Data types and constants ************************/
/*************************************************************************/

/*----------------------------- Basic types -----------------------------*/

/**
 * binrec_t:  Type of a translation handle.  This handle stores global
 * translation settings, such as optimization flags and functions to use
 * for memory allocation.
 */
typedef struct binrec_t binrec_t;

/**
 * binrec_entry_t:  Function type of the native code blocks generated by
 * binrec_translate().
 *
 * [Parameters]
 *     state: Processor state block.  The state block structure must
 *         conform to the structure offsets specified in the setup data
 *         passed to binrec_create_handle().
 */
typedef void (*binrec_entry_t)(void *state);

/**
 * binrec_loglevel_t:  Enumeration of log levels which can be passed to
 * the log function specified in binrec_setup_t.
 */
typedef enum binrec_loglevel_t {
    BINREC_LOGLEVEL_INFO,    // Informational messages.
    BINREC_LOGLEVEL_WARNING, // Messages indicating a potential problem.
    BINREC_LOGLEVEL_ERROR,   // Messages indicating failure of some operation.
} binrec_loglevel_t;

/*--------------------------- Setup structure ---------------------------*/

/**
 * binrec_setup_t:  Structure which defines various parameters used by the
 * translator.  Used by binrec_create_handle().
 */
typedef struct binrec_setup_t {

    /**
     * memory_base:  Pointer to a region of host memory reserved as the
     * address space of the guest code.  binrec_translate() calls will read
     * source machine instructions from this region, and generated native
     * machine code will perform loads and stores by adding the target
     * address to this pointer.  Client code is responsible for mapping the
     * parts of this region corresponding to valid guest memory and for
     * handling any invalid-access exceptions which occur during execution
     * (due to code reading from an invalid address).
     */
    void *memory_base;

    /**
     * state_offset_*:  Offsets from the beginning of the processor state
     * block (as passed to the generated native code) to the various guest
     * registers and other processor state.  Each block of registers is
     * assumed to be contiguous; for example, GPR 1 is accessed by loading
     * a 32-bit value from state_offset_gpr + 4.  All multi-byte values are
     * assumed to be stored in host endian order.
     * FIXME: Generalize for non-PPC guests.
     */
    /* General-purpose registers (32 * uint32_t) */
    int state_offset_gpr;
    /* Floating-point registers (32 * double[2]) */
    int state_offset_fpr;
    /* Paired-single quantization registers (8 * uint32_t) */
    int state_offset_gqr;
    /* Miscellaneous registers (each uint32_t) */
    int state_offset_lr;
    int state_offset_ctr;
    int state_offset_cr;
    int state_offset_xer;
    int state_offset_fpscr;
    /* lwarx/stwcx reservation state flag (uint8_t) */
    int state_offset_reserve_flag;
    /* lwarx/stwcx reservation address (uint32_t) */
    int state_offset_reserve_address;

    /**
     * userdata:  Opaque pointer which is passed to all callback functions.
     */
    void *userdata;

    /**
     * native_malloc:  Pointer to a function which allocates a block of
     * memory for native code.  If NULL, malloc() will be used and
     * alignment will be performed internally.
     *
     * [Parameters]
     *     userdata: User data pointer from setup structure.
     *     size: Size of block to allocate, in bytes.
     *     alignment: Required address alignment, in bytes (guaranteed to
     *         be a power of 2).
     * [Return value]
     *     Pointer to allocated memory, or NULL on error.
     */
    void *(*native_malloc)(void *userdata, size_t size, size_t alignment);

    /**
     * native_realloc:  Pointer to a function which resizes a block of
     * memory allocated with the native_malloc function.  If NULL,
     * realloc() will be used.
     *
     * [Parameters]
     *     userdata: User data pointer from setup structure.
     *     ptr: Pointer to block to resize (guaranteed to be non-NULL).
     *     old_size: Current size of block, in bytes.
     *     new_size: New size of block, in bytes.
     *     alignment: Required address alignment, in bytes (guaranteed to
     *         be equal to the value used for initial allocation).
     * [Return value]
     *     Pointer to allocated memory, or NULL on error.
     */
    void *(*native_realloc)(void *userdata, void *ptr, size_t old_size,
                            size_t new_size, size_t alignment);

    /**
     * native_free:  Pointer to a function which frees a block of memory
     * allocated with the native_malloc function.  If NULL, free() will be
     * used.
     *
     * [Parameters]
     *     userdata: User data pointer from setup structure.
     *     ptr: Pointer to block to free (may be NULL).
     */
    void (*native_free)(void *userdata, void *ptr);

    /**
     * log:  Pointer to a function to log messages from the library.
     * If NULL, no logging will be performed.
     *
     * [Parameters]
     *     userdata: User data pointer from setup structure.
     *     level: Log level (BINREC_LOGLEVEL_*).
     *     message: Log message.
     */
    void (*log)(void *userdata, binrec_loglevel_t level, const char *message);

} binrec_setup_t;

/*------------------------- Optimization flags --------------------------*/

/**
 * BINREC_OPT_ENABLE:  Enable optimization of translated code.  If this
 * flag is not set, all other optimization flags are ignored.
 *
 * When optimization is enabled, the following transformations are always
 * performed:
 *
 * - Load instructions which reverence memory areas marked read-only
 *   (see binrec_add_readonly_region()) are replaced by load-immediate
 *   instructions using the value read from memory.
 *
 * - Dead stores (assignments to registers which are never referenced)
 *   are eliminated from the translated code.
 */
#define BINREC_OPT_ENABLE  (1<<0)

/**
 * BINREC_OPT_DROP_DEAD_BLOCKS:  Eliminate unreachable basic blocks from
 * the code stream.  This is most useful in conjunction with deconditioning.
 */
#define BINREC_OPT_DROP_DEAD_BLOCKS  (1<<1)

/**
 * BINREC_OPT_DROP_DEAD_BRANCHES:  Eliminate branches to the following
 * instruction.
 */
#define BINREC_OPT_DROP_DEAD_BRANCHES  (1<<2)

/**
 * BINREC_OPT_DECONDITION:  Convert conditional branches with constant
 * conditions to unconditional branches or NOPs.  This is most useful in
 * conjunction with constant folding.
 */
#define BINREC_OPT_DECONDITION  (1<<3)

/**
 * BINREC_OPT_FOLD_CONSTANTS:  Look for computations whose operands are all
 * constant, and convert them to load-immediate operations.  The computed
 * values are themselves treated as constant, so constantness can be
 * propagated through multiple instructions; intermediate values in a
 * computation sequence which end up being unused due to constant folding
 * will be removed via dead store elimination.
 */
#define BINREC_OPT_FOLD_CONSTANTS  (1<<4)

/**
 * BINREC_OPT_NATIVE_CALLS:  Treat subroutine-call instructions (like x86
 * CALL or PowerPC BL) as instructions with side effects rather than
 * branches.
 *
 * This optimization can significantly improve performance of non-leaf
 * functions by allowing larger parts of the function to be translated as
 * a single unit.
 *
 * While this optimization is "safe" in the sense that the translated code
 * will always behave correctly, code which uses call instructions in
 * nonstandard ways (such as a call to the next instruction to obtain the
 * instruction's address) can potentially cause a host stack overflow if
 * executed too often.
 */
#define BINREC_OPT_NATIVE_CALLS  (1<<5)

/*************************************************************************/
/**************** Interface: Library version information *****************/
/*************************************************************************/

/**
 * binrec_version:  Return the version number of the library as a string
 * (for example, "1.2.3").
 *
 * [Return value]
 *     Library version number.
 */
extern const char *binrec_version(void);

/**
 * binrec_create_handle:  Create a new translation handle.
 *
 * [Parameters]
 *     setup: Pointer to a binrec_setup_t structure that defines
 * [Return value]
 *     Newly created handle, or NULL on error.
 */
extern binrec_t *binrec_create_handle(const binrec_setup_t *setup);

/**
 * binrec_destroy_handle:  Destroy a translation handle.
 *
 * This function only destroys the translation handle itself; blocks of
 * translated code generated through the handle remain valid even after
 * the handle is destroyed.
 *
 * [Parameters]
 *     handle: Handle to destroy (may be NULL).
 */
extern void binrec_destroy_handle(binrec_t *handle);

/**
 * binrec_set_code_range:  Set the minimum and maximum addresses from which
 * to read source machine instructions.  Branch instructions which attempt
 * to jump outside this range will terminate the translation unit, and if
 * the source machine code runs off the end of the range, the unit will be
 * terminated at the final instruction completely contained within the range.
 *
 * [Parameters]
 *     handle: Handle to operate on.
 *     start: Start address of code range.
 *     length: Length of code range, in bytes.
 */
extern void binrec_set_code_range(binrec_t *handle, uint32_t start,
                                  uint32_t length);

/**
 * binrec_set_optimization_flags:  Set which optimizations should be
 * performed on translated blocks.  Enabling more optimizations will
 * improve the performance of translated code but increase the overhead
 * of translation; see the documentation on each optimization flag for
 * details.
 *
 * The set of enabled optimizations may be changed at any time without
 * impacting the behavior of already-translated blocks.
 *
 * By default, no optimizations are enabled.
 *
 * [Parameters]
 *     handle: Handle to operate on.
 *     flags: Bitmask of optimizations to apply (BINREC_OPT_*).
 */
extern void binrec_set_optimization_flags(binrec_t *handle, unsigned int flags);

/**
 * binrec_set_max_inline_length:  Set the maximum length (number of source
 * instructions, including the final return instruction) of subroutines to
 * inline.  The default is zero, meaning no subroutines will be inlined.
 *
 * If a nonzero length limit is set with this function, then when the
 * translator encounters a subroutine call instruction to a fixed address,
 * it will scan ahead up to this many instructions for a return
 * instruction.  If one is found, and if there are no branch instructions
 * that branch past the return, the subroutine will be inlined into the
 * current translation unit, saving the cost of jumping to a different
 * unit (which can be significant depending on how many guest registers
 * need to be spilled).
 *
 * If an inlined subroutine contains a further call instruction, that
 * subroutine will not be inlined regardless of its length.  (But see
 * binrec_set_max_inline_depth() to enable such recursive inlining.)
 *
 * Note that if a nonzero length limit is set, inlining may be performed
 * regardless of whether any optimization flags are set.
 *
 * [Parameters]
 *     handle: Handle to operate on.
 *     length: Maximum inline length (must be at least 0).
 */
extern void binrec_set_max_inline_length(binrec_t *handle, int length);

/**
 * binrec_set_max_inline_depth:  Set the maximum depth of subroutines to
 * inline.  The default is 1.
 *
 * If a depth limit greater than 1 is set with this function, then when a
 * call instruction is encountered during inlining, the translator will
 * perform the same inlining check on the called subroutine, up to the
 * specified depth.  For example, when translating at A in the following
 * pseudocode:
 *     A: call B
 *        ret
 *     B: call C
 *        ret
 *     C: call D
 *        ret
 *     D: call E
 *        ret
 *     E: nop
 *        ret
 * if the maximum inline depth is set to 2 (and assuming the maximum length
 * is set to at least 2), both B and C will be inlined, but D will not, and
 * the A routine will be translated as if it was written:
 *     A: call D
 *        ret
 *
 * [Parameters]
 *     handle: Handle to operate on.
 *     depth: Maximum inline depth (must be at least 1).
 */
extern void binrec_set_max_inline_depth(binrec_t *handle, int depth);

/**
 * binrec_add_readonly_region:  Mark the given region of memory as
 * read-only.  Instructions which are known to load from read-only memory
 * will be translated into load-constant operations if optimizations are
 * enabled (with the BINREC_OPT_ENABLE flag).
 *
 * This function may fail if too many misaligned regions are added; in
 * that case, rebuild the library with different values of the
 * READONLY_PAGE_BITS and MAX_PARTIAL_READONLY constants in src/common.h.
 *
 * [Parameters]
 *     handle: Handle to operate on.
 *     base: Base address (in guest memory) of read-only region.
 *     size: Size of read-only region, in bytes.
 * [Return value]
 *     True on success; false if the region could not be added because
 *     the partial-page table is full.
 */
extern int binrec_add_readonly_region(binrec_t *handle,
                                      uint32_t base, uint32_t size);

/**
 * binrec_clear_readonly_regions:  Clear all read-only memory regions
 * added with binrec_add_readonly_region().
 *
 * [Parameters]
 *     handle: Handle to operate on.
 */
extern void binrec_clear_readonly_regions(binrec_t *handle);

/**
 * binrec_translate:  Translate a block of guest machine code into native
 * machine code.  On success, the returned block can be executed by calling
 * it as a function.
 *
 * Return-value arguments are only modified on a successful return.
 *
 * [Parameters]
 *     handle: Handle to use for translation.
 *     address: Address (in guest memory) of first instruction to translate.
 *     src_length_ret: Pointer to variable to receive the length of the
 *         block of source machine code which was translated, in bytes.
 *     native_code_ret: Pointer to variable to receive a pointer to the
 *         translated (native) machine code.
 *     native_size_ret: Pointer to variable to receive the length of the
 *         translated machine code, in bytes.
 * [Return value]
 *     True (nonzero) on success, false (zero) on error.
 */
extern int binrec_translate(
    binrec_t *handle, uint32_t address, uint32_t *src_length_ret,
    binrec_entry_t *native_code_ret, size_t *native_size_ret);

/*************************************************************************/
/*************************************************************************/

#ifdef __cplusplus
}  // extern "C"
#endif

#endif  // BINREC_H
