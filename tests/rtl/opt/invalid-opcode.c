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


static unsigned int opt_flags = BINREC_OPT_FOLD_CONSTANTS;

static int add_rtl(RTLUnit *unit)
{
    int reg1, reg2, reg3, reg4;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_ARG, reg1, 0, 0, 0));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 2));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_ADD, reg3, reg2, reg2, 0));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_ADD, reg4, reg1, reg3, 0));
    /* Change the ADDs to invalid opcodes.  This case will never be
     * encountered in normal usage; this test is just to exercise the
     * default cases in various switch blocks. */
    unit->insns[2].opcode = 0;
    unit->regs[reg3].result.opcode = 0;
    unit->insns[3].opcode = 0;
    unit->regs[reg4].result.opcode = 0;

    return EXIT_SUCCESS;
}

static const char expected[] =
    "[error] Invalid opcode 0 on RESULT register 3\n"
    #ifdef RTL_DEBUG_OPTIMIZE
        "[info] Folded r3 to constant value 0 at 2\n"
        "[info] r2 no longer used, setting death = birth\n"
        "[info] Reduced r4 to register-immediate operation (r1, 0) at 3\n"
        "[info] r3 no longer used, setting death = birth\n"
    #endif
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_IMM   r2, 2\n"
    "    2: LOAD_IMM   r3, 0\n"
    "    3: <invalid opcode 0>\n"
    "\n"
    "Block 0: <none> --> [0,3] --> <none>\n"
    ;

#include "tests/rtl-optimize-test.i"
