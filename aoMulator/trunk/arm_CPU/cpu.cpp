/*
*   cpu.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <emu.h>
#include <cpu.h>


#include <core/cmd_line.h>
#include <core/bkpt_list.h>

#include <HW/HW_TI.h>
#include <HW/mem_space.h>

//#include <gio_MAS_EOD.h>


extern mem_space * mem;

char * mode_str[] = {"User","System","Supervisor","Abort","Undefined","IRQ","FIQ"};

enum REGS {R_R0 = 0x00, R_R1 = 0x01, R_R2 = 0x02, R_R3 = 0x03,
       R_R4 = 0x04, R_R5 = 0x05, R_R6 = 0x06, R_R7 = 0x07,
       R_R8 = 0x08, R_R9 = 0x09, R_R10 = 0x0A, R_FP = 0x0B,
       R_IP = 0x0C, R_SP = 0x0D, R_LR = 0x0E, R_PC = 0x0F,
       R_CPSR = 0x10,R_SPSR = 0x11};

char * reg_str[] = {"R0", "R1", "R2", "R3",
                    "R4", "R5", "R6", "R7",
                    "R8", "R9", "R10", "R11",
                    "R12", "SP", "LR", "PC",
                    "CPSR", "SPSR"};

enum MODES {M_USER=0x00,M_SYS=0x01,M_SVC=0x02,M_ABT=0x03,M_UND=0x04,M_IRQ=0x05,M_FIQ=0x06};

int cpsr_tab[16] = { M_USER , M_FIQ, M_IRQ, M_SVC,
                     0, 0, 0, M_ABT,
                     0, 0, 0, M_UND,
                     0, 0, 0, M_SYS};

int mode_tab[7] = { 0x0, 0xF, 0x3, 0x7, 0xB, 0x2, 0x1};


#define MODE          (cpsr_tab[*mode_regs[M_USER][R_CPSR] & 0xF])
#define SET_MODE(val) \
  { \
    *mode_regs[M_USER][R_CPSR]=(*mode_regs[M_USER][R_CPSR] & 0xFFFFFFE0) | 0x10 | mode_tab[val]; \
    current_reg = mode_regs[MODE]; \
  }

#define GET_FLAG(mask) (*mode_regs[M_USER][R_CPSR] & (mask))
#define SET_FLAG(mask) {*mode_regs[M_USER][R_CPSR] = (*mode_regs[M_USER][R_CPSR] & ~(mask)) | (mask); current_reg = mode_regs[MODE];}
#define CLR_FLAG(mask) {*mode_regs[M_USER][R_CPSR] = (*mode_regs[M_USER][R_CPSR] & ~(mask)); }

#define T_MASK        (0x00000020)
#define FIQ_MASK      (0x00000040)
#define IRQ_MASK      (0x00000080)
#define T_FLAG        GET_FLAG(T_MASK)
#define FIQ_FLAG      GET_FLAG(FIQ_MASK)
#define IRQ_FLAG      GET_FLAG(IRQ_MASK)

#define Q_MASK        (0x08000000)
#define V_MASK        (0x10000000)
#define C_MASK        (0x20000000)
#define Z_MASK        (0x40000000)
#define N_MASK        (0x80000000)
#define Q_FLAG        GET_FLAG(Q_MASK)
#define V_FLAG        GET_FLAG(V_MASK)
#define C_FLAG        GET_FLAG(C_MASK)
#define Z_FLAG        GET_FLAG(Z_MASK)
#define N_FLAG        GET_FLAG(N_MASK)

#define SET_Q(COND)   {if(COND) SET_FLAG(Q_MASK) else CLR_FLAG(Q_MASK)}
#define SET_V(COND)   {if(COND) SET_FLAG(V_MASK) else CLR_FLAG(V_MASK)}
#define SET_C(COND)   {if(COND) SET_FLAG(C_MASK) else CLR_FLAG(C_MASK)}
#define SET_Z(COND)   {if(COND) SET_FLAG(Z_MASK) else CLR_FLAG(Z_MASK)}
#define SET_N(COND)   {if(COND) SET_FLAG(N_MASK) else CLR_FLAG(N_MASK)}

#define GET_REG(N)    ((N)==15?((*current_reg[N])&(T_FLAG?0xFFFFFFFE:0xFFFFFFFF))+(T_FLAG?2:4):*current_reg[N])
#define REG(N)        (*current_reg[N])

#define PC_REAL       (*current_reg[15])

#define RR(N)         (reg_str[N])

//#define MOD_PC  0x3F22B88

#ifdef MOD_PC
#define CALC_PC(X) (X-MOD_PC)
#else
#define CALC_PC(X) X
#endif


#define INT_DEBUG_HEAD   {printf("@%08x:%08x|%s%s%s%s%s|%s| ",CALC_PC(old_PC),instruction, \
            N_FLAG?"N":" ",Z_FLAG?"Z":" ",C_FLAG?"C":" ",V_FLAG?"V":" ",Q_FLAG?"Q":" ", \
            cond_str[condCode&0xF]);}
#define INT_DEBUG_HEAD_THUMB   {printf("@%08x:%04x|%s%s%s%s%s| ",CALC_PC(old_PC),instruction, \
            N_FLAG?"N":" ",Z_FLAG?"Z":" ",C_FLAG?"C":" ",V_FLAG?"V":" ",Q_FLAG?"Q":" ");}

#ifdef DEBUG_MODE
#define DEBUG_HEAD         if(run_mode==STEP || disp_mode==1) INT_DEBUG_HEAD
#define DEBUG_HEAD_THUMB   if(run_mode==STEP || disp_mode==1) INT_DEBUG_HEAD_THUMB
#else
#define DEBUG_HEAD
#define DEBUG_HEAD_THUMB
#endif

#define TST_BIT(V,N)   (((V)>>(N))&0x1)

#define CHG_MODE     {   int __old_mode=MODE;      \
                         REG(R_CPSR)=REG(R_SPSR);  \
                         CHK_T_FLAG_FCT            \
                         CHK_IRQ_FCT               \
                         CHK_FIQ_FCT               \
                         if(__old_mode != MODE)    \
                         { \
                            current_reg = mode_regs[MODE]; \
                         } \
                     }

int run_mode;
int HW_mode=0;
int disp_mode=0;

uint32_t old_PC;

char * cond_str[] = {"EQ","NE","CS","CC","MI","PL","VS","VC","HI","LS","GE","LT","GT","LE","  ","ERR"};

uint32_t ** current_reg;
uint32_t * mode_regs[7][18];
uint32_t regs_data[37];

uint32_t my_data;


void thumb_mode_fct(uint32_t address);
void arm_mode_fct(uint32_t address);

void (*t_flag_mode_fct)(uint32_t address)=arm_mode_fct;



#define CHK_T_FLAG_FCT {                                 \
    if(*mode_regs[M_USER][R_CPSR] & (0x00000020))        \
    {                                                    \
        t_flag_mode_fct = thumb_mode_fct;                \
    }                                                    \
    else                                                 \
    {                                                    \
        t_flag_mode_fct = arm_mode_fct;                  \
    }                                                    \
}

#define SET_T(COND) {                      \
    if(COND)                               \
    {                                      \
        t_flag_mode_fct = thumb_mode_fct;  \
        SET_FLAG(T_MASK)                   \
    }                                      \
    else                                   \
    {                                      \
        t_flag_mode_fct = arm_mode_fct;    \
        CLR_FLAG(T_MASK)                   \
    }                                      \
}


void void_irq_fiq(void) {};

void (*cur_irq_fct)(void) = void_irq_fiq;
void (*cur_fiq_fct)(void) = void_irq_fiq;

void (*cur_undef_fct)(void) = void_irq_fiq;

bool chkIrqFlag()
{
    return !IRQ_FLAG;
}

bool chkFiqFlag()
{
    return !FIQ_FLAG;
}

void cpu_do_irq(void);
void cpu_do_fiq(void);

#define CHK_IRQ_FCT                                      \
{                                                        \
    if(chkIrqFlag() && mem->hw_TI->HW_irq->have_int_IRQ)    \
        cur_irq_fct = cpu_do_irq;                        \
    else                                                 \
        cur_irq_fct = void_irq_fiq;                      \
}

#define CHK_FIQ_FCT                                      \
{                                                        \
    if(chkFiqFlag() && mem->hw_TI->HW_irq->have_int_FIQ)    \
        cur_irq_fct = cpu_do_fiq;                        \
    else                                                 \
        cur_irq_fct = void_irq_fiq;                      \
}

void cpu_do_irq(void)
{
    //printState();
    mem->hw_TI->HW_irq->have_int_IRQ = false;
    //printf("IRQ - return at %x\n",PC_REAL+4);
    *mode_regs[M_IRQ][R_LR]=PC_REAL+4;
    *mode_regs[M_IRQ][R_SPSR]=REG(R_CPSR);
    SET_MODE(M_IRQ);
    CLR_FLAG(T_MASK);
    CHK_T_FLAG_FCT
    SET_FLAG(IRQ_MASK);
    CHK_IRQ_FCT
    CHK_FIQ_FCT
    REG(R_PC)=0x18;
}

void cpu_do_fiq(void)
{
    mem->hw_TI->HW_irq->have_int_FIQ = false;
    //printf("FIQ - return at %x\n",PC_REAL+4);
    *mode_regs[M_FIQ][R_LR]=PC_REAL+4;
    *mode_regs[M_FIQ][R_SPSR]=REG(R_CPSR);
    SET_MODE(M_FIQ);
    CLR_FLAG(T_MASK);
    CHK_T_FLAG_FCT
    SET_FLAG(FIQ_MASK);
    SET_FLAG(IRQ_MASK);
    CHK_IRQ_FCT
    CHK_FIQ_FCT
    REG(R_PC)=0x1C;
}

void cpu_do_undef(void)
{
    //printState();
    SET_MODE(M_UND);
    CLR_FLAG(T_MASK);
    CHK_T_FLAG_FCT
    SET_FLAG(IRQ_MASK);
    CHK_IRQ_FCT
    CHK_FIQ_FCT
    REG(R_PC)=0x4;
    cur_undef_fct=void_irq_fiq;
}

BKPT_LIST * bkpt;

bool data_abort=false;

void init_cpu_static_fct(void);

void void_cmdline(void) {}

void (*cmd_line_fct)(void)=void_cmdline;

#if 0
bool checkCondition(int condCode)
{

    switch(condCode & 0xF)
    {
            case 0xE:                                       // AL
                    return true;
            case 0x0:                                       // EQ
                    return Z_FLAG;
            case 0x1:                                       // NE
                    return !Z_FLAG;
            case 0x2:                                       // CS/HS
                    return C_FLAG;
            case 0x3:                                       // CC/LO
                    return !C_FLAG;
            case 0x4:                                       // MI
                    return N_FLAG;
            case 0x5:                                       // PL
                    return !N_FLAG;
            case 0x6:                                       // VS
                    return V_FLAG;
            case 0x7:                                       // VC
                    return !V_FLAG;
            case 0x8:                                       // HI
                    return (C_FLAG && !Z_FLAG);
            case 0x9:                                       // LS
                    return (!C_FLAG || Z_FLAG);
            case 0xA:                                       // GE
                    return (N_FLAG == V_FLAG);
            case 0xB:                                       // LT
                    return (N_FLAG != V_FLAG);
            case 0xC:                                       // GT
                    return (!Z_FLAG && ((N_FLAG && V_FLAG) || (!N_FLAG && !V_FLAG)));
            case 0xD:                                       // LE
                    return (Z_FLAG || ((N_FLAG && !V_FLAG) || (!N_FLAG && V_FLAG)));
            case 0xF:                                       // Error
                    printf("Error cond code = 0xF (b1111)");
                    return false;
    }
    return false;
}
#else
/*   0    1    2    3    4    5    6    7    8    9    A    B    C    D    E    F
  0000 0001 0010 0011 0100 0101 0110 0111 1000 1001 1010 1011 1100 1101 1110 1111
  NZCV  */
