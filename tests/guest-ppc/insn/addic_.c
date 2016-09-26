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
    0x34,0x60,0x12,0x34,  // addic. r3,r0,4660
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LABEL      L1\n"
    "    2: GET_ALIAS  r2, a2\n"
    "    3: ADDI       r3, r2, 4660\n"
    "    4: GET_ALIAS  r4, a5\n"
    "    5: SLTUI      r5, r3, 4660\n"
    "    6: BFINS      r6, r4, r5, 29, 1\n"
    "    7: SLTSI      r7, r3, 0\n"
    "    8: SGTSI      r8, r3, 0\n"
    "    9: SEQI       r9, r3, 0\n"
    "   10: BFEXT      r10, r6, 31, 1\n"
    "   11: BFINS      r11, r10, r9, 1, 1\n"
    "   12: BFINS      r12, r11, r8, 2, 1\n"
    "   13: BFINS      r13, r12, r7, 3, 1\n"
    "   14: SET_ALIAS  a3, r3\n"
    "   15: SET_ALIAS  a4, r13\n"
    "   16: SET_ALIAS  a5, r6\n"
    "   17: LOAD_IMM   r14, 4\n"
    "   18: SET_ALIAS  a1, r14\n"
    "   19: LABEL      L2\n"
    "   20: LOAD       r15, 928(r1)\n"
    "   21: ANDI       r16, r15, 268435455\n"
    "   22: GET_ALIAS  r17, a4\n"
    "   23: SLLI       r18, r17, 28\n"
    "   24: OR         r19, r16, r18\n"
    "   25: STORE      928(r1), r19\n"
    "   26: RETURN     r1\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 256(r1)\n"
    "Alias 3: int32 @ 268(r1)\n"
    "Alias 4: int32, no bound storage\n"
    "Alias 5: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,0] --> 1\n"
    "Block 1: 0 --> [1,18] --> 2\n"
    "Block 2: 1 --> [19,26] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
