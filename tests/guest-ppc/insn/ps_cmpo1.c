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
    0x13,0x81,0x10,0xC0,  // ps_cmpo1 cr7,f1,f2
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a4\n"
    "    3: BFEXT      r4, r3, 3, 1\n"
    "    4: SET_ALIAS  a5, r4\n"
    "    5: BFEXT      r5, r3, 2, 1\n"
    "    6: SET_ALIAS  a6, r5\n"
    "    7: BFEXT      r6, r3, 1, 1\n"
    "    8: SET_ALIAS  a7, r6\n"
    "    9: BFEXT      r7, r3, 0, 1\n"
    "   10: SET_ALIAS  a8, r7\n"
    "   11: GET_ALIAS  r8, a9\n"
    "   12: BFEXT      r9, r8, 12, 7\n"
    "   13: SET_ALIAS  a10, r9\n"
    "   14: GET_ALIAS  r10, a2\n"
    "   15: VEXTRACT   r11, r10, 1\n"
    "   16: FCAST      r12, r11\n"
    "   17: GET_ALIAS  r13, a3\n"
    "   18: VEXTRACT   r14, r13, 1\n"
    "   19: FCAST      r15, r14\n"
    "   20: FCMP       r16, r12, r15, OLT\n"
    "   21: FCMP       r17, r12, r15, OGT\n"
    "   22: FCMP       r18, r12, r15, OEQ\n"
    "   23: FCMP       r19, r12, r15, OUN\n"
    "   24: SET_ALIAS  a5, r16\n"
    "   25: SET_ALIAS  a6, r17\n"
    "   26: SET_ALIAS  a7, r18\n"
    "   27: SET_ALIAS  a8, r19\n"
    "   28: GET_ALIAS  r20, a10\n"
    "   29: ANDI       r21, r20, 112\n"
    "   30: SLLI       r22, r16, 3\n"
    "   31: SLLI       r23, r17, 2\n"
    "   32: SLLI       r24, r18, 1\n"
    "   33: OR         r25, r22, r23\n"
    "   34: OR         r26, r24, r19\n"
    "   35: OR         r27, r25, r26\n"
    "   36: OR         r28, r21, r27\n"
    "   37: SET_ALIAS  a10, r28\n"
    "   38: FGETSTATE  r29\n"
    "   39: FTESTEXC   r30, r29, INVALID\n"
    "   40: GOTO_IF_Z  r30, L1\n"
    "   41: FCLEAREXC  r31, r29\n"
    "   42: FSETSTATE  r31\n"
    "   43: GET_ALIAS  r32, a9\n"
    "   44: BITCAST    r33, r12\n"
    "   45: SLLI       r34, r33, 10\n"
    "   46: BFEXT      r35, r33, 22, 9\n"
    "   47: SEQI       r36, r35, 510\n"
    "   48: GOTO_IF_Z  r36, L3\n"
    "   49: GOTO_IF_NZ r34, L2\n"
    "   50: LABEL      L3\n"
    "   51: BITCAST    r37, r15\n"
    "   52: SLLI       r38, r37, 10\n"
    "   53: BFEXT      r39, r37, 22, 9\n"
    "   54: SEQI       r40, r39, 510\n"
    "   55: GOTO_IF_Z  r40, L4\n"
    "   56: GOTO_IF_NZ r38, L2\n"
    "   57: LABEL      L4\n"
    "   58: NOT        r41, r32\n"
    "   59: ORI        r42, r32, 524288\n"
    "   60: ANDI       r43, r41, 524288\n"
    "   61: SET_ALIAS  a9, r42\n"
    "   62: GOTO_IF_Z  r43, L5\n"
    "   63: ORI        r44, r42, -2147483648\n"
    "   64: SET_ALIAS  a9, r44\n"
    "   65: LABEL      L5\n"
    "   66: GOTO       L1\n"
    "   67: LABEL      L2\n"
    "   68: ANDI       r45, r32, 128\n"
    "   69: GOTO_IF_NZ r45, L6\n"
    "   70: NOT        r46, r32\n"
    "   71: ORI        r47, r32, 17301504\n"
    "   72: ANDI       r48, r46, 17301504\n"
    "   73: SET_ALIAS  a9, r47\n"
    "   74: GOTO_IF_Z  r48, L7\n"
    "   75: ORI        r49, r47, -2147483648\n"
    "   76: SET_ALIAS  a9, r49\n"
    "   77: LABEL      L7\n"
    "   78: GOTO       L1\n"
    "   79: LABEL      L6\n"
    "   80: NOT        r50, r32\n"
    "   81: ORI        r51, r32, 16777216\n"
    "   82: ANDI       r52, r50, 16777216\n"
    "   83: SET_ALIAS  a9, r51\n"
    "   84: GOTO_IF_Z  r52, L8\n"
    "   85: ORI        r53, r51, -2147483648\n"
    "   86: SET_ALIAS  a9, r53\n"
    "   87: LABEL      L8\n"
    "   88: LABEL      L1\n"
    "   89: LOAD_IMM   r54, 4\n"
    "   90: SET_ALIAS  a1, r54\n"
    "   91: GET_ALIAS  r55, a4\n"
    "   92: ANDI       r56, r55, -16\n"
    "   93: GET_ALIAS  r57, a5\n"
    "   94: SLLI       r58, r57, 3\n"
    "   95: OR         r59, r56, r58\n"
    "   96: GET_ALIAS  r60, a6\n"
    "   97: SLLI       r61, r60, 2\n"
    "   98: OR         r62, r59, r61\n"
    "   99: GET_ALIAS  r63, a7\n"
    "  100: SLLI       r64, r63, 1\n"
    "  101: OR         r65, r62, r64\n"
    "  102: GET_ALIAS  r66, a8\n"
    "  103: OR         r67, r65, r66\n"
    "  104: SET_ALIAS  a4, r67\n"
    "  105: GET_ALIAS  r68, a9\n"
    "  106: GET_ALIAS  r69, a10\n"
    "  107: ANDI       r70, r68, -1611134977\n"
    "  108: SLLI       r71, r69, 12\n"
    "  109: OR         r72, r70, r71\n"
    "  110: SET_ALIAS  a9, r72\n"
    "  111: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32 @ 944(r1)\n"
    "Alias 10: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,40] --> 1,15\n"
    "Block 1: 0 --> [41,48] --> 2,3\n"
    "Block 2: 1 --> [49,49] --> 3,8\n"
    "Block 3: 2,1 --> [50,55] --> 4,5\n"
    "Block 4: 3 --> [56,56] --> 5,8\n"
    "Block 5: 4,3 --> [57,62] --> 6,7\n"
    "Block 6: 5 --> [63,64] --> 7\n"
    "Block 7: 6,5 --> [65,66] --> 15\n"
    "Block 8: 2,4 --> [67,69] --> 9,12\n"
    "Block 9: 8 --> [70,74] --> 10,11\n"
    "Block 10: 9 --> [75,76] --> 11\n"
    "Block 11: 10,9 --> [77,78] --> 15\n"
    "Block 12: 8 --> [79,84] --> 13,14\n"
    "Block 13: 12 --> [85,86] --> 14\n"
    "Block 14: 13,12 --> [87,87] --> 15\n"
    "Block 15: 14,0,7,11 --> [88,111] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"