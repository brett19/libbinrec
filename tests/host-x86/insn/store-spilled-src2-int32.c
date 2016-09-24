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
    .host = BINREC_ARCH_X86_64_WINDOWS,  // To check whether XMM15 is pushed.
};
static const unsigned int host_opt = 0;

static int add_rtl(RTLUnit *unit)
{
    int dummy_regs[13];
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(dummy_regs[i] = rtl_alloc_register(unit, RTLTYPE_INT32));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, dummy_regs[i], 0, 0, 0));
    }

    int reg1, reg2;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 0));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_STORE, 0, reg2, reg1, 0));
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, dummy_regs[i], 0, 0));
    }
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, reg2, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x53,                               // push %rbx
    0x55,                               // push %rbp
    0x56,                               // push %rsi
    0x57,                               // push %rdi
    0x41,0x54,                          // push %r12
    0x41,0x55,                          // push %r13
    0x41,0x56,                          // push %r14
    0x48,0x83,0xEC,0x20,                // sub $32,%rsp
    0x44,0x0F,0x29,0x7C,0x24,0x10,      // movaps %xmm15,16(%rsp)
    0x45,0x33,0xF6,                     // xor %r14d,%r14d
    0x44,0x89,0x34,0x24,                // mov %r14d,(%rsp)
    0x45,0x33,0xF6,                     // xor %r14d,%r14d
    0xF3,0x44,0x0F,0x10,0x3C,0x24,      // movss (%rsp),%xmm15
    0xF3,0x45,0x0F,0x11,0x3E,           // movss %xmm15,(%r14)
    0x44,0x0F,0x28,0x7C,0x24,0x10,      // movaps 16(%rsp),%xmm15
    0x48,0x83,0xC4,0x20,                // add $32,%rsp
    0x41,0x5E,                          // pop %r14
    0x41,0x5D,                          // pop %r13
    0x41,0x5C,                          // pop %r12
    0x5F,                               // pop %rdi
    0x5E,                               // pop %rsi
    0x5D,                               // pop %rbp
    0x5B,                               // pop %rbx
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
