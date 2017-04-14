/*
 * libbinrec: a recompiling translator for machine code
 * Copyright (c) 2016 Andrew Church <achurch@achurch.org>
 *
 * This software may be copied and redistributed under certain conditions;
 * see the file "COPYING" in the source code distribution for details.
 * NO WARRANTY is provided with this software.
 */

#include "tests/guest-ppc/insn/common.h"

static const uint8_t input[] = {
    0x10,0x22,0x20,0xDC,  // ps_madds0 f1,f2,f3,f4
};

static const unsigned int guest_opt = BINREC_OPT_G_PPC_NO_FPSCR_STATE
                                    | BINREC_OPT_G_PPC_FAST_FMADDS;
static const unsigned int common_opt = 0;

static const bool expected_success = true;

static const char expected[] =
    "[info] Scanning terminated at requested limit 0x3\n"
    "    0: LOAD_ARG   r1, 0\n"
    "    1: LOAD_ARG   r2, 1\n"
    "    2: GET_ALIAS  r3, a3\n"
    "    3: GET_ALIAS  r4, a4\n"
    "    4: GET_ALIAS  r5, a5\n"
    "    5: VEXTRACT   r6, r3, 0\n"
    "    6: VEXTRACT   r7, r3, 1\n"
    "    7: VEXTRACT   r8, r4, 0\n"
    "    8: VEXTRACT   r9, r5, 0\n"
    "    9: VEXTRACT   r10, r5, 1\n"
    "   10: SET_ALIAS  a6, r6\n"
    "   11: SET_ALIAS  a7, r8\n"
    "   12: BITCAST    r11, r8\n"
    "   13: BITCAST    r12, r6\n"
    "   14: BFEXT      r13, r11, 0, 52\n"
    "   15: GOTO_IF_Z  r13, L1\n"
    "   16: BFEXT      r14, r12, 52, 11\n"
    "   17: BFEXT      r15, r11, 52, 11\n"
    "   18: SEQI       r16, r14, 2047\n"
    "   19: GOTO_IF_NZ r16, L1\n"
    "   20: SEQI       r17, r15, 2047\n"
    "   21: GOTO_IF_NZ r17, L1\n"
    "   22: GOTO_IF_NZ r15, L2\n"
    "   23: CLZ        r18, r13\n"
    "   24: ADDI       r19, r18, -11\n"
    "   25: SUB        r20, r14, r19\n"
    "   26: SGTSI      r21, r20, 0\n"
    "   27: GOTO_IF_Z  r21, L1\n"
    "   28: LOAD_IMM   r22, 0x8000000000000000\n"
    "   29: AND        r23, r11, r22\n"
    "   30: SLL        r24, r13, r19\n"
    "   31: BFINS      r25, r12, r20, 52, 11\n"
    "   32: OR         r26, r23, r24\n"
    "   33: BITCAST    r27, r25\n"
    "   34: BITCAST    r28, r26\n"
    "   35: SET_ALIAS  a6, r27\n"
    "   36: SET_ALIAS  a7, r28\n"
    "   37: LABEL      L2\n"
    "   38: GET_ALIAS  r29, a7\n"
    "   39: BITCAST    r30, r29\n"
    "   40: ANDI       r31, r30, -134217728\n"
    "   41: ANDI       r32, r30, 134217728\n"
    "   42: ADD        r33, r31, r32\n"
    "   43: BITCAST    r34, r33\n"
    "   44: SET_ALIAS  a7, r34\n"
    "   45: BFEXT      r35, r33, 52, 11\n"
    "   46: SEQI       r36, r35, 2047\n"
    "   47: GOTO_IF_Z  r36, L1\n"
    "   48: LOAD_IMM   r37, 0x10000000000000\n"
    "   49: SUB        r38, r33, r37\n"
    "   50: BITCAST    r39, r38\n"
    "   51: SET_ALIAS  a7, r39\n"
    "   52: BFEXT      r40, r12, 52, 11\n"
    "   53: GOTO_IF_Z  r40, L3\n"
    "   54: SGTUI      r41, r40, 2045\n"
    "   55: GOTO_IF_NZ r41, L1\n"
    "   56: ADD        r42, r12, r37\n"
    "   57: BITCAST    r43, r42\n"
    "   58: SET_ALIAS  a6, r43\n"
    "   59: GOTO       L1\n"
    "   60: LABEL      L3\n"
    "   61: SLLI       r44, r12, 1\n"
    "   62: BFINS      r45, r12, r44, 0, 63\n"
    "   63: BITCAST    r46, r45\n"
    "   64: SET_ALIAS  a6, r46\n"
    "   65: LABEL      L1\n"
    "   66: GET_ALIAS  r47, a6\n"
    "   67: GET_ALIAS  r48, a7\n"
    "   68: SET_ALIAS  a6, r7\n"
    "   69: SET_ALIAS  a7, r8\n"
    "   70: BITCAST    r49, r8\n"
    "   71: BITCAST    r50, r7\n"
    "   72: BFEXT      r51, r49, 0, 52\n"
    "   73: GOTO_IF_Z  r51, L4\n"
    "   74: BFEXT      r52, r50, 52, 11\n"
    "   75: BFEXT      r53, r49, 52, 11\n"
    "   76: SEQI       r54, r52, 2047\n"
    "   77: GOTO_IF_NZ r54, L4\n"
    "   78: SEQI       r55, r53, 2047\n"
    "   79: GOTO_IF_NZ r55, L4\n"
    "   80: GOTO_IF_NZ r53, L5\n"
    "   81: CLZ        r56, r51\n"
    "   82: ADDI       r57, r56, -11\n"
    "   83: SUB        r58, r52, r57\n"
    "   84: SGTSI      r59, r58, 0\n"
    "   85: GOTO_IF_Z  r59, L4\n"
    "   86: LOAD_IMM   r60, 0x8000000000000000\n"
    "   87: AND        r61, r49, r60\n"
    "   88: SLL        r62, r51, r57\n"
    "   89: BFINS      r63, r50, r58, 52, 11\n"
    "   90: OR         r64, r61, r62\n"
    "   91: BITCAST    r65, r63\n"
    "   92: BITCAST    r66, r64\n"
    "   93: SET_ALIAS  a6, r65\n"
    "   94: SET_ALIAS  a7, r66\n"
    "   95: LABEL      L5\n"
    "   96: GET_ALIAS  r67, a7\n"
    "   97: BITCAST    r68, r67\n"
    "   98: ANDI       r69, r68, -134217728\n"
    "   99: ANDI       r70, r68, 134217728\n"
    "  100: ADD        r71, r69, r70\n"
    "  101: BITCAST    r72, r71\n"
    "  102: SET_ALIAS  a7, r72\n"
    "  103: BFEXT      r73, r71, 52, 11\n"
    "  104: SEQI       r74, r73, 2047\n"
    "  105: GOTO_IF_Z  r74, L4\n"
    "  106: LOAD_IMM   r75, 0x10000000000000\n"
    "  107: SUB        r76, r71, r75\n"
    "  108: BITCAST    r77, r76\n"
    "  109: SET_ALIAS  a7, r77\n"
    "  110: BFEXT      r78, r50, 52, 11\n"
    "  111: GOTO_IF_Z  r78, L6\n"
    "  112: SGTUI      r79, r78, 2045\n"
    "  113: GOTO_IF_NZ r79, L4\n"
    "  114: ADD        r80, r50, r75\n"
    "  115: BITCAST    r81, r80\n"
    "  116: SET_ALIAS  a6, r81\n"
    "  117: GOTO       L4\n"
    "  118: LABEL      L6\n"
    "  119: SLLI       r82, r50, 1\n"
    "  120: BFINS      r83, r50, r82, 0, 63\n"
    "  121: BITCAST    r84, r83\n"
    "  122: SET_ALIAS  a6, r84\n"
    "  123: LABEL      L4\n"
    "  124: GET_ALIAS  r85, a6\n"
    "  125: GET_ALIAS  r86, a7\n"
    "  126: FMADD      r87, r47, r48, r9\n"
    "  127: LOAD_IMM   r88, 0.0\n"
    "  128: FCMP       r89, r47, r88, NUN\n"
    "  129: FCMP       r90, r48, r88, UN\n"
    "  130: FCMP       r91, r9, r88, UN\n"
    "  131: AND        r92, r89, r90\n"
    "  132: AND        r93, r92, r91\n"
    "  133: BITCAST    r94, r9\n"
    "  134: LOAD_IMM   r95, 0x8000000000000\n"
    "  135: OR         r96, r94, r95\n"
    "  136: BITCAST    r97, r96\n"
    "  137: SELECT     r98, r97, r87, r93\n"
    "  138: FCVT       r99, r98\n"
    "  139: FMADD      r100, r85, r86, r10\n"
    "  140: LOAD_IMM   r101, 0.0\n"
    "  141: FCMP       r102, r85, r101, NUN\n"
    "  142: FCMP       r103, r86, r101, UN\n"
    "  143: FCMP       r104, r10, r101, UN\n"
    "  144: AND        r105, r102, r103\n"
    "  145: AND        r106, r105, r104\n"
    "  146: BITCAST    r107, r10\n"
    "  147: LOAD_IMM   r108, 0x8000000000000\n"
    "  148: OR         r109, r107, r108\n"
    "  149: BITCAST    r110, r109\n"
    "  150: SELECT     r111, r110, r100, r106\n"
    "  151: FCVT       r112, r111\n"
    "  152: VBUILD2    r113, r99, r112\n"
    "  153: VFCVT      r114, r113\n"
    "  154: SET_ALIAS  a2, r114\n"
    "  155: LOAD_IMM   r115, 4\n"
    "  156: SET_ALIAS  a1, r115\n"
    "  157: RETURN\n"
    "\n"
    "Alias 1: int32 @ 964(r1)\n"
    "Alias 2: float64[2] @ 400(r1)\n"
    "Alias 3: float64[2] @ 416(r1)\n"
    "Alias 4: float64[2] @ 432(r1)\n"
    "Alias 5: float64[2] @ 448(r1)\n"
    "Alias 6: float64, no bound storage\n"
    "Alias 7: float64, no bound storage\n"
    "\n"
    "Block 0: <none> --> [0,15] --> 1,11\n"
    "Block 1: 0 --> [16,19] --> 2,11\n"
    "Block 2: 1 --> [20,21] --> 3,11\n"
    "Block 3: 2 --> [22,22] --> 4,6\n"
    "Block 4: 3 --> [23,27] --> 5,11\n"
    "Block 5: 4 --> [28,36] --> 6\n"
    "Block 6: 5,3 --> [37,47] --> 7,11\n"
    "Block 7: 6 --> [48,53] --> 8,10\n"
    "Block 8: 7 --> [54,55] --> 9,11\n"
    "Block 9: 8 --> [56,59] --> 11\n"
    "Block 10: 7 --> [60,64] --> 11\n"
    "Block 11: 10,9,0,1,2,4,6,8 --> [65,73] --> 12,22\n"
    "Block 12: 11 --> [74,77] --> 13,22\n"
    "Block 13: 12 --> [78,79] --> 14,22\n"
    "Block 14: 13 --> [80,80] --> 15,17\n"
    "Block 15: 14 --> [81,85] --> 16,22\n"
    "Block 16: 15 --> [86,94] --> 17\n"
    "Block 17: 16,14 --> [95,105] --> 18,22\n"
    "Block 18: 17 --> [106,111] --> 19,21\n"
    "Block 19: 18 --> [112,113] --> 20,22\n"
    "Block 20: 19 --> [114,117] --> 22\n"
    "Block 21: 18 --> [118,122] --> 22\n"
    "Block 22: 21,20,11,12,13,15,17,19 --> [123,157] --> <none>\n"
    ;

#include "tests/rtl-disasm-test.i"
