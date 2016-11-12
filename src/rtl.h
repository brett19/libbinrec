/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#ifndef SRC_RTL_H
#define SRC_RTL_H

#ifndef SRC_COMMON_H
    #include "src/common.h"
#endif

/*
 * This header defines constants and structures for the RTL (register
 * transfer language) used for translation.  The process of translation
 * consists of generating RTL instructions corresponding to a given
 * sequence of input (guest) machine instructions, optionally optimizing
 * the RTL instructions, then translating the RTL into output (host)
 * machine instructions.
 *
 * In a typical RTL, each instruction performs exactly one operation on a
 * small number of operands (usually one or two, sometimes three), such as
 * loading a value from memory or adding two values together.  With a few
 * exceptions, every operand to an RTL instruction is a register -- hence
 * the name "register transfer language" -- though many of these
 * "registers" are only used for temporary storage of values and will be
 * optimized out in later stages.  This design simplifies the optimization
 * and code generation stages by reducing the number of distinct operations
 * that must be handled.
 *
 * Note that input machine instructions do not always map directly to RTL
 * instructions.  As a somewhat extreme example, the "movsb" instruction
 * in the Intel x86 architecture might translate to:
 *     LOAD      reg1, (state_reg, #esi_offset)
 *     LOAD_U8   reg2, (reg1)
 *     ADDI      reg3, reg1, #1
 *     STORE     (state_reg, #esi_offset), reg3
 *     LOAD      reg4, (state_reg, #edi_offset)
 *     STORE_I8  (reg4), reg2
 *     ADDI      reg5, reg4, #1
 *     STORE     (state_reg, #edi_offset), reg5
 *
 * The RTL used here deviates slightly from a "pure" RTL in that it allows
 * certain operations involving immediate values (notably memory access
 * with immediate offset and integer register-immediate operations, as seen
 * in the example above) to be written as a single instruction.  This is
 * for convenience of translating operations which are common across many
 * architectures, since otherwise the effort to break them down into
 * multiple RTL instructions would be effectively wasted when the output
 * translator converted them back to the original form.
 *
 * RTL generated by input parsers must use SSA (static single assignment)
 * form; attempting to use a register as an output in more than one
 * instruction is invalid, and will result in an error if operand sanity
 * checks are enabled.  Guest instructions which modify (both read and
 * write) an operand should create a second RTL register to hold the result
 * of the instruction.
 *
 * "Alias registers" are an exception to the SSA rule.  These may only be
 * used as the source of a GET_ALIAS instruction or the destination of a
 * SET_ALIAS instruction, but they may be reassigned freely.  This allows
 * them to serve as "aliases" for guest architecture registers, for
 * example, and allows the input translator to defer some data flow
 * analysis to the RTL engine, which will then infer phi functions from
 * the alias assignments.  (Currently, there is no interface for explicitly
 * specifying phi functions.)  Note that alias registers have a separate
 * namespace from ordinary registers, so alias register numbers will not be
 * unique with respect to ordinary register numbers.
 *
 * At a low level, alias registers are effectively aliases for specific
 * memory locations.  These can be specified explicitly by calling
 * rtl_set_alias_storage(), or they can be left to the host translator,
 * which will typically allocate a stack frame slot for the alias.  In the
 * latter case, the guest translator is responsible for setting an initial
 * value with a SET_ALIAS instruction, or subsequent GET_ALIAS instructions
 * will read undefined data.  From the guest translator's point of view,
 * GET_ALIAS and SET_ALIAS instructions can be thought of as equivalent to
 * LOAD and STORE, but the host translator can make use of the data flow
 * information implied by the use of alias registers to omit some of those
 * loads and stores by keeping the value in a hardware register.
 *
 * Within this library, a "unit" or "translation unit" refers to a sequence
 * of guest instructions translated together into a single host code block;
 * the RTLUnit structure encapsulates the state associated with a
 * translation unit.  A "basic block" has the usual meaning of a sequence
 * of instructions which has exactly one entry and one exit point; the
 * RTLBlock structure encapsulates information about a basic block, with
 * one RTLUnit containing an arbitrary number of RTLBlocks.  Each RTLBlock
 * contains a sequence of RTL instructions, each of which is represented
 * internally by an RTLInsn structure.  Note that RTLInsns are stored in a
 * single contiguous array in RTLUnit, and RTLBlocks contain only pointers
 * to (more precisely, array indices of) the first and last instructions in
 * the block.
 */

