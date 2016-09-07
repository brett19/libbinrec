/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/rtl-internal.h"
#include "tests/common.h"


static const binrec_setup_t setup = {
    .host = BINREC_ARCH_X86_64_SYSV,
};

static bool add_rtl(RTLUnit *unit)
{
    EXPECT(rtl_add_insn(unit, RTLOP_NOP, 0, 0, 0, 0));
    return true;
}

static const uint8_t expected_code[] = {
    0x48,0x83,0xEC,0x08,            // sub $8,%rsp
    0x48,0x83,0xC4,0x08,            // add $8,%rsp
    0xC3,                           // ret
};

static const char expected_log[] = "";

#include "tests/rtl-translate-test.i"
