/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#define SUPPRESS_ENDIAN_INTRINSICS
#include "src/endian.h"
#include "tests/common.h"


int main(int argc, char **argv)
{
    static const union {uint8_t b[2]; uint16_t i;} value = {.b = {0x12, 0x34}};
    volatile uint16_t test = value.i;
    EXPECT_EQ(bswap_be16(test), 0x1234);
    return EXIT_SUCCESS;
}
