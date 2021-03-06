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
    /* Allocate enough dummy registers to use 128 bytes of stack space
     * and force subsequent spills to use 32-bit displacements. */
    int regs1[30];
    for (int i = 0; i < lenof(regs1); i++) {
        EXPECT(regs1[i] = rtl_alloc_register(unit, RTLTYPE_INT64));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, regs1[i], 0, 0, 0));
    }

    int label;
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(rtl_add_insn(unit, RTLOP_GOTO, 0, 0, 0, label));

    /* Generate enough jumped spills to trigger OOM, but leave at least
     * one unspilled register so we test both code paths. */
    int regs2[13];
    for (int i = 0; i < lenof(regs2); i++) {
        EXPECT(regs2[i] = rtl_alloc_register(unit, RTLTYPE_INT32));
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, regs2[i], 0, 0, 0));
    }
    for (int i = 0; i < lenof(regs2); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, regs2[i], 0, 0));
    }

    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));

    for (int i = 0; i < lenof(regs1); i++) {
        EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, regs1[i], 0, 0));
    }

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x53,                                    // push %rbx
    0x55,                                    // push %rbp
    0x41,0x54,                               // push %r12
    0x41,0x55,                               // push %r13
    0x41,0x56,                               // push %r14
    0x48,0x81,0xEC,0xF0,0x00,0x00,0x00,      // sub $240,%rsp
    0x4C,0x89,0x34,0x24,                     // mov %r14,(%rsp)
    0x4C,0x89,0x74,0x24,0x08,                // mov %r14,8(%rsp)
    0x4C,0x89,0x74,0x24,0x10,                // mov %r14,16(%rsp)
    0x4C,0x89,0x74,0x24,0x18,                // mov %r14,24(%rsp)
    0x4C,0x89,0x74,0x24,0x20,                // mov %r14,32(%rsp)
    0x4C,0x89,0x74,0x24,0x28,                // mov %r14,40(%rsp)
    0x4C,0x89,0x74,0x24,0x30,                // mov %r14,48(%rsp)
    0x4C,0x89,0x74,0x24,0x38,                // mov %r14,56(%rsp)
    0x4C,0x89,0x74,0x24,0x40,                // mov %r14,64(%rsp)
    0x4C,0x89,0x74,0x24,0x48,                // mov %r14,72(%rsp)
    0x4C,0x89,0x74,0x24,0x50,                // mov %r14,80(%rsp)
    0x4C,0x89,0x74,0x24,0x58,                // mov %r14,88(%rsp)
    0x4C,0x89,0x74,0x24,0x60,                // mov %r14,96(%rsp)
    0x4C,0x89,0x74,0x24,0x68,                // mov %r14,104(%rsp)
    0x4C,0x89,0x74,0x24,0x70,                // mov %r14,112(%rsp)
    0x4C,0x89,0x74,0x24,0x78,                // mov %r14,120(%rsp)
    0x48,0x89,0x8C,0x24,0xE0,0x00,0x00,0x00, // mov %rcx,224(%rsp)
    0x48,0x89,0x94,0x24,0xD8,0x00,0x00,0x00, // mov %rdx,216(%rsp)
    0x48,0x89,0x9C,0x24,0xA0,0x00,0x00,0x00, // mov %rbx,160(%rsp)
    0x48,0x89,0xAC,0x24,0x98,0x00,0x00,0x00, // mov %rbp,152(%rsp)
    0x48,0x89,0xB4,0x24,0xD0,0x00,0x00,0x00, // mov %rsi,208(%rsp)
    0x48,0x89,0xBC,0x24,0xC8,0x00,0x00,0x00, // mov %rdi,200(%rsp)
    0x4C,0x89,0x84,0x24,0xC0,0x00,0x00,0x00, // mov %r8,192(%rsp)
    0x4C,0x89,0x8C,0x24,0xB8,0x00,0x00,0x00, // mov %r9,184(%rsp)
    0x4C,0x89,0x94,0x24,0xB0,0x00,0x00,0x00, // mov %r10,176(%rsp)
    0x4C,0x89,0x9C,0x24,0xA8,0x00,0x00,0x00, // mov %r11,168(%rsp)
    0x4C,0x89,0xA4,0x24,0x90,0x00,0x00,0x00, // mov %r12,144(%rsp)
    0x4C,0x89,0xAC,0x24,0x88,0x00,0x00,0x00, // mov %r13,136(%rsp)
    0x4C,0x89,0xB4,0x24,0x80,0x00,0x00,0x00, // mov %r14,128(%rsp)
    0xE9,0x68,0x00,0x00,0x00,                // jmp L1
    0x4C,0x89,0xB4,0x24,0x80,0x00,0x00,0x00, // mov %r14,128(%rsp)
    0x4C,0x89,0xAC,0x24,0x88,0x00,0x00,0x00, // mov %r13,136(%rsp)
    0x4C,0x89,0xA4,0x24,0x90,0x00,0x00,0x00, // mov %r12,144(%rsp)
    0x48,0x89,0xAC,0x24,0x98,0x00,0x00,0x00, // mov %rbp,152(%rsp)
    0x48,0x89,0x9C,0x24,0xA0,0x00,0x00,0x00, // mov %rbx,160(%rsp)
    0x4C,0x89,0x9C,0x24,0xA8,0x00,0x00,0x00, // mov %r11,168(%rsp)
    0x4C,0x89,0x94,0x24,0xB0,0x00,0x00,0x00, // mov %r10,176(%rsp)
    0x4C,0x89,0x8C,0x24,0xB8,0x00,0x00,0x00, // mov %r9,184(%rsp)
    0x4C,0x89,0x84,0x24,0xC0,0x00,0x00,0x00, // mov %r8,192(%rsp)
    0x48,0x89,0xBC,0x24,0xC8,0x00,0x00,0x00, // mov %rdi,200(%rsp)
    0x48,0x89,0xB4,0x24,0xD0,0x00,0x00,0x00, // mov %rsi,208(%rsp)
    0x48,0x89,0x94,0x24,0xD8,0x00,0x00,0x00, // mov %rdx,216(%rsp)
    0x48,0x89,0x8C,0x24,0xE0,0x00,0x00,0x00, // mov %rcx,224(%rsp)
    0x48,0x81,0xC4,0xF0,0x00,0x00,0x00,      // L1: add $240,%rsp
    0x41,0x5E,                               // pop %r14
    0x41,0x5D,                               // pop %r13
    0x41,0x5C,                               // pop %r12
    0x5D,                                    // pop %rbp
    0x5B,                                    // pop %rbx
    0xC3,                                    // ret
};

