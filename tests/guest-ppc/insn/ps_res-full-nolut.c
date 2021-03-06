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
    0x10,0x20,0x10,0x30,  // ps_res f1,f2
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_NATIVE_RECIPROCAL;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: FGETSTATE  r4\n"
    "    4: VFCMP      r5, r3, r3, UN\n"
    "    5: VFCVT      r6, r3\n"
    "    6: SET_ALIAS  a5, r6\n"
    "    7: GOTO_IF_Z  r5, L1\n"
    "    8: VEXTRACT   r7, r3, 0\n"
    "    9: VEXTRACT   r8, r3, 1\n"
    "   10: BFEXT      r9, r5, 0, 32\n"
    "   11: BFEXT      r10, r5, 32, 32\n"
    "   12: ZCAST      r11, r9\n"
    "   13: ZCAST      r12, r10\n"
    "   14: BITCAST    r13, r7\n"
    "   15: BITCAST    r14, r8\n"
    "   16: NOT        r15, r13\n"
    "   17: NOT        r16, r14\n"
    "   18: LOAD_IMM   r17, 0x8000000000000\n"
    "   19: AND        r18, r15, r17\n"
    "   20: AND        r19, r16, r17\n"
    "   21: VEXTRACT   r20, r6, 0\n"
    "   22: VEXTRACT   r21, r6, 1\n"
    "   23: SRLI       r22, r18, 29\n"
    "   24: SRLI       r23, r19, 29\n"
    "   25: ZCAST      r24, r22\n"
    "   26: ZCAST      r25, r23\n"
    "   27: BITCAST    r26, r20\n"
    "   28: BITCAST    r27, r21\n"
    "   29: AND        r28, r24, r11\n"
    "   30: AND        r29, r25, r12\n"
    "   31: XOR        r30, r26, r28\n"
    "   32: XOR        r31, r27, r29\n"
    "   33: BITCAST    r32, r30\n"
    "   34: BITCAST    r33, r31\n"
    "   35: VBUILD2    r34, r32, r33\n"
    "   36: SET_ALIAS  a5, r34\n"
    "   37: LABEL      L1\n"
    "   38: GET_ALIAS  r35, a5\n"
    "   39: FSETSTATE  r4\n"
    "   40: LOAD_IMM   r36, 1.0f\n"
    "   41: VBROADCAST r37, r36\n"
    "   42: FDIV       r38, r37, r35\n"
    "   43: FGETSTATE  r39\n"
    "   44: FTESTEXC   r40, r39, INVALID\n"
    "   45: GOTO_IF_Z  r40, L2\n"
    "   46: GET_ALIAS  r41, a2\n"
    "   47: FCLEAREXC  r42, r39\n"
    "   48: FSETSTATE  r42\n"
    "   49: VEXTRACT   r43, r35, 0\n"
    "   50: FDIV       r44, r36, r43\n"
    "   51: GET_ALIAS  r45, a4\n"
    "   52: FGETSTATE  r46\n"
    "   53: FCLEAREXC  r47, r46\n"
    "   54: FSETSTATE  r47\n"
    "   55: FTESTEXC   r48, r46, INVALID\n"
    "   56: GOTO_IF_Z  r48, L4\n"
    "   57: NOT        r49, r45\n"
    "   58: ORI        r50, r45, 16777216\n"
    "   59: ANDI       r51, r49, 16777216\n"
    "   60: SET_ALIAS  a4, r50\n"
    "   61: GOTO_IF_Z  r51, L5\n"
    "   62: ORI        r52, r50, -2147483648\n"
    "   63: SET_ALIAS  a4, r52\n"
    "   64: LABEL      L5\n"
    "   65: ANDI       r53, r45, 128\n"
    "   66: GOTO_IF_Z  r53, L4\n"
    "   67: LABEL      L6\n"
    "   68: GET_ALIAS  r54, a4\n"
    "   69: BFEXT      r55, r54, 12, 7\n"
    "   70: ANDI       r56, r55, 31\n"
    "   71: GET_ALIAS  r57, a4\n"
    "   72: BFINS      r58, r57, r56, 12, 7\n"
    "   73: SET_ALIAS  a4, r58\n"
    "   74: GOTO       L3\n"
    "   75: LABEL      L4\n"
    "   76: FTESTEXC   r59, r46, ZERO_DIVIDE\n"
    "   77: GOTO_IF_Z  r59, L7\n"
    "   78: NOT        r60, r45\n"
    "   79: ORI        r61, r45, 67108864\n"
    "   80: ANDI       r62, r60, 67108864\n"
    "   81: SET_ALIAS  a4, r61\n"
    "   82: GOTO_IF_Z  r62, L8\n"
    "   83: ORI        r63, r61, -2147483648\n"
    "   84: SET_ALIAS  a4, r63\n"
    "   85: LABEL      L8\n"
    "   86: ANDI       r64, r45, 16\n"
    "   87: GOTO_IF_NZ r64, L6\n"
    "   88: LABEL      L7\n"
    "   89: FCVT       r65, r44\n"
    "   90: VBROADCAST r66, r65\n"
    "   91: SET_ALIAS  a2, r66\n"
    "   92: BITCAST    r67, r44\n"
    "   93: SGTUI      r68, r67, 0\n"
    "   94: SRLI       r69, r67, 31\n"
    "   95: BFEXT      r73, r67, 23, 8\n"
    "   96: SEQI       r70, r73, 0\n"
    "   97: SEQI       r71, r73, 255\n"
    "   98: SLLI       r74, r67, 9\n"
    "   99: SEQI       r72, r74, 0\n"
    "  100: AND        r75, r70, r72\n"
    "  101: XORI       r76, r72, 1\n"
    "  102: AND        r77, r71, r76\n"
    "  103: AND        r78, r70, r68\n"
    "  104: OR         r79, r78, r77\n"
    "  105: OR         r80, r75, r77\n"
    "  106: XORI       r81, r80, 1\n"
    "  107: XORI       r82, r69, 1\n"
    "  108: AND        r83, r69, r81\n"
    "  109: AND        r84, r82, r81\n"
    "  110: SLLI       r85, r79, 4\n"
    "  111: SLLI       r86, r83, 3\n"
    "  112: SLLI       r87, r84, 2\n"
    "  113: SLLI       r88, r75, 1\n"
    "  114: OR         r89, r85, r86\n"
    "  115: OR         r90, r87, r88\n"
    "  116: OR         r91, r89, r71\n"
    "  117: OR         r92, r91, r90\n"
    "  118: FTESTEXC   r93, r46, INEXACT\n"
    "  119: SLLI       r94, r93, 5\n"
    "  120: OR         r95, r92, r94\n"
    "  121: GET_ALIAS  r96, a4\n"
    "  122: BFINS      r97, r96, r95, 12, 7\n"
    "  123: SET_ALIAS  a4, r97\n"
    "  124: FTESTEXC   r98, r46, OVERFLOW\n"
    "  125: GOTO_IF_Z  r98, L9\n"
    "  126: GET_ALIAS  r99, a4\n"
    "  127: NOT        r100, r99\n"
    "  128: ORI        r101, r99, 268435456\n"
    "  129: ANDI       r102, r100, 268435456\n"
    "  130: SET_ALIAS  a4, r101\n"
    "  131: GOTO_IF_Z  r102, L10\n"
    "  132: ORI        r103, r101, -2147483648\n"
    "  133: SET_ALIAS  a4, r103\n"
    "  134: LABEL      L10\n"
    "  135: LABEL      L9\n"
    "  136: FTESTEXC   r104, r46, UNDERFLOW\n"
    "  137: GOTO_IF_Z  r104, L3\n"
    "  138: GET_ALIAS  r105, a4\n"
    "  139: NOT        r106, r105\n"
    "  140: ORI        r107, r105, 134217728\n"
    "  141: ANDI       r108, r106, 134217728\n"
    "  142: SET_ALIAS  a4, r107\n"
    "  143: GOTO_IF_Z  r108, L11\n"
    "  144: ORI        r109, r107, -2147483648\n"
    "  145: SET_ALIAS  a4, r109\n"
    "  146: LABEL      L11\n"
    "  147: LABEL      L3\n"
    "  148: GET_ALIAS  r110, a2\n"
    "  149: VEXTRACT   r111, r110, 0\n"
    "  150: GET_ALIAS  r112, a4\n"
    "  151: BFEXT      r113, r112, 12, 7\n"
    "  152: VEXTRACT   r114, r35, 1\n"
    "  153: FDIV       r115, r36, r114\n"
    "  154: GET_ALIAS  r116, a4\n"
    "  155: FGETSTATE  r117\n"
    "  156: FCLEAREXC  r118, r117\n"
    "  157: FSETSTATE  r118\n"
    "  158: FTESTEXC   r119, r117, INVALID\n"
    "  159: GOTO_IF_Z  r119, L13\n"
    "  160: NOT        r120, r116\n"
    "  161: ORI        r121, r116, 16777216\n"
    "  162: ANDI       r122, r120, 16777216\n"
    "  163: SET_ALIAS  a4, r121\n"
    "  164: GOTO_IF_Z  r122, L14\n"
    "  165: ORI        r123, r121, -2147483648\n"
    "  166: SET_ALIAS  a4, r123\n"
    "  167: LABEL      L14\n"
    "  168: ANDI       r124, r116, 128\n"
    "  169: GOTO_IF_Z  r124, L13\n"
    "  170: LABEL      L15\n"
    "  171: GET_ALIAS  r125, a4\n"
    "  172: BFEXT      r126, r125, 12, 7\n"
    "  173: ANDI       r127, r126, 31\n"
    "  174: GET_ALIAS  r128, a4\n"
    "  175: BFINS      r129, r128, r127, 12, 7\n"
    "  176: SET_ALIAS  a4, r129\n"
    "  177: GOTO       L12\n"
    "  178: LABEL      L13\n"
    "  179: FTESTEXC   r130, r117, ZERO_DIVIDE\n"
    "  180: GOTO_IF_Z  r130, L16\n"
    "  181: NOT        r131, r116\n"
    "  182: ORI        r132, r116, 67108864\n"
    "  183: ANDI       r133, r131, 67108864\n"
    "  184: SET_ALIAS  a4, r132\n"
    "  185: GOTO_IF_Z  r133, L17\n"
    "  186: ORI        r134, r132, -2147483648\n"
    "  187: SET_ALIAS  a4, r134\n"
    "  188: LABEL      L17\n"
    "  189: ANDI       r135, r116, 16\n"
    "  190: GOTO_IF_NZ r135, L15\n"
    "  191: LABEL      L16\n"
    "  192: FCVT       r136, r115\n"
    "  193: VBROADCAST r137, r136\n"
    "  194: SET_ALIAS  a2, r137\n"
    "  195: BITCAST    r138, r115\n"
    "  196: SGTUI      r139, r138, 0\n"
    "  197: SRLI       r140, r138, 31\n"
    "  198: BFEXT      r144, r138, 23, 8\n"
    "  199: SEQI       r141, r144, 0\n"
    "  200: SEQI       r142, r144, 255\n"
    "  201: SLLI       r145, r138, 9\n"
    "  202: SEQI       r143, r145, 0\n"
    "  203: AND        r146, r141, r143\n"
    "  204: XORI       r147, r143, 1\n"
    "  205: AND        r148, r142, r147\n"
    "  206: AND        r149, r141, r139\n"
    "  207: OR         r150, r149, r148\n"
    "  208: OR         r151, r146, r148\n"
    "  209: XORI       r152, r151, 1\n"
    "  210: XORI       r153, r140, 1\n"
    "  211: AND        r154, r140, r152\n"
    "  212: AND        r155, r153, r152\n"
    "  213: SLLI       r156, r150, 4\n"
    "  214: SLLI       r157, r154, 3\n"
    "  215: SLLI       r158, r155, 2\n"
    "  216: SLLI       r159, r146, 1\n"
    "  217: OR         r160, r156, r157\n"
    "  218: OR         r161, r158, r159\n"
    "  219: OR         r162, r160, r142\n"
    "  220: OR         r163, r162, r161\n"
    "  221: FTESTEXC   r164, r117, INEXACT\n"
    "  222: SLLI       r165, r164, 5\n"
    "  223: OR         r166, r163, r165\n"
    "  224: GET_ALIAS  r167, a4\n"
    "  225: BFINS      r168, r167, r166, 12, 7\n"
    "  226: SET_ALIAS  a4, r168\n"
    "  227: FTESTEXC   r169, r117, OVERFLOW\n"
    "  228: GOTO_IF_Z  r169, L18\n"
    "  229: GET_ALIAS  r170, a4\n"
    "  230: NOT        r171, r170\n"
    "  231: ORI        r172, r170, 268435456\n"
    "  232: ANDI       r173, r171, 268435456\n"
    "  233: SET_ALIAS  a4, r172\n"
    "  234: GOTO_IF_Z  r173, L19\n"
    "  235: ORI        r174, r172, -2147483648\n"
    "  236: SET_ALIAS  a4, r174\n"
    "  237: LABEL      L19\n"
    "  238: LABEL      L18\n"
    "  239: FTESTEXC   r175, r117, UNDERFLOW\n"
    "  240: GOTO_IF_Z  r175, L12\n"
    "  241: GET_ALIAS  r176, a4\n"
    "  242: NOT        r177, r176\n"
    "  243: ORI        r178, r176, 134217728\n"
    "  244: ANDI       r179, r177, 134217728\n"
    "  245: SET_ALIAS  a4, r178\n"
    "  246: GOTO_IF_Z  r179, L20\n"
    "  247: ORI        r180, r178, -2147483648\n"
    "  248: SET_ALIAS  a4, r180\n"
    "  249: LABEL      L20\n"
    "  250: LABEL      L12\n"
    "  251: GET_ALIAS  r181, a2\n"
    "  252: VEXTRACT   r182, r181, 0\n"
    "  253: GET_ALIAS  r183, a4\n"
    "  254: BFEXT      r184, r183, 12, 7\n"
    "  255: ANDI       r185, r184, 32\n"
    "  256: OR         r186, r113, r185\n"
    "  257: GET_ALIAS  r187, a4\n"
    "  258: BFINS      r188, r187, r186, 12, 7\n"
    "  259: SET_ALIAS  a4, r188\n"
    "  260: GET_ALIAS  r189, a4\n"
    "  261: ANDI       r190, r189, 128\n"
    "  262: GOTO_IF_Z  r190, L21\n"
    "  263: SET_ALIAS  a2, r41\n"
    "  264: GOTO       L22\n"
    "  265: LABEL      L21\n"
    "  266: VBUILD2    r191, r111, r182\n"
    "  267: SET_ALIAS  a2, r191\n"
    "  268: GOTO       L22\n"
    "  269: LABEL      L2\n"
    "  270: GET_ALIAS  r192, a4\n"
    "  271: FGETSTATE  r193\n"
    "  272: FCLEAREXC  r194, r193\n"
    "  273: FSETSTATE  r194\n"
    "  274: FTESTEXC   r195, r193, ZERO_DIVIDE\n"
    "  275: GOTO_IF_Z  r195, L24\n"
    "  276: NOT        r196, r192\n"
    "  277: ORI        r197, r192, 67108864\n"
    "  278: ANDI       r198, r196, 67108864\n"
    "  279: SET_ALIAS  a4, r197\n"
    "  280: GOTO_IF_Z  r198, L25\n"
    "  281: ORI        r199, r197, -2147483648\n"
    "  282: SET_ALIAS  a4, r199\n"
    "  283: LABEL      L25\n"
    "  284: ANDI       r200, r192, 16\n"
    "  285: GOTO_IF_Z  r200, L24\n"
    "  286: VEXTRACT   r201, r37, 0\n"
    "  287: VEXTRACT   r202, r37, 1\n"
    "  288: VEXTRACT   r203, r35, 0\n"
    "  289: VEXTRACT   r204, r35, 1\n"
    "  290: FDIV       r205, r201, r203\n"
    "  291: FGETSTATE  r206\n"
    "  292: FDIV       r207, r202, r204\n"
    "  293: FTESTEXC   r208, r206, ZERO_DIVIDE\n"
    "  294: BITCAST    r209, r205\n"
    "  295: SGTUI      r210, r209, 0\n"
    "  296: SRLI       r211, r209, 31\n"
    "  297: BFEXT      r215, r209, 23, 8\n"
    "  298: SEQI       r212, r215, 0\n"
    "  299: SEQI       r213, r215, 255\n"
    "  300: SLLI       r216, r209, 9\n"
    "  301: SEQI       r214, r216, 0\n"
    "  302: AND        r217, r212, r214\n"
    "  303: XORI       r218, r214, 1\n"
    "  304: AND        r219, r213, r218\n"
    "  305: AND        r220, r212, r210\n"
    "  306: OR         r221, r220, r219\n"
    "  307: OR         r222, r217, r219\n"
    "  308: XORI       r223, r222, 1\n"
    "  309: XORI       r224, r211, 1\n"
    "  310: AND        r225, r211, r223\n"
    "  311: AND        r226, r224, r223\n"
    "  312: SLLI       r227, r221, 4\n"
    "  313: SLLI       r228, r225, 3\n"
    "  314: SLLI       r229, r226, 2\n"
    "  315: SLLI       r230, r217, 1\n"
    "  316: OR         r231, r227, r228\n"
    "  317: OR         r232, r229, r230\n"
    "  318: OR         r233, r231, r213\n"
    "  319: OR         r234, r233, r232\n"
    "  320: LOAD_IMM   r235, 0\n"
    "  321: SELECT     r236, r235, r234, r208\n"
    "  322: FGETSTATE  r237\n"
    "  323: FSETSTATE  r194\n"
    "  324: FTESTEXC   r238, r237, INEXACT\n"
    "  325: GOTO_IF_Z  r238, L26\n"
    "  326: GET_ALIAS  r239, a4\n"
    "  327: NOT        r240, r239\n"
    "  328: ORI        r241, r239, 33554432\n"
    "  329: ANDI       r242, r240, 33554432\n"
    "  330: SET_ALIAS  a4, r241\n"
    "  331: GOTO_IF_Z  r242, L27\n"
    "  332: ORI        r243, r241, -2147483648\n"
    "  333: SET_ALIAS  a4, r243\n"
    "  334: LABEL      L27\n"
    "  335: LABEL      L26\n"
    "  336: FTESTEXC   r244, r237, OVERFLOW\n"
    "  337: GOTO_IF_Z  r244, L28\n"
    "  338: GET_ALIAS  r245, a4\n"
    "  339: NOT        r246, r245\n"
    "  340: ORI        r247, r245, 268435456\n"
    "  341: ANDI       r248, r246, 268435456\n"
    "  342: SET_ALIAS  a4, r247\n"
    "  343: GOTO_IF_Z  r248, L29\n"
    "  344: ORI        r249, r247, -2147483648\n"
    "  345: SET_ALIAS  a4, r249\n"
    "  346: LABEL      L29\n"
    "  347: LABEL      L28\n"
    "  348: FTESTEXC   r250, r237, UNDERFLOW\n"
    "  349: GOTO_IF_Z  r250, L30\n"
    "  350: GET_ALIAS  r251, a4\n"
    "  351: NOT        r252, r251\n"
    "  352: ORI        r253, r251, 134217728\n"
    "  353: ANDI       r254, r252, 134217728\n"
    "  354: SET_ALIAS  a4, r253\n"
    "  355: GOTO_IF_Z  r254, L31\n"
    "  356: ORI        r255, r253, -2147483648\n"
    "  357: SET_ALIAS  a4, r255\n"
    "  358: LABEL      L31\n"
    "  359: LABEL      L30\n"
    "  360: SLLI       r256, r238, 5\n"
    "  361: OR         r257, r256, r236\n"
    "  362: GET_ALIAS  r258, a4\n"
    "  363: BFINS      r259, r258, r257, 12, 7\n"
    "  364: SET_ALIAS  a4, r259\n"
    "  365: GOTO       L23\n"
    "  366: LABEL      L24\n"
    "  367: VFCVT      r260, r38\n"
    "  368: SET_ALIAS  a2, r260\n"
    "  369: VEXTRACT   r261, r38, 0\n"
    "  370: BITCAST    r262, r261\n"
    "  371: SGTUI      r263, r262, 0\n"
    "  372: SRLI       r264, r262, 31\n"
    "  373: BFEXT      r268, r262, 23, 8\n"
    "  374: SEQI       r265, r268, 0\n"
    "  375: SEQI       r266, r268, 255\n"
    "  376: SLLI       r269, r262, 9\n"
    "  377: SEQI       r267, r269, 0\n"
    "  378: AND        r270, r265, r267\n"
    "  379: XORI       r271, r267, 1\n"
    "  380: AND        r272, r266, r271\n"
    "  381: AND        r273, r265, r263\n"
    "  382: OR         r274, r273, r272\n"
    "  383: OR         r275, r270, r272\n"
    "  384: XORI       r276, r275, 1\n"
    "  385: XORI       r277, r264, 1\n"
    "  386: AND        r278, r264, r276\n"
    "  387: AND        r279, r277, r276\n"
    "  388: SLLI       r280, r274, 4\n"
    "  389: SLLI       r281, r278, 3\n"
    "  390: SLLI       r282, r279, 2\n"
    "  391: SLLI       r283, r270, 1\n"
    "  392: OR         r284, r280, r281\n"
    "  393: OR         r285, r282, r283\n"
    "  394: OR         r286, r284, r266\n"
    "  395: OR         r287, r286, r285\n"
    "  396: FTESTEXC   r288, r193, INEXACT\n"
    "  397: SLLI       r289, r288, 5\n"
    "  398: OR         r290, r287, r289\n"
    "  399: GET_ALIAS  r291, a4\n"
    "  400: BFINS      r292, r291, r290, 12, 7\n"
    "  401: SET_ALIAS  a4, r292\n"
    "  402: GOTO_IF_Z  r288, L32\n"
    "  403: GET_ALIAS  r293, a4\n"
    "  404: NOT        r294, r293\n"
    "  405: ORI        r295, r293, 33554432\n"
    "  406: ANDI       r296, r294, 33554432\n"
    "  407: SET_ALIAS  a4, r295\n"
    "  408: GOTO_IF_Z  r296, L33\n"
    "  409: ORI        r297, r295, -2147483648\n"
    "  410: SET_ALIAS  a4, r297\n"
    "  411: LABEL      L33\n"
    "  412: LABEL      L32\n"
    "  413: FTESTEXC   r298, r193, OVERFLOW\n"
    "  414: GOTO_IF_Z  r298, L34\n"
    "  415: GET_ALIAS  r299, a4\n"
    "  416: NOT        r300, r299\n"
    "  417: ORI        r301, r299, 268435456\n"
    "  418: ANDI       r302, r300, 268435456\n"
    "  419: SET_ALIAS  a4, r301\n"
    "  420: GOTO_IF_Z  r302, L35\n"
    "  421: ORI        r303, r301, -2147483648\n"
    "  422: SET_ALIAS  a4, r303\n"
    "  423: LABEL      L35\n"
    "  424: LABEL      L34\n"
    "  425: FTESTEXC   r304, r193, UNDERFLOW\n"
    "  426: GOTO_IF_Z  r304, L23\n"
    "  427: GET_ALIAS  r305, a4\n"
    "  428: NOT        r306, r305\n"
    "  429: ORI        r307, r305, 134217728\n"
    "  430: ANDI       r308, r306, 134217728\n"
    "  431: SET_ALIAS  a4, r307\n"
    "  432: GOTO_IF_Z  r308, L36\n"
    "  433: ORI        r309, r307, -2147483648\n"
    "  434: SET_ALIAS  a4, r309\n"
    "  435: LABEL      L36\n"
    "  436: LABEL      L23\n"
    "  437: LABEL      L22\n"
    "  438: LOAD_IMM   r310, 4\n"
    "  439: SET_ALIAS  a1, r310\n"
    "  440: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: float32[2], no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,7] --> 1,2\n"
    "Block 1: 0 --> [8,36] --> 2\n"
    "Block 2: 1,0 --> [37,45] --> 3,40\n"
    "Block 3: 2 --> [46,56] --> 4,8\n"
    "Block 4: 3 --> [57,61] --> 5,6\n"
    "Block 5: 4 --> [62,63] --> 6\n"
    "Block 6: 5,4 --> [64,66] --> 7,8\n"
    "Block 7: 6,11 --> [67,74] --> 20\n"
    "Block 8: 3,6 --> [75,77] --> 9,12\n"
    "Block 9: 8 --> [78,82] --> 10,11\n"
    "Block 10: 9 --> [83,84] --> 11\n"
    "Block 11: 10,9 --> [85,87] --> 12,7\n"
    "Block 12: 11,8 --> [88,125] --> 13,16\n"
    "Block 13: 12 --> [126,131] --> 14,15\n"
    "Block 14: 13 --> [132,133] --> 15\n"
    "Block 15: 14,13 --> [134,134] --> 16\n"
    "Block 16: 15,12 --> [135,137] --> 17,20\n"
    "Block 17: 16 --> [138,143] --> 18,19\n"
    "Block 18: 17 --> [144,145] --> 19\n"
    "Block 19: 18,17 --> [146,146] --> 20\n"
    "Block 20: 19,7,16 --> [147,159] --> 21,25\n"
    "Block 21: 20 --> [160,164] --> 22,23\n"
    "Block 22: 21 --> [165,166] --> 23\n"
    "Block 23: 22,21 --> [167,169] --> 24,25\n"
    "Block 24: 23,28 --> [170,177] --> 37\n"
    "Block 25: 20,23 --> [178,180] --> 26,29\n"
    "Block 26: 25 --> [181,185] --> 27,28\n"
    "Block 27: 26 --> [186,187] --> 28\n"
    "Block 28: 27,26 --> [188,190] --> 29,24\n"
    "Block 29: 28,25 --> [191,228] --> 30,33\n"
    "Block 30: 29 --> [229,234] --> 31,32\n"
    "Block 31: 30 --> [235,236] --> 32\n"
    "Block 32: 31,30 --> [237,237] --> 33\n"
    "Block 33: 32,29 --> [238,240] --> 34,37\n"
    "Block 34: 33 --> [241,246] --> 35,36\n"
    "Block 35: 34 --> [247,248] --> 36\n"
    "Block 36: 35,34 --> [249,249] --> 37\n"
    "Block 37: 36,24,33 --> [250,262] --> 38,39\n"
    "Block 38: 37 --> [263,264] --> 70\n"
    "Block 39: 37 --> [265,268] --> 70\n"
    "Block 40: 2 --> [269,275] --> 41,57\n"
    "Block 41: 40 --> [276,280] --> 42,43\n"
    "Block 42: 41 --> [281,282] --> 43\n"
    "Block 43: 42,41 --> [283,285] --> 44,57\n"
    "Block 44: 43 --> [286,325] --> 45,48\n"
    "Block 45: 44 --> [326,331] --> 46,47\n"
    "Block 46: 45 --> [332,333] --> 47\n"
    "Block 47: 46,45 --> [334,334] --> 48\n"
    "Block 48: 47,44 --> [335,337] --> 49,52\n"
    "Block 49: 48 --> [338,343] --> 50,51\n"
    "Block 50: 49 --> [344,345] --> 51\n"
    "Block 51: 50,49 --> [346,346] --> 52\n"
    "Block 52: 51,48 --> [347,349] --> 53,56\n"
    "Block 53: 52 --> [350,355] --> 54,55\n"
    "Block 54: 53 --> [356,357] --> 55\n"
    "Block 55: 54,53 --> [358,358] --> 56\n"
    "Block 56: 55,52 --> [359,365] --> 69\n"
    "Block 57: 40,43 --> [366,402] --> 58,61\n"
    "Block 58: 57 --> [403,408] --> 59,60\n"
    "Block 59: 58 --> [409,410] --> 60\n"
    "Block 60: 59,58 --> [411,411] --> 61\n"
    "Block 61: 60,57 --> [412,414] --> 62,65\n"
    "Block 62: 61 --> [415,420] --> 63,64\n"
    "Block 63: 62 --> [421,422] --> 64\n"
    "Block 64: 63,62 --> [423,423] --> 65\n"
    "Block 65: 64,61 --> [424,426] --> 66,69\n"
    "Block 66: 65 --> [427,432] --> 67,68\n"
    "Block 67: 66 --> [433,434] --> 68\n"
    "Block 68: 67,66 --> [435,435] --> 69\n"
    "Block 69: 68,56,65 --> [436,436] --> 70\n"
    "Block 70: 69,38,39 --> [437,440] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
