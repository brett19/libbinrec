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
    0xFF,0x81,0x10,0x40,  // fcmpo cr7,f1,f2
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: GET_ALIAS  r4, a3\n"
    "    4: FCMP       r5, r3, r4, OLT\n"
    "    5: FCMP       r6, r3, r4, OGT\n"
    "    6: FCMP       r7, r3, r4, OEQ\n"
    "    7: FCMP       r8, r3, r4, OUN\n"
    "    8: GET_ALIAS  r9, a4\n"
    "    9: SLLI       r10, r5, 3\n"
    "   10: SLLI       r11, r6, 2\n"
    "   11: SLLI       r12, r7, 1\n"
    "   12: OR         r13, r10, r11\n"
    "   13: OR         r14, r12, r8\n"
    "   14: OR         r15, r13, r14\n"
    "   15: BFINS      r16, r9, r15, 0, 4\n"
    "   16: SET_ALIAS  a4, r16\n"
    "   17: GET_ALIAS  r17, a5\n"
    "   18: BFEXT      r18, r17, 12, 7\n"
    "   19: ANDI       r19, r18, 112\n"
    "   20: SLLI       r20, r5, 3\n"
    "   21: SLLI       r21, r6, 2\n"
    "   22: SLLI       r22, r7, 1\n"
    "   23: OR         r23, r20, r21\n"
    "   24: OR         r24, r22, r8\n"
    "   25: OR         r25, r23, r24\n"
    "   26: OR         r26, r19, r25\n"
    "   27: GET_ALIAS  r27, a5\n"
    "   28: BFINS      r28, r27, r26, 12, 7\n"
    "   29: SET_ALIAS  a5, r28\n"
    "   30: FGETSTATE  r29\n"
    "   31: FTESTEXC   r30, r29, INVALID\n"
    "   32: GOTO_IF_Z  r30, L1\n"
    "   33: FCLEAREXC  r31, r29\n"
    "   34: FSETSTATE  r31\n"
    "   35: BITCAST    r32, r3\n"
    "   36: SLLI       r33, r32, 13\n"
    "   37: BFEXT      r34, r32, 51, 12\n"
    "   38: SEQI       r35, r34, 4094\n"
    "   39: GOTO_IF_Z  r35, L3\n"
    "   40: GOTO_IF_NZ r33, L2\n"
    "   41: LABEL      L3\n"
    "   42: BITCAST    r36, r4\n"
    "   43: SLLI       r37, r36, 13\n"
    "   44: BFEXT      r38, r36, 51, 12\n"
    "   45: SEQI       r39, r38, 4094\n"
    "   46: GOTO_IF_Z  r39, L4\n"
    "   47: GOTO_IF_NZ r37, L2\n"
    "   48: LABEL      L4\n"
    "   49: NOT        r40, r28\n"
    "   50: ORI        r41, r28, 524288\n"
    "   51: ANDI       r42, r40, 524288\n"
    "   52: SET_ALIAS  a5, r41\n"
    "   53: GOTO_IF_Z  r42, L5\n"
    "   54: ORI        r43, r41, -2147483648\n"
    "   55: SET_ALIAS  a5, r43\n"
    "   56: LABEL      L5\n"
    "   57: GOTO       L1\n"
    "   58: LABEL      L2\n"
    "   59: ANDI       r44, r28, 128\n"
    "   60: GOTO_IF_NZ r44, L6\n"
    "   61: NOT        r45, r28\n"
    "   62: ORI        r46, r28, 17301504\n"
    "   63: ANDI       r47, r45, 17301504\n"
    "   64: SET_ALIAS  a5, r46\n"
    "   65: GOTO_IF_Z  r47, L7\n"
    "   66: ORI        r48, r46, -2147483648\n"
    "   67: SET_ALIAS  a5, r48\n"
    "   68: LABEL      L7\n"
    "   69: GOTO       L1\n"
    "   70: LABEL      L6\n"
    "   71: NOT        r49, r28\n"
    "   72: ORI        r50, r28, 16777216\n"
    "   73: ANDI       r51, r49, 16777216\n"
    "   74: SET_ALIAS  a5, r50\n"
    "   75: GOTO_IF_Z  r51, L8\n"
    "   76: ORI        r52, r50, -2147483648\n"
    "   77: SET_ALIAS  a5, r52\n"
    "   78: LABEL      L8\n"
    "   79: LABEL      L1\n"
    "   80: LOAD_IMM   r53, 4\n"
    "   81: SET_ALIAS  a1, r53\n"
    "   82: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "Alias 5: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,32] --> 1,15\n"
    "Block 1: 0 --> [33,39] --> 2,3\n"
    "Block 2: 1 --> [40,40] --> 3,8\n"
    "Block 3: 2,1 --> [41,46] --> 4,5\n"
    "Block 4: 3 --> [47,47] --> 5,8\n"
    "Block 5: 4,3 --> [48,53] --> 6,7\n"
    "Block 6: 5 --> [54,55] --> 7\n"
    "Block 7: 6,5 --> [56,57] --> 15\n"
    "Block 8: 2,4 --> [58,60] --> 9,12\n"
    "Block 9: 8 --> [61,65] --> 10,11\n"
    "Block 10: 9 --> [66,67] --> 11\n"
    "Block 11: 10,9 --> [68,69] --> 15\n"
    "Block 12: 8 --> [70,75] --> 13,14\n"
    "Block 13: 12 --> [76,77] --> 14\n"
    "Block 14: 13,12 --> [78,78] --> 15\n"
    "Block 15: 14,0,7,11 --> [79,82] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
