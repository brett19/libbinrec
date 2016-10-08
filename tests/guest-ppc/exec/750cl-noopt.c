/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "include/binrec.h"
#include "src/endian.h"
#include "tests/common.h"
#include "tests/execute.h"
#include "tests/guest-ppc/exec/common.h"
#include "tests/log-capture.h"

/*************************************************************************/
/*************************** PowerPC test code ***************************/
/*************************************************************************/

/* This file is automatically generated with the contents of the test code.
 * The test source (ppc750cl.s) and the script used to generate this file
 * (gen-ppc750cl-bin.pl) live in the etc/ppc directory. */
#include "tests/guest-ppc/exec/ppc750cl-bin.i"

/*************************************************************************/
/************************** Callback functions ***************************/
/*************************************************************************/

/* Handler for trap exceptions. */
static void trap_handler(PPCState *state)
{
    ASSERT(state);
    state->gpr[3] = 0;
    state->nia += 4;  // Continue with the next instruction.
}

/*-----------------------------------------------------------------------*/

static void sc_handler(PPCState *state)
{
    ASSERT(state);
    state->gpr[3] = 1;
}

/*************************************************************************/
/*************************** Test entry point ****************************/
/*************************************************************************/

int main(void)
{
    if (!binrec_host_supported(binrec_native_arch())) {
        printf("Skipping test because native architecture not supported.\n");
        return EXIT_SUCCESS;
    }

    const uint32_t memory_size = 0x2000000;
    const uint32_t start_address = 0x1000000;
    const uint32_t scratch_address = 0x100000;
    const uint32_t error_log_address = 0x200000;

    uint8_t *memory;
    EXPECT(memory = malloc(memory_size));
    memset(memory, 0, memory_size);

    memcpy(memory + start_address, ppc750cl_bin, sizeof(ppc750cl_bin));

    PPCState state;
    memset(&state, 0, sizeof(state));
    state.sc_handler = sc_handler;
    state.trap_handler = trap_handler;
    state.gpr[4] = scratch_address;
    state.gpr[5] = error_log_address;
    state.fpr[1][0] = 1.0;

    if (!call_guest_code(BINREC_ARCH_PPC_7XX, &state, memory, start_address,
                         0, 0, 0, 0, 0)) {
        const char *log_messages = get_log_messages();
        if (log_messages) {
            fputs(log_messages, stderr);
        }
        FAIL("Failed to execute guest code");
    }
    EXPECT_STREQ(get_log_messages(), NULL);

    int exitcode = EXIT_SUCCESS;
    const int expected_errors = 1078;
    const int result = state.gpr[3];
    if (result < 0) {
        exitcode = EXIT_FAILURE;
        printf("Test failed to bootstrap (error %d)\n", result);
    } else if (result != expected_errors) {
        exitcode = EXIT_FAILURE;
        printf("Wrong number of errors returned (expected %d, got %d)\n",
               expected_errors, result);
        printf("Error log follows:\n");
        const uint32_t *error_log =
            (const uint32_t *)(memory + error_log_address);
        for (int i = 0; i < result; i++, error_log += 8) {
            printf("    %08X %08X  %08X %08X  %08X %08X\n",
                   bswap_be32(error_log[0]), bswap_be32(error_log[1]),
                   bswap_be32(error_log[2]), bswap_be32(error_log[3]),
                   bswap_be32(error_log[4]), bswap_be32(error_log[5]));
        }
    }
    // FIXME: eventually record all expected errors to make sure they match

    free(memory);
    return exitcode;
}
