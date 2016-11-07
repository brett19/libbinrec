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
    0xFC,0x20,0x10,0x91,  // fmr. f1,f2
};

static const unsigned int guest_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: GET_ALIAS  r4, a9\n"
    "    4: ANDI       r5, r4, 33031936\n"
    "    5: SGTUI      r6, r5, 0\n"
    "    6: BFEXT      r7, r4, 25, 4\n"
    "    7: SLLI       r8, r6, 4\n"
    "    8: SRLI       r9, r4, 3\n"
    "    9: OR         r10, r7, r8\n"
    "   10: AND        r11, r10, r9\n"
    "   11: ANDI       r12, r11, 31\n"
    "   12: SGTUI      r13, r12, 0\n"
    "   13: BFEXT      r14, r4, 31, 1\n"
    "   14: BFEXT      r15, r4, 28, 1\n"
    "   15: SET_ALIAS  a4, r14\n"
    "   16: SET_ALIAS  a5, r13\n"
    "   17: SET_ALIAS  a6, r6\n"
    "   18: SET_ALIAS  a7, r15\n"
    "   19: SET_ALIAS  a2, r3\n"
    "   20: LOAD_IMM   r16, 4\n"
    "   21: SET_ALIAS  a1, r16\n"
    "   22: GET_ALIAS  r17, a8\n"
    "   23: ANDI       r18, r17, -251658241\n"
    "   24: GET_ALIAS  r19, a4\n"
    "   25: SLLI       r20, r19, 27\n"
    "   26: OR         r21, r18, r20\n"
    "   27: GET_ALIAS  r22, a5\n"
    "   28: SLLI       r23, r22, 26\n"
    "   29: OR         r24, r21, r23\n"
    "   30: GET_ALIAS  r25, a6\n"
    "   31: SLLI       r26, r25, 25\n"
    "   32: OR         r27, r24, r26\n"
    "   33: GET_ALIAS  r28, a7\n"
    "   34: SLLI       r29, r28, 24\n"
    "   35: OR         r30, r27, r29\n"
    "   36: SET_ALIAS  a8, r30\n"
    "   37: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: float64 @ 400(r1)\n"
    "Alias 3: float64 @ 416(r1)\n"
    "Alias 4: int32, no bound storage\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32 @ 928(r1)\n"
    "Alias 9: int32 @ 944(r1)\n"
    "\n"
    "Block 0: <none> --> [0,37] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"