/*************************************************************************/
/******************** RTL data type and opcode lists *********************/
/*************************************************************************/

/**
 * RTLDataType:  Enumeration of data types for registers used in an RTL
 * unit.  For instructions which take multiple operands, all operands must
 * be of the same type, except where noted.
 *
 * RTLTYPE_ADDRESS is a special integer variant which is the same size as
 * a memory address (i.e., a pointer) on the host architecture.  The
 * ADDRESS type is treated as distinct from other integer types; to perform
 * calculations with an address, the second operand must be cast to address
 * type with an RTLOP_ZCAST or RTLOP_SCAST instruction.
 *
 * RTLTYPE_FPSTATE is a special type reserved for holding the result of an
 * FGETSTATE instruction.  This type can only be used with the FGETSTATE,
 * FSETSTATE, and FTESTEXC instructions, and it cannot be converted to any
 * other type.
 */
typedef enum RTLDataType {
    /* Zero is invalid. */
    RTLTYPE_INT32 = 1,  // 32-bit integer
    RTLTYPE_INT64,      // 64-bit integer
    RTLTYPE_ADDRESS,    // Host-pointer-sized integer
    RTLTYPE_FLOAT32,    // 32-bit floating point
    RTLTYPE_FLOAT64,    // 64-bit floating point
    RTLTYPE_V2_FLOAT32, // Vector of 2 x RTLTYPE_FLOAT32
    RTLTYPE_V2_FLOAT64, // Vector of 2 x RTLTYPE_FLOAT64
    RTLTYPE_FPSTATE,    // Floating point state word
} RTLDataType;

/**
 * RTLOpcode:  Enumeration of operations, used as the value of the
 * RTLInsn.opcode field.
 *
 * Note that in general, computational instructions require that all
 * operands be of the same type; for example, it is invalid to ADD an
 * INT32 register and an ADDRESS register, or to add two INT32 registers
 * and store the result in an ADDRESS register.  Use an appropriate
 * conversion instruction, such as ZCAST or SCAST, to convert between types.
 */
