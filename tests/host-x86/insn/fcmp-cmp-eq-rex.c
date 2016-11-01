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
    alloc_dummy_registers(unit, 5, RTLTYPE_INT32);
    alloc_dummy_registers(unit, 2, RTLTYPE_FLOAT32);

    int reg1, reg2, reg3;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0x40000000));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_FCMP, reg3, reg1, reg2, RTLFCMP_EQ));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, reg2, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0x41,0xB8,0x00,0x00,0x80,0x3F,      // mov $0x3F800000,%r8d
    0x66,0x41,0x0F,0x6E,0xD0,           // movd %r8d,%xmm2
    0x41,0xB8,0x00,0x00,0x00,0x40,      // mov $0x40000000,%r8d
    0x66,0x41,0x0F,0x6E,0xD8,           // movd %r8d,%xmm3
    0x45,0x33,0xC0,                     // xor %r8d,%r8d
    0x0F,0x2E,0xD3,                     // ucomiss %xmm3,%xmm2
    0x7A,0x04,                          // jp L0
    0x41,0x0F,0x94,0xC0,                // setz %r8b
    0x48,0x83,0xC4,0x08,                // L0: add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
