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
    0xFE,0x80,0x01,0x0C,  // mtfsfi 5,0
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
    "    6: ANDI       r6, r5, -1610616577\n"
    "    7: ANDI       r7, r6, 33031936\n"
    "    8: SGTUI      r8, r7, 0\n"
    "    9: SLLI       r9, r8, 29\n"
    "   10: OR         r10, r6, r9\n"
    "   11: SRLI       r11, r10, 25\n"
    "   12: SRLI       r12, r10, 3\n"
    "   13: AND        r13, r11, r12\n"
    "   14: ANDI       r14, r13, 31\n"
    "   15: SGTUI      r15, r14, 0\n"
    "   16: SLLI       r16, r15, 30\n"
    "   17: OR         r17, r10, r16\n"
    "   18: SET_ALIAS  a2, r17\n"
    "   19: LOAD_IMM   r18, 4\n"
    "   20: SET_ALIAS  a1, r18\n"
    "   21: GET_ALIAS  r19, a2\n"
    "   22: GET_ALIAS  r20, a3\n"
    "   23: ANDI       r21, r19, -522241\n"
    "   24: SLLI       r22, r20, 12\n"
    "   25: OR         r23, r21, r22\n"
    "   26: SET_ALIAS  a2, r23\n"
    "   27: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 944(r1)\n"
    "Alias 3: int32, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,27] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
