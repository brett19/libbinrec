/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#ifndef TESTS_GUEST_PPC_INSN_COMMON_H
#define TESTS_GUEST_PPC_INSN_COMMON_H

#include "include/binrec.h"
#include <stdbool.h>

static const binrec_setup_t setup = {
    .state_offset_gpr = 0x100,
    .state_offset_fpr = 0x180,
    .state_offset_gqr = 0x380,
    .state_offset_cr = 0x3A0,
    .state_offset_lr = 0x3A4,
    .state_offset_ctr = 0x3A8,
    .state_offset_xer = 0x3AC,
    .state_offset_fpscr = 0x3B0,
    .state_offset_reserve_flag = 0x3B4,
    .state_offset_reserve_address = 0x3B8,
    .state_offset_nia = 0x3BC,
    .state_offset_sc_handler = 0x3C0,
    .state_offset_trap_handler = 0x3C8,
};

#endif  // TESTS_GUEST_PPC_INSN_COMMON_H