int cond_tab[0x10][0x10] = {
/*cond / result| 0 1 2 3 4 5 6 7 8 9 A B C D E F */
/*0 - EQ*/     { 0,0,0,0,1,1,1,1,0,0,0,0,1,1,1,1},  /* Z set   */
/*1 - NE*/     { 1,1,1,1,0,0,0,0,1,1,1,1,0,0,0,0},  /* Z clear */
/*2 - CS/HS*/  { 0,0,1,1,0,0,1,1,0,0,1,1,0,0,1,1},  /* C set   */
/*3 - CC/LO*/  { 1,1,0,0,1,1,0,0,1,1,0,0,1,1,0,0},  /* C clear */
/*4 - MI*/     { 0,0,0,0,0,0,0,0,1,1,1,1,1,1,1,1},  /* N set   */
/*5 - PL*/     { 1,1,1,1,1,1,1,1,0,0,0,0,0,0,0,0},  /* N clear */
/*6 - VS*/     { 0,1,0,1,0,1,0,1,0,1,0,1,0,1,0,1},  /* V set   */
/*7 - VC*/     { 1,0,1,0,1,0,1,0,1,0,1,0,1,0,1,0},  /* V clear */
/*8 - HI*/     { 0,0,1,1,0,0,0,0,0,0,1,1,0,0,0,0},  /* (C_FLAG && !Z_FLAG) */
/*9 - LS*/     { 1,1,0,0,1,1,1,1,1,1,0,0,1,1,1,1},  /* (!C_FLAG || Z_FLAG) */
/*A - GE*/     { 1,0,1,0,1,0,1,0,0,1,0,1,0,1,0,1},  /* (N_FLAG == V_FLAG)  */
/*B - LT*/     { 0,1,0,1,0,1,0,1,1,0,1,0,1,0,1,0},  /* (N_FLAG != V_FLAG)  */
/*C - GT*/     { 1,0,1,0,0,0,0,0,0,1,0,1,0,0,0,0},  /* (!Z_FLAG && ((N_FLAG && V_FLAG) || (!N_FLAG && !V_FLAG))) */
/*D - LE*/     { 0,1,0,1,1,1,1,1,1,0,1,0,1,1,1,1},  /* (Z_FLAG || ((N_FLAG && !V_FLAG) || (!N_FLAG && V_FLAG)))  */
/*E - AL*/     { 1,1,1,1,1,1,1,1,1,1,1,1,1,1,1,1},
/*F - UNK*/    { 0,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0}
};
#define checkCondition(VAL) (cond_tab[VAL&0xF][((*mode_regs[M_USER][R_CPSR])>>28)&0xF])
#endif


