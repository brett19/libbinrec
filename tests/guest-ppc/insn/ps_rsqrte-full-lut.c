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

static const unsigned int guest_opt = 0;
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
    "    7: LOAD_IMM   r7, 0\n"
    "    8: SET_ALIAS  a6, r7\n"
    "    9: VEXTRACT   r8, r6, 0\n"
    "   10: GET_ALIAS  r9, a4\n"
    "   11: BITCAST    r10, r8\n"
    "   12: LOAD_IMM   r11, 0x80000000\n"
    "   13: AND        r12, r10, r11\n"
    "   14: BFEXT      r13, r10, 0, 23\n"
    "   15: SET_ALIAS  a9, r13\n"
    "   16: BFEXT      r14, r10, 23, 8\n"
    "   17: SET_ALIAS  a8, r14\n"
    "   18: GOTO_IF_Z  r14, L4\n"
    "   19: SEQI       r15, r14, 255\n"
    "   20: GOTO_IF_NZ r15, L5\n"
    "   21: GOTO_IF_NZ r12, L6\n"
    "   22: GOTO       L7\n"
    "   23: LABEL      L5\n"
    "   24: BFEXT      r16, r10, 0, 23\n"
    "   25: GOTO_IF_NZ r16, L8\n"
    "   26: GOTO_IF_NZ r12, L6\n"
    "   27: LOAD_IMM   r17, 0.0f\n"
    "   28: SET_ALIAS  a7, r17\n"
    "   29: LOAD_IMM   r18, 2\n"
    "   30: SET_ALIAS  a5, r18\n"
    "   31: GOTO       L2\n"
    "   32: LABEL      L8\n"
    "   33: ANDI       r19, r13, 4194304\n"
    "   34: GOTO_IF_Z  r19, L9\n"
    "   35: SET_ALIAS  a7, r8\n"
    "   36: LOAD_IMM   r20, 17\n"
    "   37: SET_ALIAS  a5, r20\n"
    "   38: GOTO       L2\n"
    "   39: LABEL      L9\n"
    "   40: GET_ALIAS  r21, a4\n"
    "   41: NOT        r22, r21\n"
    "   42: ORI        r23, r21, 16777216\n"
    "   43: ANDI       r24, r22, 16777216\n"
    "   44: SET_ALIAS  a4, r23\n"
    "   45: GOTO_IF_Z  r24, L10\n"
    "   46: ORI        r25, r23, -2147483648\n"
    "   47: SET_ALIAS  a4, r25\n"
    "   48: LABEL      L10\n"
    "   49: ANDI       r26, r9, 128\n"
    "   50: GOTO_IF_NZ r26, L3\n"
    "   51: LOAD_IMM   r27, 0x400000\n"
    "   52: OR         r28, r10, r27\n"
    "   53: BITCAST    r29, r28\n"
    "   54: SET_ALIAS  a7, r29\n"
    "   55: LOAD_IMM   r30, 17\n"
    "   56: SET_ALIAS  a5, r30\n"
    "   57: GOTO       L2\n"
    "   58: LABEL      L4\n"
    "   59: BFEXT      r31, r10, 0, 23\n"
    "   60: GOTO_IF_NZ r31, L11\n"
    "   61: GET_ALIAS  r32, a4\n"
    "   62: NOT        r33, r32\n"
    "   63: ORI        r34, r32, 67108864\n"
    "   64: ANDI       r35, r33, 67108864\n"
    "   65: SET_ALIAS  a4, r34\n"
    "   66: GOTO_IF_Z  r35, L12\n"
    "   67: ORI        r36, r34, -2147483648\n"
    "   68: SET_ALIAS  a4, r36\n"
    "   69: LABEL      L12\n"
    "   70: ANDI       r37, r9, 16\n"
    "   71: GOTO_IF_NZ r37, L3\n"
    "   72: LOAD_IMM   r38, 0x7F800000\n"
    "   73: OR         r39, r12, r38\n"
    "   74: BITCAST    r40, r39\n"
    "   75: SET_ALIAS  a7, r40\n"
    "   76: LOAD_IMM   r41, 5\n"
    "   77: LOAD_IMM   r42, 9\n"
    "   78: SELECT     r43, r42, r41, r12\n"
    "   79: SET_ALIAS  a5, r43\n"
    "   80: GOTO       L2\n"
    "   81: LABEL      L11\n"
    "   82: GOTO_IF_NZ r12, L6\n"
    "   83: CLZ        r44, r31\n"
    "   84: ADDI       r45, r44, -8\n"
    "   85: ADDI       r46, r44, -9\n"
    "   86: SLL        r47, r31, r45\n"
    "   87: NEG        r48, r46\n"
    "   88: SET_ALIAS  a8, r48\n"
    "   89: BFEXT      r49, r47, 0, 23\n"
    "   90: ZCAST      r50, r49\n"
    "   91: SET_ALIAS  a9, r50\n"
    "   92: LABEL      L7\n"
    "   93: LOAD       r51, 1008(r1)\n"
    "   94: GET_ALIAS  r52, a9\n"
    "   95: SRLI       r53, r52, 19\n"
    "   96: SLLI       r54, r53, 2\n"
    "   97: GET_ALIAS  r55, a8\n"
    "   98: ANDI       r56, r55, 1\n"
    "   99: XORI       r57, r56, 1\n"
    "  100: SLLI       r58, r57, 6\n"
    "  101: OR         r59, r54, r58\n"
    "  102: LOAD_IMM   r60, 380\n"
    "  103: SUB        r61, r60, r55\n"
    "  104: SRLI       r62, r61, 1\n"
    "  105: ZCAST      r63, r59\n"
    "  106: ADD        r64, r51, r63\n"
    "  107: LOAD_U16   r65, 2(r64)\n"
    "  108: LOAD_U16   r66, 0(r64)\n"
    "  109: BFEXT      r67, r52, 8, 11\n"
    "  110: MUL        r68, r67, r65\n"
    "  111: SLLI       r69, r66, 11\n"
    "  112: SUB        r70, r69, r68\n"
    "  113: SRLI       r71, r70, 3\n"
    "  114: SLLI       r72, r62, 23\n"
    "  115: OR         r73, r71, r12\n"
    "  116: OR         r74, r73, r72\n"
    "  117: BITCAST    r75, r74\n"
    "  118: SET_ALIAS  a7, r75\n"
    "  119: BITCAST    r76, r75\n"
    "  120: SGTUI      r77, r76, 0\n"
    "  121: SRLI       r78, r76, 31\n"
    "  122: BFEXT      r82, r76, 23, 8\n"
    "  123: SEQI       r79, r82, 0\n"
    "  124: SEQI       r80, r82, 255\n"
    "  125: SLLI       r83, r76, 9\n"
    "  126: SEQI       r81, r83, 0\n"
    "  127: AND        r84, r79, r81\n"
    "  128: XORI       r85, r81, 1\n"
    "  129: AND        r86, r80, r85\n"
    "  130: AND        r87, r79, r77\n"
    "  131: OR         r88, r87, r86\n"
    "  132: OR         r89, r84, r86\n"
    "  133: XORI       r90, r89, 1\n"
    "  134: XORI       r91, r78, 1\n"
    "  135: AND        r92, r78, r90\n"
    "  136: AND        r93, r91, r90\n"
    "  137: SLLI       r94, r88, 4\n"
    "  138: SLLI       r95, r92, 3\n"
    "  139: SLLI       r96, r93, 2\n"
    "  140: SLLI       r97, r84, 1\n"
    "  141: OR         r98, r94, r95\n"
    "  142: OR         r99, r96, r97\n"
    "  143: OR         r100, r98, r80\n"
    "  144: OR         r101, r100, r99\n"
    "  145: SET_ALIAS  a5, r101\n"
    "  146: GOTO       L2\n"
    "  147: LABEL      L6\n"
    "  148: GET_ALIAS  r102, a4\n"
    "  149: NOT        r103, r102\n"
    "  150: ORI        r104, r102, 512\n"
    "  151: ANDI       r105, r103, 512\n"
    "  152: SET_ALIAS  a4, r104\n"
    "  153: GOTO_IF_Z  r105, L13\n"
    "  154: ORI        r106, r104, -2147483648\n"
    "  155: SET_ALIAS  a4, r106\n"
    "  156: LABEL      L13\n"
    "  157: ANDI       r107, r9, 128\n"
    "  158: GOTO_IF_NZ r107, L3\n"
    "  159: LOAD_IMM   r108, 17\n"
    "  160: SET_ALIAS  a5, r108\n"
    "  161: LOAD_IMM   r109, nan(0x400000)\n"
    "  162: SET_ALIAS  a7, r109\n"
    "  163: GOTO       L2\n"
    "  164: LABEL      L3\n"
    "  165: GET_ALIAS  r110, a5\n"
    "  166: ANDI       r111, r110, 31\n"
    "  167: SET_ALIAS  a5, r111\n"
    "  168: GOTO       L1\n"
    "  169: LABEL      L2\n"
    "  170: GOTO       L14\n"
    "  171: LABEL      L1\n"
    "  172: LOAD_IMM   r112, 1\n"
    "  173: SET_ALIAS  a6, r112\n"
    "  174: LABEL      L14\n"
    "  175: GET_ALIAS  r113, a5\n"
    "  176: GET_ALIAS  r114, a7\n"
    "  177: VEXTRACT   r115, r6, 1\n"
    "  178: GET_ALIAS  r116, a4\n"
    "  179: BITCAST    r117, r115\n"
    "  180: LOAD_IMM   r118, 0x80000000\n"
    "  181: AND        r119, r117, r118\n"
    "  182: BFEXT      r120, r117, 0, 23\n"
    "  183: SET_ALIAS  a12, r120\n"
    "  184: BFEXT      r121, r117, 23, 8\n"
    "  185: SET_ALIAS  a11, r121\n"
    "  186: GOTO_IF_Z  r121, L18\n"
    "  187: SEQI       r122, r121, 255\n"
    "  188: GOTO_IF_NZ r122, L19\n"
    "  189: GOTO_IF_NZ r119, L20\n"
    "  190: GOTO       L21\n"
    "  191: LABEL      L19\n"
    "  192: BFEXT      r123, r117, 0, 23\n"
    "  193: GOTO_IF_NZ r123, L22\n"
    "  194: GOTO_IF_NZ r119, L20\n"
    "  195: LOAD_IMM   r124, 0.0f\n"
    "  196: SET_ALIAS  a10, r124\n"
    "  197: LOAD_IMM   r125, 2\n"
    "  198: SET_ALIAS  a5, r125\n"
    "  199: GOTO       L16\n"
    "  200: LABEL      L22\n"
    "  201: ANDI       r126, r120, 4194304\n"
    "  202: GOTO_IF_Z  r126, L23\n"
    "  203: SET_ALIAS  a10, r115\n"
    "  204: LOAD_IMM   r127, 17\n"
    "  205: SET_ALIAS  a5, r127\n"
    "  206: GOTO       L16\n"
    "  207: LABEL      L23\n"
    "  208: GET_ALIAS  r128, a4\n"
    "  209: NOT        r129, r128\n"
    "  210: ORI        r130, r128, 16777216\n"
    "  211: ANDI       r131, r129, 16777216\n"
    "  212: SET_ALIAS  a4, r130\n"
    "  213: GOTO_IF_Z  r131, L24\n"
    "  214: ORI        r132, r130, -2147483648\n"
    "  215: SET_ALIAS  a4, r132\n"
    "  216: LABEL      L24\n"
    "  217: ANDI       r133, r116, 128\n"
    "  218: GOTO_IF_NZ r133, L17\n"
    "  219: LOAD_IMM   r134, 0x400000\n"
    "  220: OR         r135, r117, r134\n"
    "  221: BITCAST    r136, r135\n"
    "  222: SET_ALIAS  a10, r136\n"
    "  223: LOAD_IMM   r137, 17\n"
    "  224: SET_ALIAS  a5, r137\n"
    "  225: GOTO       L16\n"
    "  226: LABEL      L18\n"
    "  227: BFEXT      r138, r117, 0, 23\n"
    "  228: GOTO_IF_NZ r138, L25\n"
    "  229: GET_ALIAS  r139, a4\n"
    "  230: NOT        r140, r139\n"
    "  231: ORI        r141, r139, 67108864\n"
    "  232: ANDI       r142, r140, 67108864\n"
    "  233: SET_ALIAS  a4, r141\n"
    "  234: GOTO_IF_Z  r142, L26\n"
    "  235: ORI        r143, r141, -2147483648\n"
    "  236: SET_ALIAS  a4, r143\n"
    "  237: LABEL      L26\n"
    "  238: ANDI       r144, r116, 16\n"
    "  239: GOTO_IF_NZ r144, L17\n"
    "  240: LOAD_IMM   r145, 0x7F800000\n"
    "  241: OR         r146, r119, r145\n"
    "  242: BITCAST    r147, r146\n"
    "  243: SET_ALIAS  a10, r147\n"
    "  244: LOAD_IMM   r148, 5\n"
    "  245: LOAD_IMM   r149, 9\n"
    "  246: SELECT     r150, r149, r148, r119\n"
    "  247: SET_ALIAS  a5, r150\n"
    "  248: GOTO       L16\n"
    "  249: LABEL      L25\n"
    "  250: GOTO_IF_NZ r119, L20\n"
    "  251: CLZ        r151, r138\n"
    "  252: ADDI       r152, r151, -8\n"
    "  253: ADDI       r153, r151, -9\n"
    "  254: SLL        r154, r138, r152\n"
    "  255: NEG        r155, r153\n"
    "  256: SET_ALIAS  a11, r155\n"
    "  257: BFEXT      r156, r154, 0, 23\n"
    "  258: ZCAST      r157, r156\n"
    "  259: SET_ALIAS  a12, r157\n"
    "  260: LABEL      L21\n"
    "  261: LOAD       r158, 1008(r1)\n"
    "  262: GET_ALIAS  r159, a12\n"
    "  263: SRLI       r160, r159, 19\n"
    "  264: SLLI       r161, r160, 2\n"
    "  265: GET_ALIAS  r162, a11\n"
    "  266: ANDI       r163, r162, 1\n"
    "  267: XORI       r164, r163, 1\n"
    "  268: SLLI       r165, r164, 6\n"
    "  269: OR         r166, r161, r165\n"
    "  270: LOAD_IMM   r167, 380\n"
    "  271: SUB        r168, r167, r162\n"
    "  272: SRLI       r169, r168, 1\n"
    "  273: ZCAST      r170, r166\n"
    "  274: ADD        r171, r158, r170\n"
    "  275: LOAD_U16   r172, 2(r171)\n"
    "  276: LOAD_U16   r173, 0(r171)\n"
    "  277: BFEXT      r174, r159, 8, 11\n"
    "  278: MUL        r175, r174, r172\n"
    "  279: SLLI       r176, r173, 11\n"
    "  280: SUB        r177, r176, r175\n"
    "  281: SRLI       r178, r177, 3\n"
    "  282: SLLI       r179, r169, 23\n"
    "  283: OR         r180, r178, r119\n"
    "  284: OR         r181, r180, r179\n"
    "  285: BITCAST    r182, r181\n"
    "  286: SET_ALIAS  a10, r182\n"
    "  287: BITCAST    r183, r182\n"
    "  288: SGTUI      r184, r183, 0\n"
    "  289: SRLI       r185, r183, 31\n"
    "  290: BFEXT      r189, r183, 23, 8\n"
    "  291: SEQI       r186, r189, 0\n"
    "  292: SEQI       r187, r189, 255\n"
    "  293: SLLI       r190, r183, 9\n"
    "  294: SEQI       r188, r190, 0\n"
    "  295: AND        r191, r186, r188\n"
    "  296: XORI       r192, r188, 1\n"
    "  297: AND        r193, r187, r192\n"
    "  298: AND        r194, r186, r184\n"
    "  299: OR         r195, r194, r193\n"
    "  300: OR         r196, r191, r193\n"
    "  301: XORI       r197, r196, 1\n"
    "  302: XORI       r198, r185, 1\n"
    "  303: AND        r199, r185, r197\n"
    "  304: AND        r200, r198, r197\n"
    "  305: SLLI       r201, r195, 4\n"
    "  306: SLLI       r202, r199, 3\n"
    "  307: SLLI       r203, r200, 2\n"
    "  308: SLLI       r204, r191, 1\n"
    "  309: OR         r205, r201, r202\n"
    "  310: OR         r206, r203, r204\n"
    "  311: OR         r207, r205, r187\n"
    "  312: OR         r208, r207, r206\n"
    "  313: SET_ALIAS  a5, r208\n"
    "  314: GOTO       L16\n"
    "  315: LABEL      L20\n"
    "  316: GET_ALIAS  r209, a4\n"
    "  317: NOT        r210, r209\n"
    "  318: ORI        r211, r209, 512\n"
    "  319: ANDI       r212, r210, 512\n"
    "  320: SET_ALIAS  a4, r211\n"
    "  321: GOTO_IF_Z  r212, L27\n"
    "  322: ORI        r213, r211, -2147483648\n"
    "  323: SET_ALIAS  a4, r213\n"
    "  324: LABEL      L27\n"
    "  325: ANDI       r214, r116, 128\n"
    "  326: GOTO_IF_NZ r214, L17\n"
    "  327: LOAD_IMM   r215, 17\n"
    "  328: SET_ALIAS  a5, r215\n"
    "  329: LOAD_IMM   r216, nan(0x400000)\n"
    "  330: SET_ALIAS  a10, r216\n"
    "  331: GOTO       L16\n"
    "  332: LABEL      L17\n"
    "  333: GET_ALIAS  r217, a5\n"
    "  334: ANDI       r218, r217, 31\n"
    "  335: SET_ALIAS  a5, r218\n"
    "  336: GOTO       L15\n"
    "  337: LABEL      L16\n"
    "  338: GOTO       L28\n"
    "  339: LABEL      L15\n"
    "  340: LOAD_IMM   r219, 1\n"
    "  341: SET_ALIAS  a6, r219\n"
    "  342: LABEL      L28\n"
    "  343: SET_ALIAS  a5, r113\n"
    "  344: GET_ALIAS  r220, a10\n"
    "  345: GET_ALIAS  r221, a6\n"
    "  346: GOTO_IF_NZ r221, L29\n"
    "  347: VBUILD2    r222, r114, r220\n"
    "  348: VFCVT      r223, r222\n"
    "  349: SET_ALIAS  a2, r223\n"
    "  350: LABEL      L29\n"
    "  351: LOAD_IMM   r224, 4\n"
    "  352: SET_ALIAS  a1, r224\n"
    "  353: GET_ALIAS  r225, a4\n"
    "  354: GET_ALIAS  r226, a5\n"
    "  355: ANDI       r227, r225, -1611134977\n"
    "  356: SLLI       r228, r226, 12\n"
    "  357: OR         r229, r227, r228\n"
    "  358: SET_ALIAS  a4, r229\n"
    "  359: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 944(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: float32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32, no bound storage\n"
    "Alias 10: float32, no bound storage\n"
    "Alias 11: int32, no bound storage\n"
    "Alias 12: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,18] --> 1,13\n"
    "Block 1: 0 --> [19,20] --> 2,4\n"
    "Block 2: 1 --> [21,21] --> 3,21\n"
    "Block 3: 2 --> [22,22] --> 20\n"
    "Block 4: 1 --> [23,25] --> 5,7\n"
    "Block 5: 4 --> [26,26] --> 6,21\n"
    "Block 6: 5 --> [27,31] --> 26\n"
    "Block 7: 4 --> [32,34] --> 8,9\n"
    "Block 8: 7 --> [35,38] --> 26\n"
    "Block 9: 7 --> [39,45] --> 10,11\n"
    "Block 10: 9 --> [46,47] --> 11\n"
    "Block 11: 10,9 --> [48,50] --> 12,25\n"
    "Block 12: 11 --> [51,57] --> 26\n"
    "Block 13: 0 --> [58,60] --> 14,18\n"
    "Block 14: 13 --> [61,66] --> 15,16\n"
    "Block 15: 14 --> [67,68] --> 16\n"
    "Block 16: 15,14 --> [69,71] --> 17,25\n"
    "Block 17: 16 --> [72,80] --> 26\n"
    "Block 18: 13 --> [81,82] --> 19,21\n"
    "Block 19: 18 --> [83,91] --> 20\n"
    "Block 20: 19,3 --> [92,146] --> 26\n"
    "Block 21: 2,5,18 --> [147,153] --> 22,23\n"
    "Block 22: 21 --> [154,155] --> 23\n"
    "Block 23: 22,21 --> [156,158] --> 24,25\n"
    "Block 24: 23 --> [159,163] --> 26\n"
    "Block 25: 11,16,23 --> [164,168] --> 27\n"
    "Block 26: 6,8,12,17,20,24 --> [169,170] --> 28\n"
    "Block 27: 25 --> [171,173] --> 28\n"
    "Block 28: 27,26 --> [174,186] --> 29,41\n"
    "Block 29: 28 --> [187,188] --> 30,32\n"
    "Block 30: 29 --> [189,189] --> 31,49\n"
    "Block 31: 30 --> [190,190] --> 48\n"
    "Block 32: 29 --> [191,193] --> 33,35\n"
    "Block 33: 32 --> [194,194] --> 34,49\n"
    "Block 34: 33 --> [195,199] --> 54\n"
    "Block 35: 32 --> [200,202] --> 36,37\n"
    "Block 36: 35 --> [203,206] --> 54\n"
    "Block 37: 35 --> [207,213] --> 38,39\n"
    "Block 38: 37 --> [214,215] --> 39\n"
    "Block 39: 38,37 --> [216,218] --> 40,53\n"
    "Block 40: 39 --> [219,225] --> 54\n"
    "Block 41: 28 --> [226,228] --> 42,46\n"
    "Block 42: 41 --> [229,234] --> 43,44\n"
    "Block 43: 42 --> [235,236] --> 44\n"
    "Block 44: 43,42 --> [237,239] --> 45,53\n"
    "Block 45: 44 --> [240,248] --> 54\n"
    "Block 46: 41 --> [249,250] --> 47,49\n"
    "Block 47: 46 --> [251,259] --> 48\n"
    "Block 48: 47,31 --> [260,314] --> 54\n"
    "Block 49: 30,33,46 --> [315,321] --> 50,51\n"
    "Block 50: 49 --> [322,323] --> 51\n"
    "Block 51: 50,49 --> [324,326] --> 52,53\n"
    "Block 52: 51 --> [327,331] --> 54\n"
    "Block 53: 39,44,51 --> [332,336] --> 55\n"
    "Block 54: 34,36,40,45,48,52 --> [337,338] --> 56\n"
    "Block 55: 53 --> [339,341] --> 56\n"
    "Block 56: 55,54 --> [342,346] --> 57,58\n"
    "Block 57: 56 --> [347,349] --> 58\n"
    "Block 58: 57,56 --> [350,359] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"