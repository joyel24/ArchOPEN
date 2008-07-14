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

/* redefine PC/LR ... to have CPSR at the end and R0 at 0*/
#undef PC
#undef LR
#undef SP
#undef CPSR
#define PC 15
#define LR 14
#define SP 13
#define CPSR 16

#define C_FLAG(REGS) (REGS[CPSR]&(0x20000000))

#define ARM_I_BIT   0x02000000
static uint32_t arm_dataProcessing_operand(uint32_t opcode, int * Rn, const uint32_t * reg_set)
{
    uint32_t res=0;
    int j;
    if(Rn)
        *Rn=(opcode>>16)&0xFF;
    if(opcode & ARM_I_BIT) // 32-bit immediate
    {
        int immed_8 = opcode & 0xFF;
        int rotate_imm = (opcode >> 8) & 0xF;
        res = ((immed_8 << (32-rotate_imm*2)) | (immed_8 >> (rotate_imm*2))) ;
    }
    else
    {
        int Rm = opcode & 0xF;
        int shift = (opcode >> 5) & 0x3;
        uint32_t RmVal=reg_set[Rm];

        if(!(opcode&0x10)) // immediate shifts
        {
            int shift_imm = (opcode >> 7) & 0x1F;

            switch(shift)
            {
                case  0x0:                            // logical shift left
                    if(shift_imm == 0)
                        res = RmVal;
                    else
                        res=RmVal << shift_imm;
                    break;
                case  0x1:                            // logical shift right
                    if(shift_imm == 0)
                        res = 0;
                    else
                        res =  RmVal >> shift_imm;
                    break;
                case 0x2:                            // arithmetic shift right
                    if(shift_imm == 0)
                    {
                        if(((RmVal >> 31) & 0x1) == 0)
                            res=0;
                        else
                            res=0xFFFFFFFF;
                    }
                    else
                    {
                        if(RmVal > 0x7FFFFFFF) // neg number => keep neg sign
                        {
                            for (j = 0; j < shift_imm; j++)
                            {
                                    RmVal = (RmVal >> 1) | 0x80000000;
                            }
                            res = RmVal;
                        }
                        else
                            res = RmVal >> shift_imm;
                    }
                    break;
                case 0x3:                            // rotate right
                    if(shift_imm == 0) // with extend
                    {
                        res = RmVal >> 1;
                        if(C_FLAG(reg_set))
                            res = res | 0x80000000;
                    }
                    else // normal rotate
                        res = (RmVal << (32-shift_imm)) | (RmVal >> shift_imm);
                    break;
            }
        }
        else // register shifts
        {
            int Rs = (opcode >> 8) & 0xF;
            uint32_t RsVal=reg_set[Rs]& 0xff;
            if(RsVal == 0 && shift == 0) // no shift
                res = reg_set[Rm];
            else
            {
                RmVal=reg_set[Rm];
                switch(shift)
                {
                    case 0x0:                            // logical shift left
                        if(RsVal == 0)
                            res = RmVal;
                        else if(RsVal < 32)
                            res = RmVal << RsVal;
                        else if(RsVal == 32)
                            res = 0;
                        else
                            res = 0;
                        break;
                    case 0x1:                            // logical shift right
                        if(RsVal == 0)
                            res = RmVal;
                        else if(RsVal < 32)
                            res = RmVal >> RsVal;
                        else if(RsVal == 32)
                            res = 0;
                        else
                            res = 0;
                        break;
                    case 0x2:                            // arithmetic shift right
                        if(RsVal == 0)
                            res = RmVal;
                        else if(RsVal < 32)
                        {
                            if(RmVal > 0x7FFFFFFF) // neg number => keep neg sign
                            {
                                for (j = 0; j < RsVal; j++) {
                                    RmVal = (RmVal >> 1) | 0x80000000;
                                }
                                res = RmVal;
                            }
                            else
                                res = RmVal >> RsVal;
                        }
                        else if(RsVal >= 32)
                        {
                            if(((RmVal >> 31) & 0x1) == 0x0)
                                    res = 0x0;
                            else
                                    res = 0xFFFFFFFF;
                        }
                        break;
                    case 0x3:                            // rotate right
                        if(RsVal == 0)
                            res = RmVal;
                        else if((RsVal & 0xF) ==0)
                            res =(int) RmVal;
                        else
                            res = (RmVal << (32-(RsVal & 0xF))) | (RmVal >> (RsVal & 0xF));
                        break;
                }
            }
        }
    }
    return  res;
}


