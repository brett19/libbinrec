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
    int label;
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(rtl_add_insn(unit, RTLOP_GOTO, 0, 0, 0, label));

    int reg1, reg2, reg3, chain_insn;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 1));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 2));
    EXPECT_EQ(chain_insn = rtl_add_chain_insn(unit, reg1, reg2), 3);
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 3));
    EXPECT(rtl_add_insn(unit, RTLOP_CHAIN_RESOLVE, 0, reg3, 0, chain_insn));
    EXPECT(rtl_add_insn(unit, RTLOP_RETURN, 0, 0, 0, 0));

    int reg4, reg5;
    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT32));
    /* reg4 should not clobber reg1 and reg2 since those are used below. */
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg4, 0, 0, 4));
    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_SELECT, reg5, reg1, reg2, reg4));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x41,0x57,                          // push %r15
    0xE9,0x5C,0x00,0x00,0x00,           // jmp L4
    0xB8,0x01,0x00,0x00,0x00,           // mov $1,%eax
    0xB9,0x02,0x00,0x00,0x00,           // mov $2,%ecx
    0x0F,0x1F,0x80,0x00,0x00,0x00,0x00, // nopl 0(%rax)
    0xE9,0x12,0x00,0x00,0x00,           // L1: jmp L2
    0x00,0x00,0x00,0x00,0x00,           // (data)
    0x48,0x8B,0xF8,                     // mov %rax,%rdi
    0x48,0x8B,0xF1,                     // mov %rcx,%rsi
    0x49,0x8B,0xC7,                     // mov %r15,%rax
    0x41,0x5F,                          // pop %r15
    0xFF,0xE0,                          // jmp *%rax
    0xBA,0x03,0x00,0x00,0x00,           // L2: mov $3,%edx
    0x48,0x85,0xD2,                     // test %rdx,%rdx
    0x74,0x25,                          // jz L4
    0x4C,0x8B,0xFA,                     // mov %rdx,%r15
    0x49,0xC1,0xEF,0x30,                // shr $48,%r15
    0x74,0x08,                          // jz L3
    0x66,0x44,0x89,0x3D,0xD6,0xFF,0xFF, // mov %r15w,-42(%rip)
      0xFF,
    0x48,0xC1,0xE2,0x10,                // L3: shl $16,%rdx
    0x48,0x81,0xCA,0x49,0xBF,0x00,0x00, // or $0xBF49,%rdx
    0x48,0x89,0x15,0xBC,0xFF,0xFF,0xFF, // mov %rdx,-68(%rip)
    0xEB,0xBA,                          // jmp L1
    0xE9,0x0B,0x00,0x00,0x00,           // jmp epilogue
    0xBA,0x04,0x00,0x00,0x00,           // L4: mov $4,%edx
    0x85,0xD2,                          // test %edx,%edx
    0x48,0x0F,0x44,0xC1,                // cmovz %rcx,%rax
    0x41,0x5F,                          // epilogue: pop %r15
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