typedef enum RTLOpcode {
    /* Zero is invalid. */

    /* No operation.  For debugging purposes, an immediate value (such as
     * an instruction address) can be given in the "other" operand, which
     * will typically be encoded into a no-op instruction in the output.
     * Registers can also be given in the dest, src1, and src2 fields;
     * these have no effect on code behavior, but they do extend the live
     * range of the registers, which can be useful in forcing certain
     * registers to remain live in tests.  If registers are given, they
     * must obey the usual rules of definedness (dest must be undefined,
     * src1 and src2 must be defined).  Passing a register as the
     * destination of a NOP and then using that register as the source of
     * another (non-NOP) instruction results in undefined behavior. */
    RTLOP_NOP = 1,

    /* Alias register operations. */
    RTLOP_SET_ALIAS,    // alias[other] = src1
                        //    [other is an alias register index]
    RTLOP_GET_ALIAS,    // dest = alias[other]
                        //    [other is an alias register index]

    /* Non-computational operations. */
    RTLOP_MOVE,         // dest = src1
    RTLOP_SELECT,       // dest = other ? src1 : src2
                        //    [other must be of integer type]

    /* Integer operations. */
    RTLOP_SCAST,        // dest = (typeof(dest))(signed)src1
    RTLOP_ZCAST,        // dest = (typeof(dest))(unsigned)src1
    RTLOP_SEXT8,        // dest = (int8_t)src1
    RTLOP_SEXT16,       // dest = (int16_t)src1
                        //    [signed casts with explicit size since we
                        //     don't have 8-bit or 16-bit types]
    RTLOP_ADD,          // dest = src1 + src2
    RTLOP_SUB,          // dest = src1 - src2
    RTLOP_NEG,          // dest = -src1
    RTLOP_MUL,          // dest = src1 * src2
    RTLOP_MULHU,        // dest = ((unsigned)src1 * (unsigned)src2) >> size
                        //    [size = number of bits in value type]
    RTLOP_MULHS,        // dest = ((signed)src1 * (signed)src2) >> size
                        //    [size = number of bits in value type]
    RTLOP_DIVU,         // dest = (unsigned)src1 / (unsigned)src2
                        //    [behavior undefined if src2 == 0]
    RTLOP_DIVS,         // dest = (signed)src1 / (signed)src2
                        //    [behavior undefined if src2 == 0]
    RTLOP_MODU,         // dest = (unsigned)src1 % (unsigned)src2
                        //    [behavior undefined if src2 == 0]
    RTLOP_MODS,         // dest = (signed)src1 % (signed)src2
                        //    [behavior undefined if src2 == 0]
    RTLOP_AND,          // dest = src1 & src2
    RTLOP_OR,           // dest = src1 | src2
    RTLOP_XOR,          // dest = src1 ^ src2
    RTLOP_NOT,          // dest = ~src1
    RTLOP_SLL,          // dest = src1 << src2
                        //    [src2 may be any integer type; result is
                        //     undefined when src2 >= #bits(src1)]
    RTLOP_SRL,          // dest = (unsigned)src1 >> src2
                        //    [src2 may be any integer type; result is
                        //     undefined when src2 >= #bits(src1)]
    RTLOP_SRA,          // dest = (signed)src1 >> src2
                        //    [src2 may be any integer type; result is
                        //     undefined when src2 >= #bits(src1)]
    RTLOP_ROL,          // dest = src1 ROL (src2 % #bits(src1))
                        //    [src2 may be any integer type]
    RTLOP_ROR,          // dest = src1 ROR (src2 % #bits(src1))
                        //    [src2 may be any integer type]
    RTLOP_CLZ,          // dest = [number of leading zeros in src1]
                        //    [dest may be any integer type]
    RTLOP_BSWAP,        // dest = [reverse order of bytes in src1]
    RTLOP_SEQ,          // dest = src1 == src2 ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SLTU,         // dest = (unsigned)src1 < (unsigned)src2 ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SLTS,         // dest = (signed)src1 < (signed)src2 ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SGTU,         // dest = (unsigned)src1 > (unsigned)src2 ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SGTS,         // dest = (signed)src1 > (signed)src2 ? 1 : 0
                        //    [dest may be any integer type]

    /* Integer bitfield operations.  "start" and "count" are encoded in the
     * "other" parameter as: other = start | count<<8 */
    RTLOP_BFEXT,        // dest = (src1 >> start) & ((1<<count) - 1)
    RTLOP_BFINS,        // dest = (src1 & ~(((1<<count) - 1) << start))
                        //      | ((src2 & ((1<<count) - 1)) << start)
                        //    [conceptually, "insert src2 into src1"]

    /* Integer-immediate operations.  For instructions other than
     * shift/rotate operations, if src1 is wider than 32 bits, the
     * immediate value is sign-extended, and an operand error will be
     * generated (if OPERAND_SANITY_CHECKS is enabled) if the 64-bit value
     * of "other" is not in the range of a sign-extended 32-bit value. */
    RTLOP_ADDI,         // dest = src1 + IMMEDIATE(other)
    RTLOP_MULI,         // dest = src1 * IMMEDIATE(other)
    RTLOP_ANDI,         // dest = src1 & IMMEDIATE(other)
    RTLOP_ORI,          // dest = src1 | IMMEDIATE(other)
    RTLOP_XORI,         // dest = src1 ^ IMMEDIATE(other)
    RTLOP_SLLI,         // dest = src1 << IMMEDIATE(other)
                        //    [result is undefined when other >= #bits(src1)]
    RTLOP_SRLI,         // dest = (unsigned)src1 >> IMMEDIATE(other)
                        //    [result is undefined when other >= #bits(src1)]
    RTLOP_SRAI,         // dest = (signed)src1 >> IMMEDIATE(other)
                        //    [result is undefined when other >= #bits(src1)]
    RTLOP_RORI,         // dest = src1 ROR (IMMEDIATE(other) % #bits(src1))
                        //    [to rotate left, use count = #bits(src1) - count]
    RTLOP_SEQI,         // dest = src1 == src2 ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SLTUI,        // dest = (unsigned)src1 < IMMEDIATE(other) ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SLTSI,        // dest = (signed)src1 < IMMEDIATE(other) ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SGTUI,        // dest = (unsigned)src1 > IMMEDIATE(other) ? 1 : 0
                        //    [dest may be any integer type]
    RTLOP_SGTSI,        // dest = (signed)src1 > IMMEDIATE(other) ? 1 : 0
                        //    [dest may be any integer type]

    /* Bit-copy operation between floating-point and integer types.
     * dest and src1 must be of the same size and opposite types:
     * FLOAT32/INT32, INT64/FLOAT64, etc. */
    RTLOP_BITCAST,      // bits(dest) = bits(src1)

    /* Floating-point cast operations.  FCAST is a "pure" cast which
     * preserves SNaNs and does not raise exceptions; FCVT is an
     * arithmetic operations which quiets SNaNs and raises exceptions.
     * Note that a narrowing FCAST may convert an SNaN to an infinity. */
    RTLOP_FCAST,        // dest = raw_convert(typeof(dest), src1)
    RTLOP_FCVT,         // dest = (typeof(dest))src1

    /* Integer-to-FP cast operations. */
    RTLOP_FZCAST,       // dest = (typeof(dest))((unsigned)src1)
    RTLOP_FSCAST,       // dest = (typeof(dest))((signed)src1)

    /* FP-to-integer cast operations.  FROUNDI uses the current rounding
     * mode, while FTRUNCI rounds toward zero.  Negative overflow and
     * NaNs always convert to the minimum signed integer value for the
     * destination type; it is host-defined whether positive overflow
     * converts to the minimum or maximum signed integer value. */
    RTLOP_FROUNDI,      // dest = (signed typeof(dest))round(src1)
    RTLOP_FTRUNCI,      // dest = (signed typeof(dest))trunc(src1)

    /* Floating-point sign bit manipulation instructions.  These do not
     * raise floating-point exceptions and treat NaNs like other values. */
    RTLOP_FNEG,         // dest = -src1
    RTLOP_FABS,         // dest = abs(src1)
    RTLOP_FNABS,        // dest = -abs(src1)

    /* Floating-point arithmetic instructions.  If multiple operands are
     * NaNs, the returned NaN is the first of {src1, src2, src3}; but if
     * the BINREC_OPT_NATIVE_IEEE_NAN optimization is enabled, any input
     * NaN may be returned. */
    RTLOP_FADD,         // dest = src1 + src2
    RTLOP_FSUB,         // dest = src1 - src2
    RTLOP_FMUL,         // dest = src1 * src2
    RTLOP_FDIV,         // dest = src1 / src2
    RTLOP_FSQRT,        // dest = sqrt(src1)
    RTLOP_FCMP,         // dest = fcmp(src1, src2, IMMEDIATE(other))
                        //    [dest must be of integer type; other is an
                        //     immediate comparison type (RTLFCMP_*)]
    RTLOP_FMADD,        // dest = fma(src1, src2, src3)
    RTLOP_FMSUB,        // dest = fma(src1, src2, -src3)
    RTLOP_FNMADD,       // dest = fma(-src1, src2, src3)
    RTLOP_FNMSUB,       // dest = fma(-src1, src2, -src3)

    /* Floating-point state manipulation.  State operands must be of type
     * FPSTATE. */
    RTLOP_FGETSTATE,    // dest = fpstate()
                        //    [reads current host floating-point state]
    RTLOP_FSETSTATE,    // set_fpstate(src1)
                        //    [sets host floating-point state to src1]
    RTLOP_FTESTEXC,     // dest = fpstate_has_exception(src1, IMMEDIATE(other))
                        //    [other is one of RTLFEXC_*; dest must be of
                        //     integer type and receives 1 or 0]
    RTLOP_FCLEAREXC,    // dest = fpstate_clear_exceptions(src1)
    RTLOP_FSETROUND,    // dest = fpstate_set_round(src1, IMMEDIATE(other))
                        //    [other is one of RTLFROUND_*]
    RTLOP_FCOPYROUND,   // dest = fpstate_copy_round(src1, src2)
                        //    [copies rounding mode from src2 into src1]

    /* Vector manipulation instructions. */
    RTLOP_VBUILD2,      // dest = {src1, src2}
                        //    [dest must be a 2-element vector of typeof(src1)]
    RTLOP_VBROADCAST,   // dest[*] = src1
    RTLOP_VEXTRACT,     // dest = src1[other]
    RTLOP_VINSERT,      // dest = src1; dest[other] = src2
    RTLOP_VFCAST,       // dest[*] = (typeof(dest[*]))src1[*]
                        //    [does not quiet SNaNs or raise exceptions]
    RTLOP_VFCVT,        // dest[*] = (typeof(dest[*]))src1[*]
                        //    [arithmetic conversion, can raise exceptions;
                        //     dest and src1 must be different types]

    /* Non-memory load operations.  If LOAD_ARG instructions are used,
     * they must be the first instructions in the unit, or the argument
     * values may be overwritten. */
    RTLOP_LOAD_IMM,     // dest = IMMEDIATE(other)
                        //    [dest must be a scalar type; for floating-point
                        //     types, other is assumed to be in IEEE format]
    RTLOP_LOAD_ARG,     // dest = ARG(other)
                        //    [other is a function argument index; only
                        //     integer arguments are supported]

    /* Memory load and store operations.  Address operands must be of
     * ADDRESS type, offsets must be in [-32768,+32767], and for 8/16-bit
     * loads, the value operand must be of INT32 type.  For vector load and
     * store operations, the first element of the vector is loaded from or
     * stored to the lowest address in memory.  Behavior is undefined if
     * the address is not a multiple of the value size. */
    RTLOP_LOAD,         // dest = *(typeof(dest) *)(src1 + other)
    RTLOP_LOAD_U8,      // dest = *(uint8_t *)(src1 + other)
    RTLOP_LOAD_S8,      // dest = *(int8_t *)(src1 + other)
    RTLOP_LOAD_U16,     // dest = *(uint16_t *)(src1 + other)
    RTLOP_LOAD_S16,     // dest = *(int16_t *)(src1 + other)
    RTLOP_STORE,        // *(typeof(src2) *)(src1 + other) = src2
    RTLOP_STORE_I8,     // *(uint8_t *)(src1 + other) = src2
    RTLOP_STORE_I16,    // *(uint16_t *)(src1 + other) = src2
    /* These versions (_BR = "Byte-Reversed") swap the byte order of the
     * value operand after loading or before storing.  Load/store byte
     * order is interpreted with respect to the host architecture; the
     * guest translator is responsible for choosing between the normal and
     * byte-reversed instructions based on the host_little_endian flag in
     * binrec_t. */
    RTLOP_LOAD_BR,      // dest = bswap(*(typeof(dest) *)(src1 + other))
    RTLOP_LOAD_U16_BR,  // dest = bswap16(*(uint16_t *)(src1 + other))
    RTLOP_LOAD_S16_BR,  // dest = bswap16(*(int16_t *)(src1 + other))
    RTLOP_STORE_BR,     // *(typeof(src2) *)(src1 + other) = bswap(src2)
    RTLOP_STORE_I16_BR, // *(uint16_t *)(src1 + other) = bswap16(src2)

    /* Integer atomic operations.  Address operands (src1) must be of
     * ADDRESS type; other operands may be of any integral type, but (for
     * CMPXCHG) they must all match. */
    RTLOP_ATOMIC_INC,   // dest = (*src1)++
    RTLOP_CMPXCHG,      // dest = (*src1==src2 ? (*src1 = other, src2) : *src1)

    /* Branch operations ("other" is a label number). */
    RTLOP_LABEL,        // LABEL(other):
    RTLOP_GOTO,         // goto LABEL(other)
    RTLOP_GOTO_IF_Z,    // if (src1 == 0) goto LABEL(other)
    RTLOP_GOTO_IF_NZ,   // if (src1 != 0) goto LABEL(other)

    /* Call to host address.  src1 must be of ADDRESS type. dest, src2, and
     * other are optional, but must be integer type if given; if src2 is
     * omitted, other must also be omitted. */
    RTLOP_CALL,         // dest = (*src1)(src2, other)
    /* Call to host address.  Functionally identical to CALL, but host
     * translators should avoid effects on translation state as much as
     * possible (for example, caller-saved registers should not be avoided
     * even for registers live across a CALL_TRANSPARENT instruction).
     * This is intended for the pre- and post-instruction callbacks, so
     * that using them to debug possible library bugs has only minimal
     * effect on the generated code. */
    RTLOP_CALL_TRANSPARENT,  // dest = (*src1)(src2, other)

    /* Terminator for generated code (returns to caller).  The return value
     * is optional. */
    RTLOP_RETURN,       // return src1

    /* Explicit illegal instruction; will trigger an illegal-instruction
     * exception if executed on the host. */
    RTLOP_ILLEGAL,

} RTLOpcode;

