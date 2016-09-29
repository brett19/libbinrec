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
    0x7C,0x64,0x29,0x14,  // adde r3,r4,r5
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: GET_ALIAS  r3, a3\n"
    "    4: GET_ALIAS  r4, a4\n"
    "    5: ADD        r5, r3, r4\n"
    "    6: GET_ALIAS  r6, a5\n"
    "    7: BFEXT      r7, r6, 29, 1\n"
    "    8: ADD        r8, r5, r7\n"
    "    9: SRLI       r9, r3, 31\n"
    "   10: SRLI       r10, r4, 31\n"
    "   11: XOR        r11, r9, r10\n"
    "   12: SRLI       r12, r8, 31\n"
    "   13: XORI       r13, r12, 1\n"
    "   14: AND        r14, r9, r10\n"
    "   15: AND        r15, r11, r13\n"
    "   16: OR         r16, r14, r15\n"
    "   17: BFINS      r17, r6, r16, 29, 1\n"
    "   18: SET_ALIAS  a2, r8\n"
    "   19: SET_ALIAS  a5, r17\n"
    "   20: LOAD_IMM   r18, 4\n"
    "   21: SET_ALIAS  a1, r18\n"
    "   22: LABEL      L2\n"
    "   23: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 276(r1)\n"
    "Alias 5: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,21] --> 2\n"
    "Block 2: 1 --> [22,23] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"