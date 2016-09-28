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
    0x60,0x00,0x00,0x00,  // nop
    0x42,0x42,0x00,0x08,  // bdz 0xC
    0x48,0x00,0x00,0x00,  // b 0x8
    0x60,0x00,0x00,0x00,  // nop
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0xF\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LABEL      L1\n"
    "    2: GET_ALIAS  r2, a2\n"
    "    3: ADDI       r3, r2, -1\n"
    "    4: SET_ALIAS  a2, r3\n"
    "    5: GOTO_IF_Z  r3, L3\n"
    "    6: SET_ALIAS  a2, r3\n"
    "    7: LOAD_IMM   r4, 8\n"
    "    8: SET_ALIAS  a1, r4\n"
    "    9: LABEL      L2\n"
    "   10: GOTO       L2\n"
    "   11: LOAD_IMM   r5, 12\n"
    "   12: SET_ALIAS  a1, r5\n"
    "   13: LABEL      L3\n"
    "   14: LOAD_IMM   r6, 16\n"
    "   15: SET_ALIAS  a1, r6\n"
    "   16: LABEL      L4\n"
    "   17: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 936(r1)\n"
    "\n"
    "Block 0: <none> --> [0,0] --> 1\n"
    "Block 1: 0 --> [1,5] --> 2,5\n"
    "Block 2: 1 --> [6,8] --> 3\n"
    "Block 3: 2,3 --> [9,10] --> 3\n"
    "Block 4: <none> --> [11,12] --> 5\n"
    "Block 5: 4,1 --> [13,15] --> 6\n"
    "Block 6: 5 --> [16,17] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
