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


static unsigned int opt_flags = 0;

static int add_rtl(RTLUnit *unit)
{
    int reg1, reg2, reg3, reg4;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 0x40400000));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_FMADD, reg4, reg1, reg2, reg3));

    EXPECT_EQ(unit->num_insns, 4);
    rtl_opt_kill_insn(unit, 3, true, false);
    EXPECT_FALSE(unit->regs[reg4].live);
    EXPECT_EQ(unit->regs[reg1].death, 0);
    EXPECT_EQ(unit->regs[reg2].death, 1);
    EXPECT_EQ(unit->regs[reg3].death, 2);

    return EXIT_SUCCESS;
}

static const char expected[] =
    #ifdef RTL_DEBUG_OPTIMIZE
        "[info] Killing instruction 3\n"
        "[info] r3 no longer used, setting death = birth\n"
        "[info] r2 no longer used, setting death = birth\n"
        "[info] r1 no longer used, setting death = birth\n"
    #endif
    "    0: LOAD_IMM   r1, 1.0f\n"
    "    1: LOAD_IMM   r2, 2.0f\n"
    "    2: LOAD_IMM   r3, 3.0f\n"
    "    3: NOP\n"
    "\n"
    "Block 0: <none> --> [0,3] --> <none>\n"
    ;

#include "tests/rtl-optimize-test.i"
