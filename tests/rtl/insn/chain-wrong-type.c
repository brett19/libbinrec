/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/rtl.h"
#include "src/rtl-internal.h"
#include "tests/common.h"
#include "tests/log-capture.h"


int main(void)
{
#ifdef ENABLE_OPERAND_SANITY_CHECKS

    binrec_setup_t setup;
    memset(&setup, 0, sizeof(setup));
    setup.log = log_capture;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&setup));

    RTLUnit *unit;
    EXPECT(unit = rtl_create_unit(handle));

    int reg1, reg2;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));

    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 10));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT_EQ(unit->num_insns, 2);
    EXPECT_FALSE(unit->error);

    EXPECT_EQ(rtl_add_chain_insn(unit, reg2, reg1), -1);
    EXPECT_ICE("Operand constraint violated:"
               " rtl_register_is_int(&unit->regs[src1])");
    EXPECT_EQ(unit->num_insns, 2);
    EXPECT(unit->error);
    unit->error = false;

    EXPECT_EQ(rtl_add_chain_insn(unit, reg1, reg2), -1);
    EXPECT_ICE("Operand constraint violated:"
               " rtl_register_is_int(&unit->regs[src2])");
    EXPECT_EQ(unit->num_insns, 2);
    EXPECT(unit->error);
    unit->error = false;

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);

#endif  // ENABLE_OPERAND_SANITY_CHECKS

    return EXIT_SUCCESS;
}
