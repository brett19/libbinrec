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
static const unsigned int host_opt = BINREC_OPT_H_X86_CONDITION_CODES;

static int add_rtl(RTLUnit *unit)
{
    int reg1, reg2, label;
    EXPECT(reg1 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_LOAD_IMM, reg1, 0, 0, 1));
    EXPECT(reg2 = rtl_alloc_register(unit, RTLTYPE_INT32));
    EXPECT(rtl_add_insn(unit, RTLOP_SEQI, reg2, reg1, 0, 0));
    EXPECT(label = rtl_alloc_label(unit));
    EXPECT(rtl_add_insn(unit, RTLOP_GOTO_IF_Z, 0, reg1, 0, label));
    EXPECT(rtl_add_insn(unit, RTLOP_LABEL, 0, 0, 0, label));

    return EXIT_SUCCESS;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,                // sub $8,%rsp
    0xB8,0x01,0x00,0x00,0x00,           // mov $1,%eax
    0x33,0xC9,                          // xor %ecx,%ecx
    0x85,0xC0,                          // test %eax,%eax
    0x0F,0x94,0xC1,                     // setz %cl
    0x0F,0x84,0x00,0x00,0x00,0x00,      // jz L1
    0x48,0x83,0xC4,0x08,                // L1: add $8,%rsp
    0xC3,                               // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
