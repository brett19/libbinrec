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
    0xFF,0x80,0x01,0x0C,  // mtfsfi 7,0
};

static const unsigned int guest_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a3, r4\n"
    "    5: GET_ALIAS  r5, a2\n"
    "    6: ANDI       r6, r5, -16\n"
    "    7: SET_ALIAS  a2, r6\n"
    "    8: FSETROUND  NEAREST\n"
    "    9: LOAD_IMM   r7, 4\n"
    "   10: SET_ALIAS  a1, r7\n"
    "   11: GET_ALIAS  r8, a2\n"
    "   12: GET_ALIAS  r9, a3\n"
    "   13: ANDI       r10, r8, -1611134977\n"
    "   14: SLLI       r11, r9, 12\n"
    "   15: OR         r12, r10, r11\n"
    "   16: SET_ALIAS  a2, r12\n"
    "   17: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 944(r1)\n"
    "Alias 3: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,17] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
