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

    int reg1, reg2, reg3, reg4, reg5, reg6, alias;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT64));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_V2_FLOAT32));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_V2_FLOAT32));
    EXPECT(reg6 = rtl_alloc_register(unit, RTLTYPE_V2_FLOAT64));
    EXPECT(alias = rtl_alloc_alias_register(unit, RTLTYPE_V2_FLOAT32));

    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM,
                        reg2, 0, 0, UINT64_C(0x4008000000000000)));
    EXPECT(rtl_add_insn(unit, RTLOP_GET_ALIAS, reg3, 0, 0, alias));
    EXPECT_EQ(unit->num_insns, 3);
    EXPECT_FALSE(unit->error);

    EXPECT_FALSE(rtl_add_insn(unit, RTLOP_VINSERT, reg4, reg3, reg1, 0));
    EXPECT_ICE("Operand constraint violated:"
               " rtl_register_is_vector(&unit->regs[dest])");
    EXPECT_EQ(unit->num_insns, 3);
    EXPECT(unit->error);
    unit->error = false;

    EXPECT_FALSE(rtl_add_insn(unit, RTLOP_VINSERT, reg6, reg3, reg1, 0));
    EXPECT_ICE("Operand constraint violated:"
               " unit->regs[src1].type == unit->regs[dest].type");
    EXPECT_EQ(unit->num_insns, 3);
    EXPECT(unit->error);
    unit->error = false;

    EXPECT_FALSE(rtl_add_insn(unit, RTLOP_VINSERT, reg5, reg3, reg2, 0));
    EXPECT_ICE("Operand constraint violated:"
               " unit->regs[src2].type == rtl_vector_element_type(unit->regs[dest].type)");
    EXPECT_EQ(unit->num_insns, 3);
    EXPECT(unit->error);
    unit->error = false;

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);

#endif  // ENABLE_OPERAND_SANITY_CHECKS

    return EXIT_SUCCESS;
}