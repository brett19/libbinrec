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
    0xEC,0x20,0x10,0x30,  // fres f1,f2
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_NATIVE_RECIPROCAL;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: FCAST      r4, r3\n"
    "    4: LOAD_IMM   r5, 1.0f\n"
    "    5: FDIV       r6, r5, r4\n"
    "    6: GET_ALIAS  r7, a4\n"
    "    7: FGETSTATE  r8\n"
    "    8: FCLEAREXC  r9, r8\n"
    "    9: FSETSTATE  r9\n"
    "   10: FTESTEXC   r10, r8, INVALID\n"
    "   11: GOTO_IF_Z  r10, L2\n"
    "   12: NOT        r11, r7\n"
    "   13: ORI        r12, r7, 16777216\n"
    "   14: ANDI       r13, r11, 16777216\n"
    "   15: SET_ALIAS  a4, r12\n"
    "   16: GOTO_IF_Z  r13, L3\n"
    "   17: ORI        r14, r12, -2147483648\n"
    "   18: SET_ALIAS  a4, r14\n"
    "   19: LABEL      L3\n"
    "   20: ANDI       r15, r7, 128\n"
    "   21: GOTO_IF_Z  r15, L2\n"
    "   22: LABEL      L4\n"
    "   23: GET_ALIAS  r16, a4\n"
    "   24: BFEXT      r17, r16, 12, 7\n"
    "   25: ANDI       r18, r17, 31\n"
    "   26: GET_ALIAS  r19, a4\n"
    "   27: BFINS      r20, r19, r18, 12, 7\n"
    "   28: SET_ALIAS  a4, r20\n"
    "   29: GOTO       L1\n"
    "   30: LABEL      L2\n"
    "   31: FTESTEXC   r21, r8, ZERO_DIVIDE\n"
    "   32: GOTO_IF_Z  r21, L5\n"
    "   33: NOT        r22, r7\n"
    "   34: ORI        r23, r7, 67108864\n"
    "   35: ANDI       r24, r22, 67108864\n"
    "   36: SET_ALIAS  a4, r23\n"
    "   37: GOTO_IF_Z  r24, L6\n"
    "   38: ORI        r25, r23, -2147483648\n"
    "   39: SET_ALIAS  a4, r25\n"
    "   40: LABEL      L6\n"
    "   41: ANDI       r26, r7, 16\n"
    "   42: GOTO_IF_NZ r26, L4\n"
    "   43: LABEL      L5\n"
    "   44: FCVT       r27, r6\n"
    "   45: STORE      408(r1), r27\n"
    "   46: SET_ALIAS  a2, r27\n"
    "   47: BITCAST    r28, r6\n"
    "   48: SGTUI      r29, r28, 0\n"
    "   49: SRLI       r30, r28, 31\n"
    "   50: BFEXT      r34, r28, 23, 8\n"
    "   51: SEQI       r31, r34, 0\n"
    "   52: SEQI       r32, r34, 255\n"
    "   53: SLLI       r35, r28, 9\n"
    "   54: SEQI       r33, r35, 0\n"
    "   55: AND        r36, r31, r33\n"
    "   56: XORI       r37, r33, 1\n"
    "   57: AND        r38, r32, r37\n"
    "   58: AND        r39, r31, r29\n"
    "   59: OR         r40, r39, r38\n"
    "   60: OR         r41, r36, r38\n"
    "   61: XORI       r42, r41, 1\n"
    "   62: XORI       r43, r30, 1\n"
    "   63: AND        r44, r30, r42\n"
    "   64: AND        r45, r43, r42\n"
    "   65: SLLI       r46, r40, 4\n"
    "   66: SLLI       r47, r44, 3\n"
    "   67: SLLI       r48, r45, 2\n"
    "   68: SLLI       r49, r36, 1\n"
    "   69: OR         r50, r46, r47\n"
    "   70: OR         r51, r48, r49\n"
    "   71: OR         r52, r50, r32\n"
    "   72: OR         r53, r52, r51\n"
    "   73: FTESTEXC   r54, r8, INEXACT\n"
    "   74: SLLI       r55, r54, 5\n"
    "   75: OR         r56, r53, r55\n"
    "   76: GET_ALIAS  r57, a4\n"
    "   77: BFINS      r58, r57, r56, 12, 7\n"
    "   78: SET_ALIAS  a4, r58\n"
    "   79: FTESTEXC   r59, r8, OVERFLOW\n"
    "   80: GOTO_IF_Z  r59, L7\n"
    "   81: GET_ALIAS  r60, a4\n"
    "   82: NOT        r61, r60\n"
    "   83: ORI        r62, r60, 268435456\n"
    "   84: ANDI       r63, r61, 268435456\n"
    "   85: SET_ALIAS  a4, r62\n"
    "   86: GOTO_IF_Z  r63, L8\n"
    "   87: ORI        r64, r62, -2147483648\n"
    "   88: SET_ALIAS  a4, r64\n"
    "   89: LABEL      L8\n"
    "   90: LABEL      L7\n"
    "   91: FTESTEXC   r65, r8, UNDERFLOW\n"
    "   92: GOTO_IF_Z  r65, L1\n"
    "   93: GET_ALIAS  r66, a4\n"
    "   94: NOT        r67, r66\n"
    "   95: ORI        r68, r66, 134217728\n"
    "   96: ANDI       r69, r67, 134217728\n"
    "   97: SET_ALIAS  a4, r68\n"
    "   98: GOTO_IF_Z  r69, L9\n"
    "   99: ORI        r70, r68, -2147483648\n"
    "  100: SET_ALIAS  a4, r70\n"
    "  101: LABEL      L9\n"
    "  102: LABEL      L1\n"
    "  103: LOAD_IMM   r71, 4\n"
    "  104: SET_ALIAS  a1, r71\n"
    "  105: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,11] --> 1,5\n"
    "Block 1: 0 --> [12,16] --> 2,3\n"
    "Block 2: 1 --> [17,18] --> 3\n"
    "Block 3: 2,1 --> [19,21] --> 4,5\n"
    "Block 4: 3,8 --> [22,29] --> 17\n"
    "Block 5: 0,3 --> [30,32] --> 6,9\n"
    "Block 6: 5 --> [33,37] --> 7,8\n"
    "Block 7: 6 --> [38,39] --> 8\n"
    "Block 8: 7,6 --> [40,42] --> 9,4\n"
    "Block 9: 8,5 --> [43,80] --> 10,13\n"
    "Block 10: 9 --> [81,86] --> 11,12\n"
    "Block 11: 10 --> [87,88] --> 12\n"
    "Block 12: 11,10 --> [89,89] --> 13\n"
    "Block 13: 12,9 --> [90,92] --> 14,17\n"
    "Block 14: 13 --> [93,98] --> 15,16\n"
    "Block 15: 14 --> [99,100] --> 16\n"
    "Block 16: 15,14 --> [101,101] --> 17\n"
    "Block 17: 16,4,13 --> [102,105] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
