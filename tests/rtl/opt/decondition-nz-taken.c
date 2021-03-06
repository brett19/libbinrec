/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/rtl-internal.h"
#include "tests/common.h"


static unsigned int opt_flags = BINREC_OPT_DECONDITION;

static int add_rtl(RTLUnit *unit)
{
    int label, reg1;
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 1));
    EXPECT(rtl_add_insn(unit, RTLOP_GOTO_IF_NZ, 0, reg1, 0, label));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, 0, 0, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));
    EXPECT(rtl_add_insn(unit, RTLOP_RETURN, 0, reg1, 0, 0));

    return EXIT_SUCCESS;
}

static const char expected[] =
    #ifdef RTL_DEBUG_OPTIMIZE
        "[info] Branch at 1 always taken, converting to GOTO and dropping edge 0->1\n"
    #endif
    "    0: LOAD_IMM   r1, 1\n"
    "    1: GOTO       L1\n"
    "    2: NOP\n"
    "    3: LABEL      L1\n"
    "    4: RETURN     r1\n"
    "\n"
    "Block 0: <none> --> [0,1] --> 2\n"
    "Block 1: <none> --> [2,2] --> 2\n"
    "Block 2: 1,0 --> [3,4] --> <none>\n"
    ;

#include "tests/rtl-optimize-test.i"
