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
    EXPECT_EQ(unit->num_blocks, 0);

    EXPECT(rtl_block_add(unit));
    EXPECT(rtl_block_add(unit));
    EXPECT_EQ(unit->num_blocks, 2);

    EXPECT(rtl_block_add_edge(unit, 1, 0));
    EXPECT(rtl_block_add_edge(unit, 1, 0));
    EXPECT_EQ(unit->blocks[0].entries[0], 1);
    EXPECT_EQ(unit->blocks[0].entries[1], -1);  // No duplicate entry.
    EXPECT_EQ(unit->blocks[0].entry_overflow, -1);
    EXPECT_EQ(unit->blocks[0].exits[0], -1);
    EXPECT_EQ(unit->blocks[1].entries[0], -1);
    EXPECT_EQ(unit->blocks[1].entry_overflow, -1);
    EXPECT_EQ(unit->blocks[1].exits[0], 0);
    EXPECT_EQ(unit->blocks[1].exits[1], -1);  // No duplicate exit.

    EXPECT_FALSE(unit->error);
    EXPECT_STREQ(get_log_messages(), NULL);

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);
    return EXIT_SUCCESS;
}
