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
    0xC0,0x23,0x00,0x00,  // lfs f1,0(r3)
    0xC8,0x23,0x00,0x08,  // lfd f1,8(r3)
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_ASSUME_NO_SNAN
                                    | BINREC_OPT_G_PPC_NO_FPSCR_STATE;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x7\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: ZCAST      r4, r3\n"
    "    4: ADD        r5, r2, r4\n"
    "    5: LOAD_BR    r6, 0(r5)\n"
    "    6: ZCAST      r7, r3\n"
    "    7: ADD        r8, r2, r7\n"
    "    8: LOAD_BR    r9, 8(r8)\n"
    "    9: FCVT       r10, r6\n"
    "   10: STORE      408(r1), r10\n"
    "   11: SET_ALIAS  a3, r9\n"
    "   12: LOAD_IMM   r11, 8\n"
    "   13: SET_ALIAS  a1, r11\n"
    "   14: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64 @ 400(r1)\n"
    "\n"
    "Block 0: <none> --> [0,14] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"