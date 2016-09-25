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
    int reg1, reg2, reg3, alias1, alias2;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_ARG, reg1, 0, 0, 0));
    EXPECT(alias1 = rtl_alloc_alias_register(unit, RTLTYPE_FLOAT));
    rtl_set_alias_storage(unit, alias1, reg1, 0x1234);
    EXPECT(alias2 = rtl_alloc_alias_register(unit, RTLTYPE_FLOAT));
    rtl_set_alias_storage(unit, alias2, reg1, 0x5678);
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT(rtl_add_insn(unit, RTLOP_SET_ALIAS, 0, reg2, 0, alias1));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_FLOAT));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 0x40400000));
    EXPECT(rtl_add_insn(unit, RTLOP_SET_ALIAS, 0, reg3, 0, alias2));

    int reg4, reg5, reg6, label;
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_FLOAT));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg4, 0, 0, 0x40800000)); // Touch XMM0.
    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_FLOAT));
    EXPECT(rtl_add_insn(unit, RTLOP_GET_ALIAS, reg5, 0, 0, alias1)); // XMM0->1
    EXPECT(reg6 = rtl_alloc_register(unit, RTLTYPE_FLOAT));
    EXPECT(rtl_add_insn(unit, RTLOP_GET_ALIAS, reg6, 0, 0, alias2)); // XMM1->2
    EXPECT(rtl_add_insn(unit, RTLOP_STORE, 0, reg1, reg5, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_STORE, 0, reg1, reg6, 4));
    EXPECT(rtl_add_insn(unit, RTLOP_SET_ALIAS, 0, reg6, 0, alias1));

    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, 0, 0));
    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0xB8,0x00,0x00,0x00,0x40,           // mov $0x40000000,%eax
    0x66,0x0F,0x6E,0xC0,                // movd %eax,%xmm0
    0xB8,0x00,0x00,0x40,0x40,           // mov $0x40400000,%eax
    0x66,0x0F,0x6E,0xC8,                // movd %eax,%xmm1
    0xF3,0x0F,0x11,0x8F,0x78,0x56,0x00, // movss %xmm1,0x5678(%rdi)
      0x00,
    0x0F,0x57,0xC8,                     // xorps %xmm0,%xmm1
    0x0F,0x57,0xC1,                     // xorps %xmm1,%xmm0
    0x0F,0x57,0xC8,                     // xorps %xmm0,%xmm1
    0x0F,0x28,0xD0,                     // movaps %xmm0,%xmm2
    0xB8,0x00,0x00,0x80,0x40,           // mov $0x40800000,%eax
    0x66,0x0F,0x6E,0xC0,                // movd %eax,%xmm0
    0xF3,0x0F,0x11,0x0F,                // movss %xmm1,(%rdi)
    0xF3,0x0F,0x11,0x57,0x04,           // movss %xmm2,4(%rdi)
    0xF3,0x0F,0x11,0x97,0x34,0x12,0x00, // movss %xmm2,0x1234(%rdi)
      0x00,
    0x48,0x83,0xC4,0x08,                // add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"