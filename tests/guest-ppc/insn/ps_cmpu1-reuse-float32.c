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
    0xFC,0x20,0x08,0x18,  // frsp f1,f1
    0x13,0x81,0x10,0x80,  // ps_cmpu1 cr7,f1,f2
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_NO_FPSCR_STATE;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x7\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a2\n"
    "    3: VEXTRACT   r4, r3, 0\n"
    "    4: FCVT       r5, r4\n"
    "    5: GET_ALIAS  r6, a3\n"
    "    6: VEXTRACT   r7, r6, 1\n"
    "    7: FCAST      r8, r7\n"
    "    8: FCMP       r9, r5, r8, LT\n"
    "    9: FCMP       r10, r5, r8, GT\n"
    "   10: FCMP       r11, r5, r8, EQ\n"
    "   11: FCMP       r12, r5, r8, UN\n"
    "   12: GET_ALIAS  r13, a4\n"
    "   13: SLLI       r14, r9, 3\n"
    "   14: SLLI       r15, r10, 2\n"
    "   15: SLLI       r16, r11, 1\n"
    "   16: OR         r17, r14, r15\n"
    "   17: OR         r18, r16, r12\n"
    "   18: OR         r19, r17, r18\n"
    "   19: BFINS      r20, r13, r19, 0, 4\n"
    "   20: SET_ALIAS  a4, r20\n"
    "   21: FCVT       r21, r5\n"
    "   22: VBROADCAST r22, r21\n"
    "   23: SET_ALIAS  a2, r22\n"
    "   24: LOAD_IMM   r23, 8\n"
    "   25: SET_ALIAS  a1, r23\n"
    "   26: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: int32 @ 928(r1)\n"
    "\n"
    "Block 0: <none> --> [0,26] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
