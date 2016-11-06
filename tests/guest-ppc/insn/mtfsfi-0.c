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
    0xFC,0x00,0xF1,0x0C,  // mtfsfi 0,15
};

static const unsigned int guest_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a3, r4\n"
    "    5: GET_ALIAS  r5, a2\n"
    "    6: ANDI       r6, r5, 805306367\n"
    "    7: ORI        r7, r6, -1879048192\n"
    "    8: SRLI       r8, r7, 25\n"
    "    9: SRLI       r9, r7, 3\n"
    "   10: AND        r10, r8, r9\n"
    "   11: ANDI       r11, r10, 31\n"
    "   12: SGTUI      r12, r11, 0\n"
    "   13: SLLI       r13, r12, 30\n"
    "   14: OR         r14, r7, r13\n"
    "   15: SET_ALIAS  a2, r14\n"
    "   16: LOAD_IMM   r15, 4\n"
    "   17: SET_ALIAS  a1, r15\n"
    "   18: GET_ALIAS  r16, a2\n"
    "   19: GET_ALIAS  r17, a3\n"
    "   20: ANDI       r18, r16, -522241\n"
    "   21: SLLI       r19, r17, 12\n"
    "   22: OR         r20, r18, r19\n"
    "   23: SET_ALIAS  a2, r20\n"
    "   24: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 944(r1)\n"
    "Alias 3: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,24] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
