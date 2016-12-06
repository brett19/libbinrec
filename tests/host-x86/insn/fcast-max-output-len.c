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
    int dummy_alias[8];
    for (int i = 0; i < lenof(dummy_alias); i++) {
        EXPECT(dummy_alias[i] = rtl_alloc_alias_register(unit,
                                                         RTLTYPE_V2_FLOAT64));
    }

    alloc_dummy_registers(unit, 5, RTLTYPE_INT32);
    int dummy_regs[14];
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(dummy_regs[i] = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, dummy_regs[i], 0, 0, 0));
    }

    int reg1, reg2, spiller;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(spiller = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, spiller, 0, 0, 0));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT64));
    EXPECT(rtl_add_insn(unit, RTLOP_FCAST, reg2, reg1, 0, 0));
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, dummy_regs[i], 0, 0));
    }
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, 0, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x81,0xEC,0x88,0x00,0x00,0x00,      // sub $136,%rsp
    0xEB,0x17,                               // jmp 0x20
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,      // (padding)

    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,0xFF, // (data)
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // (data)

    0x41,0xB8,0x00,0x00,0x80,0x3F,           // mov $0x3F800000,%r8d
    0x66,0x45,0x0F,0x6E,0xF0,                // movd %r8d,%xmm14
    0xF3,0x44,0x0F,0x11,0xB4,0x24,0x84,0x00, // movss %xmm14,132(%rsp)
      0x00,0x00,
    0x0F,0xAE,0x9C,0x24,0x80,0x00,0x00,0x00, // stmxcsr 128(%rsp)
    0xF3,0x44,0x0F,0x10,0xB4,0x24,0x84,0x00, // movss 132(%rsp),%xmm14
      0x00,0x00,
    0x66,0x45,0x0F,0x7E,0xF0,                // movd %xmm14,%r8d
    0x45,0x0F,0x2E,0xF6,                     // ucomiss %xmm14,%xmm14
    0xF3,0x45,0x0F,0x5A,0xF6,                // cvtss2sd %xmm14,%xmm14
    0x7B,0x11,                               // jnp L0
    0x41,0xF7,0xC0,0x00,0x00,0x40,0x00,      // test $0x400000,%r8d
    0x75,0x08,                               // jnz L0
    0x44,0x0F,0x54,0x35,0xA8,0xFF,0xFF,0xFF, // andps -88(%rip),%xmm14
    0x0F,0xAE,0x94,0x24,0x80,0x00,0x00,0x00, // L0: ldmxcsr 128(%rsp)
    0x48,0x81,0xC4,0x88,0x00,0x00,0x00,      // add $136,%rsp
    0xC3,                                    // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