#define RTLOP__FIRST  RTLOP_NOP
#define RTLOP__LAST   RTLOP_ILLEGAL

/**
 * RTLFloatCompare:  Enumeration of comparison types for use with the FCMP
 * instruction.  The comparison operand is one of the values from this
 * enumeration possibly OR'd with the RTLFCMP_INVERT and RTLFCMP_ORDERED
 * flags.
 */
typedef enum RTLFloatCompare {
    RTLFCMP_LT = 0,
    RTLFCMP_LE = 1,
    RTLFCMP_GT = 2,
    RTLFCMP_GE = 3,
    RTLFCMP_EQ = 4,
    RTLFCMP_UN = 5,
} RTLFloatCompare;
enum {
    RTLFCMP_INVERT = 8,  // Invert the sense of the comparison.
    RTLFCMP_NLT = RTLFCMP_INVERT | RTLFCMP_LT,
    RTLFCMP_NLE = RTLFCMP_INVERT | RTLFCMP_LE,
    RTLFCMP_NGT = RTLFCMP_INVERT | RTLFCMP_GT,
    RTLFCMP_NGE = RTLFCMP_INVERT | RTLFCMP_GE,
    RTLFCMP_NEQ = RTLFCMP_INVERT | RTLFCMP_EQ,
    RTLFCMP_NUN = RTLFCMP_INVERT | RTLFCMP_UN,

