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
    0x0E,0xA3,0x12,0x34,  // twi 21,r3,4660
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: LOAD_IMM   r3, 4660\n"
    "    4: GET_ALIAS  r4, a2\n"
    "    5: SGTS       r5, r4, r3\n"
    "    6: XORI       r6, r5, 1\n"
    "    7: SGTU       r7, r4, r3\n"
    "    8: OR         r8, r6, r7\n"
    "    9: GOTO_IF_Z  r8, L3\n"
    "   10: LOAD_IMM   r9, 0\n"
    "   11: SET_ALIAS  a1, r9\n"
    "   12: LOAD       r10, 968(r1)\n"
    "   13: CALL_ADDR  @r10, r1\n"
    "   14: RETURN\n"
    "   15: LABEL      L3\n"
    "   16: LOAD_IMM   r11, 4\n"
    "   17: SET_ALIAS  a1, r11\n"
    "   18: LABEL      L2\n"
    "   19: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,9] --> 2,3\n"
    "Block 2: 1 --> [10,14] --> <none>\n"
    "Block 3: 1 --> [15,17] --> 4\n"
    "Block 4: 3 --> [18,19] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"