#define ARM_B_BL_MASK 0x0e000000
#define ARM_B_BL 0x0a000000
/** Determine the destination address for an B or BL branch instruction.
 */
static uint32_t
arm_B_BL_jump( uint32_t opcode, int *thumb_flag __attribute__((unused)), const uint32_t *reg_set) {
  uint32_t dest_addr;
  uint32_t change = opcode & 0xffffff;

  if ( change & 0x800000) {
    /* sign extend */
    change |= 0xff000000;
  }

  change <<= 2;
  dest_addr = reg_set[PC] + change;

  return dest_addr;
}

#define ARM_BLX1_MASK 0xfe000000
#define ARM_BLX1      0xfa000000
/** Determine the destination address for an BLX1 branch instruction.
 */
static uint32_t
arm_BLX1_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {
  uint32_t dest_addr;
  uint32_t change = opcode & 0xffffff;

  *thumb_flag = 1;

  if ( change & 0x800000) {
    /* sign extend */
    change |= 0xff000000;
  }

  change <<= 2;
  /* add the change value and the H bit value shifted */
  dest_addr = reg_set[PC] + change + ((opcode & 0x01000000) ? (1 << 1) : 0);

  return dest_addr;
}


#define ARM_BLX2_MASK 0x0ffffff0
#define ARM_BLX2      0x012fff30
#define ARM_BX_MASK   0x0ffffff0
#define ARM_BX        0x012fff10
/** Determine the destination address for an BLX1 branch instruction.
 */
static uint32_t
arm_BLX2_BX_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {
  uint32_t dest_addr;
  uint32_t addr_reg = opcode & 0xf;
  uint32_t step_addr = reg_set[addr_reg];

  /* set the thumb state of the destination */
  if ( step_addr & 0x1) {
    *thumb_flag = 1;
  }

  dest_addr = step_addr & 0xfffffffe;
  return dest_addr;
}


#define ARM_LDR_MASK 0x0c50f000
#define ARM_LDR      0x0410f000
#define ARM_LDR_Rn_MASK 0x000f0000
#define ARM_LDR_Rm_MASK 0x0000000f
#define ARM_LDR_I_BIT 0x02000000
#define ARM_LDR_P_BIT 0x01000000
#define ARM_LDR_U_BIT 0x00800000
#define ARM_LDR_OFFSET_MASK 0x00000fff
#define ARM_LDR_SHIFT_MASK 0x00000ff0
#define ARM_LDR_SHIFT_TYPE_MASK 0x00000060
#define ARM_LDR_SHIFT_IMM_MASK 0x00000f80
/** Determine the destination address for a LDR instruction.
 */