    RTLFCMP_ORDERED = 16,  // Raise an invalid operation exception on QNaNs.
    RTLFCMP_OLT = RTLFCMP_ORDERED | RTLFCMP_LT,
    RTLFCMP_OLE = RTLFCMP_ORDERED | RTLFCMP_LE,
    RTLFCMP_OGT = RTLFCMP_ORDERED | RTLFCMP_GT,
    RTLFCMP_OGE = RTLFCMP_ORDERED | RTLFCMP_GE,
    RTLFCMP_OEQ = RTLFCMP_ORDERED | RTLFCMP_EQ,
    RTLFCMP_OUN = RTLFCMP_ORDERED | RTLFCMP_UN,
    RTLFCMP_ONLT = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_LT,
    RTLFCMP_ONLE = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_LE,
    RTLFCMP_ONGT = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_GT,
    RTLFCMP_ONGE = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_GE,
    RTLFCMP_ONEQ = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_EQ,
    RTLFCMP_ONUN = RTLFCMP_ORDERED | RTLFCMP_INVERT | RTLFCMP_UN,
};

/**
 * RTLFloatException:  Enumeration of floating-point exceptions for use
 * with the FTESTEXC instruction.
 */
typedef enum RTLFloatException {
    RTLFEXC_INEXACT = 0,
    RTLFEXC_INVALID,
    RTLFEXC_OVERFLOW,
    RTLFEXC_UNDERFLOW,
    RTLFEXC_ZERO_DIVIDE,
} RTLFloatException;