#include "thumb_data_processing.h"
#include "thumb_load_store.h"
#include "thumb_load_store_multi.h"
#include "thumb_misc.h"

struct fct_data {
    int blank_size;
    int mask;
    void (*fct)(uint32_t instruction);
};

void (*thumb_fct[1024])(uint32_t ) ;

#include "thumb_fct_ini_tab.h"

void ini_thumb_fct(void)
{
    int i,j,k;
    /* clearing the whole fct array */
    for(i=0;i<1024;i++)
        thumb_fct[i] = NULL;

    /* init array using fct_ini_tab */
    for(i=0;fct_ini_tab[i].fct!=NULL;i++)
    {
        j=0x1<<fct_ini_tab[i].blank_size;
        for(k=0;k<j;k++)
        {
            /*if(thumb_fct[fct_ini_tab[i].mask|k])
                printf("error thumb_fct[%x] already defined (cur %x)\n",fct_ini_tab[i].mask|k,i);
            else*/
                thumb_fct[fct_ini_tab[i].mask|k] = fct_ini_tab[i].fct;
        }
    }

    /*Let's see if we've forgot something*/
    k=0;
    for(j=0;j<1024;j++)
        if(!thumb_fct[j])
        {
            k++;
            printf("%04d|error thumb_fct[%x] not defined\n",k,j);
        }
    printf("[ini_thumb fct] Processed %04d fct, we've missed : %04d cells in thumb_fct\n",i,k);
}



