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
    0xFC,0x22,0x18,0x24,  // fdiv f1,f2,f3
};

static const unsigned int guest_opt = 0;
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
    "    7: FDIV       r7, r5, r6\n"
    "    8: LOAD_IMM   r8, 0x20000000000000\n"
    "    9: BITCAST    r9, r7\n"
    "   10: SLLI       r10, r9, 1\n"
    "   11: SEQ        r11, r10, r8\n"
    "   12: GOTO_IF_Z  r11, L1\n"
    "   13: FGETSTATE  r12\n"
    "   14: FSETROUND  r13, r12, TRUNC\n"
    "   15: FSETSTATE  r13\n"
    "   16: FDIV       r14, r5, r6\n"
    "   17: FGETSTATE  r15\n"
    "   18: FCOPYROUND r16, r15, r12\n"
    "   19: FSETSTATE  r16\n"
    "   20: LABEL      L1\n"
    "   21: GET_ALIAS  r17, a5\n"
    "   22: FGETSTATE  r18\n"
    "   23: FCLEAREXC  r19, r18\n"
    "   24: FSETSTATE  r19\n"
    "   25: FTESTEXC   r20, r18, INVALID\n"
    "   26: GOTO_IF_Z  r20, L2\n"
    "   27: BITCAST    r21, r5\n"
    "   28: SLLI       r22, r21, 13\n"
    "   29: BFEXT      r23, r21, 51, 12\n"
    "   30: SEQI       r24, r23, 4094\n"
    "   31: GOTO_IF_Z  r24, L4\n"
    "   32: GOTO_IF_NZ r22, L3\n"
    "   33: LABEL      L4\n"
    "   34: BITCAST    r25, r6\n"
    "   35: SLLI       r26, r25, 13\n"
    "   36: BFEXT      r27, r25, 51, 12\n"
    "   37: SEQI       r28, r27, 4094\n"
    "   38: GOTO_IF_Z  r28, L5\n"
    "   39: GOTO_IF_NZ r26, L3\n"
    "   40: LABEL      L5\n"
    "   41: BITCAST    r29, r5\n"
    "   42: BITCAST    r30, r6\n"
    "   43: OR         r31, r29, r30\n"
    "   44: SLLI       r32, r31, 1\n"
    "   45: GOTO_IF_NZ r32, L6\n"
    "   46: NOT        r33, r17\n"
    "   47: ORI        r34, r17, 2097152\n"
    "   48: ANDI       r35, r33, 2097152\n"
    "   49: SET_ALIAS  a5, r34\n"
    "   50: GOTO_IF_Z  r35, L7\n"
    "   51: ORI        r36, r34, -2147483648\n"
    "   52: SET_ALIAS  a5, r36\n"
    "   53: LABEL      L7\n"
    "   54: GOTO       L8\n"
    "   55: LABEL      L6\n"
    "   56: NOT        r37, r17\n"
    "   57: ORI        r38, r17, 4194304\n"
    "   58: ANDI       r39, r37, 4194304\n"
    "   59: SET_ALIAS  a5, r38\n"
    "   60: GOTO_IF_Z  r39, L9\n"
    "   61: ORI        r40, r38, -2147483648\n"
    "   62: SET_ALIAS  a5, r40\n"
    "   63: LABEL      L9\n"
    "   64: GOTO       L8\n"
    "   65: LABEL      L3\n"
    "   66: NOT        r41, r17\n"
    "   67: ORI        r42, r17, 16777216\n"
    "   68: ANDI       r43, r41, 16777216\n"
    "   69: SET_ALIAS  a5, r42\n"
    "   70: GOTO_IF_Z  r43, L10\n"
    "   71: ORI        r44, r42, -2147483648\n"
    "   72: SET_ALIAS  a5, r44\n"
    "   73: LABEL      L10\n"
    "   74: LABEL      L8\n"
    "   75: ANDI       r45, r17, 128\n"
    "   76: GOTO_IF_Z  r45, L2\n"
    "   77: LABEL      L11\n"
    "   78: GET_ALIAS  r46, a6\n"
    "   79: ANDI       r47, r46, 31\n"
    "   80: SET_ALIAS  a6, r47\n"
    "   81: GOTO       L12\n"
    "   82: LABEL      L2\n"
    "   83: FTESTEXC   r48, r18, ZERO_DIVIDE\n"
    "   84: GOTO_IF_Z  r48, L13\n"
    "   85: NOT        r49, r17\n"
    "   86: ORI        r50, r17, 67108864\n"
    "   87: ANDI       r51, r49, 67108864\n"
    "   88: SET_ALIAS  a5, r50\n"
    "   89: GOTO_IF_Z  r51, L14\n"
    "   90: ORI        r52, r50, -2147483648\n"
    "   91: SET_ALIAS  a5, r52\n"
    "   92: LABEL      L14\n"
    "   93: ANDI       r53, r17, 16\n"
    "   94: GOTO_IF_NZ r53, L11\n"
    "   95: LABEL      L13\n"
    "   96: SET_ALIAS  a2, r7\n"
    "   97: BITCAST    r54, r7\n"
    "   98: SGTUI      r55, r54, 0\n"
    "   99: SLTSI      r56, r54, 0\n"
    "  100: BFEXT      r60, r54, 52, 11\n"
    "  101: SEQI       r57, r60, 0\n"
    "  102: SEQI       r58, r60, 2047\n"
    "  103: SLLI       r61, r54, 12\n"
    "  104: SEQI       r59, r61, 0\n"
    "  105: AND        r62, r57, r59\n"
    "  106: XORI       r63, r59, 1\n"
    "  107: AND        r64, r58, r63\n"
    "  108: AND        r65, r57, r55\n"
    "  109: OR         r66, r65, r64\n"
    "  110: OR         r67, r62, r64\n"
    "  111: XORI       r68, r67, 1\n"
    "  112: XORI       r69, r56, 1\n"
    "  113: AND        r70, r56, r68\n"
    "  114: AND        r71, r69, r68\n"
    "  115: SLLI       r72, r66, 4\n"
    "  116: SLLI       r73, r70, 3\n"
    "  117: SLLI       r74, r71, 2\n"
    "  118: SLLI       r75, r62, 1\n"
    "  119: OR         r76, r72, r73\n"
    "  120: OR         r77, r74, r75\n"
    "  121: OR         r78, r76, r58\n"
    "  122: OR         r79, r78, r77\n"
    "  123: FTESTEXC   r80, r18, INEXACT\n"
    "  124: SLLI       r81, r80, 5\n"
    "  125: OR         r82, r79, r81\n"
    "  126: SET_ALIAS  a6, r82\n"
    "  127: GOTO_IF_Z  r80, L15\n"
    "  128: GET_ALIAS  r83, a5\n"
    "  129: NOT        r84, r83\n"
    "  130: ORI        r85, r83, 33554432\n"
    "  131: ANDI       r86, r84, 33554432\n"
    "  132: SET_ALIAS  a5, r85\n"
    "  133: GOTO_IF_Z  r86, L16\n"
    "  134: ORI        r87, r85, -2147483648\n"
    "  135: SET_ALIAS  a5, r87\n"
    "  136: LABEL      L16\n"
    "  137: LABEL      L15\n"
    "  138: FTESTEXC   r88, r18, OVERFLOW\n"
    "  139: GOTO_IF_Z  r88, L17\n"
    "  140: GET_ALIAS  r89, a5\n"
    "  141: NOT        r90, r89\n"
    "  142: ORI        r91, r89, 268435456\n"
    "  143: ANDI       r92, r90, 268435456\n"
    "  144: SET_ALIAS  a5, r91\n"
    "  145: GOTO_IF_Z  r92, L18\n"
    "  146: ORI        r93, r91, -2147483648\n"
    "  147: SET_ALIAS  a5, r93\n"
    "  148: LABEL      L18\n"
    "  149: LABEL      L17\n"
    "  150: FTESTEXC   r94, r18, UNDERFLOW\n"
    "  151: GOTO_IF_Z  r94, L12\n"
    "  152: GET_ALIAS  r95, a5\n"
    "  153: NOT        r96, r95\n"
    "  154: ORI        r97, r95, 134217728\n"
    "  155: ANDI       r98, r96, 134217728\n"
    "  156: SET_ALIAS  a5, r97\n"
    "  157: GOTO_IF_Z  r98, L19\n"
    "  158: ORI        r99, r97, -2147483648\n"
    "  159: SET_ALIAS  a5, r99\n"
    "  160: LABEL      L19\n"
    "  161: LABEL      L12\n"
    "  162: LOAD_IMM   r100, 4\n"
    "  163: SET_ALIAS  a1, r100\n"
    "  164: GET_ALIAS  r101, a5\n"
    "  165: GET_ALIAS  r102, a6\n"
    "  166: ANDI       r103, r101, -1611134977\n"
    "  167: SLLI       r104, r102, 12\n"
    "  168: OR         r105, r103, r104\n"
    "  169: SET_ALIAS  a5, r105\n"
    "  170: RETURN\n"
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
    "Block 2: 1,0 --> [20,26] --> 3,19\n"
    "Block 3: 2 --> [27,31] --> 4,5\n"
    "Block 4: 3 --> [32,32] --> 5,14\n"
    "Block 5: 4,3 --> [33,38] --> 6,7\n"
    "Block 6: 5 --> [39,39] --> 7,14\n"
    "Block 7: 6,5 --> [40,45] --> 8,11\n"
    "Block 8: 7 --> [46,50] --> 9,10\n"
    "Block 9: 8 --> [51,52] --> 10\n"
    "Block 10: 9,8 --> [53,54] --> 17\n"
    "Block 11: 7 --> [55,60] --> 12,13\n"
    "Block 12: 11 --> [61,62] --> 13\n"
    "Block 13: 12,11 --> [63,64] --> 17\n"
    "Block 14: 4,6 --> [65,70] --> 15,16\n"
    "Block 15: 14 --> [71,72] --> 16\n"
    "Block 16: 15,14 --> [73,73] --> 17\n"
    "Block 17: 16,10,13 --> [74,76] --> 18,19\n"
    "Block 18: 17,22 --> [77,81] --> 35\n"
    "Block 19: 2,17 --> [82,84] --> 20,23\n"
    "Block 20: 19 --> [85,89] --> 21,22\n"
    "Block 21: 20 --> [90,91] --> 22\n"
    "Block 22: 21,20 --> [92,94] --> 23,18\n"
    "Block 23: 22,19 --> [95,127] --> 24,27\n"
    "Block 24: 23 --> [128,133] --> 25,26\n"
    "Block 25: 24 --> [134,135] --> 26\n"
    "Block 26: 25,24 --> [136,136] --> 27\n"
    "Block 27: 26,23 --> [137,139] --> 28,31\n"
    "Block 28: 27 --> [140,145] --> 29,30\n"
    "Block 29: 28 --> [146,147] --> 30\n"
    "Block 30: 29,28 --> [148,148] --> 31\n"
    "Block 31: 30,27 --> [149,151] --> 32,35\n"
    "Block 32: 31 --> [152,157] --> 33,34\n"
    "Block 33: 32 --> [158,159] --> 34\n"
    "Block 34: 33,32 --> [160,160] --> 35\n"
    "Block 35: 34,18,31 --> [161,170] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
