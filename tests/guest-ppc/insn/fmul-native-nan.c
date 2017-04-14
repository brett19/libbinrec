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
    0xFC,0x22,0x00,0xF2,  // fmul f1,f2,f3
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = BINREC_OPT_NATIVE_IEEE_NAN;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: GET_ALIAS  r4, a4\n"
    "    4: FMUL       r5, r3, r4\n"
    "    5: LOAD_IMM   r6, 0x20000000000000\n"
    "    6: BITCAST    r7, r5\n"
    "    7: SLLI       r8, r7, 1\n"
    "    8: SEQ        r9, r8, r6\n"
    "    9: GOTO_IF_Z  r9, L1\n"
    "   10: FGETSTATE  r10\n"
    "   11: FSETROUND  r11, r10, TRUNC\n"
    "   12: FSETSTATE  r11\n"
    "   13: FMUL       r12, r3, r4\n"
    "   14: FGETSTATE  r13\n"
    "   15: FCOPYROUND r14, r13, r10\n"
    "   16: FSETSTATE  r14\n"
    "   17: LABEL      L1\n"
    "   18: GET_ALIAS  r15, a5\n"
    "   19: FGETSTATE  r16\n"
    "   20: FCLEAREXC  r17, r16\n"
    "   21: FSETSTATE  r17\n"
    "   22: FTESTEXC   r18, r16, INVALID\n"
    "   23: GOTO_IF_Z  r18, L3\n"
    "   24: BITCAST    r19, r3\n"
    "   25: SLLI       r20, r19, 13\n"
    "   26: BFEXT      r21, r19, 51, 12\n"
    "   27: SEQI       r22, r21, 4094\n"
    "   28: GOTO_IF_Z  r22, L5\n"
    "   29: GOTO_IF_NZ r20, L4\n"
    "   30: LABEL      L5\n"
    "   31: BITCAST    r23, r4\n"
    "   32: SLLI       r24, r23, 13\n"
    "   33: BFEXT      r25, r23, 51, 12\n"
    "   34: SEQI       r26, r25, 4094\n"
    "   35: GOTO_IF_Z  r26, L6\n"
    "   36: GOTO_IF_NZ r24, L4\n"
    "   37: LABEL      L6\n"
    "   38: NOT        r27, r15\n"
    "   39: ORI        r28, r15, 1048576\n"
    "   40: ANDI       r29, r27, 1048576\n"
    "   41: SET_ALIAS  a5, r28\n"
    "   42: GOTO_IF_Z  r29, L7\n"
    "   43: ORI        r30, r28, -2147483648\n"
    "   44: SET_ALIAS  a5, r30\n"
    "   45: LABEL      L7\n"
    "   46: GOTO       L8\n"
    "   47: LABEL      L4\n"
    "   48: NOT        r31, r15\n"
    "   49: ORI        r32, r15, 16777216\n"
    "   50: ANDI       r33, r31, 16777216\n"
    "   51: SET_ALIAS  a5, r32\n"
    "   52: GOTO_IF_Z  r33, L9\n"
    "   53: ORI        r34, r32, -2147483648\n"
    "   54: SET_ALIAS  a5, r34\n"
    "   55: LABEL      L9\n"
    "   56: LABEL      L8\n"
    "   57: ANDI       r35, r15, 128\n"
    "   58: GOTO_IF_Z  r35, L3\n"
    "   59: GET_ALIAS  r36, a5\n"
    "   60: BFEXT      r37, r36, 12, 7\n"
    "   61: ANDI       r38, r37, 31\n"
    "   62: GET_ALIAS  r39, a5\n"
    "   63: BFINS      r40, r39, r38, 12, 7\n"
    "   64: SET_ALIAS  a5, r40\n"
    "   65: GOTO       L2\n"
    "   66: LABEL      L3\n"
    "   67: SET_ALIAS  a2, r5\n"
    "   68: BITCAST    r41, r5\n"
    "   69: SGTUI      r42, r41, 0\n"
    "   70: SLTSI      r43, r41, 0\n"
    "   71: BFEXT      r47, r41, 52, 11\n"
    "   72: SEQI       r44, r47, 0\n"
    "   73: SEQI       r45, r47, 2047\n"
    "   74: SLLI       r48, r41, 12\n"
    "   75: SEQI       r46, r48, 0\n"
    "   76: AND        r49, r44, r46\n"
    "   77: XORI       r50, r46, 1\n"
    "   78: AND        r51, r45, r50\n"
    "   79: AND        r52, r44, r42\n"
    "   80: OR         r53, r52, r51\n"
    "   81: OR         r54, r49, r51\n"
    "   82: XORI       r55, r54, 1\n"
    "   83: XORI       r56, r43, 1\n"
    "   84: AND        r57, r43, r55\n"
    "   85: AND        r58, r56, r55\n"
    "   86: SLLI       r59, r53, 4\n"
    "   87: SLLI       r60, r57, 3\n"
    "   88: SLLI       r61, r58, 2\n"
    "   89: SLLI       r62, r49, 1\n"
    "   90: OR         r63, r59, r60\n"
    "   91: OR         r64, r61, r62\n"
    "   92: OR         r65, r63, r45\n"
    "   93: OR         r66, r65, r64\n"
    "   94: FTESTEXC   r67, r16, INEXACT\n"
    "   95: SLLI       r68, r67, 5\n"
    "   96: OR         r69, r66, r68\n"
    "   97: GET_ALIAS  r70, a5\n"
    "   98: BFINS      r71, r70, r69, 12, 7\n"
    "   99: SET_ALIAS  a5, r71\n"
    "  100: GOTO_IF_Z  r67, L10\n"
    "  101: GET_ALIAS  r72, a5\n"
    "  102: NOT        r73, r72\n"
    "  103: ORI        r74, r72, 33554432\n"
    "  104: ANDI       r75, r73, 33554432\n"
    "  105: SET_ALIAS  a5, r74\n"
    "  106: GOTO_IF_Z  r75, L11\n"
    "  107: ORI        r76, r74, -2147483648\n"
    "  108: SET_ALIAS  a5, r76\n"
    "  109: LABEL      L11\n"
    "  110: LABEL      L10\n"
    "  111: FTESTEXC   r77, r16, OVERFLOW\n"
    "  112: GOTO_IF_Z  r77, L12\n"
    "  113: GET_ALIAS  r78, a5\n"
    "  114: NOT        r79, r78\n"
    "  115: ORI        r80, r78, 268435456\n"
    "  116: ANDI       r81, r79, 268435456\n"
    "  117: SET_ALIAS  a5, r80\n"
    "  118: GOTO_IF_Z  r81, L13\n"
    "  119: ORI        r82, r80, -2147483648\n"
    "  120: SET_ALIAS  a5, r82\n"
    "  121: LABEL      L13\n"
    "  122: LABEL      L12\n"
    "  123: FTESTEXC   r83, r16, UNDERFLOW\n"
    "  124: GOTO_IF_Z  r83, L2\n"
    "  125: GET_ALIAS  r84, a5\n"
    "  126: NOT        r85, r84\n"
    "  127: ORI        r86, r84, 134217728\n"
    "  128: ANDI       r87, r85, 134217728\n"
    "  129: SET_ALIAS  a5, r86\n"
    "  130: GOTO_IF_Z  r87, L14\n"
    "  131: ORI        r88, r86, -2147483648\n"
    "  132: SET_ALIAS  a5, r88\n"
    "  133: LABEL      L14\n"
    "  134: LABEL      L2\n"
    "  135: LOAD_IMM   r89, 4\n"
    "  136: SET_ALIAS  a1, r89\n"
    "  137: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: float64 @ 432(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,9] --> 1,2\n"
    "Block 1: 0 --> [10,16] --> 2\n"
    "Block 2: 1,0 --> [17,23] --> 3,15\n"
    "Block 3: 2 --> [24,28] --> 4,5\n"
    "Block 4: 3 --> [29,29] --> 5,10\n"
    "Block 5: 4,3 --> [30,35] --> 6,7\n"
    "Block 6: 5 --> [36,36] --> 7,10\n"
    "Block 7: 6,5 --> [37,42] --> 8,9\n"
    "Block 8: 7 --> [43,44] --> 9\n"
    "Block 9: 8,7 --> [45,46] --> 13\n"
    "Block 10: 4,6 --> [47,52] --> 11,12\n"
    "Block 11: 10 --> [53,54] --> 12\n"
    "Block 12: 11,10 --> [55,55] --> 13\n"
    "Block 13: 12,9 --> [56,58] --> 14,15\n"
    "Block 14: 13 --> [59,65] --> 27\n"
    "Block 15: 2,13 --> [66,100] --> 16,19\n"
    "Block 16: 15 --> [101,106] --> 17,18\n"
    "Block 17: 16 --> [107,108] --> 18\n"
    "Block 18: 17,16 --> [109,109] --> 19\n"
    "Block 19: 18,15 --> [110,112] --> 20,23\n"
    "Block 20: 19 --> [113,118] --> 21,22\n"
    "Block 21: 20 --> [119,120] --> 22\n"
    "Block 22: 21,20 --> [121,121] --> 23\n"
    "Block 23: 22,19 --> [122,124] --> 24,27\n"
    "Block 24: 23 --> [125,130] --> 25,26\n"
    "Block 25: 24 --> [131,132] --> 26\n"
    "Block 26: 25,24 --> [133,133] --> 27\n"
    "Block 27: 26,14,23 --> [134,137] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
