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
    0x7C,0x80,0x00,0x26,  // mfcr r4
    0x54,0x83,0x17,0xC0,  // rlwinm r3,r4,2,31,0
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x7\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a4\n"
    "    3: SET_ALIAS  a3, r3\n"
    "    4: RORI       r4, r3, 30\n"
    "    5: ANDI       r5, r4, -2147483647\n"
    "    6: SET_ALIAS  a2, r5\n"
    "    7: LOAD_IMM   r6, 8\n"
    "    8: SET_ALIAS  a1, r6\n"
    "    9: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "\n"
    "Block 0: <none> --> [0,9] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
