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
    0x40,0x42,0x00,0x08,  // bc 2,2,0xC
    0x48,0x00,0x00,0x00,  // b 0x8
    0x60,0x00,0x00,0x00,  // nop
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0xF\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LOAD       r3, 928(r1)\n"
    "    3: BFEXT      r4, r3, 28, 4\n"
    "    4: SET_ALIAS  a2, r4\n"
    "    5: LABEL      L1\n"
    "    6: GET_ALIAS  r5, a3\n"
    "    7: ADDI       r6, r5, -1\n"
    "    8: GOTO_IF_NZ r6, L5\n"
    "    9: GET_ALIAS  r7, a2\n"
    "   10: ANDI       r8, r7, 2\n"
    "   11: SET_ALIAS  a3, r6\n"
    "   12: GOTO_IF_Z  r8, L3\n"
    "   13: LABEL      L5\n"
    "   14: SET_ALIAS  a3, r6\n"
    "   15: LOAD_IMM   r9, 8\n"
    "   16: SET_ALIAS  a1, r9\n"
    "   17: LABEL      L2\n"
    "   18: GOTO       L2\n"
    "   19: LOAD_IMM   r10, 12\n"
    "   20: SET_ALIAS  a1, r10\n"
    "   21: LABEL      L3\n"
    "   22: LOAD_IMM   r11, 16\n"
    "   23: SET_ALIAS  a1, r11\n"
    "   24: LABEL      L4\n"
    "   25: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32, no bound storage\n"
    "Alias 3: int32 @ 936(r1)\n"
    "\n"
    "Block 0: <none> --> [0,4] --> 1\n"
    "Block 1: 0 --> [5,8] --> 2,3\n"
    "Block 2: 1 --> [9,12] --> 3,6\n"
    "Block 3: 2,1 --> [13,16] --> 4\n"
    "Block 4: 3,4 --> [17,18] --> 4\n"
    "Block 5: <none> --> [19,20] --> 6\n"
    "Block 6: 5,2 --> [21,23] --> 7\n"
    "Block 7: 6 --> [24,25] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"