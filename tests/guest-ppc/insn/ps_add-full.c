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

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: GET_ALIAS  r4, a4\n"
    "    4: FADD       r5, r3, r4\n"
    "    5: VFCVT      r6, r5\n"
    "    6: LOAD_IMM   r7, 0x1000000\n"
    "    7: VEXTRACT   r8, r6, 1\n"
    "    8: BITCAST    r9, r8\n"
    "    9: SLLI       r10, r9, 1\n"
    "   10: SEQ        r11, r10, r7\n"
    "   11: GOTO_IF_NZ r11, L1\n"
    "   12: VEXTRACT   r12, r6, 0\n"
    "   13: BITCAST    r13, r12\n"
    "   14: SLLI       r14, r13, 1\n"
    "   15: SEQ        r15, r14, r7\n"
    "   16: GOTO_IF_Z  r15, L2\n"
    "   17: LABEL      L1\n"
    "   18: FGETSTATE  r16\n"
    "   19: FSETROUND  r17, r16, TRUNC\n"
    "   20: FSETSTATE  r17\n"
    "   21: FADD       r18, r3, r4\n"
    "   22: VFCVT      r19, r18\n"
    "   23: FGETSTATE  r20\n"
    "   24: FCOPYROUND r21, r20, r16\n"
    "   25: FSETSTATE  r21\n"
    "   26: LABEL      L2\n"
    "   27: FGETSTATE  r22\n"
    "   28: FTESTEXC   r23, r22, INVALID\n"
    "   29: GOTO_IF_Z  r23, L3\n"
    "   30: GET_ALIAS  r24, a2\n"
    "   31: FCLEAREXC  r25, r22\n"
    "   32: FSETSTATE  r25\n"
    "   33: VEXTRACT   r26, r3, 0\n"
    "   34: VEXTRACT   r27, r4, 0\n"
    "   35: FADD       r28, r26, r27\n"
    "   36: FCVT       r29, r28\n"
    "   37: GET_ALIAS  r30, a5\n"
    "   38: FGETSTATE  r31\n"
    "   39: FCLEAREXC  r32, r31\n"
    "   40: FSETSTATE  r32\n"
    "   41: FTESTEXC   r33, r31, INVALID\n"
    "   42: GOTO_IF_Z  r33, L5\n"
    "   43: BITCAST    r34, r26\n"
    "   44: SLLI       r35, r34, 13\n"
    "   45: BFEXT      r36, r34, 51, 12\n"
    "   46: SEQI       r37, r36, 4094\n"
    "   47: GOTO_IF_Z  r37, L7\n"
    "   48: GOTO_IF_NZ r35, L6\n"
    "   49: LABEL      L7\n"
    "   50: BITCAST    r38, r27\n"
    "   51: SLLI       r39, r38, 13\n"
    "   52: BFEXT      r40, r38, 51, 12\n"
    "   53: SEQI       r41, r40, 4094\n"
    "   54: GOTO_IF_Z  r41, L8\n"
    "   55: GOTO_IF_NZ r39, L6\n"
    "   56: LABEL      L8\n"
    "   57: NOT        r42, r30\n"
    "   58: ORI        r43, r30, 8388608\n"
    "   59: ANDI       r44, r42, 8388608\n"
    "   60: SET_ALIAS  a5, r43\n"
    "   61: GOTO_IF_Z  r44, L9\n"
    "   62: ORI        r45, r43, -2147483648\n"
    "   63: SET_ALIAS  a5, r45\n"
    "   64: LABEL      L9\n"
    "   65: ANDI       r46, r30, 128\n"
    "   66: GOTO_IF_Z  r46, L10\n"
    "   67: GET_ALIAS  r47, a5\n"
    "   68: BFEXT      r48, r47, 12, 7\n"
    "   69: ANDI       r49, r48, 31\n"
    "   70: GET_ALIAS  r50, a5\n"
    "   71: BFINS      r51, r50, r49, 12, 7\n"
    "   72: SET_ALIAS  a5, r51\n"
    "   73: GOTO       L4\n"
    "   74: LABEL      L10\n"
    "   75: LOAD_IMM   r52, nan(0x400000)\n"
    "   76: FCVT       r53, r52\n"
    "   77: VBROADCAST r54, r53\n"
    "   78: SET_ALIAS  a2, r54\n"
    "   79: LOAD_IMM   r55, 17\n"
    "   80: GET_ALIAS  r56, a5\n"
    "   81: BFINS      r57, r56, r55, 12, 7\n"
    "   82: SET_ALIAS  a5, r57\n"
    "   83: GOTO       L4\n"
    "   84: LABEL      L6\n"
    "   85: NOT        r58, r30\n"
    "   86: ORI        r59, r30, 16777216\n"
    "   87: ANDI       r60, r58, 16777216\n"
    "   88: SET_ALIAS  a5, r59\n"
    "   89: GOTO_IF_Z  r60, L11\n"
    "   90: ORI        r61, r59, -2147483648\n"
    "   91: SET_ALIAS  a5, r61\n"
    "   92: LABEL      L11\n"
    "   93: ANDI       r62, r30, 128\n"
    "   94: GOTO_IF_Z  r62, L5\n"
    "   95: GET_ALIAS  r63, a5\n"
    "   96: BFEXT      r64, r63, 12, 7\n"
    "   97: ANDI       r65, r64, 31\n"
    "   98: GET_ALIAS  r66, a5\n"
    "   99: BFINS      r67, r66, r65, 12, 7\n"
    "  100: SET_ALIAS  a5, r67\n"
    "  101: GOTO       L4\n"
    "  102: LABEL      L5\n"
    "  103: FCVT       r68, r29\n"
    "  104: VBROADCAST r69, r68\n"
    "  105: SET_ALIAS  a2, r69\n"
    "  106: BITCAST    r70, r29\n"
    "  107: SGTUI      r71, r70, 0\n"
    "  108: SRLI       r72, r70, 31\n"
    "  109: BFEXT      r76, r70, 23, 8\n"
    "  110: SEQI       r73, r76, 0\n"
    "  111: SEQI       r74, r76, 255\n"
    "  112: SLLI       r77, r70, 9\n"
    "  113: SEQI       r75, r77, 0\n"
    "  114: AND        r78, r73, r75\n"
    "  115: XORI       r79, r75, 1\n"
    "  116: AND        r80, r74, r79\n"
    "  117: AND        r81, r73, r71\n"
    "  118: OR         r82, r81, r80\n"
    "  119: OR         r83, r78, r80\n"
    "  120: XORI       r84, r83, 1\n"
    "  121: XORI       r85, r72, 1\n"
    "  122: AND        r86, r72, r84\n"
    "  123: AND        r87, r85, r84\n"
    "  124: SLLI       r88, r82, 4\n"
    "  125: SLLI       r89, r86, 3\n"
    "  126: SLLI       r90, r87, 2\n"
    "  127: SLLI       r91, r78, 1\n"
    "  128: OR         r92, r88, r89\n"
    "  129: OR         r93, r90, r91\n"
    "  130: OR         r94, r92, r74\n"
    "  131: OR         r95, r94, r93\n"
    "  132: FTESTEXC   r96, r31, INEXACT\n"
    "  133: SLLI       r97, r96, 5\n"
    "  134: OR         r98, r95, r97\n"
    "  135: GET_ALIAS  r99, a5\n"
    "  136: BFINS      r100, r99, r98, 12, 7\n"
    "  137: SET_ALIAS  a5, r100\n"
    "  138: GOTO_IF_Z  r96, L12\n"
    "  139: GET_ALIAS  r101, a5\n"
    "  140: NOT        r102, r101\n"
    "  141: ORI        r103, r101, 33554432\n"
    "  142: ANDI       r104, r102, 33554432\n"
    "  143: SET_ALIAS  a5, r103\n"
    "  144: GOTO_IF_Z  r104, L13\n"
    "  145: ORI        r105, r103, -2147483648\n"
    "  146: SET_ALIAS  a5, r105\n"
    "  147: LABEL      L13\n"
    "  148: LABEL      L12\n"
    "  149: FTESTEXC   r106, r31, OVERFLOW\n"
    "  150: GOTO_IF_Z  r106, L14\n"
    "  151: GET_ALIAS  r107, a5\n"
    "  152: NOT        r108, r107\n"
    "  153: ORI        r109, r107, 268435456\n"
    "  154: ANDI       r110, r108, 268435456\n"
    "  155: SET_ALIAS  a5, r109\n"
    "  156: GOTO_IF_Z  r110, L15\n"
    "  157: ORI        r111, r109, -2147483648\n"
    "  158: SET_ALIAS  a5, r111\n"
    "  159: LABEL      L15\n"
    "  160: LABEL      L14\n"
    "  161: FTESTEXC   r112, r31, UNDERFLOW\n"
    "  162: GOTO_IF_Z  r112, L4\n"
    "  163: GET_ALIAS  r113, a5\n"
    "  164: NOT        r114, r113\n"
    "  165: ORI        r115, r113, 134217728\n"
    "  166: ANDI       r116, r114, 134217728\n"
    "  167: SET_ALIAS  a5, r115\n"
    "  168: GOTO_IF_Z  r116, L16\n"
    "  169: ORI        r117, r115, -2147483648\n"
    "  170: SET_ALIAS  a5, r117\n"
    "  171: LABEL      L16\n"
    "  172: LABEL      L4\n"
    "  173: GET_ALIAS  r118, a2\n"
    "  174: VEXTRACT   r119, r118, 0\n"
    "  175: GET_ALIAS  r120, a5\n"
    "  176: BFEXT      r121, r120, 12, 7\n"
    "  177: VEXTRACT   r122, r3, 1\n"
    "  178: VEXTRACT   r123, r4, 1\n"
    "  179: FADD       r124, r122, r123\n"
    "  180: FCVT       r125, r124\n"
    "  181: GET_ALIAS  r126, a5\n"
    "  182: FGETSTATE  r127\n"
    "  183: FCLEAREXC  r128, r127\n"
    "  184: FSETSTATE  r128\n"
    "  185: FTESTEXC   r129, r127, INVALID\n"
    "  186: GOTO_IF_Z  r129, L18\n"
    "  187: BITCAST    r130, r122\n"
    "  188: SLLI       r131, r130, 13\n"
    "  189: BFEXT      r132, r130, 51, 12\n"
    "  190: SEQI       r133, r132, 4094\n"
    "  191: GOTO_IF_Z  r133, L20\n"
    "  192: GOTO_IF_NZ r131, L19\n"
    "  193: LABEL      L20\n"
    "  194: BITCAST    r134, r123\n"
    "  195: SLLI       r135, r134, 13\n"
    "  196: BFEXT      r136, r134, 51, 12\n"
    "  197: SEQI       r137, r136, 4094\n"
    "  198: GOTO_IF_Z  r137, L21\n"
    "  199: GOTO_IF_NZ r135, L19\n"
    "  200: LABEL      L21\n"
    "  201: NOT        r138, r126\n"
    "  202: ORI        r139, r126, 8388608\n"
    "  203: ANDI       r140, r138, 8388608\n"
    "  204: SET_ALIAS  a5, r139\n"
    "  205: GOTO_IF_Z  r140, L22\n"
    "  206: ORI        r141, r139, -2147483648\n"
    "  207: SET_ALIAS  a5, r141\n"
    "  208: LABEL      L22\n"
    "  209: ANDI       r142, r126, 128\n"
    "  210: GOTO_IF_Z  r142, L23\n"
    "  211: GET_ALIAS  r143, a5\n"
    "  212: BFEXT      r144, r143, 12, 7\n"
    "  213: ANDI       r145, r144, 31\n"
    "  214: GET_ALIAS  r146, a5\n"
    "  215: BFINS      r147, r146, r145, 12, 7\n"
    "  216: SET_ALIAS  a5, r147\n"
    "  217: GOTO       L17\n"
    "  218: LABEL      L23\n"
    "  219: LOAD_IMM   r148, nan(0x400000)\n"
    "  220: FCVT       r149, r148\n"
    "  221: VBROADCAST r150, r149\n"
    "  222: SET_ALIAS  a2, r150\n"
    "  223: LOAD_IMM   r151, 17\n"
    "  224: GET_ALIAS  r152, a5\n"
    "  225: BFINS      r153, r152, r151, 12, 7\n"
    "  226: SET_ALIAS  a5, r153\n"
    "  227: GOTO       L17\n"
    "  228: LABEL      L19\n"
    "  229: NOT        r154, r126\n"
    "  230: ORI        r155, r126, 16777216\n"
    "  231: ANDI       r156, r154, 16777216\n"
    "  232: SET_ALIAS  a5, r155\n"
    "  233: GOTO_IF_Z  r156, L24\n"
    "  234: ORI        r157, r155, -2147483648\n"
    "  235: SET_ALIAS  a5, r157\n"
    "  236: LABEL      L24\n"
    "  237: ANDI       r158, r126, 128\n"
    "  238: GOTO_IF_Z  r158, L18\n"
    "  239: GET_ALIAS  r159, a5\n"
    "  240: BFEXT      r160, r159, 12, 7\n"
    "  241: ANDI       r161, r160, 31\n"
    "  242: GET_ALIAS  r162, a5\n"
    "  243: BFINS      r163, r162, r161, 12, 7\n"
    "  244: SET_ALIAS  a5, r163\n"
    "  245: GOTO       L17\n"
    "  246: LABEL      L18\n"
    "  247: FCVT       r164, r125\n"
    "  248: VBROADCAST r165, r164\n"
    "  249: SET_ALIAS  a2, r165\n"
    "  250: BITCAST    r166, r125\n"
    "  251: SGTUI      r167, r166, 0\n"
    "  252: SRLI       r168, r166, 31\n"
    "  253: BFEXT      r172, r166, 23, 8\n"
    "  254: SEQI       r169, r172, 0\n"
    "  255: SEQI       r170, r172, 255\n"
    "  256: SLLI       r173, r166, 9\n"
    "  257: SEQI       r171, r173, 0\n"
    "  258: AND        r174, r169, r171\n"
    "  259: XORI       r175, r171, 1\n"
    "  260: AND        r176, r170, r175\n"
    "  261: AND        r177, r169, r167\n"
    "  262: OR         r178, r177, r176\n"
    "  263: OR         r179, r174, r176\n"
    "  264: XORI       r180, r179, 1\n"
    "  265: XORI       r181, r168, 1\n"
    "  266: AND        r182, r168, r180\n"
    "  267: AND        r183, r181, r180\n"
    "  268: SLLI       r184, r178, 4\n"
    "  269: SLLI       r185, r182, 3\n"
    "  270: SLLI       r186, r183, 2\n"
    "  271: SLLI       r187, r174, 1\n"
    "  272: OR         r188, r184, r185\n"
    "  273: OR         r189, r186, r187\n"
    "  274: OR         r190, r188, r170\n"
    "  275: OR         r191, r190, r189\n"
    "  276: FTESTEXC   r192, r127, INEXACT\n"
    "  277: SLLI       r193, r192, 5\n"
    "  278: OR         r194, r191, r193\n"
    "  279: GET_ALIAS  r195, a5\n"
    "  280: BFINS      r196, r195, r194, 12, 7\n"
    "  281: SET_ALIAS  a5, r196\n"
    "  282: GOTO_IF_Z  r192, L25\n"
    "  283: GET_ALIAS  r197, a5\n"
    "  284: NOT        r198, r197\n"
    "  285: ORI        r199, r197, 33554432\n"
    "  286: ANDI       r200, r198, 33554432\n"
    "  287: SET_ALIAS  a5, r199\n"
    "  288: GOTO_IF_Z  r200, L26\n"
    "  289: ORI        r201, r199, -2147483648\n"
    "  290: SET_ALIAS  a5, r201\n"
    "  291: LABEL      L26\n"
    "  292: LABEL      L25\n"
    "  293: FTESTEXC   r202, r127, OVERFLOW\n"
    "  294: GOTO_IF_Z  r202, L27\n"
    "  295: GET_ALIAS  r203, a5\n"
    "  296: NOT        r204, r203\n"
    "  297: ORI        r205, r203, 268435456\n"
    "  298: ANDI       r206, r204, 268435456\n"
    "  299: SET_ALIAS  a5, r205\n"
    "  300: GOTO_IF_Z  r206, L28\n"
    "  301: ORI        r207, r205, -2147483648\n"
    "  302: SET_ALIAS  a5, r207\n"
    "  303: LABEL      L28\n"
    "  304: LABEL      L27\n"
    "  305: FTESTEXC   r208, r127, UNDERFLOW\n"
    "  306: GOTO_IF_Z  r208, L17\n"
    "  307: GET_ALIAS  r209, a5\n"
    "  308: NOT        r210, r209\n"
    "  309: ORI        r211, r209, 134217728\n"
    "  310: ANDI       r212, r210, 134217728\n"
    "  311: SET_ALIAS  a5, r211\n"
    "  312: GOTO_IF_Z  r212, L29\n"
    "  313: ORI        r213, r211, -2147483648\n"
    "  314: SET_ALIAS  a5, r213\n"
    "  315: LABEL      L29\n"
    "  316: LABEL      L17\n"
    "  317: GET_ALIAS  r214, a2\n"
    "  318: VEXTRACT   r215, r214, 0\n"
    "  319: GET_ALIAS  r216, a5\n"
    "  320: BFEXT      r217, r216, 12, 7\n"
    "  321: ANDI       r218, r217, 32\n"
    "  322: OR         r219, r121, r218\n"
    "  323: GET_ALIAS  r220, a5\n"
    "  324: BFINS      r221, r220, r219, 12, 7\n"
    "  325: SET_ALIAS  a5, r221\n"
    "  326: GET_ALIAS  r222, a5\n"
    "  327: ANDI       r223, r222, 128\n"
    "  328: GOTO_IF_Z  r223, L30\n"
    "  329: SET_ALIAS  a2, r24\n"
    "  330: GOTO       L31\n"
    "  331: LABEL      L30\n"
    "  332: VBUILD2    r224, r119, r215\n"
    "  333: SET_ALIAS  a2, r224\n"
    "  334: GOTO       L31\n"
    "  335: LABEL      L3\n"
    "  336: GET_ALIAS  r225, a5\n"
    "  337: FGETSTATE  r226\n"
    "  338: FCLEAREXC  r227, r226\n"
    "  339: FSETSTATE  r227\n"
    "  340: VFCVT      r228, r6\n"
    "  341: SET_ALIAS  a2, r228\n"
    "  342: VEXTRACT   r229, r6, 0\n"
    "  343: BITCAST    r230, r229\n"
    "  344: SGTUI      r231, r230, 0\n"
    "  345: SRLI       r232, r230, 31\n"
    "  346: BFEXT      r236, r230, 23, 8\n"
    "  347: SEQI       r233, r236, 0\n"
    "  348: SEQI       r234, r236, 255\n"
    "  349: SLLI       r237, r230, 9\n"
    "  350: SEQI       r235, r237, 0\n"
    "  351: AND        r238, r233, r235\n"
    "  352: XORI       r239, r235, 1\n"
    "  353: AND        r240, r234, r239\n"
    "  354: AND        r241, r233, r231\n"
    "  355: OR         r242, r241, r240\n"
    "  356: OR         r243, r238, r240\n"
    "  357: XORI       r244, r243, 1\n"
    "  358: XORI       r245, r232, 1\n"
    "  359: AND        r246, r232, r244\n"
    "  360: AND        r247, r245, r244\n"
    "  361: SLLI       r248, r242, 4\n"
    "  362: SLLI       r249, r246, 3\n"
    "  363: SLLI       r250, r247, 2\n"
    "  364: SLLI       r251, r238, 1\n"
    "  365: OR         r252, r248, r249\n"
    "  366: OR         r253, r250, r251\n"
    "  367: OR         r254, r252, r234\n"
    "  368: OR         r255, r254, r253\n"
    "  369: FTESTEXC   r256, r226, INEXACT\n"
    "  370: SLLI       r257, r256, 5\n"
    "  371: OR         r258, r255, r257\n"
    "  372: GET_ALIAS  r259, a5\n"
    "  373: BFINS      r260, r259, r258, 12, 7\n"
    "  374: SET_ALIAS  a5, r260\n"
    "  375: GOTO_IF_Z  r256, L33\n"
    "  376: GET_ALIAS  r261, a5\n"
    "  377: NOT        r262, r261\n"
    "  378: ORI        r263, r261, 33554432\n"
    "  379: ANDI       r264, r262, 33554432\n"
    "  380: SET_ALIAS  a5, r263\n"
    "  381: GOTO_IF_Z  r264, L34\n"
    "  382: ORI        r265, r263, -2147483648\n"
    "  383: SET_ALIAS  a5, r265\n"
    "  384: LABEL      L34\n"
    "  385: LABEL      L33\n"
    "  386: FTESTEXC   r266, r226, OVERFLOW\n"
    "  387: GOTO_IF_Z  r266, L35\n"
    "  388: GET_ALIAS  r267, a5\n"
    "  389: NOT        r268, r267\n"
    "  390: ORI        r269, r267, 268435456\n"
    "  391: ANDI       r270, r268, 268435456\n"
    "  392: SET_ALIAS  a5, r269\n"
    "  393: GOTO_IF_Z  r270, L36\n"
    "  394: ORI        r271, r269, -2147483648\n"
    "  395: SET_ALIAS  a5, r271\n"
    "  396: LABEL      L36\n"
    "  397: LABEL      L35\n"
    "  398: FTESTEXC   r272, r226, UNDERFLOW\n"
    "  399: GOTO_IF_Z  r272, L32\n"
    "  400: GET_ALIAS  r273, a5\n"
    "  401: NOT        r274, r273\n"
    "  402: ORI        r275, r273, 134217728\n"
    "  403: ANDI       r276, r274, 134217728\n"
    "  404: SET_ALIAS  a5, r275\n"
    "  405: GOTO_IF_Z  r276, L37\n"
    "  406: ORI        r277, r275, -2147483648\n"
    "  407: SET_ALIAS  a5, r277\n"
    "  408: LABEL      L37\n"
    "  409: LABEL      L32\n"
    "  410: LABEL      L31\n"
    "  411: LOAD_IMM   r278, 4\n"
    "  412: SET_ALIAS  a1, r278\n"
    "  413: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: float64[2] @ 432(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,11] --> 1,2\n"
    "Block 1: 0 --> [12,16] --> 2,3\n"
    "Block 2: 1,0 --> [17,25] --> 3\n"
    "Block 3: 2,1 --> [26,29] --> 4,59\n"
    "Block 4: 3 --> [30,42] --> 5,18\n"
    "Block 5: 4 --> [43,47] --> 6,7\n"
    "Block 6: 5 --> [48,48] --> 7,14\n"
    "Block 7: 6,5 --> [49,54] --> 8,9\n"
    "Block 8: 7 --> [55,55] --> 9,14\n"
    "Block 9: 8,7 --> [56,61] --> 10,11\n"
    "Block 10: 9 --> [62,63] --> 11\n"
    "Block 11: 10,9 --> [64,66] --> 12,13\n"
    "Block 12: 11 --> [67,73] --> 30\n"
    "Block 13: 11 --> [74,83] --> 30\n"
    "Block 14: 6,8 --> [84,89] --> 15,16\n"
    "Block 15: 14 --> [90,91] --> 16\n"
    "Block 16: 15,14 --> [92,94] --> 17,18\n"
    "Block 17: 16 --> [95,101] --> 30\n"
    "Block 18: 4,16 --> [102,138] --> 19,22\n"
    "Block 19: 18 --> [139,144] --> 20,21\n"
    "Block 20: 19 --> [145,146] --> 21\n"
    "Block 21: 20,19 --> [147,147] --> 22\n"
    "Block 22: 21,18 --> [148,150] --> 23,26\n"
    "Block 23: 22 --> [151,156] --> 24,25\n"
    "Block 24: 23 --> [157,158] --> 25\n"
    "Block 25: 24,23 --> [159,159] --> 26\n"
    "Block 26: 25,22 --> [160,162] --> 27,30\n"
    "Block 27: 26 --> [163,168] --> 28,29\n"
    "Block 28: 27 --> [169,170] --> 29\n"
    "Block 29: 28,27 --> [171,171] --> 30\n"
    "Block 30: 29,12,13,17,26 --> [172,186] --> 31,44\n"
    "Block 31: 30 --> [187,191] --> 32,33\n"
    "Block 32: 31 --> [192,192] --> 33,40\n"
    "Block 33: 32,31 --> [193,198] --> 34,35\n"
    "Block 34: 33 --> [199,199] --> 35,40\n"
    "Block 35: 34,33 --> [200,205] --> 36,37\n"
    "Block 36: 35 --> [206,207] --> 37\n"
    "Block 37: 36,35 --> [208,210] --> 38,39\n"
    "Block 38: 37 --> [211,217] --> 56\n"
    "Block 39: 37 --> [218,227] --> 56\n"
    "Block 40: 32,34 --> [228,233] --> 41,42\n"
    "Block 41: 40 --> [234,235] --> 42\n"
    "Block 42: 41,40 --> [236,238] --> 43,44\n"
    "Block 43: 42 --> [239,245] --> 56\n"
    "Block 44: 30,42 --> [246,282] --> 45,48\n"
    "Block 45: 44 --> [283,288] --> 46,47\n"
    "Block 46: 45 --> [289,290] --> 47\n"
    "Block 47: 46,45 --> [291,291] --> 48\n"
    "Block 48: 47,44 --> [292,294] --> 49,52\n"
    "Block 49: 48 --> [295,300] --> 50,51\n"
    "Block 50: 49 --> [301,302] --> 51\n"
    "Block 51: 50,49 --> [303,303] --> 52\n"
    "Block 52: 51,48 --> [304,306] --> 53,56\n"
    "Block 53: 52 --> [307,312] --> 54,55\n"
    "Block 54: 53 --> [313,314] --> 55\n"
    "Block 55: 54,53 --> [315,315] --> 56\n"
    "Block 56: 55,38,39,43,52 --> [316,328] --> 57,58\n"
    "Block 57: 56 --> [329,330] --> 72\n"
    "Block 58: 56 --> [331,334] --> 72\n"
    "Block 59: 3 --> [335,375] --> 60,63\n"
    "Block 60: 59 --> [376,381] --> 61,62\n"
    "Block 61: 60 --> [382,383] --> 62\n"
    "Block 62: 61,60 --> [384,384] --> 63\n"
    "Block 63: 62,59 --> [385,387] --> 64,67\n"
    "Block 64: 63 --> [388,393] --> 65,66\n"
    "Block 65: 64 --> [394,395] --> 66\n"
    "Block 66: 65,64 --> [396,396] --> 67\n"
    "Block 67: 66,63 --> [397,399] --> 68,71\n"
    "Block 68: 67 --> [400,405] --> 69,70\n"
    "Block 69: 68 --> [406,407] --> 70\n"
    "Block 70: 69,68 --> [408,408] --> 71\n"
    "Block 71: 70,67 --> [409,409] --> 72\n"
    "Block 72: 71,57,58 --> [410,413] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
