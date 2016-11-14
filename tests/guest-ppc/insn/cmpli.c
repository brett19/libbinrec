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
    0x2B,0x80,0xAB,0xCD,  // cmpli cr7,r0,43981
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: BFEXT      r4, r3, 3, 1\n"
    "    4: SET_ALIAS  a4, r4\n"
    "    5: BFEXT      r5, r3, 2, 1\n"
    "    6: SET_ALIAS  a5, r5\n"
    "    7: BFEXT      r6, r3, 1, 1\n"
    "    8: SET_ALIAS  a6, r6\n"
    "    9: BFEXT      r7, r3, 0, 1\n"
    "   10: SET_ALIAS  a7, r7\n"
    "   11: GET_ALIAS  r8, a2\n"
    "   12: SLTUI      r9, r8, 43981\n"
    "   13: SGTUI      r10, r8, 43981\n"
    "   14: SEQI       r11, r8, 43981\n"
    "   15: GET_ALIAS  r12, a8\n"
    "   16: BFEXT      r13, r12, 31, 1\n"
    "   17: SET_ALIAS  a4, r9\n"
    "   18: SET_ALIAS  a5, r10\n"
    "   19: SET_ALIAS  a6, r11\n"
    "   20: SET_ALIAS  a7, r13\n"
    "   21: LOAD_IMM   r14, 4\n"
    "   22: SET_ALIAS  a1, r14\n"
    "   23: GET_ALIAS  r15, a3\n"
    "   24: ANDI       r16, r15, -16\n"
    "   25: GET_ALIAS  r17, a4\n"
    "   26: SLLI       r18, r17, 3\n"
    "   27: OR         r19, r16, r18\n"
    "   28: GET_ALIAS  r20, a5\n"
    "   29: SLLI       r21, r20, 2\n"
    "   30: OR         r22, r19, r21\n"
    "   31: GET_ALIAS  r23, a6\n"
    "   32: SLLI       r24, r23, 1\n"
    "   33: OR         r25, r22, r24\n"
    "   34: GET_ALIAS  r26, a7\n"
    "   35: OR         r27, r25, r26\n"
    "   36: SET_ALIAS  a3, r27\n"
    "   37: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 256(r1)\n"
    "Alias 3: int32 @ 928(r1)\n"
    "Alias 4: int32, no bound storage\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,37] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
