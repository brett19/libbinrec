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
static const unsigned int host_opt = BINREC_OPT_H_X86_FORWARD_CONDITIONS;

static int add_rtl(RTLUnit *unit)
{
    int label, reg1, reg2, reg3, reg4;
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    int dummy_reg[14];
    for (int i = 0; i < lenof(dummy_reg); i++) {
        EXPECT(dummy_reg[i] = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, dummy_reg[i], 0, 0, 0));
    }
    for (int i = 0; i < lenof(dummy_reg); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, dummy_reg[i], 0, 0));
    }
    /* Consume reg1's register to verify that the temporary is actually
     * allocated and stored. */
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 0x40400000));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_FCMP, reg4, reg1, reg2, RTLFCMP_GT));
    EXPECT(rtl_add_insn(unit, RTLOP_GOTO_IF_NZ, 0, reg4, 0, label));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg3, 0, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, 0, 0));  // Force reg1 spill.

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0xB8,0x00,0x00,0x80,0x3F,           // L1: mov $0x3F800000,%eax
    0x66,0x0F,0x6E,0xC0,                // movd %eax,%xmm0
    0xB8,0x00,0x00,0x00,0x40,           // mov $0x40000000,%eax
    0x66,0x0F,0x6E,0xC8,                // movd %eax,%xmm1
    0xF3,0x0F,0x11,0x04,0x24,           // movss %xmm0,(%rsp)
    0xB8,0x00,0x00,0x40,0x40,           // mov $0x40400000,%eax
    0x66,0x0F,0x6E,0xC0,                // movd %eax,%xmm0
    0xF3,0x0F,0x10,0x14,0x24,           // movss (%rsp),%xmm2
    0x0F,0x2E,0xD1,                     // ucomiss %xmm1,%xmm2
    0x77,0xD6,                          // ja L1
    0x48,0x83,0xC4,0x08,                // add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] =
    #ifdef RTL_DEBUG_OPTIMIZE
        "[info] Extending r2 live range to 33\n"
        "[info] Killing instruction 32\n"
    #endif
    "";

#include "tests/rtl-translate-test.i"
