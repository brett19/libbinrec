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

static const unsigned int guest_opt = BINREC_OPT_G_PPC_IGNORE_FPSCR_VXFOO;

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
    "   37: FCLEAREXC\n"
    "   38: GET_ALIAS  r27, a9\n"
    "   39: NOT        r28, r27\n"
    "   40: ORI        r29, r27, 16777216\n"
    "   41: ANDI       r30, r28, 16777216\n"
    "   42: SET_ALIAS  a9, r29\n"
    "   43: GOTO_IF_Z  r30, L2\n"
    "   44: ORI        r31, r29, -2147483648\n"
    "   45: SET_ALIAS  a9, r31\n"
    "   46: LABEL      L2\n"
    "   47: LABEL      L1\n"
    "   48: LOAD_IMM   r32, 4\n"
    "   49: SET_ALIAS  a1, r32\n"
    "   50: GET_ALIAS  r33, a4\n"
    "   51: ANDI       r34, r33, -16\n"
    "   52: GET_ALIAS  r35, a5\n"
    "   53: SLLI       r36, r35, 3\n"
    "   54: OR         r37, r34, r36\n"
    "   55: GET_ALIAS  r38, a6\n"
    "   56: SLLI       r39, r38, 2\n"
    "   57: OR         r40, r37, r39\n"
    "   58: GET_ALIAS  r41, a7\n"
    "   59: SLLI       r42, r41, 1\n"
    "   60: OR         r43, r40, r42\n"
    "   61: GET_ALIAS  r44, a8\n"
    "   62: OR         r45, r43, r44\n"
    "   63: SET_ALIAS  a4, r45\n"
    "   64: GET_ALIAS  r46, a9\n"
    "   65: GET_ALIAS  r47, a10\n"
    "   66: ANDI       r48, r46, -1611134977\n"
    "   67: SLLI       r49, r47, 12\n"
    "   68: OR         r50, r48, r49\n"
    "   69: SET_ALIAS  a9, r50\n"
    "   70: RETURN\n"
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
    "Block 0: <none> --> [0,36] --> 1,4\n"
    "Block 1: 0 --> [37,43] --> 2,3\n"
    "Block 2: 1 --> [44,45] --> 3\n"
    "Block 3: 2,1 --> [46,46] --> 4\n"
    "Block 4: 3,0 --> [47,70] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
