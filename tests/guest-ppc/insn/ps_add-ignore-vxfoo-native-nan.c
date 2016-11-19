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
    0x10,0x22,0x18,0x2A,  // ps_add f1,f2,f3
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_IGNORE_FPSCR_VXFOO;
static const unsigned int common_opt = BINREC_OPT_NATIVE_IEEE_NAN;

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
    "    8: VFCVT      r8, r7\n"
    "    9: LOAD_IMM   r9, 0x1000000\n"
    "   10: VEXTRACT   r10, r8, 1\n"
    "   11: BITCAST    r11, r10\n"
    "   12: SLLI       r12, r11, 1\n"
    "   13: SEQ        r13, r12, r9\n"
    "   14: GOTO_IF_NZ r13, L1\n"
    "   15: VEXTRACT   r14, r8, 0\n"
    "   16: BITCAST    r15, r14\n"
    "   17: SLLI       r16, r15, 1\n"
    "   18: SEQ        r17, r16, r9\n"
    "   19: GOTO_IF_Z  r17, L2\n"
    "   20: LABEL      L1\n"
    "   21: FGETSTATE  r18\n"
    "   22: FSETROUND  r19, r18, TRUNC\n"
    "   23: FSETSTATE  r19\n"
    "   24: FADD       r20, r5, r6\n"
    "   25: VFCVT      r21, r20\n"
    "   26: FGETSTATE  r22\n"
    "   27: FCOPYROUND r23, r22, r18\n"
    "   28: FSETSTATE  r23\n"
    "   29: LABEL      L2\n"
    "   30: GET_ALIAS  r24, a5\n"
    "   31: FGETSTATE  r25\n"
    "   32: FCLEAREXC  r26, r25\n"
    "   33: FSETSTATE  r26\n"
    "   34: VFCVT      r27, r8\n"
    "   35: SET_ALIAS  a2, r27\n"
    "   36: VEXTRACT   r28, r8, 0\n"
    "   37: BITCAST    r29, r28\n"
    "   38: SGTUI      r30, r29, 0\n"
    "   39: SRLI       r31, r29, 31\n"
    "   40: BFEXT      r35, r29, 23, 8\n"
    "   41: SEQI       r32, r35, 0\n"
    "   42: SEQI       r33, r35, 255\n"
    "   43: SLLI       r36, r29, 9\n"
    "   44: SEQI       r34, r36, 0\n"
    "   45: AND        r37, r32, r34\n"
    "   46: XORI       r38, r34, 1\n"
    "   47: AND        r39, r33, r38\n"
    "   48: AND        r40, r32, r30\n"
    "   49: OR         r41, r40, r39\n"
    "   50: OR         r42, r37, r39\n"
    "   51: XORI       r43, r42, 1\n"
    "   52: XORI       r44, r31, 1\n"
    "   53: AND        r45, r31, r43\n"
    "   54: AND        r46, r44, r43\n"
    "   55: SLLI       r47, r41, 4\n"
    "   56: SLLI       r48, r45, 3\n"
    "   57: SLLI       r49, r46, 2\n"
    "   58: SLLI       r50, r37, 1\n"
    "   59: OR         r51, r47, r48\n"
    "   60: OR         r52, r49, r50\n"
    "   61: OR         r53, r51, r33\n"
    "   62: OR         r54, r53, r52\n"
    "   63: FTESTEXC   r55, r25, INEXACT\n"
    "   64: SLLI       r56, r55, 5\n"
    "   65: OR         r57, r54, r56\n"
    "   66: SET_ALIAS  a6, r57\n"
    "   67: GOTO_IF_Z  r55, L4\n"
    "   68: GET_ALIAS  r58, a5\n"
    "   69: NOT        r59, r58\n"
    "   70: ORI        r60, r58, 33554432\n"
    "   71: ANDI       r61, r59, 33554432\n"
    "   72: SET_ALIAS  a5, r60\n"
    "   73: GOTO_IF_Z  r61, L5\n"
    "   74: ORI        r62, r60, -2147483648\n"
    "   75: SET_ALIAS  a5, r62\n"
    "   76: LABEL      L5\n"
    "   77: LABEL      L4\n"
    "   78: FTESTEXC   r63, r25, OVERFLOW\n"
    "   79: GOTO_IF_Z  r63, L6\n"
    "   80: GET_ALIAS  r64, a5\n"
    "   81: NOT        r65, r64\n"
    "   82: ORI        r66, r64, 268435456\n"
    "   83: ANDI       r67, r65, 268435456\n"
    "   84: SET_ALIAS  a5, r66\n"
    "   85: GOTO_IF_Z  r67, L7\n"
    "   86: ORI        r68, r66, -2147483648\n"
    "   87: SET_ALIAS  a5, r68\n"
    "   88: LABEL      L7\n"
    "   89: LABEL      L6\n"
    "   90: FTESTEXC   r69, r25, UNDERFLOW\n"
    "   91: GOTO_IF_Z  r69, L3\n"
    "   92: GET_ALIAS  r70, a5\n"
    "   93: NOT        r71, r70\n"
    "   94: ORI        r72, r70, 134217728\n"
    "   95: ANDI       r73, r71, 134217728\n"
    "   96: SET_ALIAS  a5, r72\n"
    "   97: GOTO_IF_Z  r73, L8\n"
    "   98: ORI        r74, r72, -2147483648\n"
    "   99: SET_ALIAS  a5, r74\n"
    "  100: LABEL      L8\n"
    "  101: LABEL      L3\n"
    "  102: LOAD_IMM   r75, 4\n"
    "  103: SET_ALIAS  a1, r75\n"
    "  104: GET_ALIAS  r76, a5\n"
    "  105: GET_ALIAS  r77, a6\n"
    "  106: ANDI       r78, r76, -1611134977\n"
    "  107: SLLI       r79, r77, 12\n"
    "  108: OR         r80, r78, r79\n"
    "  109: SET_ALIAS  a5, r80\n"
    "  110: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: float64[2] @ 432(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "Alias 6: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,14] --> 1,2\n"
    "Block 1: 0 --> [15,19] --> 2,3\n"
    "Block 2: 1,0 --> [20,28] --> 3\n"
    "Block 3: 2,1 --> [29,67] --> 4,7\n"
    "Block 4: 3 --> [68,73] --> 5,6\n"
    "Block 5: 4 --> [74,75] --> 6\n"
    "Block 6: 5,4 --> [76,76] --> 7\n"
    "Block 7: 6,3 --> [77,79] --> 8,11\n"
    "Block 8: 7 --> [80,85] --> 9,10\n"
    "Block 9: 8 --> [86,87] --> 10\n"
    "Block 10: 9,8 --> [88,88] --> 11\n"
    "Block 11: 10,7 --> [89,91] --> 12,15\n"
    "Block 12: 11 --> [92,97] --> 13,14\n"
    "Block 13: 12 --> [98,99] --> 14\n"
    "Block 14: 13,12 --> [100,100] --> 15\n"
    "Block 15: 14,11 --> [101,110] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