void thumb_mode_fct(uint32_t address)
{
    uint32_t instruction = mem->read(address,2);
    PC_REAL+=2;
    DEBUG_HEAD_THUMB;
    thumb_fct[(instruction>>6)&0x3FF](instruction);
    if(disp_mode==1 || run_mode==STEP)
        printState();
}

void doThumb(uint32_t instruction)
{
    thumb_fct[(instruction>>6)&0x3FF](instruction);
}

void arm_mode_fct(uint32_t address)
{
    PC_REAL+= 4;
    doARM(mem->read(address,4));
}

void init_cpu(void)
{
    int i,j;

    /*init regs data*/
    for(i=0;i<38;i++)
        regs_data[i]=0;


    /*ini the same regs for all modes */
    for(i=0;i<17;i++)
        for(j=0;j<7;j++)
            mode_regs[j][i] = &regs_data[i];

    /* SVC mode */
    mode_regs[M_SVC][R_SP] =  &regs_data[17];
    mode_regs[M_SVC][R_LR] =  &regs_data[18];
    mode_regs[M_SVC][R_SPSR] = &regs_data[19];

    /* ABT mode */
    mode_regs[M_ABT][R_SP] =  &regs_data[20];
    mode_regs[M_ABT][R_LR] =  &regs_data[21];
    mode_regs[M_ABT][R_SPSR] = &regs_data[22];

    /* UND mode */
    mode_regs[M_UND][R_SP] =  &regs_data[23];
    mode_regs[M_UND][R_LR] =  &regs_data[24];
    mode_regs[M_UND][R_SPSR] = &regs_data[25];

    /* IRQ mode */
    mode_regs[M_IRQ][R_SP] =  &regs_data[26];
    mode_regs[M_IRQ][R_LR] =  &regs_data[27];
    mode_regs[M_IRQ][R_SPSR] = &regs_data[28];

    /* FIQ mode */
    for(i=0;i<7;i++)
        mode_regs[M_FIQ][R_R8+i] = &regs_data[29+i];
    mode_regs[M_FIQ][R_SPSR] = &regs_data[36];

    SET_FLAG(IRQ_MASK);
    SET_FLAG(FIQ_MASK);
    CHK_IRQ_FCT
    CHK_FIQ_FCT
    SET_MODE(INIT_MODE); //M_SYS

    /* init the cmd line */

    init_cpu_static_fct();

    /* init bkpt_list */

    bkpt= new_bkpt_list(BKPT_CPU);


    ini_thumb_fct();

    CHK_T_FLAG_FCT

    printf("Init of Cpu object      DONE\n");

}

