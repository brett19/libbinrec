/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/bitutils.h"
#include "tests/common.h"
#include <inttypes.h>


int main(int argc, char **argv)
{
    for (int i = 0; i < 64; i++) {
        const uint64_t x = UINT64_C(1) << i;
        const int result = ctz64(x);
        if (result != i) {
            FAIL("ctz64(0x%"PRIX64") was %d but should have been %d",
                 x, result, i);
        }
    }

    EXPECT_EQ(ctz64(0), 64);

    return EXIT_SUCCESS;
}
