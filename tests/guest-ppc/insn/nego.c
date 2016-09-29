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
    0x7C,0x64,0x04,0xD0,  // nego r3,r4
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: GET_ALIAS  r3, a3\n"
    "    4: NOT        r4, r3\n"
    "    5: ADDI       r5, r4, 1\n"
    "    6: SRLI       r6, r4, 31\n"
    "    7: SRLI       r7, r5, 31\n"
    "    8: XORI       r8, r6, 1\n"
    "    9: AND        r9, r8, r7\n"
    "   10: GET_ALIAS  r10, a4\n"
    "   11: ANDI       r11, r10, -1073741825\n"
    "   12: LOAD_IMM   r12, 0xC0000000\n"
    "   13: SELECT     r13, r12, r9, r9\n"
    "   14: OR         r14, r11, r13\n"
    "   15: SET_ALIAS  a2, r5\n"
    "   16: SET_ALIAS  a4, r14\n"
    "   17: LOAD_IMM   r15, 4\n"
    "   18: SET_ALIAS  a1, r15\n"
    "   19: LABEL      L2\n"
    "   20: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,18] --> 2\n"
    "Block 2: 1 --> [19,20] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"