/**
 * RTLFloatRoundingMode:  Enumeration of floating-point rouding modes for
 * use with the FSETROUND instruction.
 */
typedef enum RTLFloatRoundingMode {
    RTLFROUND_NEAREST = 0,
    RTLFROUND_TRUNC,
    RTLFROUND_FLOOR,
    RTLFROUND_CEIL,
} RTLFloatRoundingMode;

/*************************************************************************/
/************************** RTL data structures **************************/
/*************************************************************************/

/**
 * RTLUnit:  State information used in translating a unit of code.
 * Opaque to callers.
 */
typedef struct RTLUnit RTLUnit;

/*************************************************************************/
/*********************** RTL translation interface ***********************/
/*************************************************************************/

/**
 * rtl_create_unit:  Create a new RTLUnit structure for translation.
 *
 * [Parameters]
 *     handle: Translation handle this until will be associated with.
 * [Return value]
 *     New unit, or NULL on error.
 */
#define rtl_create_unit INTERNAL(rtl_create_unit)
extern RTLUnit *rtl_create_unit(binrec_t *handle);

/**
 * rtl_clear_unit:  Clear all data from an RTLUnit structure.  After
 * calling this function, the unit is in the same state as a unit returned
 * from rtl_create_unit().
 *
 * [Parameters]
 *     unit: RTLUnit to clear.
 */
#define rtl_clear_unit INTERNAL(rtl_clear_unit)
extern void rtl_clear_unit(RTLUnit *unit);