uint32_t read_reg(int num)
{
    return GET_REG(num);
}

void sigint(void)
{
    CHG_RUN_MODE(STEP)
}

#include "cpu_cmd_line_fct.h"

void go(uint32_t start_address,uint32_t stack_address)
{
    REG(R_PC)=start_address;
    REG(R_SP)=stack_address;

    CHG_RUN_MODE(STEP)
    //CHG_RUN_MODE(RUN)
    uint32_t address;

    while(1)
    {

        #if 0
        if(*current_reg[R_SP]<0x70FC || *current_reg[R_SP]> 0x8000)
        {
            printf("Stack out @%x SP=%x\n",PC_REAL,*current_reg[13]);
        }
        #endif
        mem->hw_TI->nxtStep();

        /*
        ->uart_list[0]->nxtEvent();
        mem->hw_TI->uart_list[1]->nxtEvent();
        mem->hw_TI->osd->nxtEvent();

       // ((gio_MAS_EOD*)mem->hw_TI->gpio->port_list[0x4])->chkEOD();

        for(int i=0;i<4;i++)
            mem->hw_TI->timer_list[i]->nxt_cycle();
        mem->hw_TI->hw_wdt->nxt_cycle();*/

        #if 0
        if(data_abort)
        {
            data_abort = false;
            *mode_regs[M_ABT][R_LR]=PC_REAL+8;
            *mode_regs[M_ABT][R_SPSR]=REG(R_CPSR);
            SET_MODE(M_ABT);
            CLR_FLAG(T_MASK);
            CHK_T_FLAG_FCT
            SET_FLAG(IRQ_MASK);
            CHK_IRQ_FCT
            CHK_FIQ_FCT
            REG(R_PC)=0x10;
            //printf("WARNING DATA ABT\n");
            CHG_RUN_MODE(STEP)
        }
        #endif
        
        cur_undef_fct();
        cur_fiq_fct();
        cur_irq_fct();

        address = T_FLAG ? PC_REAL&0xfffffffe : (PC_REAL+2)&0xfffffffc;

        bkpt->fct(bkpt,address,0);
        cmd_line_fct();
        bkpt_step->fct(bkpt_step,address,0);

        old_PC=PC_REAL;
        t_flag_mode_fct(address);
    }
}




