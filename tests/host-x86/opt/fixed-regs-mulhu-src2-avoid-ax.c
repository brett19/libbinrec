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
static const unsigned int host_opt = BINREC_OPT_H_X86_FIXED_REGS;

static int add_rtl(RTLUnit *unit)
{
    int reg1, reg2, reg3, reg4, reg5, reg6;

    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_ADDRESS));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 1));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg2, 0, 0, 2));
    EXPECT(reg3 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg3, 0, 0, 3));
    EXPECT(reg4 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_CMPXCHG, reg4, reg1, reg2, reg3));
    /* Extend reg3's live range so it doesn't claim EDX and block reg4
     * from its fixed EDX allocation due to the CMPXCHG rules. */
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, reg3, 0, 0));

    EXPECT(reg5 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg5, 0, 0, 5));
    EXPECT(reg6 = rtl_alloc_register(unit, RTLTYPE_INT32));
    /* reg4 should not get EAX since it's also a CMPXCHG output. */
    EXPECT(rtl_add_insn(unit, RTLOP_MULHU, reg6, reg5, reg4, 0));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x41,0x57,                          // push %r15
    0xB9,0x01,0x00,0x00,0x00,           // mov $1,%ecx
    0xB8,0x02,0x00,0x00,0x00,           // mov $2,%eax
    0xBE,0x03,0x00,0x00,0x00,           // mov $3,%esi
    0xF0,0x0F,0xB1,0x31,                // lock cmpxchg %esi,(%rcx)
    0x8B,0xD0,                          // mov %eax,%edx
    0xB8,0x05,0x00,0x00,0x00,           // mov $5,%eax
    0xF7,0xE2,                          // mul %edx
    0x41,0x5F,                          // pop %r15
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