/**
 * rtl_get_error_state:  Return whether any previous "add" or "alloc"
 * operation (rtl_add_insn(), rtl_alloc_register(), etc.) has failed.
 * This is effectively a NAND operation across the return values of all
 * such functions, and can be used to avoid individual checks on every
 * return value.
 *
 * [Parameters]
 *     unit: RTLUnit to check.
 * [Return value]
 *     True if an error has occurred, false if not.
 */
#define rtl_get_error_state INTERNAL(rtl_get_error_state)
extern bool rtl_get_error_state(RTLUnit *unit);

/**
 * rtl_clear_error_state:  Clear (reset to false) the state of the error
 * flag returned by rtl_get_error_state().
 *
 * [Parameters]
 *     unit: RTLUnit to operate on.
 */
#define rtl_clear_error_state INTERNAL(rtl_clear_error_state)
extern void rtl_clear_error_state(RTLUnit *unit);

/**
 * rtl_add_insn:  Append an instruction to the given unit.  The meaning of
 * each operand depends on the instruction.
 *
 * [Parameters]
 *     unit: RTLUnit to append to.
 *     opcode: Instruction opcode (RTLOP_*).
 *     dest: Destination register for instruction.
 *     src1: First source register for instruction.
 *     src2: Second source register for instruction.
 *     other: Extra register or immediate value for instruction.
 * [Return value]
 *     True on success, false on error.
 */
#define rtl_add_insn INTERNAL(rtl_add_insn)
extern bool rtl_add_insn(RTLUnit *unit, RTLOpcode opcode,
                         int dest, int src1, int src2, uint64_t other);

/**
 * rtl_alloc_register:  Allocate a new register for use in the given unit.
 * The register's value is undefined until it has been used as the
 * destination of an instruction.
 *
 * Register numbers will always fit in a uint16_t; they are passed and
 * returned as int to reduce unnecessary size casts in the compiled code.
 *
 * [Parameters]
 *     unit: RTLUnit to allocate a register for.
 *     type: Data type for register (RTLTYPE_*).
 * [Return value]
 *     Register number (positive) on success, zero on error.
 */
#define rtl_alloc_register INTERNAL(rtl_alloc_register)
extern int rtl_alloc_register(RTLUnit *unit, RTLDataType type);

/**
 * rtl_make_unfoldable:  Prevent the given register from being used in
 * constant folding, even if it would otherwise be foldable.  This
 * function must be called after adding the instruction that sets the
 * register.
 *
 * [Parameters]
 *     unit: RTLUnit containing register to mark.
 *     reg: Register number to mark.
 */
#define rtl_make_unfoldable INTERNAL(rtl_make_unfoldable)
extern void rtl_make_unfoldable(RTLUnit *unit, int reg);

/**
 * rtl_make_unspillable:  Prevent the given register from being spilled to
 * memory.
 *
 * Calling this function on too many registers may cause host register
 * allocation to fail.
 *
 * [Parameters]
 *     unit: RTLUnit containing register to mark.
 *     reg: Register number to mark.
 */
#define rtl_make_unspillable INTERNAL(rtl_make_unspillable)
extern void rtl_make_unspillable(RTLUnit *unit, int reg);

/**
 * rtl_make_unique_pointer:  Mark the given register as being a "unique
 * pointer", which points to a region of memory which will never be accessed
 * except through this register (or another register copied from it).
 *
 * [Parameters]
 *     unit: RTLUnit containing register to mark.
 *     reg: Register number to mark.
 */
#define rtl_make_unique_pointer INTERNAL(rtl_make_unique_pointer)
extern void rtl_make_unique_pointer(RTLUnit *unit, int reg);

/**
 * rtl_alloc_alias_register:  Allocate a new register which will serve as
 * an alias for another register.  Alias registers may only be used as the
 * source or target of a MOVE instruction, but they may be reassigned freely.
 *
 * Alias register numbers will always fit in a uint16_t; they are passed and
 * returned as int to reduce unnecessary size casts in the compiled code.
 *
 * [Parameters]
 *     unit: RTLUnit to allocate an alias register for.
 *     type: Data type for register (RTLTYPE_*).
 * [Return value]
 *     Alias register number (positive) on success, zero on error.
 */
#define rtl_alloc_alias_register INTERNAL(rtl_alloc_alias_register)
extern int rtl_alloc_alias_register(RTLUnit *unit, RTLDataType type);

