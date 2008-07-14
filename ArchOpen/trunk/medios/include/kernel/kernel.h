/*
*   include/kernel/kernel.h
*
*   mediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __KERNEL_H
#define __KERNEL_H

#include <sys_def/section_types.h>
#include <sys_def/errors.h>
#include <sys_def/types.h>
#include <sys_def/arch.h>

#include <target/chip/arch/arch_def.h>

#include <lib/gdb.h>

#define SVC_STACK_SIZE 0x200

extern MED_RET_T errno;
MED_RET_T getErrno(void);

void printk(char *fmt, ...);
int printf(__const char *fmt, ...);
void print_data(char * data,int length);
void do_bkpt(void);

int getArch(void);
char * getArchName(void);
void getResolution(int * width,int * height);

unsigned int get_sp(void);
   
#define get_lr                          \
    ({                                   \
        register long _lr asm("lr");     \
        (long) _lr;               \
    })
       
void printk_init(void);
void printk_uartEnable(void);
void printk_uartDisable(void);

void str_swapChar(char * txt,int size);
void str_findEnd(char * txt,int size);

extern unsigned long tick;

extern unsigned int _end_kernel;
extern unsigned int _iram_start;
extern unsigned int _iram_end;
extern unsigned int _sdend_start;
extern unsigned int _sdend_end;

extern int has_develFct;

extern char * med_iramTop;
#define CORE_START   ((char*)&_iram_end+0x10)

int readCPUMode(void);

#define SWAB16(x) (x)
#define SWAB32(x) (x)

void kernel_doCmd(int cmd);
void kernel_cmdPipeFct(void);

#define CMD_HALT_DEVICE 1
#define CMD_ATA_SLEEP   2
#define CMD_LCD_ENABLE  3
#define CMD_LCD_DISABLE 4

#endif
