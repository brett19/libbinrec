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
    binrec_setup_t setup;
    memset(&setup, 0, sizeof(setup));
    setup.log = log_capture;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&setup));

    RTLUnit *unit;
    EXPECT(unit = rtl_create_unit(handle));

    int reg1, reg2, reg3, reg4, reg5;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));

    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 0x40400000));
    EXPECT(rtl_add_insn(unit, RTLOP_FNMADD, reg4, reg1, reg2, reg3));
    EXPECT_EQ(unit->num_insns, 4);
    EXPECT_EQ(unit->insns[3].opcode, RTLOP_FNMADD);
    EXPECT_EQ(unit->insns[3].dest, reg4);
    EXPECT_EQ(unit->insns[3].src1, reg1);
    EXPECT_EQ(unit->insns[3].src2, reg2);
    EXPECT_EQ(unit->insns[3].src3, reg3);
    EXPECT_EQ(unit->regs[reg1].birth, 0);
    EXPECT_EQ(unit->regs[reg1].death, 3);
    EXPECT_EQ(unit->regs[reg2].birth, 1);
    EXPECT_EQ(unit->regs[reg2].death, 3);
    EXPECT_EQ(unit->regs[reg3].birth, 2);
    EXPECT_EQ(unit->regs[reg3].death, 3);
    EXPECT_EQ(unit->regs[reg4].birth, 3);
    EXPECT_EQ(unit->regs[reg4].death, 3);
    EXPECT(unit->have_block);
    EXPECT_FALSE(unit->error);

    EXPECT(rtl_add_insn(unit, RTLOP_MOVE, reg5, reg4, 0, 0));
    EXPECT_FALSE(unit->error);

    EXPECT(rtl_finalize_unit(unit));

    const char *disassembly =
        "    0: LOAD_IMM   r1, 1.0f\n"
        "    1: LOAD_IMM   r2, 2.0f\n"
        "    2: LOAD_IMM   r3, 3.0f\n"
        "    3: FNMADD     r4, r1, r2, r3\n"
        "           r1: 1.0f\n"
        "           r2: 2.0f\n"
        "           r3: 3.0f\n"
        "    4: MOVE       r5, r4\n"
        "           r4: -fma(r1, r2, r3)\n"
        "\n"
        "Block 0: <none> --> [0,4] --> <none>\n"
        ;
    EXPECT_STREQ(rtl_disassemble_unit(unit, true), disassembly);

    EXPECT_STREQ(get_log_messages(), NULL);

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);
    return EXIT_SUCCESS;
}
