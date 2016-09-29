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
    0x7C,0x64,0x28,0x10,  // subfc r3,r4,r5
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
    "    7: ADDI       r7, r6, 1\n"
    "    8: SRLI       r8, r4, 31\n"
    "    9: SRLI       r9, r5, 31\n"
    "   10: XOR        r10, r8, r9\n"
    "   11: SRLI       r11, r7, 31\n"
    "   12: XORI       r12, r11, 1\n"
    "   13: AND        r13, r8, r9\n"
    "   14: AND        r14, r10, r12\n"
    "   15: OR         r15, r13, r14\n"
    "   16: GET_ALIAS  r16, a5\n"
    "   17: BFINS      r17, r16, r15, 29, 1\n"
    "   18: SET_ALIAS  a2, r7\n"
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