static const char expected_log[] = "";


/* Tweaked version of tests/rtl-translate-test.i to trigger OOM. */

#include "tests/log-capture.h"

int main(void)
{
    binrec_setup_t final_setup = setup;
    final_setup.log = log_capture;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&final_setup));
    binrec_set_optimization_flags(handle, 0, 0, host_opt);

    RTLUnit *unit;
    EXPECT(unit = rtl_create_unit(handle));

    if (add_rtl(unit) != EXIT_SUCCESS) {
        const int line = __LINE__ - 1;
        const char *log_messages = get_log_messages();
        printf("%s:%d: add_rtl(unit) failed (%s)\n%s", __FILE__, line,
               log_messages ? "log follows" : "no errors logged",
               log_messages ? log_messages : "");
        return EXIT_FAILURE;
    }

    EXPECT(rtl_finalize_unit(unit));

    handle->code_buffer_size = 192;
    handle->code_alignment = 16;
    EXPECT(handle->code_buffer = binrec_code_malloc(
               handle, handle->code_buffer_size, handle->code_alignment));

    #ifndef EXPECT_TRANSLATE_FAILURE
        EXPECT(host_x86_translate(handle, unit));
        if (!(handle->code_len == sizeof(expected_code)
              && memcmp(handle->code_buffer, expected_code,
                        sizeof(expected_code)) == 0)) {
            const char *log_messages = get_log_messages();
            if (log_messages) {
                fputs(log_messages, stdout);
            }
            EXPECT_MEMEQ(handle->code_buffer, expected_code,
                         sizeof(expected_code));
            EXPECT_EQ(handle->code_len, sizeof(expected_code));
        }
    #else
        EXPECT_FALSE(host_x86_translate(handle, unit));
    #endif

    const char *log_messages = get_log_messages();
    EXPECT_STREQ(log_messages, *expected_log ? expected_log : NULL);

    binrec_code_free(handle, handle->code_buffer);
    handle->code_buffer = NULL;
    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);
    return EXIT_SUCCESS;
}
