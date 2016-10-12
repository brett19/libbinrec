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
    0x7C,0x6D,0x42,0xE6,  // mftbu r3
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: LOAD       r3, 968(r1)\n"
    "    4: GOTO_IF_Z  r3, L3\n"
    "    5: CALL       r4, @r3, r1\n"
    "    6: SRLI       r6, r4, 32\n"
    "    7: ZCAST      r5, r6\n"
    "    8: SET_ALIAS  a2, r5\n"
    "    9: GOTO       L4\n"
    "   10: LABEL      L3\n"
    "   11: LOAD_IMM   r7, 0\n"
    "   12: SET_ALIAS  a2, r7\n"
    "   13: LABEL      L4\n"
    "   14: LOAD_IMM   r8, 4\n"
    "   15: SET_ALIAS  a1, r8\n"
    "   16: LABEL      L2\n"
    "   17: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,4] --> 2,3\n"
    "Block 2: 1 --> [5,9] --> 4\n"
    "Block 3: 1 --> [10,12] --> 4\n"
    "Block 4: 3,2 --> [13,15] --> 5\n"
    "Block 5: 4 --> [16,17] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"