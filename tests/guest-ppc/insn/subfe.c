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
    0x7C,0x64,0x29,0x10,  // subfe r3,r4,r5
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: GET_ALIAS  r3, a3\n"
    "    4: NOT        r4, r3\n"
    "    5: GET_ALIAS  r5, a4\n"
    "    6: ADD        r6, r4, r5\n"
    "    7: GET_ALIAS  r7, a5\n"
    "    8: BFEXT      r8, r7, 29, 1\n"
    "    9: ADD        r9, r6, r8\n"
    "   10: SRLI       r10, r4, 31\n"
    "   11: SRLI       r11, r5, 31\n"
    "   12: XOR        r12, r10, r11\n"
    "   13: SRLI       r13, r9, 31\n"
    "   14: XORI       r14, r13, 1\n"
    "   15: AND        r15, r10, r11\n"
    "   16: AND        r16, r12, r14\n"
    "   17: OR         r17, r15, r16\n"
    "   18: BFINS      r18, r7, r17, 29, 1\n"
    "   19: SET_ALIAS  a2, r9\n"
    "   20: SET_ALIAS  a5, r18\n"
    "   21: LOAD_IMM   r19, 4\n"
    "   22: SET_ALIAS  a1, r19\n"
    "   23: LABEL      L2\n"
    "   24: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 276(r1)\n"
    "Alias 5: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,22] --> 2\n"
    "Block 2: 1 --> [23,24] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
