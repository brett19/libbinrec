/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/rtl.h"
#include "src/rtl-internal.h"
#include "tests/common.h"
#include "tests/log-capture.h"


int main(void)
{
    binrec_setup_t setup;
    memset(&setup, 0, sizeof(setup));
    setup.log = log_capture;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&setup));

    RTLUnit *unit;
    EXPECT(unit = rtl_create_unit(handle));

    EXPECT_EQ(rtl_alloc_register(unit, RTLTYPE_INT32), 1);

    rtl_make_unique_pointer(unit, 0);
    EXPECT_EQ(unit->regs[0].unique_pointer, 0);
    EXPECT_STREQ(get_log_messages(),
                 "[error] rtl_make_unique_pointer: Invalid register 0\n");
    clear_log_messages();

    EXPECT(unit->regs_size > 2);
    rtl_make_unique_pointer(unit, 2);
    EXPECT_EQ(unit->regs[2].unique_pointer, 0);
    EXPECT_STREQ(get_log_messages(),
                 "[error] rtl_make_unique_pointer: Invalid register 2\n");
    clear_log_messages();

    rtl_make_unique_pointer(unit, 1);
    EXPECT_EQ(unit->regs[1].unique_pointer, 0);
    EXPECT_STREQ(get_log_messages(),
                 "[error] rtl_make_unique_pointer: Register 1 has invalid"
                 " type int32 (must be address)\n");
    clear_log_messages();

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);
    return EXIT_SUCCESS;
}
