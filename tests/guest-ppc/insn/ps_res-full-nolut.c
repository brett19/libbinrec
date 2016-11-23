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
    "    2: GET_ALIAS  r3, a4\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a5, r4\n"
    "    5: GET_ALIAS  r5, a3\n"
    "    6: VFCAST     r6, r5\n"
    "    7: LOAD_IMM   r7, 1.0f\n"
    "    8: VBROADCAST r8, r7\n"
    "    9: FDIV       r9, r8, r6\n"
    "   10: FGETSTATE  r10\n"
    "   11: FTESTEXC   r11, r10, INVALID\n"
    "   12: GOTO_IF_Z  r11, L1\n"
    "   13: GET_ALIAS  r12, a2\n"
    "   14: FCLEAREXC  r13, r10\n"
    "   15: FSETSTATE  r13\n"
    "   16: VEXTRACT   r14, r6, 0\n"
    "   17: FDIV       r15, r7, r14\n"
    "   18: GET_ALIAS  r16, a4\n"
    "   19: FGETSTATE  r17\n"
    "   20: FCLEAREXC  r18, r17\n"
    "   21: FSETSTATE  r18\n"
    "   22: FTESTEXC   r19, r17, INVALID\n"
    "   23: GOTO_IF_Z  r19, L3\n"
    "   24: NOT        r20, r16\n"
    "   25: ORI        r21, r16, 16777216\n"
    "   26: ANDI       r22, r20, 16777216\n"
    "   27: SET_ALIAS  a4, r21\n"
    "   28: GOTO_IF_Z  r22, L4\n"
    "   29: ORI        r23, r21, -2147483648\n"
    "   30: SET_ALIAS  a4, r23\n"
    "   31: LABEL      L4\n"
    "   32: ANDI       r24, r16, 128\n"
    "   33: GOTO_IF_Z  r24, L3\n"
    "   34: LABEL      L5\n"
    "   35: GET_ALIAS  r25, a5\n"
    "   36: ANDI       r26, r25, 31\n"
    "   37: SET_ALIAS  a5, r26\n"
    "   38: GOTO       L2\n"
    "   39: LABEL      L3\n"
    "   40: FTESTEXC   r27, r17, ZERO_DIVIDE\n"
    "   41: GOTO_IF_Z  r27, L6\n"
    "   42: NOT        r28, r16\n"
    "   43: ORI        r29, r16, 67108864\n"
    "   44: ANDI       r30, r28, 67108864\n"
    "   45: SET_ALIAS  a4, r29\n"
    "   46: GOTO_IF_Z  r30, L7\n"
    "   47: ORI        r31, r29, -2147483648\n"
    "   48: SET_ALIAS  a4, r31\n"
    "   49: LABEL      L7\n"
    "   50: ANDI       r32, r16, 16\n"
    "   51: GOTO_IF_NZ r32, L5\n"
    "   52: LABEL      L6\n"
    "   53: FCVT       r33, r15\n"
    "   54: VBROADCAST r34, r33\n"
    "   55: SET_ALIAS  a2, r34\n"
    "   56: BITCAST    r35, r15\n"
    "   57: SGTUI      r36, r35, 0\n"
    "   58: SRLI       r37, r35, 31\n"
    "   59: BFEXT      r41, r35, 23, 8\n"
    "   60: SEQI       r38, r41, 0\n"
    "   61: SEQI       r39, r41, 255\n"
    "   62: SLLI       r42, r35, 9\n"
    "   63: SEQI       r40, r42, 0\n"
    "   64: AND        r43, r38, r40\n"
    "   65: XORI       r44, r40, 1\n"
    "   66: AND        r45, r39, r44\n"
    "   67: AND        r46, r38, r36\n"
    "   68: OR         r47, r46, r45\n"
    "   69: OR         r48, r43, r45\n"
    "   70: XORI       r49, r48, 1\n"
    "   71: XORI       r50, r37, 1\n"
    "   72: AND        r51, r37, r49\n"
    "   73: AND        r52, r50, r49\n"
    "   74: SLLI       r53, r47, 4\n"
    "   75: SLLI       r54, r51, 3\n"
    "   76: SLLI       r55, r52, 2\n"
    "   77: SLLI       r56, r43, 1\n"
    "   78: OR         r57, r53, r54\n"
    "   79: OR         r58, r55, r56\n"
    "   80: OR         r59, r57, r39\n"
    "   81: OR         r60, r59, r58\n"
    "   82: FTESTEXC   r61, r17, INEXACT\n"
    "   83: SLLI       r62, r61, 5\n"
    "   84: OR         r63, r60, r62\n"
    "   85: SET_ALIAS  a5, r63\n"
    "   86: FTESTEXC   r64, r17, OVERFLOW\n"
    "   87: GOTO_IF_Z  r64, L8\n"
    "   88: GET_ALIAS  r65, a4\n"
    "   89: NOT        r66, r65\n"
    "   90: ORI        r67, r65, 268435456\n"
    "   91: ANDI       r68, r66, 268435456\n"
    "   92: SET_ALIAS  a4, r67\n"
    "   93: GOTO_IF_Z  r68, L9\n"
    "   94: ORI        r69, r67, -2147483648\n"
    "   95: SET_ALIAS  a4, r69\n"
    "   96: LABEL      L9\n"
    "   97: LABEL      L8\n"
    "   98: FTESTEXC   r70, r17, UNDERFLOW\n"
    "   99: GOTO_IF_Z  r70, L2\n"
    "  100: GET_ALIAS  r71, a4\n"
    "  101: NOT        r72, r71\n"
    "  102: ORI        r73, r71, 134217728\n"
    "  103: ANDI       r74, r72, 134217728\n"
    "  104: SET_ALIAS  a4, r73\n"
    "  105: GOTO_IF_Z  r74, L10\n"
    "  106: ORI        r75, r73, -2147483648\n"
    "  107: SET_ALIAS  a4, r75\n"
    "  108: LABEL      L10\n"
    "  109: LABEL      L2\n"
    "  110: GET_ALIAS  r76, a2\n"
    "  111: VEXTRACT   r77, r76, 0\n"
    "  112: GET_ALIAS  r78, a5\n"
    "  113: VEXTRACT   r79, r6, 1\n"
    "  114: FDIV       r80, r7, r79\n"
    "  115: GET_ALIAS  r81, a4\n"
    "  116: FGETSTATE  r82\n"
    "  117: FCLEAREXC  r83, r82\n"
    "  118: FSETSTATE  r83\n"
    "  119: FTESTEXC   r84, r82, INVALID\n"
    "  120: GOTO_IF_Z  r84, L12\n"
    "  121: NOT        r85, r81\n"
    "  122: ORI        r86, r81, 16777216\n"
    "  123: ANDI       r87, r85, 16777216\n"
    "  124: SET_ALIAS  a4, r86\n"
    "  125: GOTO_IF_Z  r87, L13\n"
    "  126: ORI        r88, r86, -2147483648\n"
    "  127: SET_ALIAS  a4, r88\n"
    "  128: LABEL      L13\n"
    "  129: ANDI       r89, r81, 128\n"
    "  130: GOTO_IF_Z  r89, L12\n"
    "  131: LABEL      L14\n"
    "  132: GET_ALIAS  r90, a5\n"
    "  133: ANDI       r91, r90, 31\n"
    "  134: SET_ALIAS  a5, r91\n"
    "  135: GOTO       L11\n"
    "  136: LABEL      L12\n"
    "  137: FTESTEXC   r92, r82, ZERO_DIVIDE\n"
    "  138: GOTO_IF_Z  r92, L15\n"
    "  139: NOT        r93, r81\n"
    "  140: ORI        r94, r81, 67108864\n"
    "  141: ANDI       r95, r93, 67108864\n"
    "  142: SET_ALIAS  a4, r94\n"
    "  143: GOTO_IF_Z  r95, L16\n"
    "  144: ORI        r96, r94, -2147483648\n"
    "  145: SET_ALIAS  a4, r96\n"
    "  146: LABEL      L16\n"
    "  147: ANDI       r97, r81, 16\n"
    "  148: GOTO_IF_NZ r97, L14\n"
    "  149: LABEL      L15\n"
    "  150: FCVT       r98, r80\n"
    "  151: VBROADCAST r99, r98\n"
    "  152: SET_ALIAS  a2, r99\n"
    "  153: BITCAST    r100, r80\n"
    "  154: SGTUI      r101, r100, 0\n"
    "  155: SRLI       r102, r100, 31\n"
    "  156: BFEXT      r106, r100, 23, 8\n"
    "  157: SEQI       r103, r106, 0\n"
    "  158: SEQI       r104, r106, 255\n"
    "  159: SLLI       r107, r100, 9\n"
    "  160: SEQI       r105, r107, 0\n"
    "  161: AND        r108, r103, r105\n"
    "  162: XORI       r109, r105, 1\n"
    "  163: AND        r110, r104, r109\n"
    "  164: AND        r111, r103, r101\n"
    "  165: OR         r112, r111, r110\n"
    "  166: OR         r113, r108, r110\n"
    "  167: XORI       r114, r113, 1\n"
    "  168: XORI       r115, r102, 1\n"
    "  169: AND        r116, r102, r114\n"
    "  170: AND        r117, r115, r114\n"
    "  171: SLLI       r118, r112, 4\n"
    "  172: SLLI       r119, r116, 3\n"
    "  173: SLLI       r120, r117, 2\n"
    "  174: SLLI       r121, r108, 1\n"
    "  175: OR         r122, r118, r119\n"
    "  176: OR         r123, r120, r121\n"
    "  177: OR         r124, r122, r104\n"
    "  178: OR         r125, r124, r123\n"
    "  179: FTESTEXC   r126, r82, INEXACT\n"
    "  180: SLLI       r127, r126, 5\n"
    "  181: OR         r128, r125, r127\n"
    "  182: SET_ALIAS  a5, r128\n"
    "  183: FTESTEXC   r129, r82, OVERFLOW\n"
    "  184: GOTO_IF_Z  r129, L17\n"
    "  185: GET_ALIAS  r130, a4\n"
    "  186: NOT        r131, r130\n"
    "  187: ORI        r132, r130, 268435456\n"
    "  188: ANDI       r133, r131, 268435456\n"
    "  189: SET_ALIAS  a4, r132\n"
    "  190: GOTO_IF_Z  r133, L18\n"
    "  191: ORI        r134, r132, -2147483648\n"
    "  192: SET_ALIAS  a4, r134\n"
    "  193: LABEL      L18\n"
    "  194: LABEL      L17\n"
    "  195: FTESTEXC   r135, r82, UNDERFLOW\n"
    "  196: GOTO_IF_Z  r135, L11\n"
    "  197: GET_ALIAS  r136, a4\n"
    "  198: NOT        r137, r136\n"
    "  199: ORI        r138, r136, 134217728\n"
    "  200: ANDI       r139, r137, 134217728\n"
    "  201: SET_ALIAS  a4, r138\n"
    "  202: GOTO_IF_Z  r139, L19\n"
    "  203: ORI        r140, r138, -2147483648\n"
    "  204: SET_ALIAS  a4, r140\n"
    "  205: LABEL      L19\n"
    "  206: LABEL      L11\n"
    "  207: GET_ALIAS  r141, a2\n"
    "  208: VEXTRACT   r142, r141, 0\n"
    "  209: GET_ALIAS  r143, a5\n"
    "  210: ANDI       r144, r143, 32\n"
    "  211: OR         r145, r78, r144\n"
    "  212: SET_ALIAS  a5, r145\n"
    "  213: GET_ALIAS  r146, a4\n"
    "  214: ANDI       r147, r146, 128\n"
    "  215: GOTO_IF_Z  r147, L20\n"
    "  216: SET_ALIAS  a2, r12\n"
    "  217: GOTO       L21\n"
    "  218: LABEL      L20\n"
    "  219: VBUILD2    r148, r77, r142\n"
    "  220: SET_ALIAS  a2, r148\n"
    "  221: GOTO       L21\n"
    "  222: LABEL      L1\n"
    "  223: GET_ALIAS  r149, a4\n"
    "  224: FGETSTATE  r150\n"
    "  225: FCLEAREXC  r151, r150\n"
    "  226: FSETSTATE  r151\n"
    "  227: FTESTEXC   r152, r150, ZERO_DIVIDE\n"
    "  228: GOTO_IF_Z  r152, L23\n"
    "  229: NOT        r153, r149\n"
    "  230: ORI        r154, r149, 67108864\n"
    "  231: ANDI       r155, r153, 67108864\n"
    "  232: SET_ALIAS  a4, r154\n"
    "  233: GOTO_IF_Z  r155, L24\n"
    "  234: ORI        r156, r154, -2147483648\n"
    "  235: SET_ALIAS  a4, r156\n"
    "  236: LABEL      L24\n"
    "  237: ANDI       r157, r149, 16\n"
    "  238: GOTO_IF_Z  r157, L23\n"
    "  239: VEXTRACT   r158, r8, 0\n"
    "  240: VEXTRACT   r159, r8, 1\n"
    "  241: VEXTRACT   r160, r6, 0\n"
    "  242: VEXTRACT   r161, r6, 1\n"
    "  243: FDIV       r162, r158, r160\n"
    "  244: FGETSTATE  r163\n"
    "  245: FDIV       r164, r159, r161\n"
    "  246: FTESTEXC   r165, r163, ZERO_DIVIDE\n"
    "  247: BITCAST    r166, r162\n"
    "  248: SGTUI      r167, r166, 0\n"
    "  249: SRLI       r168, r166, 31\n"
    "  250: BFEXT      r172, r166, 23, 8\n"
    "  251: SEQI       r169, r172, 0\n"
    "  252: SEQI       r170, r172, 255\n"
    "  253: SLLI       r173, r166, 9\n"
    "  254: SEQI       r171, r173, 0\n"
    "  255: AND        r174, r169, r171\n"
    "  256: XORI       r175, r171, 1\n"
    "  257: AND        r176, r170, r175\n"
    "  258: AND        r177, r169, r167\n"
    "  259: OR         r178, r177, r176\n"
    "  260: OR         r179, r174, r176\n"
    "  261: XORI       r180, r179, 1\n"
    "  262: XORI       r181, r168, 1\n"
    "  263: AND        r182, r168, r180\n"
    "  264: AND        r183, r181, r180\n"
    "  265: SLLI       r184, r178, 4\n"
    "  266: SLLI       r185, r182, 3\n"
    "  267: SLLI       r186, r183, 2\n"
    "  268: SLLI       r187, r174, 1\n"
    "  269: OR         r188, r184, r185\n"
    "  270: OR         r189, r186, r187\n"
    "  271: OR         r190, r188, r170\n"
    "  272: OR         r191, r190, r189\n"
    "  273: LOAD_IMM   r192, 0\n"
    "  274: SELECT     r193, r192, r191, r165\n"
    "  275: FGETSTATE  r194\n"
    "  276: FSETSTATE  r151\n"
    "  277: FTESTEXC   r195, r194, INEXACT\n"
    "  278: GOTO_IF_Z  r195, L25\n"
    "  279: GET_ALIAS  r196, a4\n"
    "  280: NOT        r197, r196\n"
    "  281: ORI        r198, r196, 33554432\n"
    "  282: ANDI       r199, r197, 33554432\n"
    "  283: SET_ALIAS  a4, r198\n"
    "  284: GOTO_IF_Z  r199, L26\n"
    "  285: ORI        r200, r198, -2147483648\n"
    "  286: SET_ALIAS  a4, r200\n"
    "  287: LABEL      L26\n"
    "  288: LABEL      L25\n"
    "  289: FTESTEXC   r201, r194, OVERFLOW\n"
    "  290: GOTO_IF_Z  r201, L27\n"
    "  291: GET_ALIAS  r202, a4\n"
    "  292: NOT        r203, r202\n"
    "  293: ORI        r204, r202, 268435456\n"
    "  294: ANDI       r205, r203, 268435456\n"
    "  295: SET_ALIAS  a4, r204\n"
    "  296: GOTO_IF_Z  r205, L28\n"
    "  297: ORI        r206, r204, -2147483648\n"
    "  298: SET_ALIAS  a4, r206\n"
    "  299: LABEL      L28\n"
    "  300: LABEL      L27\n"
    "  301: FTESTEXC   r207, r194, UNDERFLOW\n"
    "  302: GOTO_IF_Z  r207, L29\n"
    "  303: GET_ALIAS  r208, a4\n"
    "  304: NOT        r209, r208\n"
    "  305: ORI        r210, r208, 134217728\n"
    "  306: ANDI       r211, r209, 134217728\n"
    "  307: SET_ALIAS  a4, r210\n"
    "  308: GOTO_IF_Z  r211, L30\n"
    "  309: ORI        r212, r210, -2147483648\n"
    "  310: SET_ALIAS  a4, r212\n"
    "  311: LABEL      L30\n"
    "  312: LABEL      L29\n"
    "  313: SLLI       r213, r195, 5\n"
    "  314: OR         r214, r213, r193\n"
    "  315: SET_ALIAS  a5, r214\n"
    "  316: GOTO       L22\n"
    "  317: LABEL      L23\n"
    "  318: VFCVT      r215, r9\n"
    "  319: SET_ALIAS  a2, r215\n"
    "  320: VEXTRACT   r216, r9, 0\n"
    "  321: BITCAST    r217, r216\n"
    "  322: SGTUI      r218, r217, 0\n"
    "  323: SRLI       r219, r217, 31\n"
    "  324: BFEXT      r223, r217, 23, 8\n"
    "  325: SEQI       r220, r223, 0\n"
    "  326: SEQI       r221, r223, 255\n"
    "  327: SLLI       r224, r217, 9\n"
    "  328: SEQI       r222, r224, 0\n"
    "  329: AND        r225, r220, r222\n"
    "  330: XORI       r226, r222, 1\n"
    "  331: AND        r227, r221, r226\n"
    "  332: AND        r228, r220, r218\n"
    "  333: OR         r229, r228, r227\n"
    "  334: OR         r230, r225, r227\n"
    "  335: XORI       r231, r230, 1\n"
    "  336: XORI       r232, r219, 1\n"
    "  337: AND        r233, r219, r231\n"
    "  338: AND        r234, r232, r231\n"
    "  339: SLLI       r235, r229, 4\n"
    "  340: SLLI       r236, r233, 3\n"
    "  341: SLLI       r237, r234, 2\n"
    "  342: SLLI       r238, r225, 1\n"
    "  343: OR         r239, r235, r236\n"
    "  344: OR         r240, r237, r238\n"
    "  345: OR         r241, r239, r221\n"
    "  346: OR         r242, r241, r240\n"
    "  347: FTESTEXC   r243, r150, INEXACT\n"
    "  348: SLLI       r244, r243, 5\n"
    "  349: OR         r245, r242, r244\n"
    "  350: SET_ALIAS  a5, r245\n"
    "  351: GOTO_IF_Z  r243, L31\n"
    "  352: GET_ALIAS  r246, a4\n"
    "  353: NOT        r247, r246\n"
    "  354: ORI        r248, r246, 33554432\n"
    "  355: ANDI       r249, r247, 33554432\n"
    "  356: SET_ALIAS  a4, r248\n"
    "  357: GOTO_IF_Z  r249, L32\n"
    "  358: ORI        r250, r248, -2147483648\n"
    "  359: SET_ALIAS  a4, r250\n"
    "  360: LABEL      L32\n"
    "  361: LABEL      L31\n"
    "  362: FTESTEXC   r251, r150, OVERFLOW\n"
    "  363: GOTO_IF_Z  r251, L33\n"
    "  364: GET_ALIAS  r252, a4\n"
    "  365: NOT        r253, r252\n"
    "  366: ORI        r254, r252, 268435456\n"
    "  367: ANDI       r255, r253, 268435456\n"
    "  368: SET_ALIAS  a4, r254\n"
    "  369: GOTO_IF_Z  r255, L34\n"
    "  370: ORI        r256, r254, -2147483648\n"
    "  371: SET_ALIAS  a4, r256\n"
    "  372: LABEL      L34\n"
    "  373: LABEL      L33\n"
    "  374: FTESTEXC   r257, r150, UNDERFLOW\n"
    "  375: GOTO_IF_Z  r257, L22\n"
    "  376: GET_ALIAS  r258, a4\n"
    "  377: NOT        r259, r258\n"
    "  378: ORI        r260, r258, 134217728\n"
    "  379: ANDI       r261, r259, 134217728\n"
    "  380: SET_ALIAS  a4, r260\n"
    "  381: GOTO_IF_Z  r261, L35\n"
    "  382: ORI        r262, r260, -2147483648\n"
    "  383: SET_ALIAS  a4, r262\n"
    "  384: LABEL      L35\n"
    "  385: LABEL      L22\n"
    "  386: LABEL      L21\n"
    "  387: LOAD_IMM   r263, 4\n"
    "  388: SET_ALIAS  a1, r263\n"
    "  389: GET_ALIAS  r264, a4\n"
    "  390: GET_ALIAS  r265, a5\n"
    "  391: ANDI       r266, r264, -1611134977\n"
    "  392: SLLI       r267, r265, 12\n"
    "  393: OR         r268, r266, r267\n"
    "  394: SET_ALIAS  a4, r268\n"
    "  395: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,12] --> 1,38\n"
    "Block 1: 0 --> [13,23] --> 2,6\n"
    "Block 2: 1 --> [24,28] --> 3,4\n"
    "Block 3: 2 --> [29,30] --> 4\n"
    "Block 4: 3,2 --> [31,33] --> 5,6\n"
    "Block 5: 4,9 --> [34,38] --> 18\n"
    "Block 6: 1,4 --> [39,41] --> 7,10\n"
    "Block 7: 6 --> [42,46] --> 8,9\n"
    "Block 8: 7 --> [47,48] --> 9\n"
    "Block 9: 8,7 --> [49,51] --> 10,5\n"
    "Block 10: 9,6 --> [52,87] --> 11,14\n"
    "Block 11: 10 --> [88,93] --> 12,13\n"
    "Block 12: 11 --> [94,95] --> 13\n"
    "Block 13: 12,11 --> [96,96] --> 14\n"
    "Block 14: 13,10 --> [97,99] --> 15,18\n"
    "Block 15: 14 --> [100,105] --> 16,17\n"
    "Block 16: 15 --> [106,107] --> 17\n"
    "Block 17: 16,15 --> [108,108] --> 18\n"
    "Block 18: 17,5,14 --> [109,120] --> 19,23\n"
    "Block 19: 18 --> [121,125] --> 20,21\n"
    "Block 20: 19 --> [126,127] --> 21\n"
    "Block 21: 20,19 --> [128,130] --> 22,23\n"
    "Block 22: 21,26 --> [131,135] --> 35\n"
    "Block 23: 18,21 --> [136,138] --> 24,27\n"
    "Block 24: 23 --> [139,143] --> 25,26\n"
    "Block 25: 24 --> [144,145] --> 26\n"
    "Block 26: 25,24 --> [146,148] --> 27,22\n"
    "Block 27: 26,23 --> [149,184] --> 28,31\n"
    "Block 28: 27 --> [185,190] --> 29,30\n"
    "Block 29: 28 --> [191,192] --> 30\n"
    "Block 30: 29,28 --> [193,193] --> 31\n"
    "Block 31: 30,27 --> [194,196] --> 32,35\n"
    "Block 32: 31 --> [197,202] --> 33,34\n"
    "Block 33: 32 --> [203,204] --> 34\n"
    "Block 34: 33,32 --> [205,205] --> 35\n"
    "Block 35: 34,22,31 --> [206,215] --> 36,37\n"
    "Block 36: 35 --> [216,217] --> 68\n"
    "Block 37: 35 --> [218,221] --> 68\n"
    "Block 38: 0 --> [222,228] --> 39,55\n"
    "Block 39: 38 --> [229,233] --> 40,41\n"
    "Block 40: 39 --> [234,235] --> 41\n"
    "Block 41: 40,39 --> [236,238] --> 42,55\n"
    "Block 42: 41 --> [239,278] --> 43,46\n"
    "Block 43: 42 --> [279,284] --> 44,45\n"
    "Block 44: 43 --> [285,286] --> 45\n"
    "Block 45: 44,43 --> [287,287] --> 46\n"
    "Block 46: 45,42 --> [288,290] --> 47,50\n"
    "Block 47: 46 --> [291,296] --> 48,49\n"
    "Block 48: 47 --> [297,298] --> 49\n"
    "Block 49: 48,47 --> [299,299] --> 50\n"
    "Block 50: 49,46 --> [300,302] --> 51,54\n"
    "Block 51: 50 --> [303,308] --> 52,53\n"
    "Block 52: 51 --> [309,310] --> 53\n"
    "Block 53: 52,51 --> [311,311] --> 54\n"
    "Block 54: 53,50 --> [312,316] --> 67\n"
    "Block 55: 38,41 --> [317,351] --> 56,59\n"
    "Block 56: 55 --> [352,357] --> 57,58\n"
    "Block 57: 56 --> [358,359] --> 58\n"
    "Block 58: 57,56 --> [360,360] --> 59\n"
    "Block 59: 58,55 --> [361,363] --> 60,63\n"
    "Block 60: 59 --> [364,369] --> 61,62\n"
    "Block 61: 60 --> [370,371] --> 62\n"
    "Block 62: 61,60 --> [372,372] --> 63\n"
    "Block 63: 62,59 --> [373,375] --> 64,67\n"
    "Block 64: 63 --> [376,381] --> 65,66\n"
    "Block 65: 64 --> [382,383] --> 66\n"
    "Block 66: 65,64 --> [384,384] --> 67\n"
    "Block 67: 66,54,63 --> [385,385] --> 68\n"
    "Block 68: 67,36,37 --> [386,395] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"