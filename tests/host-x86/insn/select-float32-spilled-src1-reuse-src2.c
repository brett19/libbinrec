/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "tests/common.h"
#include "tests/host-x86/common.h"


static const binrec_setup_t setup = {
    .host = BINREC_ARCH_X86_64_SYSV,
};
static const unsigned int host_opt = 0;

static int add_rtl(RTLUnit *unit)
{
    int dummy_regs[14];
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(dummy_regs[i] = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, dummy_regs[i], 0, 0, 0));
    }
    alloc_dummy_registers(unit, 3, RTLTYPE_INT32);

    int reg1, reg2, reg3, reg4;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 3));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_SELECT, reg4, reg1, reg2, reg3));
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, dummy_regs[i], 0, 0));
    }
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, 0, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0xBE,0x00,0x00,0x80,0x3F,           // mov $0x3F800000,%esi
    0x66,0x44,0x0F,0x6E,0xF6,           // movd %esi,%xmm14
    0xF3,0x44,0x0F,0x11,0x34,0x24,      // movss %xmm14,(%rsp)
    0xBE,0x00,0x00,0x00,0x40,           // mov $0x40000000,%esi
    0x66,0x44,0x0F,0x6E,0xF6,           // movd %esi,%xmm14
    0xBE,0x03,0x00,0x00,0x00,           // mov $3,%esi
    0x85,0xF6,                          // test %esi,%esi
    0x74,0x06,                          // jz L0
    0xF3,0x44,0x0F,0x10,0x34,0x24,      // movss (%rsp),%xmm14
    0x48,0x83,0xC4,0x08,                // L0: add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
