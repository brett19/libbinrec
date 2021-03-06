/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

/*
 * This source file implements a test which passes an RTL instruction
 * sequence to an output translator and compares the generated output code
 * to an expected data buffer.  It is intended to be #included into a
 * source file which defines the parameters of the test.
 *
 * The test source file should define the following variables before
 * including this file:
 *
 * - static const binrec_setup_t setup
 *      Define this to a setup structure which will be passed to
 *      binrec_create_handle().
 *
 * - static const unsigned int host_opt
 *      Define this to the set of host optimization flags to enable.
 *
 * - bool add_rtl(RTLUnit *unit)
 *      This function will be called to fill the RTL unit with
 *      instructions before calling the translation function.  It should
 *      return EXIT_SUCCESS on success, EXIT_FAILURE if any errors occur.
 *
 * - static const uint8_t expected_code[]
 *      Define this to a buffer containing the expected translation output.
 *      If translation is expected to fail, #define EXPECT_TRANSLATE_FAILURE
 *      instead of defining this array.
 *
 * - static const char expected_log[]
 *      Define this to a buffer containing the expected log messages, if any.
 */

#include "src/common.h"
#include "src/host-x86.h"
#include "src/rtl.h"
#include "tests/common.h"
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

    if (!rtl_finalize_unit(unit)) {
        const char *log_messages = get_log_messages();
        if (log_messages) {
            fputs(log_messages, stdout);
        }
        FAIL("rtl_finalize_unit(unit) was not true as expected");
    }

    #ifdef EXPECT_TRANSLATE_FAILURE
        handle->code_buffer_size = 1;
    #else
        handle->code_buffer_size = sizeof(expected_code);
    #endif
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
