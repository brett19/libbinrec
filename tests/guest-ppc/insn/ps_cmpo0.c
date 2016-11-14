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
    0x13,0x81,0x10,0x40,  // ps_cmpo0 cr7,f1,f2
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
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
    "   15: GET_ALIAS  r11, a3\n"
    "   16: FCMP       r12, r10, r11, OLT\n"
    "   17: FCMP       r13, r10, r11, OGT\n"
    "   18: FCMP       r14, r10, r11, OEQ\n"
    "   19: FCMP       r15, r10, r11, OUN\n"
    "   20: SET_ALIAS  a5, r12\n"
    "   21: SET_ALIAS  a6, r13\n"
    "   22: SET_ALIAS  a7, r14\n"
    "   23: SET_ALIAS  a8, r15\n"
    "   24: GET_ALIAS  r16, a10\n"
    "   25: ANDI       r17, r16, 112\n"
    "   26: SLLI       r18, r12, 3\n"
    "   27: SLLI       r19, r13, 2\n"
    "   28: SLLI       r20, r14, 1\n"
    "   29: OR         r21, r18, r19\n"
    "   30: OR         r22, r20, r15\n"
    "   31: OR         r23, r21, r22\n"
    "   32: OR         r24, r17, r23\n"
    "   33: SET_ALIAS  a10, r24\n"
    "   34: FGETSTATE  r25\n"
    "   35: FTESTEXC   r26, r25, INVALID\n"
    "   36: GOTO_IF_Z  r26, L1\n"
    "   37: FCLEAREXC  r27, r25\n"
    "   38: FSETSTATE  r27\n"
    "   39: GET_ALIAS  r28, a9\n"
    "   40: BITCAST    r29, r10\n"
    "   41: SLLI       r30, r29, 13\n"
    "   42: BFEXT      r31, r29, 51, 12\n"
    "   43: SEQI       r32, r31, 4094\n"
    "   44: GOTO_IF_Z  r32, L3\n"
    "   45: GOTO_IF_NZ r30, L2\n"
    "   46: LABEL      L3\n"
    "   47: BITCAST    r33, r11\n"
    "   48: SLLI       r34, r33, 13\n"
    "   49: BFEXT      r35, r33, 51, 12\n"
    "   50: SEQI       r36, r35, 4094\n"
    "   51: GOTO_IF_Z  r36, L4\n"
    "   52: GOTO_IF_NZ r34, L2\n"
    "   53: LABEL      L4\n"
    "   54: NOT        r37, r28\n"
    "   55: ORI        r38, r28, 524288\n"
    "   56: ANDI       r39, r37, 524288\n"
    "   57: SET_ALIAS  a9, r38\n"
    "   58: GOTO_IF_Z  r39, L5\n"
    "   59: ORI        r40, r38, -2147483648\n"
    "   60: SET_ALIAS  a9, r40\n"
    "   61: LABEL      L5\n"
    "   62: GOTO       L1\n"
    "   63: LABEL      L2\n"
    "   64: ANDI       r41, r28, 128\n"
    "   65: GOTO_IF_NZ r41, L6\n"
    "   66: NOT        r42, r28\n"
    "   67: ORI        r43, r28, 17301504\n"
    "   68: ANDI       r44, r42, 17301504\n"
    "   69: SET_ALIAS  a9, r43\n"
    "   70: GOTO_IF_Z  r44, L7\n"
    "   71: ORI        r45, r43, -2147483648\n"
    "   72: SET_ALIAS  a9, r45\n"
    "   73: LABEL      L7\n"
    "   74: GOTO       L1\n"
    "   75: LABEL      L6\n"
    "   76: NOT        r46, r28\n"
    "   77: ORI        r47, r28, 16777216\n"
    "   78: ANDI       r48, r46, 16777216\n"
    "   79: SET_ALIAS  a9, r47\n"
    "   80: GOTO_IF_Z  r48, L8\n"
    "   81: ORI        r49, r47, -2147483648\n"
    "   82: SET_ALIAS  a9, r49\n"
    "   83: LABEL      L8\n"
    "   84: LABEL      L1\n"
    "   85: LOAD_IMM   r50, 4\n"
    "   86: SET_ALIAS  a1, r50\n"
    "   87: GET_ALIAS  r51, a4\n"
    "   88: ANDI       r52, r51, -16\n"
    "   89: GET_ALIAS  r53, a5\n"
    "   90: SLLI       r54, r53, 3\n"
    "   91: OR         r55, r52, r54\n"
    "   92: GET_ALIAS  r56, a6\n"
    "   93: SLLI       r57, r56, 2\n"
    "   94: OR         r58, r55, r57\n"
    "   95: GET_ALIAS  r59, a7\n"
    "   96: SLLI       r60, r59, 1\n"
    "   97: OR         r61, r58, r60\n"
    "   98: GET_ALIAS  r62, a8\n"
    "   99: OR         r63, r61, r62\n"
    "  100: SET_ALIAS  a4, r63\n"
    "  101: GET_ALIAS  r64, a9\n"
    "  102: GET_ALIAS  r65, a10\n"
    "  103: ANDI       r66, r64, -1611134977\n"
    "  104: SLLI       r67, r65, 12\n"
    "  105: OR         r68, r66, r67\n"
    "  106: SET_ALIAS  a9, r68\n"
    "  107: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32 @ 944(r1)\n"
    "Alias 10: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,36] --> 1,15\n"
    "Block 1: 0 --> [37,44] --> 2,3\n"
    "Block 2: 1 --> [45,45] --> 3,8\n"
    "Block 3: 2,1 --> [46,51] --> 4,5\n"
    "Block 4: 3 --> [52,52] --> 5,8\n"
    "Block 5: 4,3 --> [53,58] --> 6,7\n"
    "Block 6: 5 --> [59,60] --> 7\n"
    "Block 7: 6,5 --> [61,62] --> 15\n"
    "Block 8: 2,4 --> [63,65] --> 9,12\n"
    "Block 9: 8 --> [66,70] --> 10,11\n"
    "Block 10: 9 --> [71,72] --> 11\n"
    "Block 11: 10,9 --> [73,74] --> 15\n"
    "Block 12: 8 --> [75,80] --> 13,14\n"
    "Block 13: 12 --> [81,82] --> 14\n"
    "Block 14: 13,12 --> [83,83] --> 15\n"
    "Block 15: 14,0,7,11 --> [84,107] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