void doARM(uint32_t instruction)
{
    int condCode = (instruction >> 28) & 0xf;
    int instr_num = (instruction >> 25) & 0x7;

    DEBUG_HEAD;

    switch(instr_num)
    {
        case 0x0:
            if((instruction>>4)&0x1)
            {                           /* BIT 4 == 1 */

                if((instruction>>7)&0x1)
                {                           /* BIT 7 == 1 */
                    if(((instruction>>5)&0x1)==0 && ((instruction>>6)&0x1)==0)
                    {                           /* Multiply / swap */
                        if(((instruction>>24)&0x1)==0)
                        {                           /* BIT 24 == 1 */ /* Multiply */
                            arm_Mul(condCode,instruction);
                        }
                        else
                        {                           /* BIT 24 == 0 */ /* Swap */
                            if(checkCondition(condCode))
                                arm_Swap(instruction);
                            else
                                DEBUG("SWP CC not met\n");
                        }
                    }                            /* Multiply / swap */
                    else
                    {                            /* extra Load/Store */
                        arm_LoadStore(condCode,instr_num,instruction);
                    }                            /* extra Load/Store */

                }                           /* BIT 7 == 1 */
                else
                {                           /* BIT 7 == 0 */ /* Multiplies extra Load/Store */
                    if((instruction & 0x01900000) == 0x01000000) /* MISC instructions */
                    {
                        switch((instruction>>5)&0x3)
                        {
                            case 0:
                                if((instruction>>22)&0x1)
                                {                           /* BIT 22 == 1 */ /* CLZ */
                                    INT_DEBUG_HEAD
                                    printf("CLZ : Undef instruction\n");
                                    CHG_RUN_MODE(STEP)
                                    
                                }
                                else
                                {                           /* BIT 22 == 0 */ /* BX */
                                    if(checkCondition(condCode))
                                    {
                                        DEBUG("BX ");

                                        int Rm = instruction & 0xF;

                                        if(GET_REG(Rm) & 0x1)
                                        {
                                            SET_FLAG(T_MASK);
                                            CHK_T_FLAG_FCT
                                            DEBUG("Thumb - ");
                                        }
                                        else
                                        {
                                            CLR_FLAG(T_MASK);
                                            CHK_T_FLAG_FCT
                                            DEBUG("ARM - ");
                                        }
                                        REG(R_PC) = GET_REG(Rm) & 0xFFFFFFE;
                                        DEBUG("=> %08x\n",GET_REG(R_PC));
                                    }
                                    else
                                    {
                                        DEBUG("BX CC not met\n");
                                    }
                                }
                                break;
                            case 1:
                                INT_DEBUG_HEAD
                                printf("BLX: Undef instruction\n");
                                CHG_RUN_MODE(STEP)
                                break;
                            case 2:
                                arm_DSP(condCode,instruction);
                                break;
                            case 3:
                                INT_DEBUG_HEAD
                                printf("BKPT: instruction\n");
                                CHG_RUN_MODE(STEP)
                                //exit(0);
                                break;
                        }
                    }
                    else                                         /* data processing register shift*/
                        arm_DataProcessing(condCode,instruction);
                }                           /* BIT 7 == 0 */ /* Multiplies extra Load/Store */
            }                           /* BIT 4 == 1 */
            else
            {                           /* BIT 4 == 0 */
                if((instruction & 0x01900000) == 0x01000000) /* MISC instructions */
                {
                    if((instruction>>7)&0x1)    /* BIT 7 == 1 */ /* DSP mul */
                        arm_DSP(condCode,instruction);
                    else                        /* BIT 7 == 0 */ /* MSR/MRS */
                        arm_MSR_MRS(condCode,instr_num,instruction);
                }
                else                                         /* data processing immediate shift*/
                    arm_DataProcessing(condCode,instruction);
            }                           /* BIT 4 == 0 */
            break;
        case 0x1:
            if((instruction & 0x01900000) == 0x01000000)
                if((instruction>>21)&0x1)   /* Move immediate to status reg */
                    arm_MSR_MRS(condCode,instr_num,instruction);
                else
                {
                    INT_DEBUG_HEAD
                    printf("Undef instruction (1) %x\n",instruction);
                    CHG_RUN_MODE(STEP)
                }
            else   /* data processing immediate */
                arm_DataProcessing(condCode,instruction);
            break;
        case 0x3:
            if(condCode == 0xf)
            {
                INT_DEBUG_HEAD
                printf("Undef instruction (2) %x\n",instruction);
                CHG_RUN_MODE(STEP)
            }
        case 0x2:
            arm_LoadStore(condCode,instr_num,instruction);
            break;
        case 0x4:
            if(checkCondition(condCode))
                arm_LoadStoreMulti(instruction);
            else
                DEBUG("Load/Store multi CC not met\n");                
            break;
        case 0x5:
            if(checkCondition(condCode))       /* B, BL */
            {
                if ((instruction>>24)&0x1)
                {
                    REG(R_LR) = PC_REAL;
                    DEBUG("BL => ");
                }
                else
                    DEBUG("B => ");

                uint32_t offset = (instruction & 0xffffff);

                if (offset > 0x7fffff)
                    offset = offset - 0x1000000 ;

                REG(R_PC) = GET_REG(R_PC) + offset *4;

                DEBUG("%08x\n",PC_REAL);
            }
            else
            {
                DEBUG("B/BL CC not met\n");
            }
            break;
        case 0x6:
            if(checkCondition(condCode))
                arm_CoProcessor(condCode,instruction);
            else
                DEBUG("CoProcessor CC not met\n");
            break;
        case 0x7:
            if(((instruction>>24)&0x1)==0)
            {
                if(checkCondition(condCode))
                    arm_CoProcessor(condCode,instruction);
                else
                    DEBUG("CoProcessor CC not met\n");
            }
            else
            {
                if(checkCondition(condCode))
                {
                    DEBUG("SWI :%08x\n",instruction & 0x00FFFFFF);
                    *mode_regs[M_SVC][R_LR]=PC_REAL;
                    *mode_regs[M_SVC][R_SPSR]=REG(R_CPSR);
                    SET_MODE(M_SVC);
                    CLR_FLAG(T_MASK);
                    CHK_T_FLAG_FCT
                    SET_FLAG(IRQ_MASK);
                    REG(R_PC)=0x8;
                    CHK_IRQ_FCT
                    CHK_FIQ_FCT
                }
                else
                    DEBUG("SWI CC not met\n");
            }
            break;
         default:
             printf("You should not be here\n");
             CHG_RUN_MODE(STEP)
    }
    if(disp_mode==1 || run_mode==STEP)
        printState();
}

/* special ops */

#include "cpu_ops.h"

/* load store */

#include "data_processing.h"

/* load store */

#include "cpu_load_store.h"

/* load store MULTI */

#include "cpu_load_store_multi.h"

/* multiply */

#include "cpu_multiply.h"

/* misc ARM functions */

#include "arm_misc.h"


void printState(void)
{
    int i=0;
    printf("Mode : %s (%d) - PC:%08x, LR:%08x, SP:%08x\n",mode_str[MODE],MODE,
            GET_REG(R_PC),
            GET_REG(R_LR),
            GET_REG(R_SP));
    for(i=0;i<5;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");

    for(i;i<9;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");
    for(i;i<13;i++)
        printf("R%d:%08x ",i,GET_REG(i));
    printf("\n");

    printf("CPSR: %08x\n",GET_REG(R_CPSR));

    if(MODE != M_USER && MODE != M_SYS)
        printf("SPSR: %08x\n",GET_REG(R_SPSR));

}
