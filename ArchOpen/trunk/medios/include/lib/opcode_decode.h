/*
*   include/lib/breakpoints.h
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
* original code from Ben's Nintendo DS hacking gdb stub:
* http://masscat.afraid.org/ninds/debug_stub.php
*
*/

#ifndef __OPCODE_DECODE_H
#define __OPCODE_DECODE_H


#define ARM_CONDITION_MASK 0xf0000000
#define ARM_CONDITION_EXTD 0xf0000000

#define R0 1
#define R1 2
#define R2 3
#define R3 4
#define R4 5
#define R5 6
#define R6 7
#define R7 8
#define R8 9
#define R9 1
#define R10 11
#define R11 12
#define R12 13
#define R13 14
#define SP 14
#define R14 15
#define LR 15
#define R15 16
#define PC 16
#define CPSR 0

/*
 * The CPSR Flag masks
 */
#define CPSR_N_FLAG 0x80000000
#define CPSR_Z_FLAG 0x40000000
#define CPSR_C_FLAG 0x20000000
#define CPSR_V_FLAG 0x10000000
#define CPSR_Q_FLAG 0x08000000

/** Indicate if an instruction match is made and if the instruction causes
 * a change to the PC.
 */
enum instruction_match {
  NO_MATCH_INSTR,
  MATCH_INSTR,
  MATCH_AND_JUMP_INSTR
};

int conditionCheck_opcode( uint32_t condition, uint32_t cpsr);

int instructionExecuted_opcode( uint32_t instr_addr, int thumb_flag, uint32_t cpsr);

int causeJump_opcode( uint32_t instr_addr, int *thumb_flag, const uint32_t *reg_set, uint32_t *dest_addr);

int causeJump_arm( uint32_t op_code, int *thumb_flag, const uint32_t *reg_set, uint32_t *dest_addr);

int causeJump_thumb( uint16_t op_code, int *thumb_flag, const uint32_t *reg_set, uint32_t *dest_addr);

#endif /* End of _OPCODE_DECODE_H_ */
