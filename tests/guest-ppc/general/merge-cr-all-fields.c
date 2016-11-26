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
    0x7C,0x8F,0x01,0x20,  // mtcrf 240,r4
    0x7C,0xA0,0xF1,0x20,  // mtcrf 15,r5
    0x7C,0x60,0x00,0x26,  // mfcr r3
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_USE_SPLIT_FIELDS;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0xB\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a5\n"
    "    3: BFEXT      r4, r3, 31, 1\n"
    "    4: SET_ALIAS  a6, r4\n"
    "    5: BFEXT      r5, r3, 30, 1\n"
    "    6: SET_ALIAS  a7, r5\n"
    "    7: BFEXT      r6, r3, 29, 1\n"
    "    8: SET_ALIAS  a8, r6\n"
    "    9: BFEXT      r7, r3, 28, 1\n"
    "   10: SET_ALIAS  a9, r7\n"
    "   11: BFEXT      r8, r3, 27, 1\n"
    "   12: SET_ALIAS  a10, r8\n"
    "   13: BFEXT      r9, r3, 26, 1\n"
    "   14: SET_ALIAS  a11, r9\n"
    "   15: BFEXT      r10, r3, 25, 1\n"
    "   16: SET_ALIAS  a12, r10\n"
    "   17: BFEXT      r11, r3, 24, 1\n"
    "   18: SET_ALIAS  a13, r11\n"
    "   19: BFEXT      r12, r3, 23, 1\n"
    "   20: SET_ALIAS  a14, r12\n"
    "   21: BFEXT      r13, r3, 22, 1\n"
    "   22: SET_ALIAS  a15, r13\n"
    "   23: BFEXT      r14, r3, 21, 1\n"
    "   24: SET_ALIAS  a16, r14\n"
    "   25: BFEXT      r15, r3, 20, 1\n"
    "   26: SET_ALIAS  a17, r15\n"
    "   27: BFEXT      r16, r3, 19, 1\n"
    "   28: SET_ALIAS  a18, r16\n"
    "   29: BFEXT      r17, r3, 18, 1\n"
    "   30: SET_ALIAS  a19, r17\n"
    "   31: BFEXT      r18, r3, 17, 1\n"
    "   32: SET_ALIAS  a20, r18\n"
    "   33: BFEXT      r19, r3, 16, 1\n"
    "   34: SET_ALIAS  a21, r19\n"
    "   35: BFEXT      r20, r3, 15, 1\n"
    "   36: SET_ALIAS  a22, r20\n"
    "   37: BFEXT      r21, r3, 14, 1\n"
    "   38: SET_ALIAS  a23, r21\n"
    "   39: BFEXT      r22, r3, 13, 1\n"
    "   40: SET_ALIAS  a24, r22\n"
    "   41: BFEXT      r23, r3, 12, 1\n"
    "   42: SET_ALIAS  a25, r23\n"
    "   43: BFEXT      r24, r3, 11, 1\n"
    "   44: SET_ALIAS  a26, r24\n"
    "   45: BFEXT      r25, r3, 10, 1\n"
    "   46: SET_ALIAS  a27, r25\n"
    "   47: BFEXT      r26, r3, 9, 1\n"
    "   48: SET_ALIAS  a28, r26\n"
    "   49: BFEXT      r27, r3, 8, 1\n"
    "   50: SET_ALIAS  a29, r27\n"
    "   51: BFEXT      r28, r3, 7, 1\n"
    "   52: SET_ALIAS  a30, r28\n"
    "   53: BFEXT      r29, r3, 6, 1\n"
    "   54: SET_ALIAS  a31, r29\n"
    "   55: BFEXT      r30, r3, 5, 1\n"
    "   56: SET_ALIAS  a32, r30\n"
    "   57: BFEXT      r31, r3, 4, 1\n"
    "   58: SET_ALIAS  a33, r31\n"
    "   59: BFEXT      r32, r3, 3, 1\n"
    "   60: SET_ALIAS  a34, r32\n"
    "   61: BFEXT      r33, r3, 2, 1\n"
    "   62: SET_ALIAS  a35, r33\n"
    "   63: BFEXT      r34, r3, 1, 1\n"
    "   64: SET_ALIAS  a36, r34\n"
    "   65: BFEXT      r35, r3, 0, 1\n"
    "   66: SET_ALIAS  a37, r35\n"
    "   67: GET_ALIAS  r36, a3\n"
    "   68: BFEXT      r37, r36, 31, 1\n"
    "   69: BFEXT      r38, r36, 30, 1\n"
    "   70: BFEXT      r39, r36, 29, 1\n"
    "   71: BFEXT      r40, r36, 28, 1\n"
    "   72: SET_ALIAS  a6, r37\n"
    "   73: SET_ALIAS  a7, r38\n"
    "   74: SET_ALIAS  a8, r39\n"
    "   75: SET_ALIAS  a9, r40\n"
    "   76: BFEXT      r41, r36, 27, 1\n"
    "   77: BFEXT      r42, r36, 26, 1\n"
    "   78: BFEXT      r43, r36, 25, 1\n"
    "   79: BFEXT      r44, r36, 24, 1\n"
    "   80: SET_ALIAS  a10, r41\n"
    "   81: SET_ALIAS  a11, r42\n"
    "   82: SET_ALIAS  a12, r43\n"
    "   83: SET_ALIAS  a13, r44\n"
    "   84: BFEXT      r45, r36, 23, 1\n"
    "   85: BFEXT      r46, r36, 22, 1\n"
    "   86: BFEXT      r47, r36, 21, 1\n"
    "   87: BFEXT      r48, r36, 20, 1\n"
    "   88: SET_ALIAS  a14, r45\n"
    "   89: SET_ALIAS  a15, r46\n"
    "   90: SET_ALIAS  a16, r47\n"
    "   91: SET_ALIAS  a17, r48\n"
    "   92: BFEXT      r49, r36, 19, 1\n"
    "   93: BFEXT      r50, r36, 18, 1\n"
    "   94: BFEXT      r51, r36, 17, 1\n"
    "   95: BFEXT      r52, r36, 16, 1\n"
    "   96: SET_ALIAS  a18, r49\n"
    "   97: SET_ALIAS  a19, r50\n"
    "   98: SET_ALIAS  a20, r51\n"
    "   99: SET_ALIAS  a21, r52\n"
    "  100: GET_ALIAS  r53, a4\n"
    "  101: BFEXT      r54, r53, 15, 1\n"
    "  102: BFEXT      r55, r53, 14, 1\n"
    "  103: BFEXT      r56, r53, 13, 1\n"
    "  104: BFEXT      r57, r53, 12, 1\n"
    "  105: SET_ALIAS  a22, r54\n"
    "  106: SET_ALIAS  a23, r55\n"
    "  107: SET_ALIAS  a24, r56\n"
    "  108: SET_ALIAS  a25, r57\n"
    "  109: BFEXT      r58, r53, 11, 1\n"
    "  110: BFEXT      r59, r53, 10, 1\n"
    "  111: BFEXT      r60, r53, 9, 1\n"
    "  112: BFEXT      r61, r53, 8, 1\n"
    "  113: SET_ALIAS  a26, r58\n"
    "  114: SET_ALIAS  a27, r59\n"
    "  115: SET_ALIAS  a28, r60\n"
    "  116: SET_ALIAS  a29, r61\n"
    "  117: BFEXT      r62, r53, 7, 1\n"
    "  118: BFEXT      r63, r53, 6, 1\n"
    "  119: BFEXT      r64, r53, 5, 1\n"
    "  120: BFEXT      r65, r53, 4, 1\n"
    "  121: SET_ALIAS  a30, r62\n"
    "  122: SET_ALIAS  a31, r63\n"
    "  123: SET_ALIAS  a32, r64\n"
    "  124: SET_ALIAS  a33, r65\n"
    "  125: BFEXT      r66, r53, 3, 1\n"
    "  126: BFEXT      r67, r53, 2, 1\n"
    "  127: BFEXT      r68, r53, 1, 1\n"
    "  128: BFEXT      r69, r53, 0, 1\n"
    "  129: SET_ALIAS  a34, r66\n"
    "  130: SET_ALIAS  a35, r67\n"
    "  131: SET_ALIAS  a36, r68\n"
    "  132: SET_ALIAS  a37, r69\n"
    "  133: SLLI       r70, r37, 31\n"
    "  134: SLLI       r71, r38, 30\n"
    "  135: OR         r72, r70, r71\n"
    "  136: SLLI       r73, r39, 29\n"
    "  137: OR         r74, r72, r73\n"
    "  138: SLLI       r75, r40, 28\n"
    "  139: OR         r76, r74, r75\n"
    "  140: SLLI       r77, r41, 27\n"
    "  141: OR         r78, r76, r77\n"
    "  142: SLLI       r79, r42, 26\n"
    "  143: OR         r80, r78, r79\n"
    "  144: SLLI       r81, r43, 25\n"
    "  145: OR         r82, r80, r81\n"
    "  146: SLLI       r83, r44, 24\n"
    "  147: OR         r84, r82, r83\n"
    "  148: SLLI       r85, r45, 23\n"
    "  149: OR         r86, r84, r85\n"
    "  150: SLLI       r87, r46, 22\n"
    "  151: OR         r88, r86, r87\n"
    "  152: SLLI       r89, r47, 21\n"
    "  153: OR         r90, r88, r89\n"
    "  154: SLLI       r91, r48, 20\n"
    "  155: OR         r92, r90, r91\n"
    "  156: SLLI       r93, r49, 19\n"
    "  157: OR         r94, r92, r93\n"
    "  158: SLLI       r95, r50, 18\n"
    "  159: OR         r96, r94, r95\n"
    "  160: SLLI       r97, r51, 17\n"
    "  161: OR         r98, r96, r97\n"
    "  162: SLLI       r99, r52, 16\n"
    "  163: OR         r100, r98, r99\n"
    "  164: SLLI       r101, r54, 15\n"
    "  165: OR         r102, r100, r101\n"
    "  166: SLLI       r103, r55, 14\n"
    "  167: OR         r104, r102, r103\n"
    "  168: SLLI       r105, r56, 13\n"
    "  169: OR         r106, r104, r105\n"
    "  170: SLLI       r107, r57, 12\n"
    "  171: OR         r108, r106, r107\n"
    "  172: SLLI       r109, r58, 11\n"
    "  173: OR         r110, r108, r109\n"
    "  174: SLLI       r111, r59, 10\n"
    "  175: OR         r112, r110, r111\n"
    "  176: SLLI       r113, r60, 9\n"
    "  177: OR         r114, r112, r113\n"
    "  178: SLLI       r115, r61, 8\n"
    "  179: OR         r116, r114, r115\n"
    "  180: SLLI       r117, r62, 7\n"
    "  181: OR         r118, r116, r117\n"
    "  182: SLLI       r119, r63, 6\n"
    "  183: OR         r120, r118, r119\n"
    "  184: SLLI       r121, r64, 5\n"
    "  185: OR         r122, r120, r121\n"
    "  186: SLLI       r123, r65, 4\n"
    "  187: OR         r124, r122, r123\n"
    "  188: SLLI       r125, r66, 3\n"
    "  189: OR         r126, r124, r125\n"
    "  190: SLLI       r127, r67, 2\n"
    "  191: OR         r128, r126, r127\n"
    "  192: SLLI       r129, r68, 1\n"
    "  193: OR         r130, r128, r129\n"
    "  194: OR         r131, r130, r69\n"
    "  195: SET_ALIAS  a5, r131\n"
    "  196: SET_ALIAS  a2, r131\n"
    "  197: LOAD_IMM   r132, 12\n"
    "  198: SET_ALIAS  a1, r132\n"
    "  199: GET_ALIAS  r133, a6\n"
    "  200: SLLI       r134, r133, 31\n"
    "  201: GET_ALIAS  r135, a7\n"
    "  202: SLLI       r136, r135, 30\n"
    "  203: OR         r137, r134, r136\n"
    "  204: GET_ALIAS  r138, a8\n"
    "  205: SLLI       r139, r138, 29\n"
    "  206: OR         r140, r137, r139\n"
    "  207: GET_ALIAS  r141, a9\n"
    "  208: SLLI       r142, r141, 28\n"
    "  209: OR         r143, r140, r142\n"
    "  210: GET_ALIAS  r144, a10\n"
    "  211: SLLI       r145, r144, 27\n"
    "  212: OR         r146, r143, r145\n"
    "  213: GET_ALIAS  r147, a11\n"
    "  214: SLLI       r148, r147, 26\n"
    "  215: OR         r149, r146, r148\n"
    "  216: GET_ALIAS  r150, a12\n"
    "  217: SLLI       r151, r150, 25\n"
    "  218: OR         r152, r149, r151\n"
    "  219: GET_ALIAS  r153, a13\n"
    "  220: SLLI       r154, r153, 24\n"
    "  221: OR         r155, r152, r154\n"
    "  222: GET_ALIAS  r156, a14\n"
    "  223: SLLI       r157, r156, 23\n"
    "  224: OR         r158, r155, r157\n"
    "  225: GET_ALIAS  r159, a15\n"
    "  226: SLLI       r160, r159, 22\n"
    "  227: OR         r161, r158, r160\n"
    "  228: GET_ALIAS  r162, a16\n"
    "  229: SLLI       r163, r162, 21\n"
    "  230: OR         r164, r161, r163\n"
    "  231: GET_ALIAS  r165, a17\n"
    "  232: SLLI       r166, r165, 20\n"
    "  233: OR         r167, r164, r166\n"
    "  234: GET_ALIAS  r168, a18\n"
    "  235: SLLI       r169, r168, 19\n"
    "  236: OR         r170, r167, r169\n"
    "  237: GET_ALIAS  r171, a19\n"
    "  238: SLLI       r172, r171, 18\n"
    "  239: OR         r173, r170, r172\n"
    "  240: GET_ALIAS  r174, a20\n"
    "  241: SLLI       r175, r174, 17\n"
    "  242: OR         r176, r173, r175\n"
    "  243: GET_ALIAS  r177, a21\n"
    "  244: SLLI       r178, r177, 16\n"
    "  245: OR         r179, r176, r178\n"
    "  246: GET_ALIAS  r180, a22\n"
    "  247: SLLI       r181, r180, 15\n"
    "  248: OR         r182, r179, r181\n"
    "  249: GET_ALIAS  r183, a23\n"
    "  250: SLLI       r184, r183, 14\n"
    "  251: OR         r185, r182, r184\n"
    "  252: GET_ALIAS  r186, a24\n"
    "  253: SLLI       r187, r186, 13\n"
    "  254: OR         r188, r185, r187\n"
    "  255: GET_ALIAS  r189, a25\n"
    "  256: SLLI       r190, r189, 12\n"
    "  257: OR         r191, r188, r190\n"
    "  258: GET_ALIAS  r192, a26\n"
    "  259: SLLI       r193, r192, 11\n"
    "  260: OR         r194, r191, r193\n"
    "  261: GET_ALIAS  r195, a27\n"
    "  262: SLLI       r196, r195, 10\n"
    "  263: OR         r197, r194, r196\n"
    "  264: GET_ALIAS  r198, a28\n"
    "  265: SLLI       r199, r198, 9\n"
    "  266: OR         r200, r197, r199\n"
    "  267: GET_ALIAS  r201, a29\n"
    "  268: SLLI       r202, r201, 8\n"
    "  269: OR         r203, r200, r202\n"
    "  270: GET_ALIAS  r204, a30\n"
    "  271: SLLI       r205, r204, 7\n"
    "  272: OR         r206, r203, r205\n"
    "  273: GET_ALIAS  r207, a31\n"
    "  274: SLLI       r208, r207, 6\n"
    "  275: OR         r209, r206, r208\n"
    "  276: GET_ALIAS  r210, a32\n"
    "  277: SLLI       r211, r210, 5\n"
    "  278: OR         r212, r209, r211\n"
    "  279: GET_ALIAS  r213, a33\n"
    "  280: SLLI       r214, r213, 4\n"
    "  281: OR         r215, r212, r214\n"
    "  282: GET_ALIAS  r216, a34\n"
    "  283: SLLI       r217, r216, 3\n"
    "  284: OR         r218, r215, r217\n"
    "  285: GET_ALIAS  r219, a35\n"
    "  286: SLLI       r220, r219, 2\n"
    "  287: OR         r221, r218, r220\n"
    "  288: GET_ALIAS  r222, a36\n"
    "  289: SLLI       r223, r222, 1\n"
    "  290: OR         r224, r221, r223\n"
    "  291: GET_ALIAS  r225, a37\n"
    "  292: OR         r226, r224, r225\n"
    "  293: SET_ALIAS  a5, r226\n"
    "  294: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 276(r1)\n"
    "Alias 5: int32 @ 928(r1)\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32, no bound storage\n"
    "Alias 10: int32, no bound storage\n"
    "Alias 11: int32, no bound storage\n"
    "Alias 12: int32, no bound storage\n"
    "Alias 13: int32, no bound storage\n"
    "Alias 14: int32, no bound storage\n"
    "Alias 15: int32, no bound storage\n"
    "Alias 16: int32, no bound storage\n"
    "Alias 17: int32, no bound storage\n"
    "Alias 18: int32, no bound storage\n"
    "Alias 19: int32, no bound storage\n"
    "Alias 20: int32, no bound storage\n"
    "Alias 21: int32, no bound storage\n"
    "Alias 22: int32, no bound storage\n"
    "Alias 23: int32, no bound storage\n"
    "Alias 24: int32, no bound storage\n"
    "Alias 25: int32, no bound storage\n"
    "Alias 26: int32, no bound storage\n"
    "Alias 27: int32, no bound storage\n"
    "Alias 28: int32, no bound storage\n"
    "Alias 29: int32, no bound storage\n"
    "Alias 30: int32, no bound storage\n"
    "Alias 31: int32, no bound storage\n"
    "Alias 32: int32, no bound storage\n"
    "Alias 33: int32, no bound storage\n"
    "Alias 34: int32, no bound storage\n"
    "Alias 35: int32, no bound storage\n"
    "Alias 36: int32, no bound storage\n"
    "Alias 37: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,294] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
