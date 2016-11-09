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
    0x5C,0x83,0x29,0x8F,  // rlwnm. r3,r4,r5,6,7
};

static const unsigned int guest_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: GET_ALIAS  r3, a5\n"
    "    3: BFEXT      r4, r3, 31, 1\n"
    "    4: SET_ALIAS  a6, r4\n"
    "    5: BFEXT      r5, r3, 30, 1\n"
    "    6: SET_ALIAS  a7, r5\n"
    "    7: BFEXT      r6, r3, 29, 1\n"
    "    8: SET_ALIAS  a8, r6\n"
    "    9: BFEXT      r7, r3, 28, 1\n"
    "   10: SET_ALIAS  a9, r7\n"
    "   11: GET_ALIAS  r8, a3\n"
    "   12: GET_ALIAS  r9, a4\n"
    "   13: ROL        r10, r8, r9\n"
    "   14: ANDI       r11, r10, 50331648\n"
    "   15: SET_ALIAS  a2, r11\n"
    "   16: SLTSI      r12, r11, 0\n"
    "   17: SGTSI      r13, r11, 0\n"
    "   18: SEQI       r14, r11, 0\n"
    "   19: GET_ALIAS  r15, a10\n"
    "   20: BFEXT      r16, r15, 31, 1\n"
    "   21: SET_ALIAS  a6, r12\n"
    "   22: SET_ALIAS  a7, r13\n"
    "   23: SET_ALIAS  a8, r14\n"
    "   24: SET_ALIAS  a9, r16\n"
    "   25: LOAD_IMM   r17, 4\n"
    "   26: SET_ALIAS  a1, r17\n"
    "   27: GET_ALIAS  r18, a5\n"
    "   28: ANDI       r19, r18, 268435455\n"
    "   29: GET_ALIAS  r20, a6\n"
    "   30: SLLI       r21, r20, 31\n"
    "   31: OR         r22, r19, r21\n"
    "   32: GET_ALIAS  r23, a7\n"
    "   33: SLLI       r24, r23, 30\n"
    "   34: OR         r25, r22, r24\n"
    "   35: GET_ALIAS  r26, a8\n"
    "   36: SLLI       r27, r26, 29\n"
    "   37: OR         r28, r25, r27\n"
    "   38: GET_ALIAS  r29, a9\n"
    "   39: SLLI       r30, r29, 28\n"
    "   40: OR         r31, r28, r30\n"
    "   41: SET_ALIAS  a5, r31\n"
    "   42: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 276(r1)\n"
    "Alias 5: int32 @ 928(r1)\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32, no bound storage\n"
    "Alias 10: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,42] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
