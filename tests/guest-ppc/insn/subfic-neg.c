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
    0x20,0x60,0xAB,0xCD,  // subfic r3,r0,-21555
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: GET_ALIAS  r3, a2\n"
    "    4: LOAD_IMM   r4, -21555\n"
    "    5: SUB        r5, r4, r3\n"
    "    6: GET_ALIAS  r6, a4\n"
    "    7: SGTU       r7, r4, r5\n"
    "    8: BFINS      r8, r6, r7, 29, 1\n"
    "    9: SET_ALIAS  a3, r5\n"
    "   10: SET_ALIAS  a4, r8\n"
    "   11: LOAD_IMM   r9, 4\n"
    "   12: SET_ALIAS  a1, r9\n"
    "   13: LABEL      L2\n"
    "   14: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 256(r1)\n"
    "Alias 3: int32 @ 268(r1)\n"
    "Alias 4: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,12] --> 2\n"
    "Block 2: 1 --> [13,14] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
