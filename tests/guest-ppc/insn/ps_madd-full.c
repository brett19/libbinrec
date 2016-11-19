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
    0x10,0x22,0x20,0xFA,  // ps_madd f1,f2,f3,f4
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
    "    6: GET_ALIAS  r6, a4\n"
    "    7: GET_ALIAS  r7, a5\n"
    "    8: VEXTRACT   r8, r5, 0\n"
    "    9: VEXTRACT   r9, r5, 1\n"
    "   10: VEXTRACT   r10, r6, 0\n"
    "   11: VEXTRACT   r11, r6, 1\n"
    "   12: VEXTRACT   r12, r7, 0\n"
    "   13: VEXTRACT   r13, r7, 1\n"
    "   14: SET_ALIAS  a8, r8\n"
    "   15: SET_ALIAS  a9, r10\n"
    "   16: BITCAST    r14, r10\n"
    "   17: BITCAST    r15, r8\n"
    "   18: BFEXT      r16, r14, 0, 52\n"
    "   19: GOTO_IF_Z  r16, L1\n"
    "   20: BFEXT      r17, r15, 52, 11\n"
    "   21: BFEXT      r18, r14, 52, 11\n"
    "   22: SEQI       r19, r17, 2047\n"
    "   23: GOTO_IF_NZ r19, L1\n"
    "   24: SEQI       r20, r18, 2047\n"
    "   25: GOTO_IF_NZ r20, L1\n"
    "   26: GOTO_IF_NZ r18, L2\n"
    "   27: CLZ        r21, r16\n"
    "   28: ADDI       r22, r21, -11\n"
    "   29: SUB        r23, r17, r22\n"
    "   30: SGTSI      r24, r23, 0\n"
    "   31: GOTO_IF_Z  r24, L1\n"
    "   32: LOAD_IMM   r25, 0x8000000000000000\n"
    "   33: AND        r26, r14, r25\n"
    "   34: SLL        r27, r16, r22\n"
    "   35: BFINS      r28, r15, r23, 52, 11\n"
    "   36: OR         r29, r26, r27\n"
    "   37: BITCAST    r30, r28\n"
    "   38: BITCAST    r31, r29\n"
    "   39: SET_ALIAS  a8, r30\n"
    "   40: SET_ALIAS  a9, r31\n"
    "   41: LABEL      L2\n"
    "   42: GET_ALIAS  r32, a9\n"
    "   43: BITCAST    r33, r32\n"
    "   44: ANDI       r34, r33, -134217728\n"
    "   45: ANDI       r35, r33, 134217728\n"
    "   46: ADD        r36, r34, r35\n"
    "   47: BITCAST    r37, r36\n"
    "   48: SET_ALIAS  a9, r37\n"
    "   49: BFEXT      r38, r36, 52, 11\n"
    "   50: SEQI       r39, r38, 2047\n"
    "   51: GOTO_IF_Z  r39, L1\n"
    "   52: LOAD_IMM   r40, 0x10000000000000\n"
    "   53: SUB        r41, r36, r40\n"
    "   54: BITCAST    r42, r41\n"
    "   55: SET_ALIAS  a9, r42\n"
    "   56: BFEXT      r43, r15, 52, 11\n"
    "   57: GOTO_IF_Z  r43, L3\n"
    "   58: SGTUI      r44, r43, 2045\n"
    "   59: GOTO_IF_NZ r44, L1\n"
    "   60: ADD        r45, r15, r40\n"
    "   61: BITCAST    r46, r45\n"
    "   62: SET_ALIAS  a8, r46\n"
    "   63: GOTO       L1\n"
    "   64: LABEL      L3\n"
    "   65: SLLI       r47, r15, 1\n"
    "   66: BFINS      r48, r15, r47, 0, 63\n"
    "   67: BITCAST    r49, r48\n"
    "   68: SET_ALIAS  a8, r49\n"
    "   69: LABEL      L1\n"
    "   70: GET_ALIAS  r50, a8\n"
    "   71: GET_ALIAS  r51, a9\n"
    "   72: GET_ALIAS  r52, a2\n"
    "   73: FMADD      r53, r50, r51, r12\n"
    "   74: FGETSTATE  r54\n"
    "   75: FTESTEXC   r55, r54, INVALID\n"
    "   76: LOAD_IMM   r56, 0.0\n"
    "   77: FCMP       r57, r50, r56, NUN\n"
    "   78: FCMP       r58, r51, r56, UN\n"
    "   79: FCMP       r59, r12, r56, UN\n"
    "   80: AND        r60, r57, r58\n"
    "   81: AND        r61, r60, r59\n"
    "   82: BITCAST    r62, r12\n"
    "   83: LOAD_IMM   r63, 0x8000000000000\n"
    "   84: OR         r64, r62, r63\n"
    "   85: BITCAST    r65, r64\n"
    "   86: SELECT     r66, r65, r53, r61\n"
    "   87: FCVT       r67, r66\n"
    "   88: LOAD_IMM   r68, 0x1000000\n"
    "   89: BITCAST    r69, r67\n"
    "   90: SLLI       r70, r69, 1\n"
    "   91: SEQ        r71, r70, r68\n"
    "   92: GOTO_IF_Z  r71, L4\n"
    "   93: FGETSTATE  r72\n"
    "   94: FSETROUND  r73, r72, TRUNC\n"
    "   95: FSETSTATE  r73\n"
    "   96: FMADD      r74, r50, r51, r12\n"
    "   97: FCVT       r75, r74\n"
    "   98: FGETSTATE  r76\n"
    "   99: FCOPYROUND r77, r76, r72\n"
    "  100: FSETSTATE  r77\n"
    "  101: LABEL      L4\n"
    "  102: GET_ALIAS  r78, a6\n"
    "  103: FGETSTATE  r79\n"
    "  104: FCLEAREXC  r80, r79\n"
    "  105: FSETSTATE  r80\n"
    "  106: FTESTEXC   r81, r79, INVALID\n"
    "  107: GOTO_IF_Z  r81, L5\n"
    "  108: BITCAST    r82, r50\n"
    "  109: SLLI       r83, r82, 13\n"
    "  110: BFEXT      r84, r82, 51, 12\n"
    "  111: SEQI       r85, r84, 4094\n"
    "  112: GOTO_IF_Z  r85, L7\n"
    "  113: GOTO_IF_NZ r83, L6\n"
    "  114: LABEL      L7\n"
    "  115: BITCAST    r86, r51\n"
    "  116: SLLI       r87, r86, 13\n"
    "  117: BFEXT      r88, r86, 51, 12\n"
    "  118: SEQI       r89, r88, 4094\n"
    "  119: GOTO_IF_Z  r89, L8\n"
    "  120: GOTO_IF_NZ r87, L6\n"
    "  121: LABEL      L8\n"
    "  122: BITCAST    r90, r12\n"
    "  123: SLLI       r91, r90, 13\n"
    "  124: BFEXT      r92, r90, 51, 12\n"
    "  125: SEQI       r93, r92, 4094\n"
    "  126: GOTO_IF_Z  r93, L10\n"
    "  127: GOTO_IF_NZ r91, L9\n"
    "  128: LABEL      L10\n"
    "  129: BITCAST    r94, r50\n"
    "  130: BITCAST    r95, r51\n"
    "  131: SLLI       r96, r94, 1\n"
    "  132: SLLI       r97, r95, 1\n"
    "  133: LOAD_IMM   r98, 0xFFE0000000000000\n"
    "  134: SEQ        r99, r96, r98\n"
    "  135: GOTO_IF_Z  r99, L11\n"
    "  136: GOTO_IF_Z  r97, L12\n"
    "  137: GOTO       L13\n"
    "  138: LABEL      L11\n"
    "  139: GOTO_IF_NZ r96, L13\n"
    "  140: LABEL      L12\n"
    "  141: NOT        r100, r78\n"
    "  142: ORI        r101, r78, 1048576\n"
    "  143: ANDI       r102, r100, 1048576\n"
    "  144: SET_ALIAS  a6, r101\n"
    "  145: GOTO_IF_Z  r102, L14\n"
    "  146: ORI        r103, r101, -2147483648\n"
    "  147: SET_ALIAS  a6, r103\n"
    "  148: LABEL      L14\n"
    "  149: GOTO       L15\n"
    "  150: LABEL      L13\n"
    "  151: NOT        r104, r78\n"
    "  152: ORI        r105, r78, 8388608\n"
    "  153: ANDI       r106, r104, 8388608\n"
    "  154: SET_ALIAS  a6, r105\n"
    "  155: GOTO_IF_Z  r106, L16\n"
    "  156: ORI        r107, r105, -2147483648\n"
    "  157: SET_ALIAS  a6, r107\n"
    "  158: LABEL      L16\n"
    "  159: LABEL      L15\n"
    "  160: ANDI       r108, r78, 128\n"
    "  161: GOTO_IF_Z  r108, L17\n"
    "  162: GET_ALIAS  r109, a7\n"
    "  163: ANDI       r110, r109, 31\n"
    "  164: SET_ALIAS  a7, r110\n"
    "  165: GOTO       L18\n"
    "  166: LABEL      L17\n"
    "  167: LOAD_IMM   r111, nan(0x400000)\n"
    "  168: FCVT       r112, r111\n"
    "  169: VBROADCAST r113, r112\n"
    "  170: SET_ALIAS  a2, r113\n"
    "  171: LOAD_IMM   r114, 17\n"
    "  172: SET_ALIAS  a7, r114\n"
    "  173: GOTO       L18\n"
    "  174: LABEL      L9\n"
    "  175: FMUL       r115, r50, r51\n"
    "  176: FGETSTATE  r116\n"
    "  177: FSETSTATE  r80\n"
    "  178: FTESTEXC   r117, r116, INVALID\n"
    "  179: GOTO_IF_Z  r117, L6\n"
    "  180: NOT        r118, r78\n"
    "  181: ORI        r119, r78, 17825792\n"
    "  182: ANDI       r120, r118, 17825792\n"
    "  183: SET_ALIAS  a6, r119\n"
    "  184: GOTO_IF_Z  r120, L19\n"
    "  185: ORI        r121, r119, -2147483648\n"
    "  186: SET_ALIAS  a6, r121\n"
    "  187: LABEL      L19\n"
    "  188: GOTO       L20\n"
    "  189: LABEL      L6\n"
    "  190: NOT        r122, r78\n"
    "  191: ORI        r123, r78, 16777216\n"
    "  192: ANDI       r124, r122, 16777216\n"
    "  193: SET_ALIAS  a6, r123\n"
    "  194: GOTO_IF_Z  r124, L21\n"
    "  195: ORI        r125, r123, -2147483648\n"
    "  196: SET_ALIAS  a6, r125\n"
    "  197: LABEL      L21\n"
    "  198: LABEL      L20\n"
    "  199: ANDI       r126, r78, 128\n"
    "  200: GOTO_IF_Z  r126, L5\n"
    "  201: GET_ALIAS  r127, a7\n"
    "  202: ANDI       r128, r127, 31\n"
    "  203: SET_ALIAS  a7, r128\n"
    "  204: GOTO       L18\n"
    "  205: LABEL      L5\n"
    "  206: FCVT       r129, r67\n"
    "  207: VBROADCAST r130, r129\n"
    "  208: SET_ALIAS  a2, r130\n"
    "  209: BITCAST    r131, r67\n"
    "  210: SGTUI      r132, r131, 0\n"
    "  211: SRLI       r133, r131, 31\n"
    "  212: BFEXT      r137, r131, 23, 8\n"
    "  213: SEQI       r134, r137, 0\n"
    "  214: SEQI       r135, r137, 255\n"
    "  215: SLLI       r138, r131, 9\n"
    "  216: SEQI       r136, r138, 0\n"
    "  217: AND        r139, r134, r136\n"
    "  218: XORI       r140, r136, 1\n"
    "  219: AND        r141, r135, r140\n"
    "  220: AND        r142, r134, r132\n"
    "  221: OR         r143, r142, r141\n"
    "  222: OR         r144, r139, r141\n"
    "  223: XORI       r145, r144, 1\n"
    "  224: XORI       r146, r133, 1\n"
    "  225: AND        r147, r133, r145\n"
    "  226: AND        r148, r146, r145\n"
    "  227: SLLI       r149, r143, 4\n"
    "  228: SLLI       r150, r147, 3\n"
    "  229: SLLI       r151, r148, 2\n"
    "  230: SLLI       r152, r139, 1\n"
    "  231: OR         r153, r149, r150\n"
    "  232: OR         r154, r151, r152\n"
    "  233: OR         r155, r153, r135\n"
    "  234: OR         r156, r155, r154\n"
    "  235: FTESTEXC   r157, r79, INEXACT\n"
    "  236: SLLI       r158, r157, 5\n"
    "  237: OR         r159, r156, r158\n"
    "  238: SET_ALIAS  a7, r159\n"
    "  239: GOTO_IF_Z  r157, L22\n"
    "  240: GET_ALIAS  r160, a6\n"
    "  241: NOT        r161, r160\n"
    "  242: ORI        r162, r160, 33554432\n"
    "  243: ANDI       r163, r161, 33554432\n"
    "  244: SET_ALIAS  a6, r162\n"
    "  245: GOTO_IF_Z  r163, L23\n"
    "  246: ORI        r164, r162, -2147483648\n"
    "  247: SET_ALIAS  a6, r164\n"
    "  248: LABEL      L23\n"
    "  249: LABEL      L22\n"
    "  250: FTESTEXC   r165, r79, OVERFLOW\n"
    "  251: GOTO_IF_Z  r165, L24\n"
    "  252: GET_ALIAS  r166, a6\n"
    "  253: NOT        r167, r166\n"
    "  254: ORI        r168, r166, 268435456\n"
    "  255: ANDI       r169, r167, 268435456\n"
    "  256: SET_ALIAS  a6, r168\n"
    "  257: GOTO_IF_Z  r169, L25\n"
    "  258: ORI        r170, r168, -2147483648\n"
    "  259: SET_ALIAS  a6, r170\n"
    "  260: LABEL      L25\n"
    "  261: LABEL      L24\n"
    "  262: FTESTEXC   r171, r79, UNDERFLOW\n"
    "  263: GOTO_IF_Z  r171, L18\n"
    "  264: GET_ALIAS  r172, a6\n"
    "  265: NOT        r173, r172\n"
    "  266: ORI        r174, r172, 134217728\n"
    "  267: ANDI       r175, r173, 134217728\n"
    "  268: SET_ALIAS  a6, r174\n"
    "  269: GOTO_IF_Z  r175, L26\n"
    "  270: ORI        r176, r174, -2147483648\n"
    "  271: SET_ALIAS  a6, r176\n"
    "  272: LABEL      L26\n"
    "  273: LABEL      L18\n"
    "  274: GET_ALIAS  r177, a2\n"
    "  275: VEXTRACT   r178, r177, 0\n"
    "  276: GET_ALIAS  r179, a7\n"
    "  277: FMADD      r180, r9, r11, r13\n"
    "  278: FGETSTATE  r181\n"
    "  279: FTESTEXC   r182, r181, INVALID\n"
    "  280: LOAD_IMM   r183, 0.0\n"
    "  281: FCMP       r184, r9, r183, NUN\n"
    "  282: FCMP       r185, r11, r183, UN\n"
    "  283: FCMP       r186, r13, r183, UN\n"
    "  284: AND        r187, r184, r185\n"
    "  285: AND        r188, r187, r186\n"
    "  286: BITCAST    r189, r13\n"
    "  287: LOAD_IMM   r190, 0x8000000000000\n"
    "  288: OR         r191, r189, r190\n"
    "  289: BITCAST    r192, r191\n"
    "  290: SELECT     r193, r192, r180, r188\n"
    "  291: FCVT       r194, r193\n"
    "  292: LOAD_IMM   r195, 0x1000000\n"
    "  293: BITCAST    r196, r194\n"
    "  294: SLLI       r197, r196, 1\n"
    "  295: SEQ        r198, r197, r195\n"
    "  296: GOTO_IF_Z  r198, L27\n"
    "  297: FGETSTATE  r199\n"
    "  298: FSETROUND  r200, r199, TRUNC\n"
    "  299: FSETSTATE  r200\n"
    "  300: FMADD      r201, r9, r11, r13\n"
    "  301: FCVT       r202, r201\n"
    "  302: FGETSTATE  r203\n"
    "  303: FCOPYROUND r204, r203, r199\n"
    "  304: FSETSTATE  r204\n"
    "  305: LABEL      L27\n"
    "  306: GET_ALIAS  r205, a6\n"
    "  307: FGETSTATE  r206\n"
    "  308: FCLEAREXC  r207, r206\n"
    "  309: FSETSTATE  r207\n"
    "  310: FTESTEXC   r208, r206, INVALID\n"
    "  311: GOTO_IF_Z  r208, L28\n"
    "  312: BITCAST    r209, r9\n"
    "  313: SLLI       r210, r209, 13\n"
    "  314: BFEXT      r211, r209, 51, 12\n"
    "  315: SEQI       r212, r211, 4094\n"
    "  316: GOTO_IF_Z  r212, L30\n"
    "  317: GOTO_IF_NZ r210, L29\n"
    "  318: LABEL      L30\n"
    "  319: BITCAST    r213, r11\n"
    "  320: SLLI       r214, r213, 13\n"
    "  321: BFEXT      r215, r213, 51, 12\n"
    "  322: SEQI       r216, r215, 4094\n"
    "  323: GOTO_IF_Z  r216, L31\n"
    "  324: GOTO_IF_NZ r214, L29\n"
    "  325: LABEL      L31\n"
    "  326: BITCAST    r217, r13\n"
    "  327: SLLI       r218, r217, 13\n"
    "  328: BFEXT      r219, r217, 51, 12\n"
    "  329: SEQI       r220, r219, 4094\n"
    "  330: GOTO_IF_Z  r220, L33\n"
    "  331: GOTO_IF_NZ r218, L32\n"
    "  332: LABEL      L33\n"
    "  333: BITCAST    r221, r9\n"
    "  334: BITCAST    r222, r11\n"
    "  335: SLLI       r223, r221, 1\n"
    "  336: SLLI       r224, r222, 1\n"
    "  337: LOAD_IMM   r225, 0xFFE0000000000000\n"
    "  338: SEQ        r226, r223, r225\n"
    "  339: GOTO_IF_Z  r226, L34\n"
    "  340: GOTO_IF_Z  r224, L35\n"
    "  341: GOTO       L36\n"
    "  342: LABEL      L34\n"
    "  343: GOTO_IF_NZ r223, L36\n"
    "  344: LABEL      L35\n"
    "  345: NOT        r227, r205\n"
    "  346: ORI        r228, r205, 1048576\n"
    "  347: ANDI       r229, r227, 1048576\n"
    "  348: SET_ALIAS  a6, r228\n"
    "  349: GOTO_IF_Z  r229, L37\n"
    "  350: ORI        r230, r228, -2147483648\n"
    "  351: SET_ALIAS  a6, r230\n"
    "  352: LABEL      L37\n"
    "  353: GOTO       L38\n"
    "  354: LABEL      L36\n"
    "  355: NOT        r231, r205\n"
    "  356: ORI        r232, r205, 8388608\n"
    "  357: ANDI       r233, r231, 8388608\n"
    "  358: SET_ALIAS  a6, r232\n"
    "  359: GOTO_IF_Z  r233, L39\n"
    "  360: ORI        r234, r232, -2147483648\n"
    "  361: SET_ALIAS  a6, r234\n"
    "  362: LABEL      L39\n"
    "  363: LABEL      L38\n"
    "  364: ANDI       r235, r205, 128\n"
    "  365: GOTO_IF_Z  r235, L40\n"
    "  366: GET_ALIAS  r236, a7\n"
    "  367: ANDI       r237, r236, 31\n"
    "  368: SET_ALIAS  a7, r237\n"
    "  369: GOTO       L41\n"
    "  370: LABEL      L40\n"
    "  371: LOAD_IMM   r238, nan(0x400000)\n"
    "  372: FCVT       r239, r238\n"
    "  373: VBROADCAST r240, r239\n"
    "  374: SET_ALIAS  a2, r240\n"
    "  375: LOAD_IMM   r241, 17\n"
    "  376: SET_ALIAS  a7, r241\n"
    "  377: GOTO       L41\n"
    "  378: LABEL      L32\n"
    "  379: FMUL       r242, r9, r11\n"
    "  380: FGETSTATE  r243\n"
    "  381: FSETSTATE  r207\n"
    "  382: FTESTEXC   r244, r243, INVALID\n"
    "  383: GOTO_IF_Z  r244, L29\n"
    "  384: NOT        r245, r205\n"
    "  385: ORI        r246, r205, 17825792\n"
    "  386: ANDI       r247, r245, 17825792\n"
    "  387: SET_ALIAS  a6, r246\n"
    "  388: GOTO_IF_Z  r247, L42\n"
    "  389: ORI        r248, r246, -2147483648\n"
    "  390: SET_ALIAS  a6, r248\n"
    "  391: LABEL      L42\n"
    "  392: GOTO       L43\n"
    "  393: LABEL      L29\n"
    "  394: NOT        r249, r205\n"
    "  395: ORI        r250, r205, 16777216\n"
    "  396: ANDI       r251, r249, 16777216\n"
    "  397: SET_ALIAS  a6, r250\n"
    "  398: GOTO_IF_Z  r251, L44\n"
    "  399: ORI        r252, r250, -2147483648\n"
    "  400: SET_ALIAS  a6, r252\n"
    "  401: LABEL      L44\n"
    "  402: LABEL      L43\n"
    "  403: ANDI       r253, r205, 128\n"
    "  404: GOTO_IF_Z  r253, L28\n"
    "  405: GET_ALIAS  r254, a7\n"
    "  406: ANDI       r255, r254, 31\n"
    "  407: SET_ALIAS  a7, r255\n"
    "  408: GOTO       L41\n"
    "  409: LABEL      L28\n"
    "  410: FCVT       r256, r194\n"
    "  411: VBROADCAST r257, r256\n"
    "  412: SET_ALIAS  a2, r257\n"
    "  413: BITCAST    r258, r194\n"
    "  414: SGTUI      r259, r258, 0\n"
    "  415: SRLI       r260, r258, 31\n"
    "  416: BFEXT      r264, r258, 23, 8\n"
    "  417: SEQI       r261, r264, 0\n"
    "  418: SEQI       r262, r264, 255\n"
    "  419: SLLI       r265, r258, 9\n"
    "  420: SEQI       r263, r265, 0\n"
    "  421: AND        r266, r261, r263\n"
    "  422: XORI       r267, r263, 1\n"
    "  423: AND        r268, r262, r267\n"
    "  424: AND        r269, r261, r259\n"
    "  425: OR         r270, r269, r268\n"
    "  426: OR         r271, r266, r268\n"
    "  427: XORI       r272, r271, 1\n"
    "  428: XORI       r273, r260, 1\n"
    "  429: AND        r274, r260, r272\n"
    "  430: AND        r275, r273, r272\n"
    "  431: SLLI       r276, r270, 4\n"
    "  432: SLLI       r277, r274, 3\n"
    "  433: SLLI       r278, r275, 2\n"
    "  434: SLLI       r279, r266, 1\n"
    "  435: OR         r280, r276, r277\n"
    "  436: OR         r281, r278, r279\n"
    "  437: OR         r282, r280, r262\n"
    "  438: OR         r283, r282, r281\n"
    "  439: FTESTEXC   r284, r206, INEXACT\n"
    "  440: SLLI       r285, r284, 5\n"
    "  441: OR         r286, r283, r285\n"
    "  442: SET_ALIAS  a7, r286\n"
    "  443: GOTO_IF_Z  r284, L45\n"
    "  444: GET_ALIAS  r287, a6\n"
    "  445: NOT        r288, r287\n"
    "  446: ORI        r289, r287, 33554432\n"
    "  447: ANDI       r290, r288, 33554432\n"
    "  448: SET_ALIAS  a6, r289\n"
    "  449: GOTO_IF_Z  r290, L46\n"
    "  450: ORI        r291, r289, -2147483648\n"
    "  451: SET_ALIAS  a6, r291\n"
    "  452: LABEL      L46\n"
    "  453: LABEL      L45\n"
    "  454: FTESTEXC   r292, r206, OVERFLOW\n"
    "  455: GOTO_IF_Z  r292, L47\n"
    "  456: GET_ALIAS  r293, a6\n"
    "  457: NOT        r294, r293\n"
    "  458: ORI        r295, r293, 268435456\n"
    "  459: ANDI       r296, r294, 268435456\n"
    "  460: SET_ALIAS  a6, r295\n"
    "  461: GOTO_IF_Z  r296, L48\n"
    "  462: ORI        r297, r295, -2147483648\n"
    "  463: SET_ALIAS  a6, r297\n"
    "  464: LABEL      L48\n"
    "  465: LABEL      L47\n"
    "  466: FTESTEXC   r298, r206, UNDERFLOW\n"
    "  467: GOTO_IF_Z  r298, L41\n"
    "  468: GET_ALIAS  r299, a6\n"
    "  469: NOT        r300, r299\n"
    "  470: ORI        r301, r299, 134217728\n"
    "  471: ANDI       r302, r300, 134217728\n"
    "  472: SET_ALIAS  a6, r301\n"
    "  473: GOTO_IF_Z  r302, L49\n"
    "  474: ORI        r303, r301, -2147483648\n"
    "  475: SET_ALIAS  a6, r303\n"
    "  476: LABEL      L49\n"
    "  477: LABEL      L41\n"
    "  478: GET_ALIAS  r304, a2\n"
    "  479: VEXTRACT   r305, r304, 0\n"
    "  480: GET_ALIAS  r306, a7\n"
    "  481: ANDI       r307, r306, 32\n"
    "  482: OR         r308, r179, r307\n"
    "  483: SET_ALIAS  a7, r308\n"
    "  484: OR         r309, r55, r182\n"
    "  485: GOTO_IF_Z  r309, L50\n"
    "  486: GET_ALIAS  r310, a6\n"
    "  487: ANDI       r311, r310, 128\n"
    "  488: GOTO_IF_Z  r311, L50\n"
    "  489: SET_ALIAS  a2, r52\n"
    "  490: GOTO       L51\n"
    "  491: LABEL      L50\n"
    "  492: VBUILD2    r312, r178, r305\n"
    "  493: SET_ALIAS  a2, r312\n"
    "  494: LABEL      L51\n"
    "  495: LOAD_IMM   r313, 4\n"
    "  496: SET_ALIAS  a1, r313\n"
    "  497: GET_ALIAS  r314, a6\n"
    "  498: GET_ALIAS  r315, a7\n"
    "  499: ANDI       r316, r314, -1611134977\n"
    "  500: SLLI       r317, r315, 12\n"
    "  501: OR         r318, r316, r317\n"
    "  502: SET_ALIAS  a6, r318\n"
    "  503: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: float64[2] @ 432(r1)\n"
    "Alias 5: float64[2] @ 448(r1)\n"
    "Alias 6: int32 @ 944(r1)\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: float64, no bound storage\n"
    "Alias 9: float64, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,19] --> 1,11\n"
    "Block 1: 0 --> [20,23] --> 2,11\n"
    "Block 2: 1 --> [24,25] --> 3,11\n"
    "Block 3: 2 --> [26,26] --> 4,6\n"
    "Block 4: 3 --> [27,31] --> 5,11\n"
    "Block 5: 4 --> [32,40] --> 6\n"
    "Block 6: 5,3 --> [41,51] --> 7,11\n"
    "Block 7: 6 --> [52,57] --> 8,10\n"
    "Block 8: 7 --> [58,59] --> 9,11\n"
    "Block 9: 8 --> [60,63] --> 11\n"
    "Block 10: 7 --> [64,68] --> 11\n"
    "Block 11: 10,9,0,1,2,4,6,8 --> [69,92] --> 12,13\n"
    "Block 12: 11 --> [93,100] --> 13\n"
    "Block 13: 12,11 --> [101,107] --> 14,42\n"
    "Block 14: 13 --> [108,112] --> 15,16\n"
    "Block 15: 14 --> [113,113] --> 16,37\n"
    "Block 16: 15,14 --> [114,119] --> 17,18\n"
    "Block 17: 16 --> [120,120] --> 18,37\n"
    "Block 18: 17,16 --> [121,126] --> 19,20\n"
    "Block 19: 18 --> [127,127] --> 20,33\n"
    "Block 20: 19,18 --> [128,135] --> 21,23\n"
    "Block 21: 20 --> [136,136] --> 22,24\n"
    "Block 22: 21 --> [137,137] --> 27\n"
    "Block 23: 20 --> [138,139] --> 24,27\n"
    "Block 24: 23,21 --> [140,145] --> 25,26\n"
    "Block 25: 24 --> [146,147] --> 26\n"
    "Block 26: 25,24 --> [148,149] --> 30\n"
    "Block 27: 22,23 --> [150,155] --> 28,29\n"
    "Block 28: 27 --> [156,157] --> 29\n"
    "Block 29: 28,27 --> [158,158] --> 30\n"
    "Block 30: 29,26 --> [159,161] --> 31,32\n"
    "Block 31: 30 --> [162,165] --> 54\n"
    "Block 32: 30 --> [166,173] --> 54\n"
    "Block 33: 19 --> [174,179] --> 34,37\n"
    "Block 34: 33 --> [180,184] --> 35,36\n"
    "Block 35: 34 --> [185,186] --> 36\n"
    "Block 36: 35,34 --> [187,188] --> 40\n"
    "Block 37: 15,17,33 --> [189,194] --> 38,39\n"
    "Block 38: 37 --> [195,196] --> 39\n"
    "Block 39: 38,37 --> [197,197] --> 40\n"
    "Block 40: 39,36 --> [198,200] --> 41,42\n"
    "Block 41: 40 --> [201,204] --> 54\n"
    "Block 42: 13,40 --> [205,239] --> 43,46\n"
    "Block 43: 42 --> [240,245] --> 44,45\n"
    "Block 44: 43 --> [246,247] --> 45\n"
    "Block 45: 44,43 --> [248,248] --> 46\n"
    "Block 46: 45,42 --> [249,251] --> 47,50\n"
    "Block 47: 46 --> [252,257] --> 48,49\n"
    "Block 48: 47 --> [258,259] --> 49\n"
    "Block 49: 48,47 --> [260,260] --> 50\n"
    "Block 50: 49,46 --> [261,263] --> 51,54\n"
    "Block 51: 50 --> [264,269] --> 52,53\n"
    "Block 52: 51 --> [270,271] --> 53\n"
    "Block 53: 52,51 --> [272,272] --> 54\n"
    "Block 54: 53,31,32,41,50 --> [273,296] --> 55,56\n"
    "Block 55: 54 --> [297,304] --> 56\n"
    "Block 56: 55,54 --> [305,311] --> 57,85\n"
    "Block 57: 56 --> [312,316] --> 58,59\n"
    "Block 58: 57 --> [317,317] --> 59,80\n"
    "Block 59: 58,57 --> [318,323] --> 60,61\n"
    "Block 60: 59 --> [324,324] --> 61,80\n"
    "Block 61: 60,59 --> [325,330] --> 62,63\n"
    "Block 62: 61 --> [331,331] --> 63,76\n"
    "Block 63: 62,61 --> [332,339] --> 64,66\n"
    "Block 64: 63 --> [340,340] --> 65,67\n"
    "Block 65: 64 --> [341,341] --> 70\n"
    "Block 66: 63 --> [342,343] --> 67,70\n"
    "Block 67: 66,64 --> [344,349] --> 68,69\n"
    "Block 68: 67 --> [350,351] --> 69\n"
    "Block 69: 68,67 --> [352,353] --> 73\n"
    "Block 70: 65,66 --> [354,359] --> 71,72\n"
    "Block 71: 70 --> [360,361] --> 72\n"
    "Block 72: 71,70 --> [362,362] --> 73\n"
    "Block 73: 72,69 --> [363,365] --> 74,75\n"
    "Block 74: 73 --> [366,369] --> 97\n"
    "Block 75: 73 --> [370,377] --> 97\n"
    "Block 76: 62 --> [378,383] --> 77,80\n"
    "Block 77: 76 --> [384,388] --> 78,79\n"
    "Block 78: 77 --> [389,390] --> 79\n"
    "Block 79: 78,77 --> [391,392] --> 83\n"
    "Block 80: 58,60,76 --> [393,398] --> 81,82\n"
    "Block 81: 80 --> [399,400] --> 82\n"
    "Block 82: 81,80 --> [401,401] --> 83\n"
    "Block 83: 82,79 --> [402,404] --> 84,85\n"
    "Block 84: 83 --> [405,408] --> 97\n"
    "Block 85: 56,83 --> [409,443] --> 86,89\n"
    "Block 86: 85 --> [444,449] --> 87,88\n"
    "Block 87: 86 --> [450,451] --> 88\n"
    "Block 88: 87,86 --> [452,452] --> 89\n"
    "Block 89: 88,85 --> [453,455] --> 90,93\n"
    "Block 90: 89 --> [456,461] --> 91,92\n"
    "Block 91: 90 --> [462,463] --> 92\n"
    "Block 92: 91,90 --> [464,464] --> 93\n"
    "Block 93: 92,89 --> [465,467] --> 94,97\n"
    "Block 94: 93 --> [468,473] --> 95,96\n"
    "Block 95: 94 --> [474,475] --> 96\n"
    "Block 96: 95,94 --> [476,476] --> 97\n"
    "Block 97: 96,74,75,84,93 --> [477,485] --> 98,100\n"
    "Block 98: 97 --> [486,488] --> 99,100\n"
    "Block 99: 98 --> [489,490] --> 101\n"
    "Block 100: 97,98 --> [491,493] --> 101\n"
    "Block 101: 100,99 --> [494,503] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"