static uint32_t
arm_LDR_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {
  uint32_t dest_addr;
  uint32_t Rn_reg = (opcode & ARM_LDR_Rn_MASK) >> 16;
  uint32_t base_addr = reg_set[Rn_reg];

  if ( opcode & ARM_LDR_P_BIT) {
    if ( opcode & ARM_LDR_I_BIT) {
      uint32_t Rm_reg = (opcode & ARM_LDR_Rm_MASK);
      uint32_t Rm_value = reg_set[Rm_reg];

      if ( opcode & ARM_LDR_SHIFT_MASK) {
	uint32_t index = 0;
	uint32_t shift_type = (opcode & ARM_LDR_SHIFT_TYPE_MASK) >> 5;
	uint32_t shift_imm = (opcode & ARM_LDR_SHIFT_IMM_MASK) >> 7;

	switch ( shift_type) {
	case 0x0: /* LSL */
	  index = Rm_value << shift_imm;
	  break;

	case 0x1: /* LSR */
	  if ( shift_imm > 0) {
	    index = Rm_value >> shift_imm;
	  }
	  break;

	case 0x2: /* ASR */
	  if ( shift_imm == 0) {
	    if ( Rm_value & 0x80000000) {
	      index = 0xffffffff;
	    }
	  }
	  else {
	    int32_t temp = Rm_value;

	    temp >>= shift_imm;
	    index = temp;
	  }
	  break;

	case 0x3: /* ROR or RRX */
	  if ( shift_imm == 0) { /* RRX */
	    index = Rm_value >> 1;
	    if ( reg_set[CPSR] & CPSR_C_FLAG)
	      index |= 0x80000000;
	  }
	  else { /* ROR */
	    index = Rm_value;
	    while ( shift_imm > 0) {
	      uint32_t bit = index & 0x1;

	      index >>= 1;
	      index |= bit << 31;
	      shift_imm -= 1;
	    }
	  }
	  break;
	}

	if ( opcode & ARM_LDR_U_BIT) {
	  base_addr += index;
	}
	else {
	  base_addr -= index;
	}
      }
      else {
	/* no shifting */
	if ( opcode & ARM_LDR_U_BIT) {
	  base_addr += Rm_value;
	}
	else {
	  base_addr -= Rm_value;
	}
      }
    }
    else {
      /* immediate */
      if ( opcode & ARM_LDR_U_BIT) {
	base_addr += opcode & ARM_LDR_OFFSET_MASK;
      }
      else {
	base_addr -= opcode & ARM_LDR_OFFSET_MASK;
      }
    }
  }
  else {
    /* post-indexing - just use the register value */
  }

  dest_addr = base_addr;

  /* set the thumb state of the destination */
  if ( dest_addr & 0x1) {
    *thumb_flag = 1;
  }

  dest_addr = dest_addr & 0xfffffffe;
  return dest_addr;
}


#define ARM_LDM_MASK 0x0e108000
#define ARM_LDM      0x08108000
#define ARM_LDM_REG_LIST 0x0000ffff
#define ARM_LDM_P_BIT (1 << 24)
#define ARM_LDM_U_BIT (1 << 23)
#define ARM_LDM_S_BIT (1 << 22)
/** Determine the destination address for a LDM instruction.
 */
static uint32_t
arm_LDM_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {
  uint32_t dest_addr;
  uint32_t addr_reg = (opcode & 0x000f0000) >> 16;
  uint32_t pc_value_addr = reg_set[addr_reg];
  uint32_t reg_list = opcode & ARM_LDM_REG_LIST;
  int reg_count = 0;

  while ( reg_list) {
    if ( reg_list & 0x1)
      reg_count += 1;
    reg_list >>= 1;
  }


  if ( opcode & ARM_LDM_P_BIT) {
    if ( opcode & ARM_LDM_U_BIT) {
      /* Increment before */
      pc_value_addr += reg_count * 4;
    }
    else {
      /* Decrement before */
      pc_value_addr -= 4;
    }
  }
  else {
    if ( opcode & ARM_LDM_U_BIT) {
      /* Increment after */
      pc_value_addr += (reg_count * 4) - 4;
    }
    else {
      /* Decrement after - no change needed */
    }
  }

  dest_addr = *(uint32_t *)pc_value_addr;

  /* set the thumb state of the destination */
  if ( opcode & ARM_LDM_S_BIT) {
    /* FIXME: need access to the excepted mode's SPSR */
  }
  else {
    /* thumb mode indicated by state of bit 0 of destination */
    *thumb_flag = dest_addr & 0x1;
  }

  dest_addr = dest_addr & 0xfffffffe;
  return dest_addr;
}


#define ARM_ADD_MASK 0x0de0f000
#define ARM_ADD      0x0080f000
#define ARM_ADD_Rn_MASK 0x000f0000
#define ARM_ADD_I_BIT 0x02000000
#define ARM_ADD_S_BIT 0x00100000
/** Determine the destination address for an ADD instruction.
 */
