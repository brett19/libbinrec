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
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: GET_ALIAS  r4, a4\n"
    "    4: FDIV       r5, r3, r4\n"
    "    5: LOAD_IMM   r6, 0x20000000000000\n"
    "    6: BITCAST    r7, r5\n"
    "    7: SLLI       r8, r7, 1\n"
    "    8: SEQ        r9, r8, r6\n"
    "    9: GOTO_IF_Z  r9, L1\n"
    "   10: FGETSTATE  r10\n"
    "   11: FSETROUND  r11, r10, TRUNC\n"
    "   12: FSETSTATE  r11\n"
    "   13: FDIV       r12, r3, r4\n"
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
    "   38: BITCAST    r27, r3\n"
    "   39: BITCAST    r28, r4\n"
    "   40: OR         r29, r27, r28\n"
    "   41: SLLI       r30, r29, 1\n"
    "   42: GOTO_IF_NZ r30, L7\n"
    "   43: NOT        r31, r15\n"
    "   44: ORI        r32, r15, 2097152\n"
    "   45: ANDI       r33, r31, 2097152\n"
    "   46: SET_ALIAS  a5, r32\n"
    "   47: GOTO_IF_Z  r33, L8\n"
    "   48: ORI        r34, r32, -2147483648\n"
    "   49: SET_ALIAS  a5, r34\n"
    "   50: LABEL      L8\n"
    "   51: GOTO       L9\n"
    "   52: LABEL      L7\n"
    "   53: NOT        r35, r15\n"
    "   54: ORI        r36, r15, 4194304\n"
    "   55: ANDI       r37, r35, 4194304\n"
    "   56: SET_ALIAS  a5, r36\n"
    "   57: GOTO_IF_Z  r37, L10\n"
    "   58: ORI        r38, r36, -2147483648\n"
    "   59: SET_ALIAS  a5, r38\n"
    "   60: LABEL      L10\n"
    "   61: LABEL      L9\n"
    "   62: ANDI       r39, r15, 128\n"
    "   63: GOTO_IF_Z  r39, L11\n"
    "   64: GET_ALIAS  r40, a5\n"
    "   65: BFEXT      r41, r40, 12, 7\n"
    "   66: ANDI       r42, r41, 31\n"
    "   67: GET_ALIAS  r43, a5\n"
    "   68: BFINS      r44, r43, r42, 12, 7\n"
    "   69: SET_ALIAS  a5, r44\n"
    "   70: GOTO       L2\n"
    "   71: LABEL      L11\n"
    "   72: LOAD_IMM   r45, nan(0x8000000000000)\n"
    "   73: SET_ALIAS  a2, r45\n"
    "   74: LOAD_IMM   r46, 17\n"
    "   75: GET_ALIAS  r47, a5\n"
    "   76: BFINS      r48, r47, r46, 12, 7\n"
    "   77: SET_ALIAS  a5, r48\n"
    "   78: GOTO       L2\n"
    "   79: LABEL      L4\n"
    "   80: NOT        r49, r15\n"
    "   81: ORI        r50, r15, 16777216\n"
    "   82: ANDI       r51, r49, 16777216\n"
    "   83: SET_ALIAS  a5, r50\n"
    "   84: GOTO_IF_Z  r51, L12\n"
    "   85: ORI        r52, r50, -2147483648\n"
    "   86: SET_ALIAS  a5, r52\n"
    "   87: LABEL      L12\n"
    "   88: ANDI       r53, r15, 128\n"
    "   89: GOTO_IF_Z  r53, L3\n"
    "   90: LABEL      L13\n"
    "   91: GET_ALIAS  r54, a5\n"
    "   92: BFEXT      r55, r54, 12, 7\n"
    "   93: ANDI       r56, r55, 31\n"
    "   94: GET_ALIAS  r57, a5\n"
    "   95: BFINS      r58, r57, r56, 12, 7\n"
    "   96: SET_ALIAS  a5, r58\n"
    "   97: GOTO       L2\n"
    "   98: LABEL      L3\n"
    "   99: FTESTEXC   r59, r16, ZERO_DIVIDE\n"
    "  100: GOTO_IF_Z  r59, L14\n"
    "  101: NOT        r60, r15\n"
    "  102: ORI        r61, r15, 67108864\n"
    "  103: ANDI       r62, r60, 67108864\n"
    "  104: SET_ALIAS  a5, r61\n"
    "  105: GOTO_IF_Z  r62, L15\n"
    "  106: ORI        r63, r61, -2147483648\n"
    "  107: SET_ALIAS  a5, r63\n"
    "  108: LABEL      L15\n"
    "  109: ANDI       r64, r15, 16\n"
    "  110: GOTO_IF_NZ r64, L13\n"
    "  111: LABEL      L14\n"
    "  112: SET_ALIAS  a2, r5\n"
    "  113: BITCAST    r65, r5\n"
    "  114: SGTUI      r66, r65, 0\n"
    "  115: SLTSI      r67, r65, 0\n"
    "  116: BFEXT      r71, r65, 52, 11\n"
    "  117: SEQI       r68, r71, 0\n"
    "  118: SEQI       r69, r71, 2047\n"
    "  119: SLLI       r72, r65, 12\n"
    "  120: SEQI       r70, r72, 0\n"
    "  121: AND        r73, r68, r70\n"
    "  122: XORI       r74, r70, 1\n"
    "  123: AND        r75, r69, r74\n"
    "  124: AND        r76, r68, r66\n"
    "  125: OR         r77, r76, r75\n"
    "  126: OR         r78, r73, r75\n"
    "  127: XORI       r79, r78, 1\n"
    "  128: XORI       r80, r67, 1\n"
    "  129: AND        r81, r67, r79\n"
    "  130: AND        r82, r80, r79\n"
    "  131: SLLI       r83, r77, 4\n"
    "  132: SLLI       r84, r81, 3\n"
    "  133: SLLI       r85, r82, 2\n"
    "  134: SLLI       r86, r73, 1\n"
    "  135: OR         r87, r83, r84\n"
    "  136: OR         r88, r85, r86\n"
    "  137: OR         r89, r87, r69\n"
    "  138: OR         r90, r89, r88\n"
    "  139: FTESTEXC   r91, r16, INEXACT\n"
    "  140: SLLI       r92, r91, 5\n"
    "  141: OR         r93, r90, r92\n"
    "  142: GET_ALIAS  r94, a5\n"
    "  143: BFINS      r95, r94, r93, 12, 7\n"
    "  144: SET_ALIAS  a5, r95\n"
    "  145: GOTO_IF_Z  r91, L16\n"
    "  146: GET_ALIAS  r96, a5\n"
    "  147: NOT        r97, r96\n"
    "  148: ORI        r98, r96, 33554432\n"
    "  149: ANDI       r99, r97, 33554432\n"
    "  150: SET_ALIAS  a5, r98\n"
    "  151: GOTO_IF_Z  r99, L17\n"
    "  152: ORI        r100, r98, -2147483648\n"
    "  153: SET_ALIAS  a5, r100\n"
    "  154: LABEL      L17\n"
    "  155: LABEL      L16\n"
    "  156: FTESTEXC   r101, r16, OVERFLOW\n"
    "  157: GOTO_IF_Z  r101, L18\n"
    "  158: GET_ALIAS  r102, a5\n"
    "  159: NOT        r103, r102\n"
    "  160: ORI        r104, r102, 268435456\n"
    "  161: ANDI       r105, r103, 268435456\n"
    "  162: SET_ALIAS  a5, r104\n"
    "  163: GOTO_IF_Z  r105, L19\n"
    "  164: ORI        r106, r104, -2147483648\n"
    "  165: SET_ALIAS  a5, r106\n"
    "  166: LABEL      L19\n"
    "  167: LABEL      L18\n"
    "  168: FTESTEXC   r107, r16, UNDERFLOW\n"
    "  169: GOTO_IF_Z  r107, L2\n"
    "  170: GET_ALIAS  r108, a5\n"
    "  171: NOT        r109, r108\n"
    "  172: ORI        r110, r108, 134217728\n"
    "  173: ANDI       r111, r109, 134217728\n"
    "  174: SET_ALIAS  a5, r110\n"
    "  175: GOTO_IF_Z  r111, L20\n"
    "  176: ORI        r112, r110, -2147483648\n"
    "  177: SET_ALIAS  a5, r112\n"
    "  178: LABEL      L20\n"
    "  179: LABEL      L2\n"
    "  180: LOAD_IMM   r113, 4\n"
    "  181: SET_ALIAS  a1, r113\n"
    "  182: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: float64 @ 432(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,9] --> 1,2\n"
    "Block 1: 0 --> [10,16] --> 2\n"
    "Block 2: 1,0 --> [17,23] --> 3,21\n"
    "Block 3: 2 --> [24,28] --> 4,5\n"
    "Block 4: 3 --> [29,29] --> 5,17\n"
    "Block 5: 4,3 --> [30,35] --> 6,7\n"
    "Block 6: 5 --> [36,36] --> 7,17\n"
    "Block 7: 6,5 --> [37,42] --> 8,11\n"
    "Block 8: 7 --> [43,47] --> 9,10\n"
    "Block 9: 8 --> [48,49] --> 10\n"
    "Block 10: 9,8 --> [50,51] --> 14\n"
    "Block 11: 7 --> [52,57] --> 12,13\n"
    "Block 12: 11 --> [58,59] --> 13\n"
    "Block 13: 12,11 --> [60,60] --> 14\n"
    "Block 14: 13,10 --> [61,63] --> 15,16\n"
    "Block 15: 14 --> [64,70] --> 37\n"
    "Block 16: 14 --> [71,78] --> 37\n"
    "Block 17: 4,6 --> [79,84] --> 18,19\n"
    "Block 18: 17 --> [85,86] --> 19\n"
    "Block 19: 18,17 --> [87,89] --> 20,21\n"
    "Block 20: 19,24 --> [90,97] --> 37\n"
    "Block 21: 2,19 --> [98,100] --> 22,25\n"
    "Block 22: 21 --> [101,105] --> 23,24\n"
    "Block 23: 22 --> [106,107] --> 24\n"
    "Block 24: 23,22 --> [108,110] --> 25,20\n"
    "Block 25: 24,21 --> [111,145] --> 26,29\n"
    "Block 26: 25 --> [146,151] --> 27,28\n"
    "Block 27: 26 --> [152,153] --> 28\n"
    "Block 28: 27,26 --> [154,154] --> 29\n"
    "Block 29: 28,25 --> [155,157] --> 30,33\n"
    "Block 30: 29 --> [158,163] --> 31,32\n"
    "Block 31: 30 --> [164,165] --> 32\n"
    "Block 32: 31,30 --> [166,166] --> 33\n"
    "Block 33: 32,29 --> [167,169] --> 34,37\n"
    "Block 34: 33 --> [170,175] --> 35,36\n"
    "Block 35: 34 --> [176,177] --> 36\n"
    "Block 36: 35,34 --> [178,178] --> 37\n"
    "Block 37: 36,15,16,20,33 --> [179,182] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
