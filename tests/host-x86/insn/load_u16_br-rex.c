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
    alloc_dummy_registers(unit, 4, RTLTYPE_INT32);

    uint32_t reg1, reg2, reg3, reg4, reg5, reg6;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_U16_BR, reg2, reg1, 0, 0));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 0));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_U16_BR, reg4, reg3, 0, 0));
    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg5, 0, 0, 0));
    EXPECT(reg6 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_U16_BR, reg6, reg5, 0, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, reg2, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg3, reg4, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg5, 0, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x53,                               // push %rbx
    0x33,0xFF,                          // xor %edi,%edi
    0x44,0x0F,0xB7,0x07,                // movzwl (%rdi),%r8d
    0x41,0x0F,0xC8,                     // bswap %r8d
    0x41,0xC1,0xE8,0x10,                // shr $16,%r8d
    0x45,0x33,0xC9,                     // xor %r9d,%r9d
    0x45,0x0F,0xB7,0x11,                // movzwl (%r9),%r10d
    0x41,0x0F,0xCA,                     // bswap %r10d
    0x41,0xC1,0xEA,0x10,                // shr $16,%r10d
    0x45,0x33,0xDB,                     // xor %r11d,%r11d
    0x41,0x0F,0xB7,0x1B,                // movzwl (%r11),%ebx
    0x0F,0xCB,                          // bswap %ebx
    0xC1,0xEB,0x10,                     // shr $16,%ebx
    0x5B,                               // pop %rbx
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"