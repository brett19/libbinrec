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
    0x0D,0x43,0x12,0x34,  // twi 10,r3,4660
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
    "    6: SLTU       r6, r4, r3\n"
    "    7: OR         r7, r5, r6\n"
    "    8: GOTO_IF_Z  r7, L3\n"
    "    9: LOAD_IMM   r8, 0\n"
    "   10: SET_ALIAS  a1, r8\n"
    "   11: LOAD       r9, 968(r1)\n"
    "   12: CALL_ADDR  @r9, r1\n"
    "   13: RETURN\n"
    "   14: LABEL      L3\n"
    "   15: LOAD_IMM   r10, 4\n"
    "   16: SET_ALIAS  a1, r10\n"
    "   17: LABEL      L2\n"
    "   18: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,8] --> 2,3\n"
    "Block 2: 1 --> [9,13] --> <none>\n"
    "Block 3: 1 --> [14,16] --> 4\n"
    "Block 4: 3 --> [17,18] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
