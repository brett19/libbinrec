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
    0x80,0x64,0xFF,0xF0,  // lwz r3,-16(r4)
    0x38,0x63,0x00,0x01,  // addi r3,r3,1
    0x90,0x64,0xFF,0xF4,  // stw r3,-12(r4)
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_FORWARD_LOADS;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0xB\n"
    "[info] Killing instruction 7\n"
    "[info] r1 death rolled back to 2\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: ZCAST      r4, r3\n"
    "    4: ADD        r5, r2, r4\n"
    "    5: LOAD       r6, -16(r5)\n"
    "    6: BSWAP      r7, r6\n"
    "    7: NOP\n"
    "    8: ADDI       r8, r7, 1\n"
    "    9: SET_ALIAS  a2, r8\n"
    "   10: ZCAST      r9, r3\n"
    "   11: ADD        r10, r2, r9\n"
    "   12: STORE_BR   -12(r10), r8\n"
    "   13: LOAD_IMM   r11, 12\n"
    "   14: SET_ALIAS  a1, r11\n"
    "   15: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "\n"
    "Block 0: <none> --> [0,15] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
