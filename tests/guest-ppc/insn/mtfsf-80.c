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
    0xFD,0x00,0x0D,0x8E,  // mtfsf 128,f1
};

static const unsigned int guest_opt = 0;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: BFEXT      r4, r3, 12, 7\n"
    "    4: SET_ALIAS  a4, r4\n"
    "    5: GET_ALIAS  r5, a2\n"
    "    6: BITCAST    r6, r5\n"
    "    7: ZCAST      r7, r6\n"
    "    8: GET_ALIAS  r8, a3\n"
    "    9: ANDI       r9, r7, -1879048192\n"
    "   10: ANDI       r10, r8, 1879048191\n"
    "   11: OR         r11, r10, r9\n"
    "   12: SET_ALIAS  a3, r11\n"
    "   13: LOAD_IMM   r12, 4\n"
    "   14: SET_ALIAS  a1, r12\n"
    "   15: GET_ALIAS  r13, a3\n"
    "   16: GET_ALIAS  r14, a4\n"
    "   17: ANDI       r15, r13, -1611134977\n"
    "   18: SLLI       r16, r14, 12\n"
    "   19: OR         r17, r15, r16\n"
    "   20: SET_ALIAS  a3, r17\n"
    "   21: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: int32 @ 944(r1)\n"
    "Alias 4: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,21] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
