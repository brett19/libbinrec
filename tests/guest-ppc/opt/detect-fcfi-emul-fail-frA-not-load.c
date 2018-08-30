/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "tests/guest-ppc/insn/common.h"

#define ADD_READONLY_REGION

static const uint8_t input[] = {
    0x94,0x21,0xFF,0xF0,  // stwu r1,-16(r1)
    0x3C,0x00,0x43,0x30,  // lis r0,0x4330
    0x90,0x01,0x00,0x08,  // stw r0,8(r1)
    0x90,0x61,0x00,0x0C,  // stw r3,12(r1)
    0xFC,0x40,0x10,0x50,  // fneg f2,f2
    0x3C,0x60,0x00,0x00,  // lis r3,0
    0xC8,0x63,0x00,0x28,  // lfd f3,const(r3)
    0xFC,0x22,0x18,0x28,  // fsub f1,f2,f3
    0x38,0x21,0x00,0x10,  // addi r1,r1,16
    0x4E,0x80,0x00,0x20,  // blr
    0x43,0x30,0x00,0x00,  // const: .int64 0x4330000000000000
      0x00,0x00,0x00,0x00,
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_DETECT_FCFI_EMUL
                                    | BINREC_OPT_G_PPC_NO_FPSCR_STATE;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    #ifdef RTL_DEBUG_OPTIMIZE
        "[info] Failed to optimize possible fcfi at 0x1C: frA is not the result of an lfd\n"
        "[info] Killing instruction 7\n"
    #endif
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: ADDI       r4, r3, -16\n"
    "    4: ZCAST      r5, r4\n"
    "    5: ADD        r6, r2, r5\n"
    "    6: STORE_BR   0(r6), r3\n"
    "    7: NOP\n"
    "    8: LOAD_IMM   r7, 0x43300000\n"
    "    9: SET_ALIAS  a2, r7\n"
    "   10: ZCAST      r8, r4\n"
    "   11: ADD        r9, r2, r8\n"
    "   12: STORE_BR   8(r9), r7\n"
    "   13: ZCAST      r10, r4\n"
    "   14: ADD        r11, r2, r10\n"
    "   15: GET_ALIAS  r12, a4\n"
    "   16: STORE_BR   12(r11), r12\n"
    "   17: GET_ALIAS  r13, a6\n"
    "   18: FNEG       r14, r13\n"
    "   19: LOAD_IMM   r15, 0\n"
    "   20: SET_ALIAS  a4, r15\n"
    "   21: ZCAST      r16, r15\n"
    "   22: ADD        r17, r2, r16\n"
    "   23: LOAD_BR    r18, 40(r17)\n"
    "   24: FSUB       r19, r14, r18\n"
    "   25: ADDI       r20, r4, 16\n"
    "   26: SET_ALIAS  a3, r20\n"
    "   27: SET_ALIAS  a5, r19\n"
    "   28: SET_ALIAS  a6, r14\n"
    "   29: SET_ALIAS  a7, r18\n"
    "   30: GET_ALIAS  r21, a8\n"
    "   31: ANDI       r22, r21, -4\n"
    "   32: SET_ALIAS  a1, r22\n"
    "   33: GOTO       L1\n"
    "   34: LOAD_IMM   r23, 40\n"
    "   35: SET_ALIAS  a1, r23\n"
    "   36: LABEL      L1\n"
    "   37: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: int32 @ 256(r1)\n"
    "Alias 3: int32 @ 260(r1)\n"
    "Alias 4: int32 @ 268(r1)\n"
    "Alias 5: float64 @ 400(r1)\n"
    "Alias 6: float64 @ 416(r1)\n"
    "Alias 7: float64 @ 432(r1)\n"
    "Alias 8: int32 @ 932(r1)\n"
    "\n"
    "Block 0: <none> --> [0,33] --> 2\n"
    "Block 1: <none> --> [34,35] --> 2\n"
    "Block 2: 1,0 --> [36,37] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"