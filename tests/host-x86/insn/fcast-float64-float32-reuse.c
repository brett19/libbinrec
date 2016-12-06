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
    alloc_dummy_registers(unit, 1, RTLTYPE_FLOAT32);

    int reg1, reg2;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_FLOAT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0x3F800000));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_FLOAT64));
    /* reg2 does not reuse XMM1 even though reg1 dies here because the
     * input and output registers need to be separate. */
    EXPECT(rtl_add_insn(unit, RTLOP_FCAST, reg2, reg1, 0, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0xEB,0x1A,                          // jmp 0x20
    0x00,0x00,0x00,0x00,0x00,0x00,0x00, // (padding)
    0x00,0x00,0x00,                     // (padding)

    0xFF,0xFF,0xFF,0xFF,0xFF,0xFF,0xF7,0xFF, // (data)
    0x00,0x00,0x00,0x00,0x00,0x00,0x00,0x00, // (data)

    0xB8,0x00,0x00,0x80,0x3F,           // mov $0x3F800000,%eax
    0x66,0x0F,0x6E,0xC8,                // movd %eax,%xmm1
    0x0F,0xAE,0x1C,0x24,                // stmxcsr (%rsp)
    0x0F,0x2E,0xC9,                     // ucomiss %xmm1,%xmm1
    0xF3,0x0F,0x5A,0xD1,                // cvtss2sd %xmm1,%xmm2
    0x7B,0x13,                          // jnp L0
    0x66,0x0F,0x7E,0xC8,                // movd %xmm1,%eax
    0xF7,0xC0,0x00,0x00,0x40,0x00,      // test $0x400000,%eax
    0x75,0x07,                          // jnz L0
    0x0F,0x54,0x15,0xC7,0xFF,0xFF,0xFF, // andps -57(%rip),%xmm2
    0x0F,0xAE,0x14,0x24,                // L0: ldmxcsr (%rsp)
    0x48,0x83,0xC4,0x08,                // add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