/**
 * rtl_set_alias_storage:  Define the storage location for an alias register.
 *
 * Binding a memory location to an alias register with this function has
 * the following implications:
 *
 * - Reading the alias with a GET_ALIAS instruction will implicitly load
 *   the value stored at that memory location if no preceding SET_ALIAS
 *   instruction has set the alias's value.
 *
 * - Writing the alias with a SET_ALIAS instruction will cause the value to
 *   be stored to that memory location at some point before control leaves
 *   the generated code, though the store may be omitted if a subsequent
 *   SET_ALIAS instruction also modifies the same alias.
 *
 * - The base register for the alias will never be spilled to memory (as
 *   if rtl_make_unspillable() had been called on the register).
 *
 * If a memory location is not bound to an alias register, reading the
 * alias before storing a value in it returns undefined data, and any value
 * stored in the alias when the generated code returns to its caller is
 * discarded.
 *
 * Calling this function on an alias which has already been used in a
 * GET_ALIAS or SET_ALIAS instruction results in undefined behavior.
 *
 * The number of distinct base registers used for aliases should be kept to
 * a minimum; host code generators may fail at register allocation if too
 * many base registers are used.
 *
 * [Parameters]
 *     unit: RTLUnit to operate on.
 *     alias: Alias register to define the storage location for.
 *     base: RTL register containing base address for memory access (must be
 *         nonzero and of type RTLTYPE_ADDRESS).
 *     offset: Byte offset for memory access (must be within [-32768,+32767]).
 */
#define rtl_set_alias_storage INTERNAL(rtl_set_alias_storage)
extern void rtl_set_alias_storage(RTLUnit *unit, int alias, int base,
                                  int16_t offset);

/**
 * rtl_alloc_label:  Allocate a new label for use in the given unit.
 *
 * Label numbers will always fit in a uint16_t; they are passed and
 * returned as int to reduce unnecessary size casts in the compiled code.
 *
 * [Parameters]
 *     unit: RTLUnit to allocate a label for.
 * [Return value]
 *     Label number (positive) on success, zero on error.
 */
#define rtl_alloc_label INTERNAL(rtl_alloc_label)
extern int rtl_alloc_label(RTLUnit *unit);

/**
 * rtl_finalize_unit:  Perform housekeeping at the end of the given unit's
 * translation.  rtl_add_insn(), rtl_alloc_register(), and rtl_alloc_label()
 * may not be called for a unit after calling this function on the unit.
 *
 * [Parameters]
 *     unit: RTLUnit to finalize.
 * [Return value]
 *     True on success, false on error.
 */
#define rtl_finalize_unit INTERNAL(rtl_finalize_unit)
extern bool rtl_finalize_unit(RTLUnit *unit);

/**
 * rtl_optimize_unit:  Perform target-independent optimization on the
 * given unit.  Before calling this function, rtl_finalize_unit() must be
 * called for the unit.
 *
 * [Parameters]
 *     unit: RTLUnit to optimize.
 *     flags: BINREC_OPT_* flags indicating which optimizations to perform.
 * [Return value]
 *     True on success, false on error.
 */
#define rtl_optimize_unit INTERNAL(rtl_optimize_unit)
extern bool rtl_optimize_unit(RTLUnit *unit, unsigned int flags);

/**
 * rtl_disassemble_unit:  Return a string containing a disassembled version
 * of the RTL code in the given unit.  The returned string pointer remains
 * valid until the next call to rtl_disassemble_unit() on the same unit, or
 * until the unit is destroyed.
 *
 * [Parameters]
 *     unit: RTLUnit to disassemble.
 *     verbose: True to include a description of each source register after
 *         each instruction, false to omit register descriptions.
 * [Return value]
 *     String containing the disassembly, or NULL on error.
 */
#define rtl_disassemble_unit INTERNAL(rtl_disassemble_unit)
extern const char *rtl_disassemble_unit(RTLUnit *unit, bool verbose);

/**
 * rtl_destroy_unit:  Destroy the given unit, freeing any resources it used.
 *
 * [Parameters]
 *     unit: RTLUnit to destroy (may be NULL).
 */
#define rtl_destroy_unit INTERNAL(rtl_destroy_unit)
extern void rtl_destroy_unit(RTLUnit *unit);

/*************************************************************************/
/*************************************************************************/

#endif  // RTL_H
