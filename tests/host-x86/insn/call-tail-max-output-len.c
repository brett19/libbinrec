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
    .host = BINREC_ARCH_X86_64_WINDOWS,
};
static const unsigned int host_opt = 0;

static int add_rtl(RTLUnit *unit)
{
    int dummy_regs[29];
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(dummy_regs[i] = rtl_alloc_register(unit, RTLTYPE_INT64));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, dummy_regs[i], 0, 0, 0));
    }
    alloc_dummy_registers(unit, 15, RTLTYPE_FLOAT);

    int reg1, reg2, reg3, reg4;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 1));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_INT64));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 2));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_INT64));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 3));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT64));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg4, 0, 0, 4));
    EXPECT(rtl_add_insn(unit, RTLOP_STORE, 0, reg1, reg2, 0));  // Touch XMM15.
    EXPECT(rtl_add_insn(unit, RTLOP_CALL, 0, reg1, reg2, reg3));
    EXPECT(rtl_add_insn(unit, RTLOP_RETURN, 0, 0, 0, 0));
    for (int i = 0; i < lenof(dummy_regs); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, dummy_regs[i], 0, 0));
    }
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg1, reg2, 0));
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg3, reg4, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x53,                                         // push %rbx
    0x55,                                         // push %rbp
    0x56,                                         // push %rsi
    0x57,                                         // push %rdi
    0x41,0x54,                                    // push %r12
    0x41,0x55,                                    // push %r13
    0x41,0x56,                                    // push %r14
    0x41,0x57,                                    // push %r15
    0x48,0x81,0xEC,0x48,0x01,0x00,0x00,           // sub $328,%rsp
    0x0F,0x29,0xB4,0x24,0xA0,0x00,0x00,0x00,      // movaps %xmm6,160(%rsp)
    0x0F,0x29,0xBC,0x24,0xB0,0x00,0x00,0x00,      // movaps %xmm7,176(%rsp)
    0x44,0x0F,0x29,0x84,0x24,0xC0,0x00,0x00,0x00, // movaps %xmm8,192(%rsp)
    0x44,0x0F,0x29,0x8C,0x24,0xD0,0x00,0x00,0x00, // movaps %xmm9,208(%rsp)
    0x44,0x0F,0x29,0x94,0x24,0xE0,0x00,0x00,0x00, // movaps %xmm10,224(%rsp)
    0x44,0x0F,0x29,0x9C,0x24,0xF0,0x00,0x00,0x00, // movaps %xmm11,240(%rsp)
    0x44,0x0F,0x29,0xA4,0x24,0x00,0x01,0x00,0x00, // movaps %xmm12,256(%rsp)
    0x44,0x0F,0x29,0xAC,0x24,0x10,0x01,0x00,0x00, // movaps %xmm13,272(%rsp)
    0x44,0x0F,0x29,0xB4,0x24,0x20,0x01,0x00,0x00, // movaps %xmm14,288(%rsp)
    0x44,0x0F,0x29,0xBC,0x24,0x30,0x01,0x00,0x00, // movaps %xmm15,304(%rsp)
    0x4C,0x89,0x34,0x24,                          // mov %r14,(%rsp)
    0x4C,0x89,0x74,0x24,0x08,                     // mov %r14,8(%rsp)
    0x4C,0x89,0x74,0x24,0x10,                     // mov %r14,16(%rsp)
    0x4C,0x89,0x74,0x24,0x18,                     // mov %r14,24(%rsp)
    0x4C,0x89,0x74,0x24,0x20,                     // mov %r14,32(%rsp)
    0x4C,0x89,0x74,0x24,0x28,                     // mov %r14,40(%rsp)
    0x4C,0x89,0x74,0x24,0x30,                     // mov %r14,48(%rsp)
    0x4C,0x89,0x74,0x24,0x38,                     // mov %r14,56(%rsp)
    0x4C,0x89,0x74,0x24,0x40,                     // mov %r14,64(%rsp)
    0x4C,0x89,0x74,0x24,0x48,                     // mov %r14,72(%rsp)
    0x4C,0x89,0x74,0x24,0x50,                     // mov %r14,80(%rsp)
    0x4C,0x89,0x74,0x24,0x58,                     // mov %r14,88(%rsp)
    0x4C,0x89,0x74,0x24,0x60,                     // mov %r14,96(%rsp)
    0x4C,0x89,0x74,0x24,0x68,                     // mov %r14,104(%rsp)
    0x4C,0x89,0x74,0x24,0x70,                     // mov %r14,112(%rsp)
    0x4C,0x89,0x74,0x24,0x78,                     // mov %r14,120(%rsp)
    0x41,0xBE,0x01,0x00,0x00,0x00,                // mov $1,%r14d
    0x4C,0x89,0xB4,0x24,0x80,0x00,0x00,0x00,      // mov %r14,128(%rsp)
    0x41,0xBE,0x02,0x00,0x00,0x00,                // mov $2,%r14d
    0x4C,0x89,0xB4,0x24,0x88,0x00,0x00,0x00,      // mov %r14,136(%rsp)
    0x41,0xBE,0x03,0x00,0x00,0x00,                // mov $3,%r14d
    0x4C,0x89,0xB4,0x24,0x90,0x00,0x00,0x00,      // mov %r14,144(%rsp)
    0x41,0xBE,0x04,0x00,0x00,0x00,                // mov $4,%r14d
    0x4C,0x8B,0xBC,0x24,0x80,0x00,0x00,0x00,      // mov 128(%rsp),%r15
    0xF2,0x44,0x0F,0x10,0xBC,0x24,0x88,0x00,0x00, // movsd 136(%rsp),%xmm15
      0x00,
    0xF2,0x45,0x0F,0x11,0x3F,                     // movsd %xmm15,(%r15)
    0x48,0x8B,0x8C,0x24,0x88,0x00,0x00,0x00,      // mov 136(%rsp),%rcx
    0x48,0x8B,0x94,0x24,0x90,0x00,0x00,0x00,      // mov 144(%rsp),%rdx
    0x48,0x8B,0x84,0x24,0x80,0x00,0x00,0x00,      // mov 128(%rsp),%rax
    0x44,0x0F,0x28,0xBC,0x24,0x30,0x01,0x00,0x00, // movaps 304(%rsp),%xmm15
    0x44,0x0F,0x28,0xB4,0x24,0x20,0x01,0x00,0x00, // movaps 288(%rsp),%xmm14
    0x44,0x0F,0x28,0xAC,0x24,0x10,0x01,0x00,0x00, // movaps 272(%rsp),%xmm13
    0x44,0x0F,0x28,0xA4,0x24,0x00,0x01,0x00,0x00, // movaps 256(%rsp),%xmm12
    0x44,0x0F,0x28,0x9C,0x24,0xF0,0x00,0x00,0x00, // movaps 240(%rsp),%xmm11
    0x44,0x0F,0x28,0x94,0x24,0xE0,0x00,0x00,0x00, // movaps 224(%rsp),%xmm10
    0x44,0x0F,0x28,0x8C,0x24,0xD0,0x00,0x00,0x00, // movaps 208(%rsp),%xmm9
    0x44,0x0F,0x28,0x84,0x24,0xC0,0x00,0x00,0x00, // movaps 192(%rsp),%xmm8
    0x0F,0x28,0xBC,0x24,0xB0,0x00,0x00,0x00,      // movaps 176(%rsp),%xmm7
    0x0F,0x28,0xB4,0x24,0xA0,0x00,0x00,0x00,      // movaps 160(%rsp),%xmm6
    0x48,0x81,0xC4,0x48,0x01,0x00,0x00,           // add $328,%rsp
    0x41,0x5F,                                    // pop %r15
    0x41,0x5E,                                    // pop %r14
    0x41,0x5D,                                    // pop %r13
    0x41,0x5C,                                    // pop %r12
    0x5F,                                         // pop %rdi
    0x5E,                                         // pop %rsi
    0x5D,                                         // pop %rbp
    0x5B,                                         // pop %rbx
    0xFF,0xE0,                                    // jmp *%rax
    0x44,0x0F,0x28,0xBC,0x24,0x30,0x01,0x00,0x00, // movaps 304(%rsp),%xmm15
    0x44,0x0F,0x28,0xB4,0x24,0x20,0x01,0x00,0x00, // movaps 288(%rsp),%xmm14
    0x44,0x0F,0x28,0xAC,0x24,0x10,0x01,0x00,0x00, // movaps 272(%rsp),%xmm13
    0x44,0x0F,0x28,0xA4,0x24,0x00,0x01,0x00,0x00, // movaps 256(%rsp),%xmm12
    0x44,0x0F,0x28,0x9C,0x24,0xF0,0x00,0x00,0x00, // movaps 240(%rsp),%xmm11
    0x44,0x0F,0x28,0x94,0x24,0xE0,0x00,0x00,0x00, // movaps 224(%rsp),%xmm10
    0x44,0x0F,0x28,0x8C,0x24,0xD0,0x00,0x00,0x00, // movaps 208(%rsp),%xmm9
    0x44,0x0F,0x28,0x84,0x24,0xC0,0x00,0x00,0x00, // movaps 192(%rsp),%xmm8
    0x0F,0x28,0xBC,0x24,0xB0,0x00,0x00,0x00,      // movaps 176(%rsp),%xmm7
    0x0F,0x28,0xB4,0x24,0xA0,0x00,0x00,0x00,      // movaps 160(%rsp),%xmm6
    0x48,0x81,0xC4,0x48,0x01,0x00,0x00,           // add $328,%rsp
    0x41,0x5F,                                    // pop %r15
    0x41,0x5E,                                    // pop %r14
    0x41,0x5D,                                    // pop %r13
    0x41,0x5C,                                    // pop %r12
    0x5F,                                         // pop %rdi
    0x5E,                                         // pop %rsi
    0x5D,                                         // pop %rbp
    0x5B,                                         // pop %rbx
    0xC3,                                         // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
