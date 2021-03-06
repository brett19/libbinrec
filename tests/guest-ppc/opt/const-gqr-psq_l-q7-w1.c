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
    0xE0,0x23,0xAF,0xF0,  // psq_l f1,-16(r3),1,2
};

#define INITIAL_STATE  &(PPCInsnTestState){.gqr = {0,0,0x00070000}}

static const unsigned int guest_opt = BINREC_OPT_G_PPC_CONSTANT_GQRS;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: ZCAST      r4, r3\n"
    "    4: ADD        r5, r2, r4\n"
    "    5: LOAD_S16_BR r6, -16(r5)\n"
    "    6: FSCAST     r7, r6\n"
    "    7: LOAD_IMM   r8, 1.0f\n"
    "    8: VBUILD2    r9, r7, r8\n"
    "    9: VFCVT      r10, r9\n"
    "   10: SET_ALIAS  a3, r10\n"
    "   11: LOAD_IMM   r11, 4\n"
    "   12: SET_ALIAS  a1, r11\n"
    "   13: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: float64[2] @ 400(r1)\n"
    "\n"
    "Block 0: <none> --> [0,13] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
