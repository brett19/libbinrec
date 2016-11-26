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
    "    2: GET_ALIAS  r3, a2\n"
    "    3: VEXTRACT   r4, r3, 1\n"
    "    4: FCAST      r5, r4\n"
    "    5: GET_ALIAS  r6, a3\n"
    "    6: VEXTRACT   r7, r6, 1\n"
    "    7: FCAST      r8, r7\n"
    "    8: FCMP       r9, r5, r8, OLT\n"
    "    9: FCMP       r10, r5, r8, OGT\n"
    "   10: FCMP       r11, r5, r8, OEQ\n"
    "   11: FCMP       r12, r5, r8, OUN\n"
    "   12: GET_ALIAS  r13, a4\n"
    "   13: SLLI       r14, r9, 3\n"
    "   14: SLLI       r15, r10, 2\n"
    "   15: SLLI       r16, r11, 1\n"
    "   16: OR         r17, r14, r15\n"
    "   17: OR         r18, r16, r12\n"
    "   18: OR         r19, r17, r18\n"
    "   19: BFINS      r20, r13, r19, 0, 4\n"
    "   20: SET_ALIAS  a4, r20\n"
    "   21: GET_ALIAS  r21, a5\n"
    "   22: BFEXT      r22, r21, 12, 7\n"
    "   23: ANDI       r23, r22, 112\n"
    "   24: SLLI       r24, r9, 3\n"
    "   25: SLLI       r25, r10, 2\n"
    "   26: SLLI       r26, r11, 1\n"
    "   27: OR         r27, r24, r25\n"
    "   28: OR         r28, r26, r12\n"
    "   29: OR         r29, r27, r28\n"
    "   30: OR         r30, r23, r29\n"
    "   31: GET_ALIAS  r31, a5\n"
    "   32: BFINS      r32, r31, r30, 12, 7\n"
    "   33: SET_ALIAS  a5, r32\n"
    "   34: FGETSTATE  r33\n"
    "   35: FTESTEXC   r34, r33, INVALID\n"
    "   36: GOTO_IF_Z  r34, L1\n"
    "   37: FCLEAREXC  r35, r33\n"
    "   38: FSETSTATE  r35\n"
    "   39: BITCAST    r36, r5\n"
    "   40: SLLI       r37, r36, 10\n"
    "   41: BFEXT      r38, r36, 22, 9\n"
    "   42: SEQI       r39, r38, 510\n"
    "   43: GOTO_IF_Z  r39, L3\n"
    "   44: GOTO_IF_NZ r37, L2\n"
    "   45: LABEL      L3\n"
    "   46: BITCAST    r40, r8\n"
    "   47: SLLI       r41, r40, 10\n"
    "   48: BFEXT      r42, r40, 22, 9\n"
    "   49: SEQI       r43, r42, 510\n"
    "   50: GOTO_IF_Z  r43, L4\n"
    "   51: GOTO_IF_NZ r41, L2\n"
    "   52: LABEL      L4\n"
    "   53: NOT        r44, r32\n"
    "   54: ORI        r45, r32, 524288\n"
    "   55: ANDI       r46, r44, 524288\n"
    "   56: SET_ALIAS  a5, r45\n"
    "   57: GOTO_IF_Z  r46, L5\n"
    "   58: ORI        r47, r45, -2147483648\n"
    "   59: SET_ALIAS  a5, r47\n"
    "   60: LABEL      L5\n"
    "   61: GOTO       L1\n"
    "   62: LABEL      L2\n"
    "   63: ANDI       r48, r32, 128\n"
    "   64: GOTO_IF_NZ r48, L6\n"
    "   65: NOT        r49, r32\n"
    "   66: ORI        r50, r32, 17301504\n"
    "   67: ANDI       r51, r49, 17301504\n"
    "   68: SET_ALIAS  a5, r50\n"
    "   69: GOTO_IF_Z  r51, L7\n"
    "   70: ORI        r52, r50, -2147483648\n"
    "   71: SET_ALIAS  a5, r52\n"
    "   72: LABEL      L7\n"
    "   73: GOTO       L1\n"
    "   74: LABEL      L6\n"
    "   75: NOT        r53, r32\n"
    "   76: ORI        r54, r32, 16777216\n"
    "   77: ANDI       r55, r53, 16777216\n"
    "   78: SET_ALIAS  a5, r54\n"
    "   79: GOTO_IF_Z  r55, L8\n"
    "   80: ORI        r56, r54, -2147483648\n"
    "   81: SET_ALIAS  a5, r56\n"
    "   82: LABEL      L8\n"
    "   83: LABEL      L1\n"
    "   84: LOAD_IMM   r57, 4\n"
    "   85: SET_ALIAS  a1, r57\n"
    "   86: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,36] --> 1,15\n"
    "Block 1: 0 --> [37,43] --> 2,3\n"
    "Block 2: 1 --> [44,44] --> 3,8\n"
    "Block 3: 2,1 --> [45,50] --> 4,5\n"
    "Block 4: 3 --> [51,51] --> 5,8\n"
    "Block 5: 4,3 --> [52,57] --> 6,7\n"
    "Block 6: 5 --> [58,59] --> 7\n"
    "Block 7: 6,5 --> [60,61] --> 15\n"
    "Block 8: 2,4 --> [62,64] --> 9,12\n"
    "Block 9: 8 --> [65,69] --> 10,11\n"
    "Block 10: 9 --> [70,71] --> 11\n"
    "Block 11: 10,9 --> [72,73] --> 15\n"
    "Block 12: 8 --> [74,79] --> 13,14\n"
    "Block 13: 12 --> [80,81] --> 14\n"
    "Block 14: 13,12 --> [82,82] --> 15\n"
    "Block 15: 14,0,7,11 --> [83,86] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
