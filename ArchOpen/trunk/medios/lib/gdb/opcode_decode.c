/*
*   lib/arm_opcode.c
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*
* original code from Ben's Nintendo DS hacking gdb stub:
* http://masscat.afraid.org/ninds/debug_stub.php
*
*/

#include <kernel/kernel.h>

#include <lib/gdb.h>
#include <lib/opcode_decode.h>

#include <lib/string.h>

//#include "debug_utilities.h"


int
conditionCheck_opcode( uint32_t condition, uint32_t cpsr) {
  int executed_flag = 0;

  /* there are 16 condition values */
  switch ( condition) {
  case 0x0: /* Equal */
    if ( cpsr & CPSR_Z_FLAG)
      executed_flag = 1;
    break;

  case 0x1: /* Not Equal */
    if ( !(cpsr & CPSR_Z_FLAG))
      executed_flag = 1;
    break;

  case 0x2: /* Carry set.unsigned higher or same */
    if ( cpsr & CPSR_C_FLAG)
      executed_flag = 1;
    break;

  case 0x3: /* Carry clear/unsigned lower */
    if ( !(cpsr & CPSR_C_FLAG))
      executed_flag = 1;
    break;

  case 0x4: /* Minux/negative */
    if ( cpsr & CPSR_N_FLAG)
      executed_flag = 1;
    break;

  case 0x5: /* Plus/positive or zero  */
    if ( !(cpsr & CPSR_N_FLAG))
      executed_flag = 1;
    break;

  case 0x6: /* Overflow */
    if ( cpsr & CPSR_V_FLAG)
      executed_flag = 1;
    break;

  case 0x7: /* No overflow */
    if ( !(cpsr & CPSR_V_FLAG))
      executed_flag = 1;
    break;

  case 0x8: /* Unsigned higher */
    if ( cpsr & CPSR_C_FLAG && !(cpsr & CPSR_Z_FLAG))
      executed_flag = 1;
    break;

  case 0x9: /* Unsigned lower or same */
    if ( !(cpsr & CPSR_C_FLAG) && cpsr & CPSR_Z_FLAG)
      executed_flag = 1;
    break;

  case 0xa: /* Signed greater than or equal */
    if ( (cpsr & CPSR_N_FLAG && cpsr & CPSR_V_FLAG) ||
	 (!(cpsr & CPSR_N_FLAG) && !(cpsr & CPSR_V_FLAG)))
      executed_flag = 1;
    break;

  case 0xb: /* Signed less than */
    if ( (cpsr & CPSR_N_FLAG && !(cpsr & CPSR_V_FLAG)) ||
	 (!(cpsr & CPSR_N_FLAG) && cpsr & CPSR_V_FLAG))
      executed_flag = 1;
    break;

  case 0xc: /* Signed greater than */
    if ( !(cpsr & CPSR_Z_FLAG) &&
	 ((cpsr & CPSR_N_FLAG && cpsr & CPSR_V_FLAG) ||
	  (!(cpsr & CPSR_N_FLAG) && !(cpsr & CPSR_V_FLAG))))
      executed_flag = 1;
    break;

  case 0xd: /* Signed less than or equal */
    if ( cpsr & CPSR_Z_FLAG ||
	 ((cpsr & CPSR_N_FLAG && !(cpsr & CPSR_V_FLAG)) ||
	  (!(cpsr & CPSR_N_FLAG) && cpsr & CPSR_V_FLAG)))
      executed_flag = 1;
    break;

  case 0xe: /* Always */
  case 0xf: /* Extended */
  default:
    executed_flag = 1;
    break;
  }

  return executed_flag;
}



/** Determine if the instruction at the supplied address is executed.
 */
int
instructionExecuted_opcode( uint32_t instr_addr, int thumb_flag, uint32_t cpsr) {
  int executed_flag = 0;

  if ( thumb_flag) {
    /* instructions are not conditionally executed in thumb mode */
    executed_flag = 1;
  }
  else {
    uint32_t instr_cond = ((*(uint32_t *)instr_addr) & ARM_CONDITION_MASK) >> 28;

    executed_flag = conditionCheck_opcode( instr_cond, cpsr);
  }

  return executed_flag;
}




/** Determine if the instruction causes a branch, i.e. changes r15 the program counter.
 */
int
causeJump_opcode( uint32_t instr_addr, int *thumb_flag, const uint32_t *reg_set, uint32_t *dest_addr) {
  int branch_flag = 0;
  *dest_addr = instr_addr;

  DEBUG_GDB("causeJump? %08x, thumb %d\n", instr_addr, *thumb_flag);

  if ( *thumb_flag) {
    uint16_t op_code = *(uint16_t *)instr_addr;
    branch_flag = causeJump_thumb( op_code, thumb_flag, reg_set, dest_addr);
  }
  else {
    /* The arm conditions are assumed to have been checked else where */
    uint32_t op_code = *(uint32_t *)instr_addr;
    branch_flag = causeJump_arm( op_code, thumb_flag, reg_set, dest_addr);
  }

  return branch_flag;
}
