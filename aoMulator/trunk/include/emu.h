/*
*   emu.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef EMU_H
#define EMU_H


#include <hardware.h>
#include <emu_types.h>

#define STEP 0
#define RUN  1

extern int run_mode;
extern int HW_mode;
extern int disp_mode;
extern uint32_t old_PC;
void void_cmdline(void);
extern void (*cmd_line_fct)(void);
extern void cmd_line(void);

#define CHG_RUN_MODE(MODE) {         \
    run_mode = MODE;                 \
    if(run_mode==RUN)                \
        cmd_line_fct=void_cmdline;   \
    else                             \
        cmd_line_fct=cmd_line;       \
}

bool chkIrqFlag();
bool chkFiqFlag();

void void_irq_fiq(void);
void cpu_do_irq(void);
void cpu_do_fiq(void);

extern void (*cur_irq_fct)(void);

#define CPU_DEBUG       (0x1)      /*00000001*/
#define NULL_HW_DEBUG   (0x1<< 1)  /*00000002*/
#define CLOCK_HW_DEBUG  (0x1<< 2)  /*00000004*/
#define UART_HW_DEBUG   (0x1<< 3)  /*00000008*/
#define RTC_HW_DEBUG    (0x1<< 4)  /*00000010*/
#define TSC_HW_DEBUG    (0x1<< 5)  /*00000020*/
#define I2C1_HW_DEBUG   (0x1<< 6)  /*00000040*/
#define I2C2_HW_DEBUG   (0x1<< 7)  /*00000080*/
#define TMR_HW_DEBUG    (0x1<< 8)  /*00000100*/
#define GPIO_HW_DEBUG   (0x1<< 9)  /*00000200*/
#define MEM_HW_DEBUG    (0x1<< 10) /*00000400*/
#define MAS_HW_DEBUG    (0x1<< 11) /*00000800*/
#define CPLD_HW_DEBUG   (0x1<< 12) /*00001000*/
#define HW_30A24_DEBUG  (0x1<< 13) /*00002000*/
#define ATA_HW_DEBUG    (0x1<< 14) /*00004000*/
#define DMA_HW_DEBUG    (0x1<< 15) /*00008000*/
#define ECR_HW_DEBUG    (0x1<< 16) /*00010000*/
#define OSD_HW_DEBUG    (0x1<< 17) /*00020000*/
#define IRQ_HW_DEBUG    (0x1<< 18) /*00040000*/
#define CPLDBTN_HW_DEBUG (0x1<< 19)/*00080000*/
#define DVR_HW_DEBUG    (0x1<< 20) /*00100000*/
#define DSP_HW_DEBUG    (0x1<< 21) /*00200000*/
#define FLASH_HW_DEBUG  (0x1<< 22) /*00400000*/

#define DEBUG_MODE
#define DEBUG_MODE_HW

#ifdef DEBUG_MODE
#define DEBUG(fmt, arg...) if(disp_mode==1 || run_mode==STEP) printf(fmt,## arg) ;
#define MKDEBUG(dest,fmt, arg...) if(disp_mode==1 || run_mode==STEP) sprintf(dest,fmt,## arg);
#else
#define DEBUG(...)
#define MKDEBUG(...)
#endif

#ifdef DEBUG_MODE_HW
#define DEBUG_HW(level,fmt, arg...) if(HW_mode&level || run_mode==STEP) {printf("@%08x:",old_PC); printf(fmt,## arg); }
#else
#define DEBUG_HW(...)
#endif

#endif
