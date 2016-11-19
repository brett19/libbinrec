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
    0xFC,0x20,0x10,0x34,  // frsqrte f1,f2
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_NATIVE_RECIPROCAL;
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
    "    6: FSQRT      r6, r5\n"
    "    7: LOAD_IMM   r7, 1.0\n"
    "    8: FDIV       r8, r7, r6\n"
    "    9: GET_ALIAS  r9, a4\n"
    "   10: FGETSTATE  r10\n"
    "   11: FCLEAREXC  r11, r10\n"
    "   12: FSETSTATE  r11\n"
    "   13: FTESTEXC   r12, r10, INVALID\n"
    "   14: GOTO_IF_Z  r12, L2\n"
    "   15: BITCAST    r13, r5\n"
    "   16: SLLI       r14, r13, 13\n"
    "   17: BFEXT      r15, r13, 51, 12\n"
    "   18: SEQI       r16, r15, 4094\n"
    "   19: GOTO_IF_Z  r16, L4\n"
    "   20: GOTO_IF_NZ r14, L3\n"
    "   21: LABEL      L4\n"
    "   22: NOT        r17, r9\n"
    "   23: ORI        r18, r9, 512\n"
    "   24: ANDI       r19, r17, 512\n"
    "   25: SET_ALIAS  a4, r18\n"
    "   26: GOTO_IF_Z  r19, L5\n"
    "   27: ORI        r20, r18, -2147483648\n"
    "   28: SET_ALIAS  a4, r20\n"
    "   29: LABEL      L5\n"
    "   30: ANDI       r21, r9, 128\n"
    "   31: GOTO_IF_Z  r21, L6\n"
    "   32: GET_ALIAS  r22, a5\n"
    "   33: ANDI       r23, r22, 31\n"
    "   34: SET_ALIAS  a5, r23\n"
    "   35: GOTO       L1\n"
    "   36: LABEL      L6\n"
    "   37: LOAD_IMM   r24, nan(0x8000000000000)\n"
    "   38: SET_ALIAS  a2, r24\n"
    "   39: LOAD_IMM   r25, 17\n"
    "   40: SET_ALIAS  a5, r25\n"
    "   41: GOTO       L1\n"
    "   42: LABEL      L3\n"
    "   43: NOT        r26, r9\n"
    "   44: ORI        r27, r9, 16777216\n"
    "   45: ANDI       r28, r26, 16777216\n"
    "   46: SET_ALIAS  a4, r27\n"
    "   47: GOTO_IF_Z  r28, L7\n"
    "   48: ORI        r29, r27, -2147483648\n"
    "   49: SET_ALIAS  a4, r29\n"
    "   50: LABEL      L7\n"
    "   51: ANDI       r30, r9, 128\n"
    "   52: GOTO_IF_Z  r30, L2\n"
    "   53: LABEL      L8\n"
    "   54: GET_ALIAS  r31, a5\n"
    "   55: ANDI       r32, r31, 31\n"
    "   56: SET_ALIAS  a5, r32\n"
    "   57: GOTO       L1\n"
    "   58: LABEL      L2\n"
    "   59: FTESTEXC   r33, r10, ZERO_DIVIDE\n"
    "   60: GOTO_IF_Z  r33, L9\n"
    "   61: NOT        r34, r9\n"
    "   62: ORI        r35, r9, 67108864\n"
    "   63: ANDI       r36, r34, 67108864\n"
    "   64: SET_ALIAS  a4, r35\n"
    "   65: GOTO_IF_Z  r36, L10\n"
    "   66: ORI        r37, r35, -2147483648\n"
    "   67: SET_ALIAS  a4, r37\n"
    "   68: LABEL      L10\n"
    "   69: ANDI       r38, r9, 16\n"
    "   70: GOTO_IF_NZ r38, L8\n"
    "   71: LABEL      L9\n"
    "   72: SET_ALIAS  a2, r8\n"
    "   73: BITCAST    r39, r8\n"
    "   74: SGTUI      r40, r39, 0\n"
    "   75: SLTSI      r41, r39, 0\n"
    "   76: BFEXT      r45, r39, 52, 11\n"
    "   77: SEQI       r42, r45, 0\n"
    "   78: SEQI       r43, r45, 2047\n"
    "   79: SLLI       r46, r39, 12\n"
    "   80: SEQI       r44, r46, 0\n"
    "   81: AND        r47, r42, r44\n"
    "   82: XORI       r48, r44, 1\n"
    "   83: AND        r49, r43, r48\n"
    "   84: AND        r50, r42, r40\n"
    "   85: OR         r51, r50, r49\n"
    "   86: OR         r52, r47, r49\n"
    "   87: XORI       r53, r52, 1\n"
    "   88: XORI       r54, r41, 1\n"
    "   89: AND        r55, r41, r53\n"
    "   90: AND        r56, r54, r53\n"
    "   91: SLLI       r57, r51, 4\n"
    "   92: SLLI       r58, r55, 3\n"
    "   93: SLLI       r59, r56, 2\n"
    "   94: SLLI       r60, r47, 1\n"
    "   95: OR         r61, r57, r58\n"
    "   96: OR         r62, r59, r60\n"
    "   97: OR         r63, r61, r43\n"
    "   98: OR         r64, r63, r62\n"
    "   99: FTESTEXC   r65, r10, INEXACT\n"
    "  100: SLLI       r66, r65, 5\n"
    "  101: OR         r67, r64, r66\n"
    "  102: SET_ALIAS  a5, r67\n"
    "  103: FTESTEXC   r68, r10, OVERFLOW\n"
    "  104: GOTO_IF_Z  r68, L11\n"
    "  105: GET_ALIAS  r69, a4\n"
    "  106: NOT        r70, r69\n"
    "  107: ORI        r71, r69, 268435456\n"
    "  108: ANDI       r72, r70, 268435456\n"
    "  109: SET_ALIAS  a4, r71\n"
    "  110: GOTO_IF_Z  r72, L12\n"
    "  111: ORI        r73, r71, -2147483648\n"
    "  112: SET_ALIAS  a4, r73\n"
    "  113: LABEL      L12\n"
    "  114: LABEL      L11\n"
    "  115: FTESTEXC   r74, r10, UNDERFLOW\n"
    "  116: GOTO_IF_Z  r74, L1\n"
    "  117: GET_ALIAS  r75, a4\n"
    "  118: NOT        r76, r75\n"
    "  119: ORI        r77, r75, 134217728\n"
    "  120: ANDI       r78, r76, 134217728\n"
    "  121: SET_ALIAS  a4, r77\n"
    "  122: GOTO_IF_Z  r78, L13\n"
    "  123: ORI        r79, r77, -2147483648\n"
    "  124: SET_ALIAS  a4, r79\n"
    "  125: LABEL      L13\n"
    "  126: LABEL      L1\n"
    "  127: LOAD_IMM   r80, 4\n"
    "  128: SET_ALIAS  a1, r80\n"
    "  129: GET_ALIAS  r81, a4\n"
    "  130: GET_ALIAS  r82, a5\n"
    "  131: ANDI       r83, r81, -1611134977\n"
    "  132: SLLI       r84, r82, 12\n"
    "  133: OR         r85, r83, r84\n"
    "  134: SET_ALIAS  a4, r85\n"
    "  135: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,14] --> 1,12\n"
    "Block 1: 0 --> [15,19] --> 2,3\n"
    "Block 2: 1 --> [20,20] --> 3,8\n"
    "Block 3: 2,1 --> [21,26] --> 4,5\n"
    "Block 4: 3 --> [27,28] --> 5\n"
    "Block 5: 4,3 --> [29,31] --> 6,7\n"
    "Block 6: 5 --> [32,35] --> 24\n"
    "Block 7: 5 --> [36,41] --> 24\n"
    "Block 8: 2 --> [42,47] --> 9,10\n"
    "Block 9: 8 --> [48,49] --> 10\n"
    "Block 10: 9,8 --> [50,52] --> 11,12\n"
    "Block 11: 10,15 --> [53,57] --> 24\n"
    "Block 12: 0,10 --> [58,60] --> 13,16\n"
    "Block 13: 12 --> [61,65] --> 14,15\n"
    "Block 14: 13 --> [66,67] --> 15\n"
    "Block 15: 14,13 --> [68,70] --> 16,11\n"
    "Block 16: 15,12 --> [71,104] --> 17,20\n"
    "Block 17: 16 --> [105,110] --> 18,19\n"
    "Block 18: 17 --> [111,112] --> 19\n"
    "Block 19: 18,17 --> [113,113] --> 20\n"
    "Block 20: 19,16 --> [114,116] --> 21,24\n"
    "Block 21: 20 --> [117,122] --> 22,23\n"
    "Block 22: 21 --> [123,124] --> 23\n"
    "Block 23: 22,21 --> [125,125] --> 24\n"
    "Block 24: 23,6,7,11,20 --> [126,135] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
