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
    0xC0,0x43,0x00,0x00,  // lfs f2,0(r3)
    0xC0,0x63,0x00,0x04,  // lfs f3,4(r3)
    0xEC,0x22,0x18,0x24,  // fdivs f1,f2,f3
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0xB\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a6\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a7, r4\n"
    "    5: GET_ALIAS  r5, a2\n"
    "    6: ZCAST      r6, r5\n"
    "    7: ADD        r7, r2, r6\n"
    "    8: LOAD_BR    r8, 0(r7)\n"
    "    9: ZCAST      r9, r5\n"
    "   10: ADD        r10, r2, r9\n"
    "   11: LOAD_BR    r11, 4(r10)\n"
    "   12: FDIV       r12, r8, r11\n"
    "   13: LOAD_IMM   r13, 0x1000000\n"
    "   14: BITCAST    r14, r12\n"
    "   15: SLLI       r15, r14, 1\n"
    "   16: SEQ        r16, r15, r13\n"
    "   17: GOTO_IF_Z  r16, L1\n"
    "   18: FGETSTATE  r17\n"
    "   19: FSETROUND  r18, r17, TRUNC\n"
    "   20: FSETSTATE  r18\n"
    "   21: FDIV       r19, r8, r11\n"
    "   22: FGETSTATE  r20\n"
    "   23: FCOPYROUND r21, r20, r17\n"
    "   24: FSETSTATE  r21\n"
    "   25: LABEL      L1\n"
    "   26: GET_ALIAS  r22, a6\n"
    "   27: FGETSTATE  r23\n"
    "   28: FCLEAREXC  r24, r23\n"
    "   29: FSETSTATE  r24\n"
    "   30: FTESTEXC   r25, r23, INVALID\n"
    "   31: GOTO_IF_Z  r25, L2\n"
    "   32: BITCAST    r26, r8\n"
    "   33: SLLI       r27, r26, 10\n"
    "   34: BFEXT      r28, r26, 22, 9\n"
    "   35: SEQI       r29, r28, 510\n"
    "   36: GOTO_IF_Z  r29, L4\n"
    "   37: GOTO_IF_NZ r27, L3\n"
    "   38: LABEL      L4\n"
    "   39: BITCAST    r30, r11\n"
    "   40: SLLI       r31, r30, 10\n"
    "   41: BFEXT      r32, r30, 22, 9\n"
    "   42: SEQI       r33, r32, 510\n"
    "   43: GOTO_IF_Z  r33, L5\n"
    "   44: GOTO_IF_NZ r31, L3\n"
    "   45: LABEL      L5\n"
    "   46: BITCAST    r34, r8\n"
    "   47: BITCAST    r35, r11\n"
    "   48: OR         r36, r34, r35\n"
    "   49: SLLI       r37, r36, 1\n"
    "   50: GOTO_IF_NZ r37, L6\n"
    "   51: NOT        r38, r22\n"
    "   52: ORI        r39, r22, 2097152\n"
    "   53: ANDI       r40, r38, 2097152\n"
    "   54: SET_ALIAS  a6, r39\n"
    "   55: GOTO_IF_Z  r40, L7\n"
    "   56: ORI        r41, r39, -2147483648\n"
    "   57: SET_ALIAS  a6, r41\n"
    "   58: LABEL      L7\n"
    "   59: GOTO       L8\n"
    "   60: LABEL      L6\n"
    "   61: NOT        r42, r22\n"
    "   62: ORI        r43, r22, 4194304\n"
    "   63: ANDI       r44, r42, 4194304\n"
    "   64: SET_ALIAS  a6, r43\n"
    "   65: GOTO_IF_Z  r44, L9\n"
    "   66: ORI        r45, r43, -2147483648\n"
    "   67: SET_ALIAS  a6, r45\n"
    "   68: LABEL      L9\n"
    "   69: LABEL      L8\n"
    "   70: ANDI       r46, r22, 128\n"
    "   71: GOTO_IF_Z  r46, L10\n"
    "   72: GET_ALIAS  r47, a7\n"
    "   73: ANDI       r48, r47, 31\n"
    "   74: SET_ALIAS  a7, r48\n"
    "   75: GOTO       L11\n"
    "   76: LABEL      L10\n"
    "   77: LOAD_IMM   r49, nan(0x400000)\n"
    "   78: FCVT       r50, r49\n"
    "   79: STORE      408(r1), r50\n"
    "   80: SET_ALIAS  a3, r50\n"
    "   81: LOAD_IMM   r51, 17\n"
    "   82: SET_ALIAS  a7, r51\n"
    "   83: GOTO       L11\n"
    "   84: LABEL      L3\n"
    "   85: NOT        r52, r22\n"
    "   86: ORI        r53, r22, 16777216\n"
    "   87: ANDI       r54, r52, 16777216\n"
    "   88: SET_ALIAS  a6, r53\n"
    "   89: GOTO_IF_Z  r54, L12\n"
    "   90: ORI        r55, r53, -2147483648\n"
    "   91: SET_ALIAS  a6, r55\n"
    "   92: LABEL      L12\n"
    "   93: ANDI       r56, r22, 128\n"
    "   94: GOTO_IF_Z  r56, L2\n"
    "   95: LABEL      L13\n"
    "   96: GET_ALIAS  r57, a7\n"
    "   97: ANDI       r58, r57, 31\n"
    "   98: SET_ALIAS  a7, r58\n"
    "   99: GOTO       L11\n"
    "  100: LABEL      L2\n"
    "  101: FTESTEXC   r59, r23, ZERO_DIVIDE\n"
    "  102: GOTO_IF_Z  r59, L14\n"
    "  103: NOT        r60, r22\n"
    "  104: ORI        r61, r22, 67108864\n"
    "  105: ANDI       r62, r60, 67108864\n"
    "  106: SET_ALIAS  a6, r61\n"
    "  107: GOTO_IF_Z  r62, L15\n"
    "  108: ORI        r63, r61, -2147483648\n"
    "  109: SET_ALIAS  a6, r63\n"
    "  110: LABEL      L15\n"
    "  111: ANDI       r64, r22, 16\n"
    "  112: GOTO_IF_NZ r64, L13\n"
    "  113: LABEL      L14\n"
    "  114: FCVT       r65, r12\n"
    "  115: STORE      408(r1), r65\n"
    "  116: SET_ALIAS  a3, r65\n"
    "  117: BITCAST    r66, r12\n"
    "  118: SGTUI      r67, r66, 0\n"
    "  119: SRLI       r68, r66, 31\n"
    "  120: BFEXT      r72, r66, 23, 8\n"
    "  121: SEQI       r69, r72, 0\n"
    "  122: SEQI       r70, r72, 255\n"
    "  123: SLLI       r73, r66, 9\n"
    "  124: SEQI       r71, r73, 0\n"
    "  125: AND        r74, r69, r71\n"
    "  126: XORI       r75, r71, 1\n"
    "  127: AND        r76, r70, r75\n"
    "  128: AND        r77, r69, r67\n"
    "  129: OR         r78, r77, r76\n"
    "  130: OR         r79, r74, r76\n"
    "  131: XORI       r80, r79, 1\n"
    "  132: XORI       r81, r68, 1\n"
    "  133: AND        r82, r68, r80\n"
    "  134: AND        r83, r81, r80\n"
    "  135: SLLI       r84, r78, 4\n"
    "  136: SLLI       r85, r82, 3\n"
    "  137: SLLI       r86, r83, 2\n"
    "  138: SLLI       r87, r74, 1\n"
    "  139: OR         r88, r84, r85\n"
    "  140: OR         r89, r86, r87\n"
    "  141: OR         r90, r88, r70\n"
    "  142: OR         r91, r90, r89\n"
    "  143: FTESTEXC   r92, r23, INEXACT\n"
    "  144: SLLI       r93, r92, 5\n"
    "  145: OR         r94, r91, r93\n"
    "  146: SET_ALIAS  a7, r94\n"
    "  147: GOTO_IF_Z  r92, L16\n"
    "  148: GET_ALIAS  r95, a6\n"
    "  149: NOT        r96, r95\n"
    "  150: ORI        r97, r95, 33554432\n"
    "  151: ANDI       r98, r96, 33554432\n"
    "  152: SET_ALIAS  a6, r97\n"
    "  153: GOTO_IF_Z  r98, L17\n"
    "  154: ORI        r99, r97, -2147483648\n"
    "  155: SET_ALIAS  a6, r99\n"
    "  156: LABEL      L17\n"
    "  157: LABEL      L16\n"
    "  158: FTESTEXC   r100, r23, OVERFLOW\n"
    "  159: GOTO_IF_Z  r100, L18\n"
    "  160: GET_ALIAS  r101, a6\n"
    "  161: NOT        r102, r101\n"
    "  162: ORI        r103, r101, 268435456\n"
    "  163: ANDI       r104, r102, 268435456\n"
    "  164: SET_ALIAS  a6, r103\n"
    "  165: GOTO_IF_Z  r104, L19\n"
    "  166: ORI        r105, r103, -2147483648\n"
    "  167: SET_ALIAS  a6, r105\n"
    "  168: LABEL      L19\n"
    "  169: LABEL      L18\n"
    "  170: FTESTEXC   r106, r23, UNDERFLOW\n"
    "  171: GOTO_IF_Z  r106, L11\n"
    "  172: GET_ALIAS  r107, a6\n"
    "  173: NOT        r108, r107\n"
    "  174: ORI        r109, r107, 134217728\n"
    "  175: ANDI       r110, r108, 134217728\n"
    "  176: SET_ALIAS  a6, r109\n"
    "  177: GOTO_IF_Z  r110, L20\n"
    "  178: ORI        r111, r109, -2147483648\n"
    "  179: SET_ALIAS  a6, r111\n"
    "  180: LABEL      L20\n"
    "  181: LABEL      L11\n"
    "  182: FCAST      r112, r8\n"
    "  183: STORE      424(r1), r112\n"
    "  184: SET_ALIAS  a4, r112\n"
    "  185: FCAST      r113, r11\n"
    "  186: STORE      440(r1), r113\n"
    "  187: SET_ALIAS  a5, r113\n"
    "  188: LOAD_IMM   r114, 12\n"
    "  189: SET_ALIAS  a1, r114\n"
    "  190: GET_ALIAS  r115, a6\n"
    "  191: GET_ALIAS  r116, a7\n"
    "  192: ANDI       r117, r115, -1611134977\n"
    "  193: SLLI       r118, r116, 12\n"
    "  194: OR         r119, r117, r118\n"
    "  195: SET_ALIAS  a6, r119\n"
    "  196: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64 @ 400(r1)\n"
    "Alias 4: float64 @ 416(r1)\n"
    "Alias 5: float64 @ 432(r1)\n"
    "Alias 6: int32 @ 944(r1)\n"
    "Alias 7: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,17] --> 1,2\n"
    "Block 1: 0 --> [18,24] --> 2\n"
    "Block 2: 1,0 --> [25,31] --> 3,21\n"
    "Block 3: 2 --> [32,36] --> 4,5\n"
    "Block 4: 3 --> [37,37] --> 5,17\n"
    "Block 5: 4,3 --> [38,43] --> 6,7\n"
    "Block 6: 5 --> [44,44] --> 7,17\n"
    "Block 7: 6,5 --> [45,50] --> 8,11\n"
    "Block 8: 7 --> [51,55] --> 9,10\n"
    "Block 9: 8 --> [56,57] --> 10\n"
    "Block 10: 9,8 --> [58,59] --> 14\n"
    "Block 11: 7 --> [60,65] --> 12,13\n"
    "Block 12: 11 --> [66,67] --> 13\n"
    "Block 13: 12,11 --> [68,68] --> 14\n"
    "Block 14: 13,10 --> [69,71] --> 15,16\n"
    "Block 15: 14 --> [72,75] --> 37\n"
    "Block 16: 14 --> [76,83] --> 37\n"
    "Block 17: 4,6 --> [84,89] --> 18,19\n"
    "Block 18: 17 --> [90,91] --> 19\n"
    "Block 19: 18,17 --> [92,94] --> 20,21\n"
    "Block 20: 19,24 --> [95,99] --> 37\n"
    "Block 21: 2,19 --> [100,102] --> 22,25\n"
    "Block 22: 21 --> [103,107] --> 23,24\n"
    "Block 23: 22 --> [108,109] --> 24\n"
    "Block 24: 23,22 --> [110,112] --> 25,20\n"
    "Block 25: 24,21 --> [113,147] --> 26,29\n"
    "Block 26: 25 --> [148,153] --> 27,28\n"
    "Block 27: 26 --> [154,155] --> 28\n"
    "Block 28: 27,26 --> [156,156] --> 29\n"
    "Block 29: 28,25 --> [157,159] --> 30,33\n"
    "Block 30: 29 --> [160,165] --> 31,32\n"
    "Block 31: 30 --> [166,167] --> 32\n"
    "Block 32: 31,30 --> [168,168] --> 33\n"
    "Block 33: 32,29 --> [169,171] --> 34,37\n"
    "Block 34: 33 --> [172,177] --> 35,36\n"
    "Block 35: 34 --> [178,179] --> 36\n"
    "Block 36: 35,34 --> [180,180] --> 37\n"
    "Block 37: 36,15,16,20,33 --> [181,196] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
