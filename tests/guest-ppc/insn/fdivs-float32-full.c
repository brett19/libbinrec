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
    "    2: GET_ALIAS  r3, a2\n"
    "    3: ZCAST      r4, r3\n"
    "    4: ADD        r5, r2, r4\n"
    "    5: LOAD_BR    r6, 0(r5)\n"
    "    6: ZCAST      r7, r3\n"
    "    7: ADD        r8, r2, r7\n"
    "    8: LOAD_BR    r9, 4(r8)\n"
    "    9: FDIV       r10, r6, r9\n"
    "   10: LOAD_IMM   r11, 0x1000000\n"
    "   11: BITCAST    r12, r10\n"
    "   12: SLLI       r13, r12, 1\n"
    "   13: SEQ        r14, r13, r11\n"
    "   14: GOTO_IF_Z  r14, L1\n"
    "   15: FGETSTATE  r15\n"
    "   16: FSETROUND  r16, r15, TRUNC\n"
    "   17: FSETSTATE  r16\n"
    "   18: FDIV       r17, r6, r9\n"
    "   19: FGETSTATE  r18\n"
    "   20: FCOPYROUND r19, r18, r15\n"
    "   21: FSETSTATE  r19\n"
    "   22: LABEL      L1\n"
    "   23: GET_ALIAS  r20, a6\n"
    "   24: FGETSTATE  r21\n"
    "   25: FCLEAREXC  r22, r21\n"
    "   26: FSETSTATE  r22\n"
    "   27: FTESTEXC   r23, r21, INVALID\n"
    "   28: GOTO_IF_Z  r23, L3\n"
    "   29: BITCAST    r24, r6\n"
    "   30: SLLI       r25, r24, 10\n"
    "   31: BFEXT      r26, r24, 22, 9\n"
    "   32: SEQI       r27, r26, 510\n"
    "   33: GOTO_IF_Z  r27, L5\n"
    "   34: GOTO_IF_NZ r25, L4\n"
    "   35: LABEL      L5\n"
    "   36: BITCAST    r28, r9\n"
    "   37: SLLI       r29, r28, 10\n"
    "   38: BFEXT      r30, r28, 22, 9\n"
    "   39: SEQI       r31, r30, 510\n"
    "   40: GOTO_IF_Z  r31, L6\n"
    "   41: GOTO_IF_NZ r29, L4\n"
    "   42: LABEL      L6\n"
    "   43: BITCAST    r32, r6\n"
    "   44: BITCAST    r33, r9\n"
    "   45: OR         r34, r32, r33\n"
    "   46: SLLI       r35, r34, 1\n"
    "   47: GOTO_IF_NZ r35, L7\n"
    "   48: NOT        r36, r20\n"
    "   49: ORI        r37, r20, 2097152\n"
    "   50: ANDI       r38, r36, 2097152\n"
    "   51: SET_ALIAS  a6, r37\n"
    "   52: GOTO_IF_Z  r38, L8\n"
    "   53: ORI        r39, r37, -2147483648\n"
    "   54: SET_ALIAS  a6, r39\n"
    "   55: LABEL      L8\n"
    "   56: GOTO       L9\n"
    "   57: LABEL      L7\n"
    "   58: NOT        r40, r20\n"
    "   59: ORI        r41, r20, 4194304\n"
    "   60: ANDI       r42, r40, 4194304\n"
    "   61: SET_ALIAS  a6, r41\n"
    "   62: GOTO_IF_Z  r42, L10\n"
    "   63: ORI        r43, r41, -2147483648\n"
    "   64: SET_ALIAS  a6, r43\n"
    "   65: LABEL      L10\n"
    "   66: LABEL      L9\n"
    "   67: ANDI       r44, r20, 128\n"
    "   68: GOTO_IF_Z  r44, L11\n"
    "   69: GET_ALIAS  r45, a6\n"
    "   70: BFEXT      r46, r45, 12, 7\n"
    "   71: ANDI       r47, r46, 31\n"
    "   72: GET_ALIAS  r48, a6\n"
    "   73: BFINS      r49, r48, r47, 12, 7\n"
    "   74: SET_ALIAS  a6, r49\n"
    "   75: GOTO       L2\n"
    "   76: LABEL      L11\n"
    "   77: LOAD_IMM   r50, nan(0x400000)\n"
    "   78: FCVT       r51, r50\n"
    "   79: STORE      408(r1), r51\n"
    "   80: SET_ALIAS  a3, r51\n"
    "   81: LOAD_IMM   r52, 17\n"
    "   82: GET_ALIAS  r53, a6\n"
    "   83: BFINS      r54, r53, r52, 12, 7\n"
    "   84: SET_ALIAS  a6, r54\n"
    "   85: GOTO       L2\n"
    "   86: LABEL      L4\n"
    "   87: NOT        r55, r20\n"
    "   88: ORI        r56, r20, 16777216\n"
    "   89: ANDI       r57, r55, 16777216\n"
    "   90: SET_ALIAS  a6, r56\n"
    "   91: GOTO_IF_Z  r57, L12\n"
    "   92: ORI        r58, r56, -2147483648\n"
    "   93: SET_ALIAS  a6, r58\n"
    "   94: LABEL      L12\n"
    "   95: ANDI       r59, r20, 128\n"
    "   96: GOTO_IF_Z  r59, L3\n"
    "   97: LABEL      L13\n"
    "   98: GET_ALIAS  r60, a6\n"
    "   99: BFEXT      r61, r60, 12, 7\n"
    "  100: ANDI       r62, r61, 31\n"
    "  101: GET_ALIAS  r63, a6\n"
    "  102: BFINS      r64, r63, r62, 12, 7\n"
    "  103: SET_ALIAS  a6, r64\n"
    "  104: GOTO       L2\n"
    "  105: LABEL      L3\n"
    "  106: FTESTEXC   r65, r21, ZERO_DIVIDE\n"
    "  107: GOTO_IF_Z  r65, L14\n"
    "  108: NOT        r66, r20\n"
    "  109: ORI        r67, r20, 67108864\n"
    "  110: ANDI       r68, r66, 67108864\n"
    "  111: SET_ALIAS  a6, r67\n"
    "  112: GOTO_IF_Z  r68, L15\n"
    "  113: ORI        r69, r67, -2147483648\n"
    "  114: SET_ALIAS  a6, r69\n"
    "  115: LABEL      L15\n"
    "  116: ANDI       r70, r20, 16\n"
    "  117: GOTO_IF_NZ r70, L13\n"
    "  118: LABEL      L14\n"
    "  119: FCVT       r71, r10\n"
    "  120: STORE      408(r1), r71\n"
    "  121: SET_ALIAS  a3, r71\n"
    "  122: BITCAST    r72, r10\n"
    "  123: SGTUI      r73, r72, 0\n"
    "  124: SRLI       r74, r72, 31\n"
    "  125: BFEXT      r78, r72, 23, 8\n"
    "  126: SEQI       r75, r78, 0\n"
    "  127: SEQI       r76, r78, 255\n"
    "  128: SLLI       r79, r72, 9\n"
    "  129: SEQI       r77, r79, 0\n"
    "  130: AND        r80, r75, r77\n"
    "  131: XORI       r81, r77, 1\n"
    "  132: AND        r82, r76, r81\n"
    "  133: AND        r83, r75, r73\n"
    "  134: OR         r84, r83, r82\n"
    "  135: OR         r85, r80, r82\n"
    "  136: XORI       r86, r85, 1\n"
    "  137: XORI       r87, r74, 1\n"
    "  138: AND        r88, r74, r86\n"
    "  139: AND        r89, r87, r86\n"
    "  140: SLLI       r90, r84, 4\n"
    "  141: SLLI       r91, r88, 3\n"
    "  142: SLLI       r92, r89, 2\n"
    "  143: SLLI       r93, r80, 1\n"
    "  144: OR         r94, r90, r91\n"
    "  145: OR         r95, r92, r93\n"
    "  146: OR         r96, r94, r76\n"
    "  147: OR         r97, r96, r95\n"
    "  148: FTESTEXC   r98, r21, INEXACT\n"
    "  149: SLLI       r99, r98, 5\n"
    "  150: OR         r100, r97, r99\n"
    "  151: GET_ALIAS  r101, a6\n"
    "  152: BFINS      r102, r101, r100, 12, 7\n"
    "  153: SET_ALIAS  a6, r102\n"
    "  154: GOTO_IF_Z  r98, L16\n"
    "  155: GET_ALIAS  r103, a6\n"
    "  156: NOT        r104, r103\n"
    "  157: ORI        r105, r103, 33554432\n"
    "  158: ANDI       r106, r104, 33554432\n"
    "  159: SET_ALIAS  a6, r105\n"
    "  160: GOTO_IF_Z  r106, L17\n"
    "  161: ORI        r107, r105, -2147483648\n"
    "  162: SET_ALIAS  a6, r107\n"
    "  163: LABEL      L17\n"
    "  164: LABEL      L16\n"
    "  165: FTESTEXC   r108, r21, OVERFLOW\n"
    "  166: GOTO_IF_Z  r108, L18\n"
    "  167: GET_ALIAS  r109, a6\n"
    "  168: NOT        r110, r109\n"
    "  169: ORI        r111, r109, 268435456\n"
    "  170: ANDI       r112, r110, 268435456\n"
    "  171: SET_ALIAS  a6, r111\n"
    "  172: GOTO_IF_Z  r112, L19\n"
    "  173: ORI        r113, r111, -2147483648\n"
    "  174: SET_ALIAS  a6, r113\n"
    "  175: LABEL      L19\n"
    "  176: LABEL      L18\n"
    "  177: FTESTEXC   r114, r21, UNDERFLOW\n"
    "  178: GOTO_IF_Z  r114, L2\n"
    "  179: GET_ALIAS  r115, a6\n"
    "  180: NOT        r116, r115\n"
    "  181: ORI        r117, r115, 134217728\n"
    "  182: ANDI       r118, r116, 134217728\n"
    "  183: SET_ALIAS  a6, r117\n"
    "  184: GOTO_IF_Z  r118, L20\n"
    "  185: ORI        r119, r117, -2147483648\n"
    "  186: SET_ALIAS  a6, r119\n"
    "  187: LABEL      L20\n"
    "  188: LABEL      L2\n"
    "  189: FGETSTATE  r120\n"
    "  190: FCMP       r121, r6, r6, UN\n"
    "  191: FCVT       r122, r6\n"
    "  192: SET_ALIAS  a7, r122\n"
    "  193: GOTO_IF_Z  r121, L21\n"
    "  194: BITCAST    r123, r6\n"
    "  195: ANDI       r124, r123, 4194304\n"
    "  196: GOTO_IF_NZ r124, L21\n"
    "  197: BITCAST    r125, r122\n"
    "  198: LOAD_IMM   r126, 0x8000000000000\n"
    "  199: XOR        r127, r125, r126\n"
    "  200: BITCAST    r128, r127\n"
    "  201: SET_ALIAS  a7, r128\n"
    "  202: LABEL      L21\n"
    "  203: GET_ALIAS  r129, a7\n"
    "  204: FSETSTATE  r120\n"
    "  205: STORE      424(r1), r129\n"
    "  206: SET_ALIAS  a4, r129\n"
    "  207: FGETSTATE  r130\n"
    "  208: FCMP       r131, r9, r9, UN\n"
    "  209: FCVT       r132, r9\n"
    "  210: SET_ALIAS  a8, r132\n"
    "  211: GOTO_IF_Z  r131, L22\n"
    "  212: BITCAST    r133, r9\n"
    "  213: ANDI       r134, r133, 4194304\n"
    "  214: GOTO_IF_NZ r134, L22\n"
    "  215: BITCAST    r135, r132\n"
    "  216: LOAD_IMM   r136, 0x8000000000000\n"
    "  217: XOR        r137, r135, r136\n"
    "  218: BITCAST    r138, r137\n"
    "  219: SET_ALIAS  a8, r138\n"
    "  220: LABEL      L22\n"
    "  221: GET_ALIAS  r139, a8\n"
    "  222: FSETSTATE  r130\n"
    "  223: STORE      440(r1), r139\n"
    "  224: SET_ALIAS  a5, r139\n"
    "  225: LOAD_IMM   r140, 12\n"
    "  226: SET_ALIAS  a1, r140\n"
    "  227: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64 @ 400(r1)\n"
    "Alias 4: float64 @ 416(r1)\n"
    "Alias 5: float64 @ 432(r1)\n"
    "Alias 6: int32 @ 944(r1)\n"
    "Alias 7: float64, no bound storage\n"
    "Alias 8: float64, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,14] --> 1,2\n"
    "Block 1: 0 --> [15,21] --> 2\n"
    "Block 2: 1,0 --> [22,28] --> 3,21\n"
    "Block 3: 2 --> [29,33] --> 4,5\n"
    "Block 4: 3 --> [34,34] --> 5,17\n"
    "Block 5: 4,3 --> [35,40] --> 6,7\n"
    "Block 6: 5 --> [41,41] --> 7,17\n"
    "Block 7: 6,5 --> [42,47] --> 8,11\n"
    "Block 8: 7 --> [48,52] --> 9,10\n"
    "Block 9: 8 --> [53,54] --> 10\n"
    "Block 10: 9,8 --> [55,56] --> 14\n"
    "Block 11: 7 --> [57,62] --> 12,13\n"
    "Block 12: 11 --> [63,64] --> 13\n"
    "Block 13: 12,11 --> [65,65] --> 14\n"
    "Block 14: 13,10 --> [66,68] --> 15,16\n"
    "Block 15: 14 --> [69,75] --> 37\n"
    "Block 16: 14 --> [76,85] --> 37\n"
    "Block 17: 4,6 --> [86,91] --> 18,19\n"
    "Block 18: 17 --> [92,93] --> 19\n"
    "Block 19: 18,17 --> [94,96] --> 20,21\n"
    "Block 20: 19,24 --> [97,104] --> 37\n"
    "Block 21: 2,19 --> [105,107] --> 22,25\n"
    "Block 22: 21 --> [108,112] --> 23,24\n"
    "Block 23: 22 --> [113,114] --> 24\n"
    "Block 24: 23,22 --> [115,117] --> 25,20\n"
    "Block 25: 24,21 --> [118,154] --> 26,29\n"
    "Block 26: 25 --> [155,160] --> 27,28\n"
    "Block 27: 26 --> [161,162] --> 28\n"
    "Block 28: 27,26 --> [163,163] --> 29\n"
    "Block 29: 28,25 --> [164,166] --> 30,33\n"
    "Block 30: 29 --> [167,172] --> 31,32\n"
    "Block 31: 30 --> [173,174] --> 32\n"
    "Block 32: 31,30 --> [175,175] --> 33\n"
    "Block 33: 32,29 --> [176,178] --> 34,37\n"
    "Block 34: 33 --> [179,184] --> 35,36\n"
    "Block 35: 34 --> [185,186] --> 36\n"
    "Block 36: 35,34 --> [187,187] --> 37\n"
    "Block 37: 36,15,16,20,33 --> [188,193] --> 38,40\n"
    "Block 38: 37 --> [194,196] --> 39,40\n"
    "Block 39: 38 --> [197,201] --> 40\n"
    "Block 40: 39,37,38 --> [202,211] --> 41,43\n"
    "Block 41: 40 --> [212,214] --> 42,43\n"
    "Block 42: 41 --> [215,219] --> 43\n"
    "Block 43: 42,40,41 --> [220,227] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
