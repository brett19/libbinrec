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
    0x7C,0x83,0x28,0x31,  // slw. r3,r4,r5
};

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 0x100000000\n"
    "    2: LABEL      L1\n"
    "    3: GET_ALIAS  r3, a3\n"
    "    4: GET_ALIAS  r4, a4\n"
    "    5: ANDI       r5, r4, 63\n"
    "    6: ZCAST      r6, r3\n"
    "    7: SLL        r7, r6, r5\n"
    "    8: ZCAST      r8, r7\n"
    "    9: SLTSI      r9, r8, 0\n"
    "   10: SGTSI      r10, r8, 0\n"
    "   11: SEQI       r11, r8, 0\n"
    "   12: GET_ALIAS  r12, a10\n"
    "   13: BFEXT      r13, r12, 31, 1\n"
    "   14: SET_ALIAS  a2, r8\n"
    "   15: SET_ALIAS  a5, r9\n"
    "   16: SET_ALIAS  a6, r10\n"
    "   17: SET_ALIAS  a7, r11\n"
    "   18: SET_ALIAS  a8, r13\n"
    "   19: LOAD_IMM   r14, 4\n"
    "   20: SET_ALIAS  a1, r14\n"
    "   21: LABEL      L2\n"
    "   22: GET_ALIAS  r15, a9\n"
    "   23: ANDI       r16, r15, 268435455\n"
    "   24: GET_ALIAS  r17, a5\n"
    "   25: SLLI       r18, r17, 31\n"
    "   26: OR         r19, r16, r18\n"
    "   27: GET_ALIAS  r20, a6\n"
    "   28: SLLI       r21, r20, 30\n"
    "   29: OR         r22, r19, r21\n"
    "   30: GET_ALIAS  r23, a7\n"
    "   31: SLLI       r24, r23, 29\n"
    "   32: OR         r25, r22, r24\n"
    "   33: GET_ALIAS  r26, a8\n"
    "   34: SLLI       r27, r26, 28\n"
    "   35: OR         r28, r25, r27\n"
    "   36: SET_ALIAS  a9, r28\n"
    "   37: RETURN\n"
    "\n"
    "Alias 1: int32 @ 956(r1)\n"
    "Alias 2: int32 @ 268(r1)\n"
    "Alias 3: int32 @ 272(r1)\n"
    "Alias 4: int32 @ 276(r1)\n"
    "Alias 5: int32, no bound storage\n"
    "Alias 6: int32, no bound storage\n"
    "Alias 7: int32, no bound storage\n"
    "Alias 8: int32, no bound storage\n"
    "Alias 9: int32 @ 928(r1)\n"
    "Alias 10: int32 @ 940(r1)\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 1\n"
    "Block 1: 0 --> [2,20] --> 2\n"
    "Block 2: 1 --> [21,37] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
