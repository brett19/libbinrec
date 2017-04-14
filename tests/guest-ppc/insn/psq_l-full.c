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
    0xE0,0x23,0x2F,0xF0,  // psq_l f1,-16(r3),0,2
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: ZCAST      r4, r3\n"
    "    4: ADD        r5, r2, r4\n"
    "    5: LOAD       r6, 904(r1)\n"
    "    6: BFEXT      r7, r6, 16, 3\n"
    "    7: ANDI       r8, r7, 4\n"
    "    8: GOTO_IF_NZ r8, L2\n"
    "    9: LOAD_BR    r9, -16(r5)\n"
    "   10: LOAD_BR    r10, -12(r5)\n"
    "   11: VBUILD2    r11, r9, r10\n"
    "   12: FGETSTATE  r12\n"
    "   13: VFCMP      r13, r11, r11, UN\n"
    "   14: VFCVT      r14, r11\n"
    "   15: SET_ALIAS  a4, r14\n"
    "   16: GOTO_IF_Z  r13, L3\n"
    "   17: VEXTRACT   r15, r11, 0\n"
    "   18: VEXTRACT   r16, r11, 1\n"
    "   19: SLLI       r17, r13, 32\n"
    "   20: BITCAST    r18, r15\n"
    "   21: BITCAST    r19, r16\n"
    "   22: NOT        r20, r18\n"
    "   23: NOT        r21, r19\n"
    "   24: ANDI       r22, r20, 4194304\n"
    "   25: ANDI       r23, r21, 4194304\n"
    "   26: VEXTRACT   r24, r14, 0\n"
    "   27: VEXTRACT   r25, r14, 1\n"
    "   28: ZCAST      r26, r22\n"
    "   29: ZCAST      r27, r23\n"
    "   30: SLLI       r28, r26, 29\n"
    "   31: SLLI       r29, r27, 29\n"
    "   32: BITCAST    r30, r24\n"
    "   33: BITCAST    r31, r25\n"
    "   34: AND        r32, r28, r17\n"
    "   35: AND        r33, r29, r13\n"
    "   36: XOR        r34, r30, r32\n"
    "   37: XOR        r35, r31, r33\n"
    "   38: BITCAST    r36, r34\n"
    "   39: BITCAST    r37, r35\n"
    "   40: VBUILD2    r38, r36, r37\n"
    "   41: SET_ALIAS  a4, r38\n"
    "   42: LABEL      L3\n"
    "   43: GET_ALIAS  r39, a4\n"
    "   44: FSETSTATE  r12\n"
    "   45: SET_ALIAS  a3, r39\n"
    "   46: GOTO       L1\n"
    "   47: LABEL      L2\n"
    "   48: SLLI       r40, r6, 2\n"
    "   49: SRAI       r41, r40, 26\n"
    "   50: SLLI       r42, r41, 23\n"
    "   51: LOAD_IMM   r43, 0x3F800000\n"
    "   52: SUB        r44, r43, r42\n"
    "   53: BITCAST    r45, r44\n"
    "   54: ANDI       r46, r7, 1\n"
    "   55: GOTO_IF_NZ r46, L4\n"
    "   56: ANDI       r47, r7, 2\n"
    "   57: GOTO_IF_NZ r47, L5\n"
    "   58: LOAD_U8    r48, -16(r5)\n"
    "   59: LOAD_U8    r49, -15(r5)\n"
    "   60: FSCAST     r50, r48\n"
    "   61: FMUL       r51, r50, r45\n"
    "   62: FSCAST     r52, r49\n"
    "   63: FMUL       r53, r52, r45\n"
    "   64: VBUILD2    r54, r51, r53\n"
    "   65: VFCVT      r55, r54\n"
    "   66: SET_ALIAS  a3, r55\n"
    "   67: GOTO       L1\n"
    "   68: LABEL      L5\n"
    "   69: LOAD_S8    r56, -16(r5)\n"
    "   70: LOAD_S8    r57, -15(r5)\n"
    "   71: FSCAST     r58, r56\n"
    "   72: FMUL       r59, r58, r45\n"
    "   73: FSCAST     r60, r57\n"
    "   74: FMUL       r61, r60, r45\n"
    "   75: VBUILD2    r62, r59, r61\n"
    "   76: VFCVT      r63, r62\n"
    "   77: SET_ALIAS  a3, r63\n"
    "   78: GOTO       L1\n"
    "   79: LABEL      L4\n"
    "   80: ANDI       r64, r7, 2\n"
    "   81: GOTO_IF_NZ r64, L6\n"
    "   82: LOAD_U16_BR r65, -16(r5)\n"
    "   83: LOAD_U16_BR r66, -14(r5)\n"
    "   84: FSCAST     r67, r65\n"
    "   85: FMUL       r68, r67, r45\n"
    "   86: FSCAST     r69, r66\n"
    "   87: FMUL       r70, r69, r45\n"
    "   88: VBUILD2    r71, r68, r70\n"
    "   89: VFCVT      r72, r71\n"
    "   90: SET_ALIAS  a3, r72\n"
    "   91: GOTO       L1\n"
    "   92: LABEL      L6\n"
    "   93: LOAD_S16_BR r73, -16(r5)\n"
    "   94: LOAD_S16_BR r74, -14(r5)\n"
    "   95: FSCAST     r75, r73\n"
    "   96: FMUL       r76, r75, r45\n"
    "   97: FSCAST     r77, r74\n"
    "   98: FMUL       r78, r77, r45\n"
    "   99: VBUILD2    r79, r76, r78\n"
    "  100: VFCVT      r80, r79\n"
    "  101: SET_ALIAS  a3, r80\n"
    "  102: LABEL      L1\n"
    "  103: LOAD_IMM   r81, 4\n"
    "  104: SET_ALIAS  a1, r81\n"
    "  105: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64[2] @ 400(r1)\n"
    "Alias 4: float64[2], no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,8] --> 1,4\n"
    "Block 1: 0 --> [9,16] --> 2,3\n"
    "Block 2: 1 --> [17,41] --> 3\n"
    "Block 3: 2,1 --> [42,46] --> 11\n"
    "Block 4: 0 --> [47,55] --> 5,8\n"
    "Block 5: 4 --> [56,57] --> 6,7\n"
    "Block 6: 5 --> [58,67] --> 11\n"
    "Block 7: 5 --> [68,78] --> 11\n"
    "Block 8: 4 --> [79,81] --> 9,10\n"
    "Block 9: 8 --> [82,91] --> 11\n"
    "Block 10: 8 --> [92,101] --> 11\n"
    "Block 11: 10,3,6,7,9 --> [102,105] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
