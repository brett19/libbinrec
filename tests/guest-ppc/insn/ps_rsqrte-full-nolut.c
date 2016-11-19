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
    0x10,0x20,0x10,0x34,  // ps_rsqrte f1,f2
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
    "    6: VFCAST     r6, r5\n"
    "    7: FSQRT      r7, r6\n"
    "    8: LOAD_IMM   r8, 1.0f\n"
    "    9: VBROADCAST r9, r8\n"
    "   10: FDIV       r10, r9, r7\n"
    "   11: FGETSTATE  r11\n"
    "   12: FTESTEXC   r12, r11, INVALID\n"
    "   13: GOTO_IF_Z  r12, L1\n"
    "   14: GET_ALIAS  r13, a2\n"
    "   15: FCLEAREXC  r14, r11\n"
    "   16: FSETSTATE  r14\n"
    "   17: VEXTRACT   r15, r6, 0\n"
    "   18: FSQRT      r16, r15\n"
    "   19: FDIV       r17, r8, r16\n"
    "   20: GET_ALIAS  r18, a4\n"
    "   21: FGETSTATE  r19\n"
    "   22: FCLEAREXC  r20, r19\n"
    "   23: FSETSTATE  r20\n"
    "   24: FTESTEXC   r21, r19, INVALID\n"
    "   25: GOTO_IF_Z  r21, L3\n"
    "   26: BITCAST    r22, r8\n"
    "   27: SLLI       r23, r22, 10\n"
    "   28: BFEXT      r24, r22, 22, 9\n"
    "   29: SEQI       r25, r24, 510\n"
    "   30: GOTO_IF_Z  r25, L5\n"
    "   31: GOTO_IF_NZ r23, L4\n"
    "   32: LABEL      L5\n"
    "   33: BITCAST    r26, r16\n"
    "   34: SLLI       r27, r26, 10\n"
    "   35: BFEXT      r28, r26, 22, 9\n"
    "   36: SEQI       r29, r28, 510\n"
    "   37: GOTO_IF_Z  r29, L6\n"
    "   38: GOTO_IF_NZ r27, L4\n"
    "   39: LABEL      L6\n"
    "   40: NOT        r30, r18\n"
    "   41: ORI        r31, r18, 512\n"
    "   42: ANDI       r32, r30, 512\n"
    "   43: SET_ALIAS  a4, r31\n"
    "   44: GOTO_IF_Z  r32, L7\n"
    "   45: ORI        r33, r31, -2147483648\n"
    "   46: SET_ALIAS  a4, r33\n"
    "   47: LABEL      L7\n"
    "   48: ANDI       r34, r18, 128\n"
    "   49: GOTO_IF_Z  r34, L8\n"
    "   50: GET_ALIAS  r35, a5\n"
    "   51: ANDI       r36, r35, 31\n"
    "   52: SET_ALIAS  a5, r36\n"
    "   53: GOTO       L2\n"
    "   54: LABEL      L8\n"
    "   55: LOAD_IMM   r37, nan(0x400000)\n"
    "   56: FCVT       r38, r37\n"
    "   57: VBROADCAST r39, r38\n"
    "   58: SET_ALIAS  a2, r39\n"
    "   59: LOAD_IMM   r40, 17\n"
    "   60: SET_ALIAS  a5, r40\n"
    "   61: GOTO       L2\n"
    "   62: LABEL      L4\n"
    "   63: NOT        r41, r18\n"
    "   64: ORI        r42, r18, 16777216\n"
    "   65: ANDI       r43, r41, 16777216\n"
    "   66: SET_ALIAS  a4, r42\n"
    "   67: GOTO_IF_Z  r43, L9\n"
    "   68: ORI        r44, r42, -2147483648\n"
    "   69: SET_ALIAS  a4, r44\n"
    "   70: LABEL      L9\n"
    "   71: ANDI       r45, r18, 128\n"
    "   72: GOTO_IF_Z  r45, L3\n"
    "   73: LABEL      L10\n"
    "   74: GET_ALIAS  r46, a5\n"
    "   75: ANDI       r47, r46, 31\n"
    "   76: SET_ALIAS  a5, r47\n"
    "   77: GOTO       L2\n"
    "   78: LABEL      L3\n"
    "   79: FTESTEXC   r48, r19, ZERO_DIVIDE\n"
    "   80: GOTO_IF_Z  r48, L11\n"
    "   81: NOT        r49, r18\n"
    "   82: ORI        r50, r18, 67108864\n"
    "   83: ANDI       r51, r49, 67108864\n"
    "   84: SET_ALIAS  a4, r50\n"
    "   85: GOTO_IF_Z  r51, L12\n"
    "   86: ORI        r52, r50, -2147483648\n"
    "   87: SET_ALIAS  a4, r52\n"
    "   88: LABEL      L12\n"
    "   89: ANDI       r53, r18, 16\n"
    "   90: GOTO_IF_NZ r53, L10\n"
    "   91: LABEL      L11\n"
    "   92: FCVT       r54, r17\n"
    "   93: VBROADCAST r55, r54\n"
    "   94: SET_ALIAS  a2, r55\n"
    "   95: BITCAST    r56, r17\n"
    "   96: SGTUI      r57, r56, 0\n"
    "   97: SRLI       r58, r56, 31\n"
    "   98: BFEXT      r62, r56, 23, 8\n"
    "   99: SEQI       r59, r62, 0\n"
    "  100: SEQI       r60, r62, 255\n"
    "  101: SLLI       r63, r56, 9\n"
    "  102: SEQI       r61, r63, 0\n"
    "  103: AND        r64, r59, r61\n"
    "  104: XORI       r65, r61, 1\n"
    "  105: AND        r66, r60, r65\n"
    "  106: AND        r67, r59, r57\n"
    "  107: OR         r68, r67, r66\n"
    "  108: OR         r69, r64, r66\n"
    "  109: XORI       r70, r69, 1\n"
    "  110: XORI       r71, r58, 1\n"
    "  111: AND        r72, r58, r70\n"
    "  112: AND        r73, r71, r70\n"
    "  113: SLLI       r74, r68, 4\n"
    "  114: SLLI       r75, r72, 3\n"
    "  115: SLLI       r76, r73, 2\n"
    "  116: SLLI       r77, r64, 1\n"
    "  117: OR         r78, r74, r75\n"
    "  118: OR         r79, r76, r77\n"
    "  119: OR         r80, r78, r60\n"
    "  120: OR         r81, r80, r79\n"
    "  121: FTESTEXC   r82, r19, INEXACT\n"
    "  122: SLLI       r83, r82, 5\n"
    "  123: OR         r84, r81, r83\n"
    "  124: SET_ALIAS  a5, r84\n"
    "  125: FTESTEXC   r85, r19, OVERFLOW\n"
    "  126: GOTO_IF_Z  r85, L13\n"
    "  127: GET_ALIAS  r86, a4\n"
    "  128: NOT        r87, r86\n"
    "  129: ORI        r88, r86, 268435456\n"
    "  130: ANDI       r89, r87, 268435456\n"
    "  131: SET_ALIAS  a4, r88\n"
    "  132: GOTO_IF_Z  r89, L14\n"
    "  133: ORI        r90, r88, -2147483648\n"
    "  134: SET_ALIAS  a4, r90\n"
    "  135: LABEL      L14\n"
    "  136: LABEL      L13\n"
    "  137: FTESTEXC   r91, r19, UNDERFLOW\n"
    "  138: GOTO_IF_Z  r91, L2\n"
    "  139: GET_ALIAS  r92, a4\n"
    "  140: NOT        r93, r92\n"
    "  141: ORI        r94, r92, 134217728\n"
    "  142: ANDI       r95, r93, 134217728\n"
    "  143: SET_ALIAS  a4, r94\n"
    "  144: GOTO_IF_Z  r95, L15\n"
    "  145: ORI        r96, r94, -2147483648\n"
    "  146: SET_ALIAS  a4, r96\n"
    "  147: LABEL      L15\n"
    "  148: LABEL      L2\n"
    "  149: GET_ALIAS  r97, a2\n"
    "  150: VEXTRACT   r98, r97, 0\n"
    "  151: GET_ALIAS  r99, a5\n"
    "  152: VEXTRACT   r100, r6, 1\n"
    "  153: FSQRT      r101, r100\n"
    "  154: FDIV       r102, r8, r101\n"
    "  155: GET_ALIAS  r103, a4\n"
    "  156: FGETSTATE  r104\n"
    "  157: FCLEAREXC  r105, r104\n"
    "  158: FSETSTATE  r105\n"
    "  159: FTESTEXC   r106, r104, INVALID\n"
    "  160: GOTO_IF_Z  r106, L17\n"
    "  161: BITCAST    r107, r8\n"
    "  162: SLLI       r108, r107, 10\n"
    "  163: BFEXT      r109, r107, 22, 9\n"
    "  164: SEQI       r110, r109, 510\n"
    "  165: GOTO_IF_Z  r110, L19\n"
    "  166: GOTO_IF_NZ r108, L18\n"
    "  167: LABEL      L19\n"
    "  168: BITCAST    r111, r101\n"
    "  169: SLLI       r112, r111, 10\n"
    "  170: BFEXT      r113, r111, 22, 9\n"
    "  171: SEQI       r114, r113, 510\n"
    "  172: GOTO_IF_Z  r114, L20\n"
    "  173: GOTO_IF_NZ r112, L18\n"
    "  174: LABEL      L20\n"
    "  175: NOT        r115, r103\n"
    "  176: ORI        r116, r103, 512\n"
    "  177: ANDI       r117, r115, 512\n"
    "  178: SET_ALIAS  a4, r116\n"
    "  179: GOTO_IF_Z  r117, L21\n"
    "  180: ORI        r118, r116, -2147483648\n"
    "  181: SET_ALIAS  a4, r118\n"
    "  182: LABEL      L21\n"
    "  183: ANDI       r119, r103, 128\n"
    "  184: GOTO_IF_Z  r119, L22\n"
    "  185: GET_ALIAS  r120, a5\n"
    "  186: ANDI       r121, r120, 31\n"
    "  187: SET_ALIAS  a5, r121\n"
    "  188: GOTO       L16\n"
    "  189: LABEL      L22\n"
    "  190: LOAD_IMM   r122, nan(0x400000)\n"
    "  191: FCVT       r123, r122\n"
    "  192: VBROADCAST r124, r123\n"
    "  193: SET_ALIAS  a2, r124\n"
    "  194: LOAD_IMM   r125, 17\n"
    "  195: SET_ALIAS  a5, r125\n"
    "  196: GOTO       L16\n"
    "  197: LABEL      L18\n"
    "  198: NOT        r126, r103\n"
    "  199: ORI        r127, r103, 16777216\n"
    "  200: ANDI       r128, r126, 16777216\n"
    "  201: SET_ALIAS  a4, r127\n"
    "  202: GOTO_IF_Z  r128, L23\n"
    "  203: ORI        r129, r127, -2147483648\n"
    "  204: SET_ALIAS  a4, r129\n"
    "  205: LABEL      L23\n"
    "  206: ANDI       r130, r103, 128\n"
    "  207: GOTO_IF_Z  r130, L17\n"
    "  208: LABEL      L24\n"
    "  209: GET_ALIAS  r131, a5\n"
    "  210: ANDI       r132, r131, 31\n"
    "  211: SET_ALIAS  a5, r132\n"
    "  212: GOTO       L16\n"
    "  213: LABEL      L17\n"
    "  214: FTESTEXC   r133, r104, ZERO_DIVIDE\n"
    "  215: GOTO_IF_Z  r133, L25\n"
    "  216: NOT        r134, r103\n"
    "  217: ORI        r135, r103, 67108864\n"
    "  218: ANDI       r136, r134, 67108864\n"
    "  219: SET_ALIAS  a4, r135\n"
    "  220: GOTO_IF_Z  r136, L26\n"
    "  221: ORI        r137, r135, -2147483648\n"
    "  222: SET_ALIAS  a4, r137\n"
    "  223: LABEL      L26\n"
    "  224: ANDI       r138, r103, 16\n"
    "  225: GOTO_IF_NZ r138, L24\n"
    "  226: LABEL      L25\n"
    "  227: FCVT       r139, r102\n"
    "  228: VBROADCAST r140, r139\n"
    "  229: SET_ALIAS  a2, r140\n"
    "  230: BITCAST    r141, r102\n"
    "  231: SGTUI      r142, r141, 0\n"
    "  232: SRLI       r143, r141, 31\n"
    "  233: BFEXT      r147, r141, 23, 8\n"
    "  234: SEQI       r144, r147, 0\n"
    "  235: SEQI       r145, r147, 255\n"
    "  236: SLLI       r148, r141, 9\n"
    "  237: SEQI       r146, r148, 0\n"
    "  238: AND        r149, r144, r146\n"
    "  239: XORI       r150, r146, 1\n"
    "  240: AND        r151, r145, r150\n"
    "  241: AND        r152, r144, r142\n"
    "  242: OR         r153, r152, r151\n"
    "  243: OR         r154, r149, r151\n"
    "  244: XORI       r155, r154, 1\n"
    "  245: XORI       r156, r143, 1\n"
    "  246: AND        r157, r143, r155\n"
    "  247: AND        r158, r156, r155\n"
    "  248: SLLI       r159, r153, 4\n"
    "  249: SLLI       r160, r157, 3\n"
    "  250: SLLI       r161, r158, 2\n"
    "  251: SLLI       r162, r149, 1\n"
    "  252: OR         r163, r159, r160\n"
    "  253: OR         r164, r161, r162\n"
    "  254: OR         r165, r163, r145\n"
    "  255: OR         r166, r165, r164\n"
    "  256: FTESTEXC   r167, r104, INEXACT\n"
    "  257: SLLI       r168, r167, 5\n"
    "  258: OR         r169, r166, r168\n"
    "  259: SET_ALIAS  a5, r169\n"
    "  260: FTESTEXC   r170, r104, OVERFLOW\n"
    "  261: GOTO_IF_Z  r170, L27\n"
    "  262: GET_ALIAS  r171, a4\n"
    "  263: NOT        r172, r171\n"
    "  264: ORI        r173, r171, 268435456\n"
    "  265: ANDI       r174, r172, 268435456\n"
    "  266: SET_ALIAS  a4, r173\n"
    "  267: GOTO_IF_Z  r174, L28\n"
    "  268: ORI        r175, r173, -2147483648\n"
    "  269: SET_ALIAS  a4, r175\n"
    "  270: LABEL      L28\n"
    "  271: LABEL      L27\n"
    "  272: FTESTEXC   r176, r104, UNDERFLOW\n"
    "  273: GOTO_IF_Z  r176, L16\n"
    "  274: GET_ALIAS  r177, a4\n"
    "  275: NOT        r178, r177\n"
    "  276: ORI        r179, r177, 134217728\n"
    "  277: ANDI       r180, r178, 134217728\n"
    "  278: SET_ALIAS  a4, r179\n"
    "  279: GOTO_IF_Z  r180, L29\n"
    "  280: ORI        r181, r179, -2147483648\n"
    "  281: SET_ALIAS  a4, r181\n"
    "  282: LABEL      L29\n"
    "  283: LABEL      L16\n"
    "  284: GET_ALIAS  r182, a2\n"
    "  285: VEXTRACT   r183, r182, 0\n"
    "  286: ANDI       r184, r99, 31\n"
    "  287: SET_ALIAS  a5, r184\n"
    "  288: GET_ALIAS  r185, a4\n"
    "  289: ANDI       r186, r185, 128\n"
    "  290: GOTO_IF_Z  r186, L30\n"
    "  291: SET_ALIAS  a2, r13\n"
    "  292: GOTO       L31\n"
    "  293: LABEL      L30\n"
    "  294: VBUILD2    r187, r98, r183\n"
    "  295: SET_ALIAS  a2, r187\n"
    "  296: GOTO       L31\n"
    "  297: LABEL      L1\n"
    "  298: GET_ALIAS  r188, a4\n"
    "  299: FGETSTATE  r189\n"
    "  300: FCLEAREXC  r190, r189\n"
    "  301: FSETSTATE  r190\n"
    "  302: FTESTEXC   r191, r189, ZERO_DIVIDE\n"
    "  303: GOTO_IF_Z  r191, L33\n"
    "  304: NOT        r192, r188\n"
    "  305: ORI        r193, r188, 67108864\n"
    "  306: ANDI       r194, r192, 67108864\n"
    "  307: SET_ALIAS  a4, r193\n"
    "  308: GOTO_IF_Z  r194, L34\n"
    "  309: ORI        r195, r193, -2147483648\n"
    "  310: SET_ALIAS  a4, r195\n"
    "  311: LABEL      L34\n"
    "  312: ANDI       r196, r188, 16\n"
    "  313: GOTO_IF_Z  r196, L33\n"
    "  314: VEXTRACT   r197, r9, 0\n"
    "  315: VEXTRACT   r198, r9, 1\n"
    "  316: VEXTRACT   r199, r6, 0\n"
    "  317: VEXTRACT   r200, r6, 1\n"
    "  318: FDIV       r201, r197, r199\n"
    "  319: FGETSTATE  r202\n"
    "  320: FDIV       r203, r198, r200\n"
    "  321: FTESTEXC   r204, r202, ZERO_DIVIDE\n"
    "  322: BITCAST    r205, r201\n"
    "  323: SGTUI      r206, r205, 0\n"
    "  324: SRLI       r207, r205, 31\n"
    "  325: BFEXT      r211, r205, 23, 8\n"
    "  326: SEQI       r208, r211, 0\n"
    "  327: SEQI       r209, r211, 255\n"
    "  328: SLLI       r212, r205, 9\n"
    "  329: SEQI       r210, r212, 0\n"
    "  330: AND        r213, r208, r210\n"
    "  331: XORI       r214, r210, 1\n"
    "  332: AND        r215, r209, r214\n"
    "  333: AND        r216, r208, r206\n"
    "  334: OR         r217, r216, r215\n"
    "  335: OR         r218, r213, r215\n"
    "  336: XORI       r219, r218, 1\n"
    "  337: XORI       r220, r207, 1\n"
    "  338: AND        r221, r207, r219\n"
    "  339: AND        r222, r220, r219\n"
    "  340: SLLI       r223, r217, 4\n"
    "  341: SLLI       r224, r221, 3\n"
    "  342: SLLI       r225, r222, 2\n"
    "  343: SLLI       r226, r213, 1\n"
    "  344: OR         r227, r223, r224\n"
    "  345: OR         r228, r225, r226\n"
    "  346: OR         r229, r227, r209\n"
    "  347: OR         r230, r229, r228\n"
    "  348: LOAD_IMM   r231, 0\n"
    "  349: SELECT     r232, r231, r230, r204\n"
    "  350: FGETSTATE  r233\n"
    "  351: FSETSTATE  r190\n"
    "  352: FTESTEXC   r234, r233, INEXACT\n"
    "  353: GOTO_IF_Z  r234, L35\n"
    "  354: GET_ALIAS  r235, a4\n"
    "  355: NOT        r236, r235\n"
    "  356: ORI        r237, r235, 33554432\n"
    "  357: ANDI       r238, r236, 33554432\n"
    "  358: SET_ALIAS  a4, r237\n"
    "  359: GOTO_IF_Z  r238, L36\n"
    "  360: ORI        r239, r237, -2147483648\n"
    "  361: SET_ALIAS  a4, r239\n"
    "  362: LABEL      L36\n"
    "  363: LABEL      L35\n"
    "  364: FTESTEXC   r240, r233, OVERFLOW\n"
    "  365: GOTO_IF_Z  r240, L37\n"
    "  366: GET_ALIAS  r241, a4\n"
    "  367: NOT        r242, r241\n"
    "  368: ORI        r243, r241, 268435456\n"
    "  369: ANDI       r244, r242, 268435456\n"
    "  370: SET_ALIAS  a4, r243\n"
    "  371: GOTO_IF_Z  r244, L38\n"
    "  372: ORI        r245, r243, -2147483648\n"
    "  373: SET_ALIAS  a4, r245\n"
    "  374: LABEL      L38\n"
    "  375: LABEL      L37\n"
    "  376: FTESTEXC   r246, r233, UNDERFLOW\n"
    "  377: GOTO_IF_Z  r246, L39\n"
    "  378: GET_ALIAS  r247, a4\n"
    "  379: NOT        r248, r247\n"
    "  380: ORI        r249, r247, 134217728\n"
    "  381: ANDI       r250, r248, 134217728\n"
    "  382: SET_ALIAS  a4, r249\n"
    "  383: GOTO_IF_Z  r250, L40\n"
    "  384: ORI        r251, r249, -2147483648\n"
    "  385: SET_ALIAS  a4, r251\n"
    "  386: LABEL      L40\n"
    "  387: LABEL      L39\n"
    "  388: SLLI       r252, r234, 5\n"
    "  389: OR         r253, r252, r232\n"
    "  390: SET_ALIAS  a5, r253\n"
    "  391: GOTO       L32\n"
    "  392: LABEL      L33\n"
    "  393: VFCVT      r254, r10\n"
    "  394: SET_ALIAS  a2, r254\n"
    "  395: VEXTRACT   r255, r10, 0\n"
    "  396: BITCAST    r256, r255\n"
    "  397: SGTUI      r257, r256, 0\n"
    "  398: SRLI       r258, r256, 31\n"
    "  399: BFEXT      r262, r256, 23, 8\n"
    "  400: SEQI       r259, r262, 0\n"
    "  401: SEQI       r260, r262, 255\n"
    "  402: SLLI       r263, r256, 9\n"
    "  403: SEQI       r261, r263, 0\n"
    "  404: AND        r264, r259, r261\n"
    "  405: XORI       r265, r261, 1\n"
    "  406: AND        r266, r260, r265\n"
    "  407: AND        r267, r259, r257\n"
    "  408: OR         r268, r267, r266\n"
    "  409: OR         r269, r264, r266\n"
    "  410: XORI       r270, r269, 1\n"
    "  411: XORI       r271, r258, 1\n"
    "  412: AND        r272, r258, r270\n"
    "  413: AND        r273, r271, r270\n"
    "  414: SLLI       r274, r268, 4\n"
    "  415: SLLI       r275, r272, 3\n"
    "  416: SLLI       r276, r273, 2\n"
    "  417: SLLI       r277, r264, 1\n"
    "  418: OR         r278, r274, r275\n"
    "  419: OR         r279, r276, r277\n"
    "  420: OR         r280, r278, r260\n"
    "  421: OR         r281, r280, r279\n"
    "  422: FTESTEXC   r282, r189, INEXACT\n"
    "  423: SLLI       r283, r282, 5\n"
    "  424: OR         r284, r281, r283\n"
    "  425: SET_ALIAS  a5, r284\n"
    "  426: GOTO_IF_Z  r282, L41\n"
    "  427: GET_ALIAS  r285, a4\n"
    "  428: NOT        r286, r285\n"
    "  429: ORI        r287, r285, 33554432\n"
    "  430: ANDI       r288, r286, 33554432\n"
    "  431: SET_ALIAS  a4, r287\n"
    "  432: GOTO_IF_Z  r288, L42\n"
    "  433: ORI        r289, r287, -2147483648\n"
    "  434: SET_ALIAS  a4, r289\n"
    "  435: LABEL      L42\n"
    "  436: LABEL      L41\n"
    "  437: FTESTEXC   r290, r189, OVERFLOW\n"
    "  438: GOTO_IF_Z  r290, L43\n"
    "  439: GET_ALIAS  r291, a4\n"
    "  440: NOT        r292, r291\n"
    "  441: ORI        r293, r291, 268435456\n"
    "  442: ANDI       r294, r292, 268435456\n"
    "  443: SET_ALIAS  a4, r293\n"
    "  444: GOTO_IF_Z  r294, L44\n"
    "  445: ORI        r295, r293, -2147483648\n"
    "  446: SET_ALIAS  a4, r295\n"
    "  447: LABEL      L44\n"
    "  448: LABEL      L43\n"
    "  449: FTESTEXC   r296, r189, UNDERFLOW\n"
    "  450: GOTO_IF_Z  r296, L32\n"
    "  451: GET_ALIAS  r297, a4\n"
    "  452: NOT        r298, r297\n"
    "  453: ORI        r299, r297, 134217728\n"
    "  454: ANDI       r300, r298, 134217728\n"
    "  455: SET_ALIAS  a4, r299\n"
    "  456: GOTO_IF_Z  r300, L45\n"
    "  457: ORI        r301, r299, -2147483648\n"
    "  458: SET_ALIAS  a4, r301\n"
    "  459: LABEL      L45\n"
    "  460: LABEL      L32\n"
    "  461: LABEL      L31\n"
    "  462: LOAD_IMM   r302, 4\n"
    "  463: SET_ALIAS  a1, r302\n"
    "  464: GET_ALIAS  r303, a4\n"
    "  465: GET_ALIAS  r304, a5\n"
    "  466: ANDI       r305, r303, -1611134977\n"
    "  467: SLLI       r306, r304, 12\n"
    "  468: OR         r307, r305, r306\n"
    "  469: SET_ALIAS  a4, r307\n"
    "  470: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,13] --> 1,56\n"
    "Block 1: 0 --> [14,25] --> 2,15\n"
    "Block 2: 1 --> [26,30] --> 3,4\n"
    "Block 3: 2 --> [31,31] --> 4,11\n"
    "Block 4: 3,2 --> [32,37] --> 5,6\n"
    "Block 5: 4 --> [38,38] --> 6,11\n"
    "Block 6: 5,4 --> [39,44] --> 7,8\n"
    "Block 7: 6 --> [45,46] --> 8\n"
    "Block 8: 7,6 --> [47,49] --> 9,10\n"
    "Block 9: 8 --> [50,53] --> 27\n"
    "Block 10: 8 --> [54,61] --> 27\n"
    "Block 11: 3,5 --> [62,67] --> 12,13\n"
    "Block 12: 11 --> [68,69] --> 13\n"
    "Block 13: 12,11 --> [70,72] --> 14,15\n"
    "Block 14: 13,18 --> [73,77] --> 27\n"
    "Block 15: 1,13 --> [78,80] --> 16,19\n"
    "Block 16: 15 --> [81,85] --> 17,18\n"
    "Block 17: 16 --> [86,87] --> 18\n"
    "Block 18: 17,16 --> [88,90] --> 19,14\n"
    "Block 19: 18,15 --> [91,126] --> 20,23\n"
    "Block 20: 19 --> [127,132] --> 21,22\n"
    "Block 21: 20 --> [133,134] --> 22\n"
    "Block 22: 21,20 --> [135,135] --> 23\n"
    "Block 23: 22,19 --> [136,138] --> 24,27\n"
    "Block 24: 23 --> [139,144] --> 25,26\n"
    "Block 25: 24 --> [145,146] --> 26\n"
    "Block 26: 25,24 --> [147,147] --> 27\n"
    "Block 27: 26,9,10,14,23 --> [148,160] --> 28,41\n"
    "Block 28: 27 --> [161,165] --> 29,30\n"
    "Block 29: 28 --> [166,166] --> 30,37\n"
    "Block 30: 29,28 --> [167,172] --> 31,32\n"
    "Block 31: 30 --> [173,173] --> 32,37\n"
    "Block 32: 31,30 --> [174,179] --> 33,34\n"
    "Block 33: 32 --> [180,181] --> 34\n"
    "Block 34: 33,32 --> [182,184] --> 35,36\n"
    "Block 35: 34 --> [185,188] --> 53\n"
    "Block 36: 34 --> [189,196] --> 53\n"
    "Block 37: 29,31 --> [197,202] --> 38,39\n"
    "Block 38: 37 --> [203,204] --> 39\n"
    "Block 39: 38,37 --> [205,207] --> 40,41\n"
    "Block 40: 39,44 --> [208,212] --> 53\n"
    "Block 41: 27,39 --> [213,215] --> 42,45\n"
    "Block 42: 41 --> [216,220] --> 43,44\n"
    "Block 43: 42 --> [221,222] --> 44\n"
    "Block 44: 43,42 --> [223,225] --> 45,40\n"
    "Block 45: 44,41 --> [226,261] --> 46,49\n"
    "Block 46: 45 --> [262,267] --> 47,48\n"
    "Block 47: 46 --> [268,269] --> 48\n"
    "Block 48: 47,46 --> [270,270] --> 49\n"
    "Block 49: 48,45 --> [271,273] --> 50,53\n"
    "Block 50: 49 --> [274,279] --> 51,52\n"
    "Block 51: 50 --> [280,281] --> 52\n"
    "Block 52: 51,50 --> [282,282] --> 53\n"
    "Block 53: 52,35,36,40,49 --> [283,290] --> 54,55\n"
    "Block 54: 53 --> [291,292] --> 86\n"
    "Block 55: 53 --> [293,296] --> 86\n"
    "Block 56: 0 --> [297,303] --> 57,73\n"
    "Block 57: 56 --> [304,308] --> 58,59\n"
    "Block 58: 57 --> [309,310] --> 59\n"
    "Block 59: 58,57 --> [311,313] --> 60,73\n"
    "Block 60: 59 --> [314,353] --> 61,64\n"
    "Block 61: 60 --> [354,359] --> 62,63\n"
    "Block 62: 61 --> [360,361] --> 63\n"
    "Block 63: 62,61 --> [362,362] --> 64\n"
    "Block 64: 63,60 --> [363,365] --> 65,68\n"
    "Block 65: 64 --> [366,371] --> 66,67\n"
    "Block 66: 65 --> [372,373] --> 67\n"
    "Block 67: 66,65 --> [374,374] --> 68\n"
    "Block 68: 67,64 --> [375,377] --> 69,72\n"
    "Block 69: 68 --> [378,383] --> 70,71\n"
    "Block 70: 69 --> [384,385] --> 71\n"
    "Block 71: 70,69 --> [386,386] --> 72\n"
    "Block 72: 71,68 --> [387,391] --> 85\n"
    "Block 73: 56,59 --> [392,426] --> 74,77\n"
    "Block 74: 73 --> [427,432] --> 75,76\n"
    "Block 75: 74 --> [433,434] --> 76\n"
    "Block 76: 75,74 --> [435,435] --> 77\n"
    "Block 77: 76,73 --> [436,438] --> 78,81\n"
    "Block 78: 77 --> [439,444] --> 79,80\n"
    "Block 79: 78 --> [445,446] --> 80\n"
    "Block 80: 79,78 --> [447,447] --> 81\n"
    "Block 81: 80,77 --> [448,450] --> 82,85\n"
    "Block 82: 81 --> [451,456] --> 83,84\n"
    "Block 83: 82 --> [457,458] --> 84\n"
    "Block 84: 83,82 --> [459,459] --> 85\n"
    "Block 85: 84,72,81 --> [460,460] --> 86\n"
    "Block 86: 85,54,55 --> [461,470] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
