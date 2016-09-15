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
    0x30, 0x60, 0x12, 0x34,  // addic r3,r0,4660
};

static const bool expected_success = true;

static const char expected[] =
    "[warning] Scanning terminated at 0x4 due to code range limit\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD       r2, 256(r1)\n"
    "    2: SET_ALIAS  a2, r2\n"
    "    3: LOAD       r3, 940(r1)\n"
    "    4: SET_ALIAS  a4, r3\n"
    "    5: LABEL      L1\n"
    "    6: GET_ALIAS  r4, a2\n"
    "    7: ADDI       r5, r4, 4660\n"
    "    8: SET_ALIAS  a3, r5\n"
    "    9: GET_ALIAS  r6, a4\n"
    "   10: SLTUI      r7, r5, 4660\n"
    "   11: BFINS      r8, r6, r7, 29, 1\n"
    "   12: SET_ALIAS  a4, r8\n"
    "   13: LOAD_IMM   r9, 4\n"
    "   14: SET_ALIAS  a1, r9\n"
    "   15: LABEL      L2\n"
    "   16: GET_ALIAS  r10, a3\n"
    "   17: STORE      268(r1), r10\n"
    "   18: GET_ALIAS  r11, a4\n"
    "   19: STORE      940(r1), r11\n"
    "   20: GET_ALIAS  r12, a1\n"
    "   21: STORE      956(r1), r12\n"
    "   22: RETURN     r1\n"
    "\n"
    "Block    0: <none> --> [0,4] --> 1\n"
    "Block    1: 0 --> [5,14] --> 2\n"
    "Block    2: 1 --> [15,22] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
