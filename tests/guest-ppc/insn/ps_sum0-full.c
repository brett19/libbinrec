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
    0x10,0x22,0x19,0x14,  // ps_sum0 f1,f2,f4,f3
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a6\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a7, r4\n"
    "    5: GET_ALIAS  r5, a3\n"
    "    6: VEXTRACT   r6, r5, 0\n"
    "    7: GET_ALIAS  r7, a4\n"
    "    8: VEXTRACT   r8, r7, 1\n"
    "    9: GET_ALIAS  r9, a5\n"
    "   10: VEXTRACT   r10, r9, 1\n"
    "   11: FCAST      r11, r10\n"
    "   12: FADD       r12, r6, r8\n"
    "   13: FCVT       r13, r12\n"
    "   14: LOAD_IMM   r14, 0x1000000\n"
    "   15: BITCAST    r15, r13\n"
    "   16: SLLI       r16, r15, 1\n"
    "   17: SEQ        r17, r16, r14\n"
    "   18: GOTO_IF_Z  r17, L1\n"
    "   19: FGETSTATE  r18\n"
    "   20: FSETROUND  r19, r18, TRUNC\n"
    "   21: FSETSTATE  r19\n"
    "   22: FADD       r20, r6, r8\n"
    "   23: FCVT       r21, r20\n"
    "   24: FGETSTATE  r22\n"
    "   25: FCOPYROUND r23, r22, r18\n"
    "   26: FSETSTATE  r23\n"
    "   27: LABEL      L1\n"
    "   28: FGETSTATE  r24\n"
    "   29: FTESTEXC   r25, r24, INVALID\n"
    "   30: GOTO_IF_Z  r25, L2\n"
    "   31: GET_ALIAS  r26, a6\n"
    "   32: FGETSTATE  r27\n"
    "   33: FCLEAREXC  r28, r27\n"
    "   34: FSETSTATE  r28\n"
    "   35: FTESTEXC   r29, r27, INVALID\n"
    "   36: GOTO_IF_Z  r29, L3\n"
    "   37: BITCAST    r30, r6\n"
    "   38: SLLI       r31, r30, 13\n"
    "   39: BFEXT      r32, r30, 51, 12\n"
    "   40: SEQI       r33, r32, 4094\n"
    "   41: GOTO_IF_Z  r33, L5\n"
    "   42: GOTO_IF_NZ r31, L4\n"
    "   43: LABEL      L5\n"
    "   44: BITCAST    r34, r8\n"
    "   45: SLLI       r35, r34, 13\n"
    "   46: BFEXT      r36, r34, 51, 12\n"
    "   47: SEQI       r37, r36, 4094\n"
    "   48: GOTO_IF_Z  r37, L6\n"
    "   49: GOTO_IF_NZ r35, L4\n"
    "   50: LABEL      L6\n"
    "   51: NOT        r38, r26\n"
    "   52: ORI        r39, r26, 8388608\n"
    "   53: ANDI       r40, r38, 8388608\n"
    "   54: SET_ALIAS  a6, r39\n"
    "   55: GOTO_IF_Z  r40, L7\n"
    "   56: ORI        r41, r39, -2147483648\n"
    "   57: SET_ALIAS  a6, r41\n"
    "   58: LABEL      L7\n"
    "   59: ANDI       r42, r26, 128\n"
    "   60: GOTO_IF_Z  r42, L8\n"
    "   61: GET_ALIAS  r43, a7\n"
    "   62: ANDI       r44, r43, 31\n"
    "   63: SET_ALIAS  a7, r44\n"
    "   64: GOTO       L9\n"
    "   65: LABEL      L8\n"
    "   66: LOAD_IMM   r45, nan(0x400000)\n"
    "   67: FCVT       r46, r45\n"
    "   68: VBROADCAST r47, r46\n"
    "   69: SET_ALIAS  a2, r47\n"
    "   70: LOAD_IMM   r48, 17\n"
    "   71: SET_ALIAS  a7, r48\n"
    "   72: GOTO       L9\n"
    "   73: LABEL      L4\n"
    "   74: NOT        r49, r26\n"
    "   75: ORI        r50, r26, 16777216\n"
    "   76: ANDI       r51, r49, 16777216\n"
    "   77: SET_ALIAS  a6, r50\n"
    "   78: GOTO_IF_Z  r51, L10\n"
    "   79: ORI        r52, r50, -2147483648\n"
    "   80: SET_ALIAS  a6, r52\n"
    "   81: LABEL      L10\n"
    "   82: ANDI       r53, r26, 128\n"
    "   83: GOTO_IF_Z  r53, L3\n"
    "   84: GET_ALIAS  r54, a7\n"
    "   85: ANDI       r55, r54, 31\n"
    "   86: SET_ALIAS  a7, r55\n"
    "   87: GOTO       L9\n"
    "   88: LABEL      L3\n"
    "   89: FCVT       r56, r13\n"
    "   90: VBROADCAST r57, r56\n"
    "   91: SET_ALIAS  a2, r57\n"
    "   92: BITCAST    r58, r13\n"
    "   93: SGTUI      r59, r58, 0\n"
    "   94: SRLI       r60, r58, 31\n"
    "   95: BFEXT      r64, r58, 23, 8\n"
    "   96: SEQI       r61, r64, 0\n"
    "   97: SEQI       r62, r64, 255\n"
    "   98: SLLI       r65, r58, 9\n"
    "   99: SEQI       r63, r65, 0\n"
    "  100: AND        r66, r61, r63\n"
    "  101: XORI       r67, r63, 1\n"
    "  102: AND        r68, r62, r67\n"
    "  103: AND        r69, r61, r59\n"
    "  104: OR         r70, r69, r68\n"
    "  105: OR         r71, r66, r68\n"
    "  106: XORI       r72, r71, 1\n"
    "  107: XORI       r73, r60, 1\n"
    "  108: AND        r74, r60, r72\n"
    "  109: AND        r75, r73, r72\n"
    "  110: SLLI       r76, r70, 4\n"
    "  111: SLLI       r77, r74, 3\n"
    "  112: SLLI       r78, r75, 2\n"
    "  113: SLLI       r79, r66, 1\n"
    "  114: OR         r80, r76, r77\n"
    "  115: OR         r81, r78, r79\n"
    "  116: OR         r82, r80, r62\n"
    "  117: OR         r83, r82, r81\n"
    "  118: FTESTEXC   r84, r27, INEXACT\n"
    "  119: SLLI       r85, r84, 5\n"
    "  120: OR         r86, r83, r85\n"
    "  121: SET_ALIAS  a7, r86\n"
    "  122: GOTO_IF_Z  r84, L11\n"
    "  123: GET_ALIAS  r87, a6\n"
    "  124: NOT        r88, r87\n"
    "  125: ORI        r89, r87, 33554432\n"
    "  126: ANDI       r90, r88, 33554432\n"
    "  127: SET_ALIAS  a6, r89\n"
    "  128: GOTO_IF_Z  r90, L12\n"
    "  129: ORI        r91, r89, -2147483648\n"
    "  130: SET_ALIAS  a6, r91\n"
    "  131: LABEL      L12\n"
    "  132: LABEL      L11\n"
    "  133: FTESTEXC   r92, r27, OVERFLOW\n"
    "  134: GOTO_IF_Z  r92, L13\n"
    "  135: GET_ALIAS  r93, a6\n"
    "  136: NOT        r94, r93\n"
    "  137: ORI        r95, r93, 268435456\n"
    "  138: ANDI       r96, r94, 268435456\n"
    "  139: SET_ALIAS  a6, r95\n"
    "  140: GOTO_IF_Z  r96, L14\n"
    "  141: ORI        r97, r95, -2147483648\n"
    "  142: SET_ALIAS  a6, r97\n"
    "  143: LABEL      L14\n"
    "  144: LABEL      L13\n"
    "  145: FTESTEXC   r98, r27, UNDERFLOW\n"
    "  146: GOTO_IF_Z  r98, L9\n"
    "  147: GET_ALIAS  r99, a6\n"
    "  148: NOT        r100, r99\n"
    "  149: ORI        r101, r99, 134217728\n"
    "  150: ANDI       r102, r100, 134217728\n"
    "  151: SET_ALIAS  a6, r101\n"
    "  152: GOTO_IF_Z  r102, L15\n"
    "  153: ORI        r103, r101, -2147483648\n"
    "  154: SET_ALIAS  a6, r103\n"
    "  155: LABEL      L15\n"
    "  156: LABEL      L9\n"
    "  157: GET_ALIAS  r104, a6\n"
    "  158: ANDI       r105, r104, 128\n"
    "  159: GOTO_IF_NZ r105, L16\n"
    "  160: GET_ALIAS  r106, a2\n"
    "  161: VEXTRACT   r107, r106, 0\n"
    "  162: FCAST      r108, r11\n"
    "  163: VBUILD2    r109, r107, r108\n"
    "  164: SET_ALIAS  a2, r109\n"
    "  165: GOTO       L16\n"
    "  166: LABEL      L2\n"
    "  167: VBUILD2    r110, r13, r11\n"
    "  168: GET_ALIAS  r111, a6\n"
    "  169: FGETSTATE  r112\n"
    "  170: FCLEAREXC  r113, r112\n"
    "  171: FSETSTATE  r113\n"
    "  172: FTESTEXC   r114, r112, INVALID\n"
    "  173: GOTO_IF_Z  r114, L17\n"
    "  174: NOT        r115, r111\n"
    "  175: ORI        r116, r111, 16777216\n"
    "  176: ANDI       r117, r115, 16777216\n"
    "  177: SET_ALIAS  a6, r116\n"
    "  178: GOTO_IF_Z  r117, L18\n"
    "  179: ORI        r118, r116, -2147483648\n"
    "  180: SET_ALIAS  a6, r118\n"
    "  181: LABEL      L18\n"
    "  182: ANDI       r119, r111, 128\n"
    "  183: GOTO_IF_Z  r119, L17\n"
    "  184: GET_ALIAS  r120, a7\n"
    "  185: ANDI       r121, r120, 31\n"
    "  186: SET_ALIAS  a7, r121\n"
    "  187: GOTO       L19\n"
    "  188: LABEL      L17\n"
    "  189: VFCAST     r122, r110\n"
    "  190: SET_ALIAS  a2, r122\n"
    "  191: VEXTRACT   r123, r110, 0\n"
    "  192: BITCAST    r124, r123\n"
    "  193: SGTUI      r125, r124, 0\n"
    "  194: SRLI       r126, r124, 31\n"
    "  195: BFEXT      r130, r124, 23, 8\n"
    "  196: SEQI       r127, r130, 0\n"
    "  197: SEQI       r128, r130, 255\n"
    "  198: SLLI       r131, r124, 9\n"
    "  199: SEQI       r129, r131, 0\n"
    "  200: AND        r132, r127, r129\n"
    "  201: XORI       r133, r129, 1\n"
    "  202: AND        r134, r128, r133\n"
    "  203: AND        r135, r127, r125\n"
    "  204: OR         r136, r135, r134\n"
    "  205: OR         r137, r132, r134\n"
    "  206: XORI       r138, r137, 1\n"
    "  207: XORI       r139, r126, 1\n"
    "  208: AND        r140, r126, r138\n"
    "  209: AND        r141, r139, r138\n"
    "  210: SLLI       r142, r136, 4\n"
    "  211: SLLI       r143, r140, 3\n"
    "  212: SLLI       r144, r141, 2\n"
    "  213: SLLI       r145, r132, 1\n"
    "  214: OR         r146, r142, r143\n"
    "  215: OR         r147, r144, r145\n"
    "  216: OR         r148, r146, r128\n"
    "  217: OR         r149, r148, r147\n"
    "  218: FTESTEXC   r150, r112, INEXACT\n"
    "  219: SLLI       r151, r150, 5\n"
    "  220: OR         r152, r149, r151\n"
    "  221: SET_ALIAS  a7, r152\n"
    "  222: GOTO_IF_Z  r150, L20\n"
    "  223: GET_ALIAS  r153, a6\n"
    "  224: NOT        r154, r153\n"
    "  225: ORI        r155, r153, 33554432\n"
    "  226: ANDI       r156, r154, 33554432\n"
    "  227: SET_ALIAS  a6, r155\n"
    "  228: GOTO_IF_Z  r156, L21\n"
    "  229: ORI        r157, r155, -2147483648\n"
    "  230: SET_ALIAS  a6, r157\n"
    "  231: LABEL      L21\n"
    "  232: LABEL      L20\n"
    "  233: FTESTEXC   r158, r112, OVERFLOW\n"
    "  234: GOTO_IF_Z  r158, L22\n"
    "  235: GET_ALIAS  r159, a6\n"
    "  236: NOT        r160, r159\n"
    "  237: ORI        r161, r159, 268435456\n"
    "  238: ANDI       r162, r160, 268435456\n"
    "  239: SET_ALIAS  a6, r161\n"
    "  240: GOTO_IF_Z  r162, L23\n"
    "  241: ORI        r163, r161, -2147483648\n"
    "  242: SET_ALIAS  a6, r163\n"
    "  243: LABEL      L23\n"
    "  244: LABEL      L22\n"
    "  245: FTESTEXC   r164, r112, UNDERFLOW\n"
    "  246: GOTO_IF_Z  r164, L19\n"
    "  247: GET_ALIAS  r165, a6\n"
    "  248: NOT        r166, r165\n"
    "  249: ORI        r167, r165, 134217728\n"
    "  250: ANDI       r168, r166, 134217728\n"
    "  251: SET_ALIAS  a6, r167\n"
    "  252: GOTO_IF_Z  r168, L24\n"
    "  253: ORI        r169, r167, -2147483648\n"
    "  254: SET_ALIAS  a6, r169\n"
    "  255: LABEL      L24\n"
    "  256: LABEL      L19\n"
    "  257: LABEL      L16\n"
    "  258: LOAD_IMM   r170, 4\n"
    "  259: SET_ALIAS  a1, r170\n"
    "  260: GET_ALIAS  r171, a6\n"
    "  261: GET_ALIAS  r172, a7\n"
    "  262: ANDI       r173, r171, -1611134977\n"
    "  263: SLLI       r174, r172, 12\n"
    "  264: OR         r175, r173, r174\n"
    "  265: SET_ALIAS  a6, r175\n"
    "  266: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: float64[2] @ 432(r1)\n"
    "Alias 5: float64[2] @ 448(r1)\n"
    "Alias 6: int32 @ 944(r1)\n"
    "Alias 7: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,18] --> 1,2\n"
    "Block 1: 0 --> [19,26] --> 2\n"
    "Block 2: 1,0 --> [27,30] --> 3,31\n"
    "Block 3: 2 --> [31,36] --> 4,17\n"
    "Block 4: 3 --> [37,41] --> 5,6\n"
    "Block 5: 4 --> [42,42] --> 6,13\n"
    "Block 6: 5,4 --> [43,48] --> 7,8\n"
    "Block 7: 6 --> [49,49] --> 8,13\n"
    "Block 8: 7,6 --> [50,55] --> 9,10\n"
    "Block 9: 8 --> [56,57] --> 10\n"
    "Block 10: 9,8 --> [58,60] --> 11,12\n"
    "Block 11: 10 --> [61,64] --> 29\n"
    "Block 12: 10 --> [65,72] --> 29\n"
    "Block 13: 5,7 --> [73,78] --> 14,15\n"
    "Block 14: 13 --> [79,80] --> 15\n"
    "Block 15: 14,13 --> [81,83] --> 16,17\n"
    "Block 16: 15 --> [84,87] --> 29\n"
    "Block 17: 3,15 --> [88,122] --> 18,21\n"
    "Block 18: 17 --> [123,128] --> 19,20\n"
    "Block 19: 18 --> [129,130] --> 20\n"
    "Block 20: 19,18 --> [131,131] --> 21\n"
    "Block 21: 20,17 --> [132,134] --> 22,25\n"
    "Block 22: 21 --> [135,140] --> 23,24\n"
    "Block 23: 22 --> [141,142] --> 24\n"
    "Block 24: 23,22 --> [143,143] --> 25\n"
    "Block 25: 24,21 --> [144,146] --> 26,29\n"
    "Block 26: 25 --> [147,152] --> 27,28\n"
    "Block 27: 26 --> [153,154] --> 28\n"
    "Block 28: 27,26 --> [155,155] --> 29\n"
    "Block 29: 28,11,12,16,25 --> [156,159] --> 30,49\n"
    "Block 30: 29 --> [160,165] --> 49\n"
    "Block 31: 2 --> [166,173] --> 32,36\n"
    "Block 32: 31 --> [174,178] --> 33,34\n"
    "Block 33: 32 --> [179,180] --> 34\n"
    "Block 34: 33,32 --> [181,183] --> 35,36\n"
    "Block 35: 34 --> [184,187] --> 48\n"
    "Block 36: 31,34 --> [188,222] --> 37,40\n"
    "Block 37: 36 --> [223,228] --> 38,39\n"
    "Block 38: 37 --> [229,230] --> 39\n"
    "Block 39: 38,37 --> [231,231] --> 40\n"
    "Block 40: 39,36 --> [232,234] --> 41,44\n"
    "Block 41: 40 --> [235,240] --> 42,43\n"
    "Block 42: 41 --> [241,242] --> 43\n"
    "Block 43: 42,41 --> [243,243] --> 44\n"
    "Block 44: 43,40 --> [244,246] --> 45,48\n"
    "Block 45: 44 --> [247,252] --> 46,47\n"
    "Block 46: 45 --> [253,254] --> 47\n"
    "Block 47: 46,45 --> [255,255] --> 48\n"
    "Block 48: 47,35,44 --> [256,256] --> 49\n"
    "Block 49: 48,29,30 --> [257,266] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