static uint32_t
arm_ADD_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {
  //uint32_t Rn_reg = (opcode & ARM_ADD_Rn_MASK) >> 16;
  uint32_t dest_addr = reg_set[PC];

  return dest_addr;
}

#define ARM_MOV_MASK    0x0deff000
#define ARM_MOV         0x01A0f000
#define ARM_MOV_Rn_MASK 0x00000000
#define ARM_MOV_I_BIT   0x02000000
#define ARM_MOC_S_BIT   0x00100000
/** Determine the destination address for an ADD instruction.
 */
static uint32_t
arm_MOV_jump( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set) {

  uint32_t dest_addr=arm_dataProcessing_operand(opcode,NULL,reg_set);

  return dest_addr;
}


/** A ARM opcode jump check.
 */
struct arm_jump_check {
  /** the value of the masked bits */
  uint32_t opcode_value;

  /** the mask used to determine the instruction */
  uint32_t opcode_mask;

  /** function to match opcode and indicate if a jump is made */
  //enum instruction_match (*opcode_match_and_jump)( uint16_t opcode, const uint32_t *reg_set);

  /** function to compute jump and mode change */
  uint32_t (*compute_jump)( uint32_t opcode, int *thumb_flag, const uint32_t *reg_set);

  /** Str describing opcode */
  char * name;
};



/** The tables of ARM jump instructions.
 */
static struct arm_jump_check armJumpTestTable_cond[] = {
  { ARM_B_BL,       ARM_B_BL_MASK, arm_B_BL_jump, "B_BL"},
  { ARM_BLX2,       ARM_BLX2_MASK, arm_BLX2_BX_jump, "BLX2"},
  { ARM_BX,         ARM_BX_MASK, arm_BLX2_BX_jump, "BX"},
  { ARM_LDR,        ARM_LDR_MASK, arm_LDR_jump, "LDR"},
  { ARM_LDM,        ARM_LDM_MASK, arm_LDM_jump, "LDM"},
  { ARM_ADD,        ARM_ADD_MASK, arm_ADD_jump, "ADD"},
  { ARM_MOV,        ARM_MOV_MASK, arm_MOV_jump, "MOV"},
  //  { ARM_SUB,        ARM_SUB_MASK, arm_SUB_jump},
  { 0, 0, NULL, NULL}
};

static struct arm_jump_check armJumpTestTable_extd[] = {
  { ARM_BLX1,       ARM_BLX1_MASK, arm_BLX1_jump, "BLX1"},
  { 0, 0, NULL, NULL}
};


/** Determine if the ARM instruction causes a branch, i.e. changes r15 the program counter
 * and work out where it goes to.
 */
int
causeJump_arm( uint32_t op_code, int *thumb_flag, const uint32_t *reg_set, uint32_t *dest_addr) {
  int branch_flag = 0;
  const struct arm_jump_check *jump_table;
  int test_index;

  DEBUG_GDB( "ARM opcode %08x\n", op_code);

  if ( (op_code & ARM_CONDITION_MASK) != ARM_CONDITION_EXTD) {
    /* use the table for conditional instructions */
    jump_table = armJumpTestTable_cond;
  }
  else {
    /* use the table for extended instructions */
    jump_table = armJumpTestTable_extd;
  }

  /* go through the jump instruction table to see if this opcode does jump and,
   * if so, where it goes to.
   */
  for ( test_index = 0; jump_table[test_index].opcode_mask != 0 &&
	  !branch_flag; test_index++) {
    const struct arm_jump_check *jump_test = &jump_table[test_index];



    if ( (op_code & jump_test->opcode_mask) == jump_test->opcode_value) {
      branch_flag = 1;
      DEBUG_GDB("Matching op: %s,  %08x, mask %08x, val %08x res %08x\n",
            jump_test->name, op_code,
            jump_test->opcode_mask, jump_test->opcode_value,
            op_code & jump_test->opcode_mask);
      
      *dest_addr = jump_test->compute_jump( op_code, thumb_flag, reg_set);

      DEBUG_GDB("arm branch %08x (thumb %d) (%d)\n", *dest_addr, *thumb_flag,
	  test_index);
    }
  }

  return branch_flag;
}
