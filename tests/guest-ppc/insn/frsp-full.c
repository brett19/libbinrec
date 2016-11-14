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
    0xFC,0x20,0x10,0x18,  // frsp f1,f2
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a4\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a5, r4\n"
    "    5: GET_ALIAS  r5, a3\n"
    "    6: FCVT       r6, r5\n"
    "    7: GET_ALIAS  r7, a4\n"
    "    8: FGETSTATE  r8\n"
    "    9: FCLEAREXC  r9, r8\n"
    "   10: FSETSTATE  r9\n"
    "   11: FTESTEXC   r10, r8, INVALID\n"
    "   12: GOTO_IF_Z  r10, L1\n"
    "   13: NOT        r11, r7\n"
    "   14: ORI        r12, r7, 16777216\n"
    "   15: ANDI       r13, r11, 16777216\n"
    "   16: SET_ALIAS  a4, r12\n"
    "   17: GOTO_IF_Z  r13, L2\n"
    "   18: ORI        r14, r12, -2147483648\n"
    "   19: SET_ALIAS  a4, r14\n"
    "   20: LABEL      L2\n"
    "   21: ANDI       r15, r7, 128\n"
    "   22: GOTO_IF_Z  r15, L1\n"
    "   23: GET_ALIAS  r16, a5\n"
    "   24: ANDI       r17, r16, 31\n"
    "   25: SET_ALIAS  a5, r17\n"
    "   26: GOTO       L3\n"
    "   27: LABEL      L1\n"
    "   28: FCVT       r18, r6\n"
    "   29: STORE      408(r1), r18\n"
    "   30: SET_ALIAS  a2, r18\n"
    "   31: BITCAST    r19, r6\n"
    "   32: SGTUI      r20, r19, 0\n"
    "   33: SRLI       r21, r19, 31\n"
    "   34: BFEXT      r25, r19, 23, 8\n"
    "   35: SEQI       r22, r25, 0\n"
    "   36: SEQI       r23, r25, 255\n"
    "   37: SLLI       r26, r19, 9\n"
    "   38: SEQI       r24, r26, 0\n"
    "   39: AND        r27, r22, r24\n"
    "   40: XORI       r28, r24, 1\n"
    "   41: AND        r29, r23, r28\n"
    "   42: AND        r30, r22, r20\n"
    "   43: OR         r31, r30, r29\n"
    "   44: OR         r32, r27, r29\n"
    "   45: XORI       r33, r32, 1\n"
    "   46: XORI       r34, r21, 1\n"
    "   47: AND        r35, r21, r33\n"
    "   48: AND        r36, r34, r33\n"
    "   49: SLLI       r37, r31, 4\n"
    "   50: SLLI       r38, r35, 3\n"
    "   51: SLLI       r39, r36, 2\n"
    "   52: SLLI       r40, r27, 1\n"
    "   53: OR         r41, r37, r38\n"
    "   54: OR         r42, r39, r40\n"
    "   55: OR         r43, r41, r23\n"
    "   56: OR         r44, r43, r42\n"
    "   57: FTESTEXC   r45, r8, INEXACT\n"
    "   58: SLLI       r46, r45, 5\n"
    "   59: OR         r47, r44, r46\n"
    "   60: SET_ALIAS  a5, r47\n"
    "   61: GOTO_IF_Z  r45, L4\n"
    "   62: GET_ALIAS  r48, a4\n"
    "   63: NOT        r49, r48\n"
    "   64: ORI        r50, r48, 33554432\n"
    "   65: ANDI       r51, r49, 33554432\n"
    "   66: SET_ALIAS  a4, r50\n"
    "   67: GOTO_IF_Z  r51, L5\n"
    "   68: ORI        r52, r50, -2147483648\n"
    "   69: SET_ALIAS  a4, r52\n"
    "   70: LABEL      L5\n"
    "   71: LABEL      L4\n"
    "   72: FTESTEXC   r53, r8, OVERFLOW\n"
    "   73: GOTO_IF_Z  r53, L6\n"
    "   74: GET_ALIAS  r54, a4\n"
    "   75: NOT        r55, r54\n"
    "   76: ORI        r56, r54, 268435456\n"
    "   77: ANDI       r57, r55, 268435456\n"
    "   78: SET_ALIAS  a4, r56\n"
    "   79: GOTO_IF_Z  r57, L7\n"
    "   80: ORI        r58, r56, -2147483648\n"
    "   81: SET_ALIAS  a4, r58\n"
    "   82: LABEL      L7\n"
    "   83: LABEL      L6\n"
    "   84: FTESTEXC   r59, r8, UNDERFLOW\n"
    "   85: GOTO_IF_Z  r59, L3\n"
    "   86: GET_ALIAS  r60, a4\n"
    "   87: NOT        r61, r60\n"
    "   88: ORI        r62, r60, 134217728\n"
    "   89: ANDI       r63, r61, 134217728\n"
    "   90: SET_ALIAS  a4, r62\n"
    "   91: GOTO_IF_Z  r63, L8\n"
    "   92: ORI        r64, r62, -2147483648\n"
    "   93: SET_ALIAS  a4, r64\n"
    "   94: LABEL      L8\n"
    "   95: LABEL      L3\n"
    "   96: LOAD_IMM   r65, 4\n"
    "   97: SET_ALIAS  a1, r65\n"
    "   98: GET_ALIAS  r66, a4\n"
    "   99: GET_ALIAS  r67, a5\n"
    "  100: ANDI       r68, r66, -1611134977\n"
    "  101: SLLI       r69, r67, 12\n"
    "  102: OR         r70, r68, r69\n"
    "  103: SET_ALIAS  a4, r70\n"
    "  104: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,12] --> 1,5\n"
    "Block 1: 0 --> [13,17] --> 2,3\n"
    "Block 2: 1 --> [18,19] --> 3\n"
    "Block 3: 2,1 --> [20,22] --> 4,5\n"
    "Block 4: 3 --> [23,26] --> 17\n"
    "Block 5: 0,3 --> [27,61] --> 6,9\n"
    "Block 6: 5 --> [62,67] --> 7,8\n"
    "Block 7: 6 --> [68,69] --> 8\n"
    "Block 8: 7,6 --> [70,70] --> 9\n"
    "Block 9: 8,5 --> [71,73] --> 10,13\n"
    "Block 10: 9 --> [74,79] --> 11,12\n"
    "Block 11: 10 --> [80,81] --> 12\n"
    "Block 12: 11,10 --> [82,82] --> 13\n"
    "Block 13: 12,9 --> [83,85] --> 14,17\n"
    "Block 14: 13 --> [86,91] --> 15,16\n"
    "Block 15: 14 --> [92,93] --> 16\n"
    "Block 16: 15,14 --> [94,94] --> 17\n"
    "Block 17: 16,4,13 --> [95,104] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"