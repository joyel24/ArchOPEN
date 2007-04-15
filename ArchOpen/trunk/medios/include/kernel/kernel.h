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

#define SVC_STACK_SIZE 0x200

void printk(char *fmt, ...);
int printf(__const char *fmt, ...);
void print_data(char * data,int length);
void do_bkpt(void);

int getArch(void);
void getResolution(int * width,int * height);

unsigned int get_sp(void);

void printk_init(void);
void printk_uartEnable(void);
void printk_uartDisable(void);

extern unsigned long tick;

extern unsigned int _end_kernel;
extern unsigned int _iram_start;
extern unsigned int _iram_end;
extern unsigned int _sdend_start;
extern unsigned int _sdend_end;

extern int has_develFct;


#define SWAB16(x) (x)
#define SWAB32(x) (x)

#endif
