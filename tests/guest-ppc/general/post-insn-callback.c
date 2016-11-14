/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "src/endian.h"
#include "src/guest-ppc.h"
#include "src/rtl.h"
#include "src/rtl-internal.h"
#include "tests/common.h"
#include "tests/guest-ppc/common.h"


int main(void)
{
    uint8_t *memory;
    EXPECT(memory = malloc(0x10000));

    static const uint32_t ppc_code[] = {
        0x38600001,  // li r3,1
        0x2F830001,  // cmpi cr7,r3,1
    };
    const uint32_t start_address = 0x1000;
    memcpy_be32(memory + start_address, ppc_code, sizeof(ppc_code));

    binrec_setup_t setup;
    memset(&setup, 0, sizeof(setup));
    setup.guest_memory_base = memory;
    setup.state_offset_gpr = 0x100;
    setup.state_offset_fpr = 0x180;
    setup.state_offset_gqr = 0x380;
    setup.state_offset_cr = 0x3A0;
    setup.state_offset_lr = 0x3A4;
    setup.state_offset_ctr = 0x3A8;
    setup.state_offset_xer = 0x3AC;
    setup.state_offset_fpscr = 0x3B0;
    setup.state_offset_reserve_flag = 0x3B4;
    setup.state_offset_reserve_state = 0x3B8;
    setup.state_offset_nia = 0x3BC;
    setup.state_offset_timebase_handler = 0x3C8;
    setup.state_offset_sc_handler = 0x3D0;
    setup.state_offset_trap_handler = 0x3D8;
    setup.state_offset_branch_callback = 0x3E0;
    binrec_t *handle;
    EXPECT(handle = binrec_create_handle(&setup));

    binrec_set_post_insn_callback(handle, (void (*)(void *, uint32_t))2);

    RTLUnit *unit;
    EXPECT(unit = rtl_create_unit(handle));
    EXPECT(guest_ppc_translate(handle, 0x1000, 0x1007, unit));
    EXPECT_STREQ(rtl_disassemble_unit(unit, false),
                 "    0: LOAD_ARG   r1, 0\n"
                 "    1: LOAD_ARG   r2, 1\n"
                 "    2: GET_ALIAS  r3, a3\n"
                 "    3: BFEXT      r4, r3, 3, 1\n"
                 "    4: SET_ALIAS  a4, r4\n"
                 "    5: BFEXT      r5, r3, 2, 1\n"
                 "    6: SET_ALIAS  a5, r5\n"
                 "    7: BFEXT      r6, r3, 1, 1\n"
                 "    8: SET_ALIAS  a6, r6\n"
                 "    9: BFEXT      r7, r3, 0, 1\n"
                 "   10: SET_ALIAS  a7, r7\n"
                 "   11: LOAD_IMM   r8, 1\n"
                 "   12: SET_ALIAS  a2, r8\n"
                 "   13: LOAD_IMM   r9, 4100\n"
                 "   14: SET_ALIAS  a1, r9\n"
                 "   15: GET_ALIAS  r10, a3\n"
                 "   16: ANDI       r11, r10, -16\n"
                 "   17: GET_ALIAS  r12, a4\n"
                 "   18: SLLI       r13, r12, 3\n"
                 "   19: OR         r14, r11, r13\n"
                 "   20: GET_ALIAS  r15, a5\n"
                 "   21: SLLI       r16, r15, 2\n"
                 "   22: OR         r17, r14, r16\n"
                 "   23: GET_ALIAS  r18, a6\n"
                 "   24: SLLI       r19, r18, 1\n"
                 "   25: OR         r20, r17, r19\n"
                 "   26: GET_ALIAS  r21, a7\n"
                 "   27: OR         r22, r20, r21\n"
                 "   28: SET_ALIAS  a3, r22\n"
                 "   29: LOAD_IMM   r23, 0x2\n"
                 "   30: LOAD_IMM   r24, 4096\n"
                 "   31: CALL_TRANSPARENT @r23, r1, r24\n"
                 "   32: SLTSI      r25, r8, 1\n"
                 "   33: SGTSI      r26, r8, 1\n"
                 "   34: SEQI       r27, r8, 1\n"
                 "   35: GET_ALIAS  r28, a8\n"
                 "   36: BFEXT      r29, r28, 31, 1\n"
                 "   37: SET_ALIAS  a4, r25\n"
                 "   38: SET_ALIAS  a5, r26\n"
                 "   39: SET_ALIAS  a6, r27\n"
                 "   40: SET_ALIAS  a7, r29\n"
                 "   41: LOAD_IMM   r30, 4104\n"
                 "   42: SET_ALIAS  a1, r30\n"
                 "   43: GET_ALIAS  r31, a3\n"
                 "   44: ANDI       r32, r31, -16\n"
                 "   45: SLLI       r33, r25, 3\n"
                 "   46: OR         r34, r32, r33\n"
                 "   47: SLLI       r35, r26, 2\n"
                 "   48: OR         r36, r34, r35\n"
                 "   49: SLLI       r37, r27, 1\n"
                 "   50: OR         r38, r36, r37\n"
                 "   51: OR         r39, r38, r29\n"
                 "   52: SET_ALIAS  a3, r39\n"
                 "   53: LOAD_IMM   r40, 0x2\n"
                 "   54: LOAD_IMM   r41, 4100\n"
                 "   55: CALL_TRANSPARENT @r40, r1, r41\n"
                 "   56: LOAD_IMM   r42, 4104\n"
                 "   57: SET_ALIAS  a1, r42\n"
                 "   58: GET_ALIAS  r43, a3\n"
                 "   59: ANDI       r44, r43, -16\n"
                 "   60: GET_ALIAS  r45, a4\n"
                 "   61: SLLI       r46, r45, 3\n"
                 "   62: OR         r47, r44, r46\n"
                 "   63: GET_ALIAS  r48, a5\n"
                 "   64: SLLI       r49, r48, 2\n"
                 "   65: OR         r50, r47, r49\n"
                 "   66: GET_ALIAS  r51, a6\n"
                 "   67: SLLI       r52, r51, 1\n"
                 "   68: OR         r53, r50, r52\n"
                 "   69: GET_ALIAS  r54, a7\n"
                 "   70: OR         r55, r53, r54\n"
                 "   71: SET_ALIAS  a3, r55\n"
                 "   72: RETURN\n"
                 "\n"
                 "Alias 1: int32 @ 956(r1)\n"
                 "Alias 2: int32 @ 268(r1)\n"
                 "Alias 3: int32 @ 928(r1)\n"
                 "Alias 4: int32, no bound storage\n"
                 "Alias 5: int32, no bound storage\n"
                 "Alias 6: int32, no bound storage\n"
                 "Alias 7: int32, no bound storage\n"
                 "Alias 8: int32 @ 940(r1)\n"
                 "\n"
                 "Block 0: <none> --> [0,72] --> <none>\n"
                 );

    rtl_destroy_unit(unit);
    binrec_destroy_handle(handle);
    free(memory);
    return EXIT_SUCCESS;
}
