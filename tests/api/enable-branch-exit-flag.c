/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "include/binrec.h"
#include "src/common.h"
#include "tests/common.h"
#include "tests/log-capture.h"
#include <limits.h>


int main(void)
{
    binrec_setup_t setup;
    memset(&setup, 0, sizeof(setup));
    setup.log = log_capture;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&setup));

    EXPECT_FALSE(handle->use_branch_exit_test);

    binrec_enable_branch_exit_test(handle, 1);
    EXPECT(handle->use_branch_exit_test);

    binrec_enable_branch_exit_test(handle, 0);
    EXPECT_FALSE(handle->use_branch_exit_test);

    binrec_enable_branch_exit_test(handle, INT_MIN);  // Low byte is zero.
    EXPECT(handle->use_branch_exit_test);

    EXPECT_STREQ(get_log_messages(), NULL);

    binrec_destroy_handle(handle);
    return EXIT_SUCCESS;
}
