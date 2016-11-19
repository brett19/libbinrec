/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "tests/guest-ppc/insn/common.h"

static const uint8_t input[] = {
    0xFC,0x22,0x18,0x2A,  // fadd f1,f2,f3
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_IGNORE_FPSCR_VXFOO;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a5\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a6, r4\n"
    "    5: GET_ALIAS  r5, a3\n"
    "    6: GET_ALIAS  r6, a4\n"
    "    7: FADD       r7, r5, r6\n"
    "    8: LOAD_IMM   r8, 0x20000000000000\n"
    "    9: BITCAST    r9, r7\n"
    "   10: SLLI       r10, r9, 1\n"
    "   11: SEQ        r11, r10, r8\n"
    "   12: GOTO_IF_Z  r11, L1\n"
    "   13: FGETSTATE  r12\n"
    "   14: FSETROUND  r13, r12, TRUNC\n"
    "   15: FSETSTATE  r13\n"
    "   16: FADD       r14, r5, r6\n"
    "   17: FGETSTATE  r15\n"
    "   18: FCOPYROUND r16, r15, r12\n"
    "   19: FSETSTATE  r16\n"
    "   20: LABEL      L1\n"
    "   21: GET_ALIAS  r17, a5\n"
    "   22: FGETSTATE  r18\n"
    "   23: FCLEAREXC  r19, r18\n"
    "   24: FSETSTATE  r19\n"
    "   25: FTESTEXC   r20, r18, INVALID\n"
    "   26: GOTO_IF_Z  r20, L3\n"
    "   27: NOT        r21, r17\n"
    "   28: ORI        r22, r17, 16777216\n"
    "   29: ANDI       r23, r21, 16777216\n"
    "   30: SET_ALIAS  a5, r22\n"
    "   31: GOTO_IF_Z  r23, L4\n"
    "   32: ORI        r24, r22, -2147483648\n"
    "   33: SET_ALIAS  a5, r24\n"
    "   34: LABEL      L4\n"
    "   35: ANDI       r25, r17, 128\n"
    "   36: GOTO_IF_Z  r25, L3\n"
    "   37: GET_ALIAS  r26, a6\n"
    "   38: ANDI       r27, r26, 31\n"
    "   39: SET_ALIAS  a6, r27\n"
    "   40: GOTO       L2\n"
    "   41: LABEL      L3\n"
    "   42: SET_ALIAS  a2, r7\n"
    "   43: BITCAST    r28, r7\n"
    "   44: SGTUI      r29, r28, 0\n"
    "   45: SLTSI      r30, r28, 0\n"
    "   46: BFEXT      r34, r28, 52, 11\n"
    "   47: SEQI       r31, r34, 0\n"
    "   48: SEQI       r32, r34, 2047\n"
    "   49: SLLI       r35, r28, 12\n"
    "   50: SEQI       r33, r35, 0\n"
    "   51: AND        r36, r31, r33\n"
    "   52: XORI       r37, r33, 1\n"
    "   53: AND        r38, r32, r37\n"
    "   54: AND        r39, r31, r29\n"
    "   55: OR         r40, r39, r38\n"
    "   56: OR         r41, r36, r38\n"
    "   57: XORI       r42, r41, 1\n"
    "   58: XORI       r43, r30, 1\n"
    "   59: AND        r44, r30, r42\n"
    "   60: AND        r45, r43, r42\n"
    "   61: SLLI       r46, r40, 4\n"
    "   62: SLLI       r47, r44, 3\n"
    "   63: SLLI       r48, r45, 2\n"
    "   64: SLLI       r49, r36, 1\n"
    "   65: OR         r50, r46, r47\n"
    "   66: OR         r51, r48, r49\n"
    "   67: OR         r52, r50, r32\n"
    "   68: OR         r53, r52, r51\n"
    "   69: FTESTEXC   r54, r18, INEXACT\n"
    "   70: SLLI       r55, r54, 5\n"
    "   71: OR         r56, r53, r55\n"
    "   72: SET_ALIAS  a6, r56\n"
    "   73: GOTO_IF_Z  r54, L5\n"
    "   74: GET_ALIAS  r57, a5\n"
    "   75: NOT        r58, r57\n"
    "   76: ORI        r59, r57, 33554432\n"
    "   77: ANDI       r60, r58, 33554432\n"
    "   78: SET_ALIAS  a5, r59\n"
    "   79: GOTO_IF_Z  r60, L6\n"
    "   80: ORI        r61, r59, -2147483648\n"
    "   81: SET_ALIAS  a5, r61\n"
    "   82: LABEL      L6\n"
    "   83: LABEL      L5\n"
    "   84: FTESTEXC   r62, r18, OVERFLOW\n"
    "   85: GOTO_IF_Z  r62, L7\n"
    "   86: GET_ALIAS  r63, a5\n"
    "   87: NOT        r64, r63\n"
    "   88: ORI        r65, r63, 268435456\n"
    "   89: ANDI       r66, r64, 268435456\n"
    "   90: SET_ALIAS  a5, r65\n"
    "   91: GOTO_IF_Z  r66, L8\n"
    "   92: ORI        r67, r65, -2147483648\n"
    "   93: SET_ALIAS  a5, r67\n"
    "   94: LABEL      L8\n"
    "   95: LABEL      L7\n"
    "   96: FTESTEXC   r68, r18, UNDERFLOW\n"
    "   97: GOTO_IF_Z  r68, L2\n"
    "   98: GET_ALIAS  r69, a5\n"
    "   99: NOT        r70, r69\n"
    "  100: ORI        r71, r69, 134217728\n"
    "  101: ANDI       r72, r70, 134217728\n"
    "  102: SET_ALIAS  a5, r71\n"
    "  103: GOTO_IF_Z  r72, L9\n"
    "  104: ORI        r73, r71, -2147483648\n"
    "  105: SET_ALIAS  a5, r73\n"
    "  106: LABEL      L9\n"
    "  107: LABEL      L2\n"
    "  108: LOAD_IMM   r74, 4\n"
    "  109: SET_ALIAS  a1, r74\n"
    "  110: GET_ALIAS  r75, a5\n"
    "  111: GET_ALIAS  r76, a6\n"
    "  112: ANDI       r77, r75, -1611134977\n"
    "  113: SLLI       r78, r76, 12\n"
    "  114: OR         r79, r77, r78\n"
    "  115: SET_ALIAS  a5, r79\n"
    "  116: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: float64 @ 432(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "Alias 6: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,12] --> 1,2\n"
    "Block 1: 0 --> [13,19] --> 2\n"
    "Block 2: 1,0 --> [20,26] --> 3,7\n"
    "Block 3: 2 --> [27,31] --> 4,5\n"
    "Block 4: 3 --> [32,33] --> 5\n"
    "Block 5: 4,3 --> [34,36] --> 6,7\n"
    "Block 6: 5 --> [37,40] --> 19\n"
    "Block 7: 2,5 --> [41,73] --> 8,11\n"
    "Block 8: 7 --> [74,79] --> 9,10\n"
    "Block 9: 8 --> [80,81] --> 10\n"
    "Block 10: 9,8 --> [82,82] --> 11\n"
    "Block 11: 10,7 --> [83,85] --> 12,15\n"
    "Block 12: 11 --> [86,91] --> 13,14\n"
    "Block 13: 12 --> [92,93] --> 14\n"
    "Block 14: 13,12 --> [94,94] --> 15\n"
    "Block 15: 14,11 --> [95,97] --> 16,19\n"
    "Block 16: 15 --> [98,103] --> 17,18\n"
    "Block 17: 16 --> [104,105] --> 18\n"
    "Block 18: 17,16 --> [106,106] --> 19\n"
    "Block 19: 18,6,15 --> [107,116] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
