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
    0xC0,0x23,0x00,0x00,  // lfs f3,0(r3)
    0x10,0x22,0x18,0x2A,  // ps_add f1,f2,f3
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x7\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a6\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a7, r4\n"
    "    5: GET_ALIAS  r5, a2\n"
    "    6: ZCAST      r6, r5\n"
    "    7: ADD        r7, r2, r6\n"
    "    8: LOAD_BR    r8, 0(r7)\n"
    "    9: GET_ALIAS  r9, a4\n"
    "   10: GET_ALIAS  r10, a5\n"
    "   11: FADD       r11, r9, r10\n"
    "   12: VFCVT      r12, r11\n"
    "   13: LOAD_IMM   r13, 0x1000000\n"
    "   14: VEXTRACT   r14, r12, 1\n"
    "   15: BITCAST    r15, r14\n"
    "   16: SLLI       r16, r15, 1\n"
    "   17: SEQ        r17, r16, r13\n"
    "   18: GOTO_IF_NZ r17, L1\n"
    "   19: VEXTRACT   r18, r12, 0\n"
    "   20: BITCAST    r19, r18\n"
    "   21: SLLI       r20, r19, 1\n"
    "   22: SEQ        r21, r20, r13\n"
    "   23: GOTO_IF_Z  r21, L2\n"
    "   24: LABEL      L1\n"
    "   25: FGETSTATE  r22\n"
    "   26: FSETROUND  r23, r22, TRUNC\n"
    "   27: FSETSTATE  r23\n"
    "   28: FADD       r24, r9, r10\n"
    "   29: VFCVT      r25, r24\n"
    "   30: FGETSTATE  r26\n"
    "   31: FCOPYROUND r27, r26, r22\n"
    "   32: FSETSTATE  r27\n"
    "   33: LABEL      L2\n"
    "   34: FGETSTATE  r28\n"
    "   35: FTESTEXC   r29, r28, INVALID\n"
    "   36: GOTO_IF_Z  r29, L3\n"
    "   37: FCLEAREXC  r30, r28\n"
    "   38: FSETSTATE  r30\n"
    "   39: VEXTRACT   r31, r9, 0\n"
    "   40: VEXTRACT   r32, r10, 0\n"
    "   41: FADD       r33, r31, r32\n"
    "   42: FCVT       r34, r33\n"
    "   43: GET_ALIAS  r35, a6\n"
    "   44: FGETSTATE  r36\n"
    "   45: FCLEAREXC  r37, r36\n"
    "   46: FSETSTATE  r37\n"
    "   47: FTESTEXC   r38, r36, INVALID\n"
    "   48: GOTO_IF_Z  r38, L5\n"
    "   49: BITCAST    r39, r31\n"
    "   50: SLLI       r40, r39, 13\n"
    "   51: BFEXT      r41, r39, 51, 12\n"
    "   52: SEQI       r42, r41, 4094\n"
    "   53: GOTO_IF_Z  r42, L7\n"
    "   54: GOTO_IF_NZ r40, L6\n"
    "   55: LABEL      L7\n"
    "   56: BITCAST    r43, r32\n"
    "   57: SLLI       r44, r43, 13\n"
    "   58: BFEXT      r45, r43, 51, 12\n"
    "   59: SEQI       r46, r45, 4094\n"
    "   60: GOTO_IF_Z  r46, L8\n"
    "   61: GOTO_IF_NZ r44, L6\n"
    "   62: LABEL      L8\n"
    "   63: NOT        r47, r35\n"
    "   64: ORI        r48, r35, 8388608\n"
    "   65: ANDI       r49, r47, 8388608\n"
    "   66: SET_ALIAS  a6, r48\n"
    "   67: GOTO_IF_Z  r49, L9\n"
    "   68: ORI        r50, r48, -2147483648\n"
    "   69: SET_ALIAS  a6, r50\n"
    "   70: LABEL      L9\n"
    "   71: ANDI       r51, r35, 128\n"
    "   72: GOTO_IF_Z  r51, L10\n"
    "   73: GET_ALIAS  r52, a7\n"
    "   74: ANDI       r53, r52, 31\n"
    "   75: SET_ALIAS  a7, r53\n"
    "   76: GOTO       L4\n"
    "   77: LABEL      L10\n"
    "   78: LOAD_IMM   r54, nan(0x400000)\n"
    "   79: FCVT       r55, r54\n"
    "   80: VBROADCAST r56, r55\n"
    "   81: SET_ALIAS  a3, r56\n"
    "   82: LOAD_IMM   r57, 17\n"
    "   83: SET_ALIAS  a7, r57\n"
    "   84: GOTO       L4\n"
    "   85: LABEL      L6\n"
    "   86: NOT        r58, r35\n"
    "   87: ORI        r59, r35, 16777216\n"
    "   88: ANDI       r60, r58, 16777216\n"
    "   89: SET_ALIAS  a6, r59\n"
    "   90: GOTO_IF_Z  r60, L11\n"
    "   91: ORI        r61, r59, -2147483648\n"
    "   92: SET_ALIAS  a6, r61\n"
    "   93: LABEL      L11\n"
    "   94: ANDI       r62, r35, 128\n"
    "   95: GOTO_IF_Z  r62, L5\n"
    "   96: GET_ALIAS  r63, a7\n"
    "   97: ANDI       r64, r63, 31\n"
    "   98: SET_ALIAS  a7, r64\n"
    "   99: GOTO       L4\n"
    "  100: LABEL      L5\n"
    "  101: FCVT       r65, r34\n"
    "  102: VBROADCAST r66, r65\n"
    "  103: SET_ALIAS  a3, r66\n"
    "  104: BITCAST    r67, r34\n"
    "  105: SGTUI      r68, r67, 0\n"
    "  106: SRLI       r69, r67, 31\n"
    "  107: BFEXT      r73, r67, 23, 8\n"
    "  108: SEQI       r70, r73, 0\n"
    "  109: SEQI       r71, r73, 255\n"
    "  110: SLLI       r74, r67, 9\n"
    "  111: SEQI       r72, r74, 0\n"
    "  112: AND        r75, r70, r72\n"
    "  113: XORI       r76, r72, 1\n"
    "  114: AND        r77, r71, r76\n"
    "  115: AND        r78, r70, r68\n"
    "  116: OR         r79, r78, r77\n"
    "  117: OR         r80, r75, r77\n"
    "  118: XORI       r81, r80, 1\n"
    "  119: XORI       r82, r69, 1\n"
    "  120: AND        r83, r69, r81\n"
    "  121: AND        r84, r82, r81\n"
    "  122: SLLI       r85, r79, 4\n"
    "  123: SLLI       r86, r83, 3\n"
    "  124: SLLI       r87, r84, 2\n"
    "  125: SLLI       r88, r75, 1\n"
    "  126: OR         r89, r85, r86\n"
    "  127: OR         r90, r87, r88\n"
    "  128: OR         r91, r89, r71\n"
    "  129: OR         r92, r91, r90\n"
    "  130: FTESTEXC   r93, r36, INEXACT\n"
    "  131: SLLI       r94, r93, 5\n"
    "  132: OR         r95, r92, r94\n"
    "  133: SET_ALIAS  a7, r95\n"
    "  134: GOTO_IF_Z  r93, L12\n"
    "  135: GET_ALIAS  r96, a6\n"
    "  136: NOT        r97, r96\n"
    "  137: ORI        r98, r96, 33554432\n"
    "  138: ANDI       r99, r97, 33554432\n"
    "  139: SET_ALIAS  a6, r98\n"
    "  140: GOTO_IF_Z  r99, L13\n"
    "  141: ORI        r100, r98, -2147483648\n"
    "  142: SET_ALIAS  a6, r100\n"
    "  143: LABEL      L13\n"
    "  144: LABEL      L12\n"
    "  145: FTESTEXC   r101, r36, OVERFLOW\n"
    "  146: GOTO_IF_Z  r101, L14\n"
    "  147: GET_ALIAS  r102, a6\n"
    "  148: NOT        r103, r102\n"
    "  149: ORI        r104, r102, 268435456\n"
    "  150: ANDI       r105, r103, 268435456\n"
    "  151: SET_ALIAS  a6, r104\n"
    "  152: GOTO_IF_Z  r105, L15\n"
    "  153: ORI        r106, r104, -2147483648\n"
    "  154: SET_ALIAS  a6, r106\n"
    "  155: LABEL      L15\n"
    "  156: LABEL      L14\n"
    "  157: FTESTEXC   r107, r36, UNDERFLOW\n"
    "  158: GOTO_IF_Z  r107, L4\n"
    "  159: GET_ALIAS  r108, a6\n"
    "  160: NOT        r109, r108\n"
    "  161: ORI        r110, r108, 134217728\n"
    "  162: ANDI       r111, r109, 134217728\n"
    "  163: SET_ALIAS  a6, r110\n"
    "  164: GOTO_IF_Z  r111, L16\n"
    "  165: ORI        r112, r110, -2147483648\n"
    "  166: SET_ALIAS  a6, r112\n"
    "  167: LABEL      L16\n"
    "  168: LABEL      L4\n"
    "  169: GET_ALIAS  r113, a3\n"
    "  170: VEXTRACT   r114, r113, 0\n"
    "  171: GET_ALIAS  r115, a7\n"
    "  172: VEXTRACT   r116, r9, 1\n"
    "  173: VEXTRACT   r117, r10, 1\n"
    "  174: FADD       r118, r116, r117\n"
    "  175: FCVT       r119, r118\n"
    "  176: GET_ALIAS  r120, a6\n"
    "  177: FGETSTATE  r121\n"
    "  178: FCLEAREXC  r122, r121\n"
    "  179: FSETSTATE  r122\n"
    "  180: FTESTEXC   r123, r121, INVALID\n"
    "  181: GOTO_IF_Z  r123, L18\n"
    "  182: BITCAST    r124, r116\n"
    "  183: SLLI       r125, r124, 13\n"
    "  184: BFEXT      r126, r124, 51, 12\n"
    "  185: SEQI       r127, r126, 4094\n"
    "  186: GOTO_IF_Z  r127, L20\n"
    "  187: GOTO_IF_NZ r125, L19\n"
    "  188: LABEL      L20\n"
    "  189: BITCAST    r128, r117\n"
    "  190: SLLI       r129, r128, 13\n"
    "  191: BFEXT      r130, r128, 51, 12\n"
    "  192: SEQI       r131, r130, 4094\n"
    "  193: GOTO_IF_Z  r131, L21\n"
    "  194: GOTO_IF_NZ r129, L19\n"
    "  195: LABEL      L21\n"
    "  196: NOT        r132, r120\n"
    "  197: ORI        r133, r120, 8388608\n"
    "  198: ANDI       r134, r132, 8388608\n"
    "  199: SET_ALIAS  a6, r133\n"
    "  200: GOTO_IF_Z  r134, L22\n"
    "  201: ORI        r135, r133, -2147483648\n"
    "  202: SET_ALIAS  a6, r135\n"
    "  203: LABEL      L22\n"
    "  204: ANDI       r136, r120, 128\n"
    "  205: GOTO_IF_Z  r136, L23\n"
    "  206: GET_ALIAS  r137, a7\n"
    "  207: ANDI       r138, r137, 31\n"
    "  208: SET_ALIAS  a7, r138\n"
    "  209: GOTO       L17\n"
    "  210: LABEL      L23\n"
    "  211: LOAD_IMM   r139, nan(0x400000)\n"
    "  212: FCVT       r140, r139\n"
    "  213: VBROADCAST r141, r140\n"
    "  214: SET_ALIAS  a3, r141\n"
    "  215: LOAD_IMM   r142, 17\n"
    "  216: SET_ALIAS  a7, r142\n"
    "  217: GOTO       L17\n"
    "  218: LABEL      L19\n"
    "  219: NOT        r143, r120\n"
    "  220: ORI        r144, r120, 16777216\n"
    "  221: ANDI       r145, r143, 16777216\n"
    "  222: SET_ALIAS  a6, r144\n"
    "  223: GOTO_IF_Z  r145, L24\n"
    "  224: ORI        r146, r144, -2147483648\n"
    "  225: SET_ALIAS  a6, r146\n"
    "  226: LABEL      L24\n"
    "  227: ANDI       r147, r120, 128\n"
    "  228: GOTO_IF_Z  r147, L18\n"
    "  229: GET_ALIAS  r148, a7\n"
    "  230: ANDI       r149, r148, 31\n"
    "  231: SET_ALIAS  a7, r149\n"
    "  232: GOTO       L17\n"
    "  233: LABEL      L18\n"
    "  234: FCVT       r150, r119\n"
    "  235: VBROADCAST r151, r150\n"
    "  236: SET_ALIAS  a3, r151\n"
    "  237: BITCAST    r152, r119\n"
    "  238: SGTUI      r153, r152, 0\n"
    "  239: SRLI       r154, r152, 31\n"
    "  240: BFEXT      r158, r152, 23, 8\n"
    "  241: SEQI       r155, r158, 0\n"
    "  242: SEQI       r156, r158, 255\n"
    "  243: SLLI       r159, r152, 9\n"
    "  244: SEQI       r157, r159, 0\n"
    "  245: AND        r160, r155, r157\n"
    "  246: XORI       r161, r157, 1\n"
    "  247: AND        r162, r156, r161\n"
    "  248: AND        r163, r155, r153\n"
    "  249: OR         r164, r163, r162\n"
    "  250: OR         r165, r160, r162\n"
    "  251: XORI       r166, r165, 1\n"
    "  252: XORI       r167, r154, 1\n"
    "  253: AND        r168, r154, r166\n"
    "  254: AND        r169, r167, r166\n"
    "  255: SLLI       r170, r164, 4\n"
    "  256: SLLI       r171, r168, 3\n"
    "  257: SLLI       r172, r169, 2\n"
    "  258: SLLI       r173, r160, 1\n"
    "  259: OR         r174, r170, r171\n"
    "  260: OR         r175, r172, r173\n"
    "  261: OR         r176, r174, r156\n"
    "  262: OR         r177, r176, r175\n"
    "  263: FTESTEXC   r178, r121, INEXACT\n"
    "  264: SLLI       r179, r178, 5\n"
    "  265: OR         r180, r177, r179\n"
    "  266: SET_ALIAS  a7, r180\n"
    "  267: GOTO_IF_Z  r178, L25\n"
    "  268: GET_ALIAS  r181, a6\n"
    "  269: NOT        r182, r181\n"
    "  270: ORI        r183, r181, 33554432\n"
    "  271: ANDI       r184, r182, 33554432\n"
    "  272: SET_ALIAS  a6, r183\n"
    "  273: GOTO_IF_Z  r184, L26\n"
    "  274: ORI        r185, r183, -2147483648\n"
    "  275: SET_ALIAS  a6, r185\n"
    "  276: LABEL      L26\n"
    "  277: LABEL      L25\n"
    "  278: FTESTEXC   r186, r121, OVERFLOW\n"
    "  279: GOTO_IF_Z  r186, L27\n"
    "  280: GET_ALIAS  r187, a6\n"
    "  281: NOT        r188, r187\n"
    "  282: ORI        r189, r187, 268435456\n"
    "  283: ANDI       r190, r188, 268435456\n"
    "  284: SET_ALIAS  a6, r189\n"
    "  285: GOTO_IF_Z  r190, L28\n"
    "  286: ORI        r191, r189, -2147483648\n"
    "  287: SET_ALIAS  a6, r191\n"
    "  288: LABEL      L28\n"
    "  289: LABEL      L27\n"
    "  290: FTESTEXC   r192, r121, UNDERFLOW\n"
    "  291: GOTO_IF_Z  r192, L17\n"
    "  292: GET_ALIAS  r193, a6\n"
    "  293: NOT        r194, r193\n"
    "  294: ORI        r195, r193, 134217728\n"
    "  295: ANDI       r196, r194, 134217728\n"
    "  296: SET_ALIAS  a6, r195\n"
    "  297: GOTO_IF_Z  r196, L29\n"
    "  298: ORI        r197, r195, -2147483648\n"
    "  299: SET_ALIAS  a6, r197\n"
    "  300: LABEL      L29\n"
    "  301: LABEL      L17\n"
    "  302: GET_ALIAS  r198, a3\n"
    "  303: VEXTRACT   r199, r198, 0\n"
    "  304: GET_ALIAS  r200, a7\n"
    "  305: ANDI       r201, r200, 32\n"
    "  306: OR         r202, r115, r201\n"
    "  307: SET_ALIAS  a7, r202\n"
    "  308: GET_ALIAS  r203, a6\n"
    "  309: ANDI       r204, r203, 128\n"
    "  310: GOTO_IF_Z  r204, L30\n"
    "  311: FCAST      r205, r8\n"
    "  312: VBROADCAST r206, r205\n"
    "  313: SET_ALIAS  a3, r206\n"
    "  314: GOTO       L31\n"
    "  315: LABEL      L30\n"
    "  316: VBUILD2    r207, r114, r199\n"
    "  317: SET_ALIAS  a3, r207\n"
    "  318: GOTO       L31\n"
    "  319: LABEL      L3\n"
    "  320: GET_ALIAS  r208, a6\n"
    "  321: FGETSTATE  r209\n"
    "  322: FCLEAREXC  r210, r209\n"
    "  323: FSETSTATE  r210\n"
    "  324: VFCVT      r211, r12\n"
    "  325: SET_ALIAS  a3, r211\n"
    "  326: VEXTRACT   r212, r12, 0\n"
    "  327: BITCAST    r213, r212\n"
    "  328: SGTUI      r214, r213, 0\n"
    "  329: SRLI       r215, r213, 31\n"
    "  330: BFEXT      r219, r213, 23, 8\n"
    "  331: SEQI       r216, r219, 0\n"
    "  332: SEQI       r217, r219, 255\n"
    "  333: SLLI       r220, r213, 9\n"
    "  334: SEQI       r218, r220, 0\n"
    "  335: AND        r221, r216, r218\n"
    "  336: XORI       r222, r218, 1\n"
    "  337: AND        r223, r217, r222\n"
    "  338: AND        r224, r216, r214\n"
    "  339: OR         r225, r224, r223\n"
    "  340: OR         r226, r221, r223\n"
    "  341: XORI       r227, r226, 1\n"
    "  342: XORI       r228, r215, 1\n"
    "  343: AND        r229, r215, r227\n"
    "  344: AND        r230, r228, r227\n"
    "  345: SLLI       r231, r225, 4\n"
    "  346: SLLI       r232, r229, 3\n"
    "  347: SLLI       r233, r230, 2\n"
    "  348: SLLI       r234, r221, 1\n"
    "  349: OR         r235, r231, r232\n"
    "  350: OR         r236, r233, r234\n"
    "  351: OR         r237, r235, r217\n"
    "  352: OR         r238, r237, r236\n"
    "  353: FTESTEXC   r239, r209, INEXACT\n"
    "  354: SLLI       r240, r239, 5\n"
    "  355: OR         r241, r238, r240\n"
    "  356: SET_ALIAS  a7, r241\n"
    "  357: GOTO_IF_Z  r239, L33\n"
    "  358: GET_ALIAS  r242, a6\n"
    "  359: NOT        r243, r242\n"
    "  360: ORI        r244, r242, 33554432\n"
    "  361: ANDI       r245, r243, 33554432\n"
    "  362: SET_ALIAS  a6, r244\n"
    "  363: GOTO_IF_Z  r245, L34\n"
    "  364: ORI        r246, r244, -2147483648\n"
    "  365: SET_ALIAS  a6, r246\n"
    "  366: LABEL      L34\n"
    "  367: LABEL      L33\n"
    "  368: FTESTEXC   r247, r209, OVERFLOW\n"
    "  369: GOTO_IF_Z  r247, L35\n"
    "  370: GET_ALIAS  r248, a6\n"
    "  371: NOT        r249, r248\n"
    "  372: ORI        r250, r248, 268435456\n"
    "  373: ANDI       r251, r249, 268435456\n"
    "  374: SET_ALIAS  a6, r250\n"
    "  375: GOTO_IF_Z  r251, L36\n"
    "  376: ORI        r252, r250, -2147483648\n"
    "  377: SET_ALIAS  a6, r252\n"
    "  378: LABEL      L36\n"
    "  379: LABEL      L35\n"
    "  380: FTESTEXC   r253, r209, UNDERFLOW\n"
    "  381: GOTO_IF_Z  r253, L32\n"
    "  382: GET_ALIAS  r254, a6\n"
    "  383: NOT        r255, r254\n"
    "  384: ORI        r256, r254, 134217728\n"
    "  385: ANDI       r257, r255, 134217728\n"
    "  386: SET_ALIAS  a6, r256\n"
    "  387: GOTO_IF_Z  r257, L37\n"
    "  388: ORI        r258, r256, -2147483648\n"
    "  389: SET_ALIAS  a6, r258\n"
    "  390: LABEL      L37\n"
    "  391: LABEL      L32\n"
    "  392: LABEL      L31\n"
    "  393: LOAD_IMM   r259, 8\n"
    "  394: SET_ALIAS  a1, r259\n"
    "  395: GET_ALIAS  r260, a6\n"
    "  396: GET_ALIAS  r261, a7\n"
    "  397: ANDI       r262, r260, -1611134977\n"
    "  398: SLLI       r263, r261, 12\n"
    "  399: OR         r264, r262, r263\n"
    "  400: SET_ALIAS  a6, r264\n"
    "  401: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64[2] @ 400(r1)\n"
    "Alias 4: float64[2] @ 416(r1)\n"
    "Alias 5: float64[2] @ 432(r1)\n"
    "Alias 6: int32 @ 944(r1)\n"
    "Alias 7: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,18] --> 1,2\n"
    "Block 1: 0 --> [19,23] --> 2,3\n"
    "Block 2: 1,0 --> [24,32] --> 3\n"
    "Block 3: 2,1 --> [33,36] --> 4,59\n"
    "Block 4: 3 --> [37,48] --> 5,18\n"
    "Block 5: 4 --> [49,53] --> 6,7\n"
    "Block 6: 5 --> [54,54] --> 7,14\n"
    "Block 7: 6,5 --> [55,60] --> 8,9\n"
    "Block 8: 7 --> [61,61] --> 9,14\n"
    "Block 9: 8,7 --> [62,67] --> 10,11\n"
    "Block 10: 9 --> [68,69] --> 11\n"
    "Block 11: 10,9 --> [70,72] --> 12,13\n"
    "Block 12: 11 --> [73,76] --> 30\n"
    "Block 13: 11 --> [77,84] --> 30\n"
    "Block 14: 6,8 --> [85,90] --> 15,16\n"
    "Block 15: 14 --> [91,92] --> 16\n"
    "Block 16: 15,14 --> [93,95] --> 17,18\n"
    "Block 17: 16 --> [96,99] --> 30\n"
    "Block 18: 4,16 --> [100,134] --> 19,22\n"
    "Block 19: 18 --> [135,140] --> 20,21\n"
    "Block 20: 19 --> [141,142] --> 21\n"
    "Block 21: 20,19 --> [143,143] --> 22\n"
    "Block 22: 21,18 --> [144,146] --> 23,26\n"
    "Block 23: 22 --> [147,152] --> 24,25\n"
    "Block 24: 23 --> [153,154] --> 25\n"
    "Block 25: 24,23 --> [155,155] --> 26\n"
    "Block 26: 25,22 --> [156,158] --> 27,30\n"
    "Block 27: 26 --> [159,164] --> 28,29\n"
    "Block 28: 27 --> [165,166] --> 29\n"
    "Block 29: 28,27 --> [167,167] --> 30\n"
    "Block 30: 29,12,13,17,26 --> [168,181] --> 31,44\n"
    "Block 31: 30 --> [182,186] --> 32,33\n"
    "Block 32: 31 --> [187,187] --> 33,40\n"
    "Block 33: 32,31 --> [188,193] --> 34,35\n"
    "Block 34: 33 --> [194,194] --> 35,40\n"
    "Block 35: 34,33 --> [195,200] --> 36,37\n"
    "Block 36: 35 --> [201,202] --> 37\n"
    "Block 37: 36,35 --> [203,205] --> 38,39\n"
    "Block 38: 37 --> [206,209] --> 56\n"
    "Block 39: 37 --> [210,217] --> 56\n"
    "Block 40: 32,34 --> [218,223] --> 41,42\n"
    "Block 41: 40 --> [224,225] --> 42\n"
    "Block 42: 41,40 --> [226,228] --> 43,44\n"
    "Block 43: 42 --> [229,232] --> 56\n"
    "Block 44: 30,42 --> [233,267] --> 45,48\n"
    "Block 45: 44 --> [268,273] --> 46,47\n"
    "Block 46: 45 --> [274,275] --> 47\n"
    "Block 47: 46,45 --> [276,276] --> 48\n"
    "Block 48: 47,44 --> [277,279] --> 49,52\n"
    "Block 49: 48 --> [280,285] --> 50,51\n"
    "Block 50: 49 --> [286,287] --> 51\n"
    "Block 51: 50,49 --> [288,288] --> 52\n"
    "Block 52: 51,48 --> [289,291] --> 53,56\n"
    "Block 53: 52 --> [292,297] --> 54,55\n"
    "Block 54: 53 --> [298,299] --> 55\n"
    "Block 55: 54,53 --> [300,300] --> 56\n"
    "Block 56: 55,38,39,43,52 --> [301,310] --> 57,58\n"
    "Block 57: 56 --> [311,314] --> 72\n"
    "Block 58: 56 --> [315,318] --> 72\n"
    "Block 59: 3 --> [319,357] --> 60,63\n"
    "Block 60: 59 --> [358,363] --> 61,62\n"
    "Block 61: 60 --> [364,365] --> 62\n"
    "Block 62: 61,60 --> [366,366] --> 63\n"
    "Block 63: 62,59 --> [367,369] --> 64,67\n"
    "Block 64: 63 --> [370,375] --> 65,66\n"
    "Block 65: 64 --> [376,377] --> 66\n"
    "Block 66: 65,64 --> [378,378] --> 67\n"
    "Block 67: 66,63 --> [379,381] --> 68,71\n"
    "Block 68: 67 --> [382,387] --> 69,70\n"
    "Block 69: 68 --> [388,389] --> 70\n"
    "Block 70: 69,68 --> [390,390] --> 71\n"
    "Block 71: 70,67 --> [391,391] --> 72\n"
    "Block 72: 71,57,58 --> [392,401] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
