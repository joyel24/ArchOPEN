/*
*   include/kernel/cpld.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CPLD_H
#define __CPLD_H

#include <driver/hardware.h>

#include <target/chip/arch/cpld.h>

extern int CPLD_VER;

#define CPLD0                  0x0
#define CPLD1                  0x1
#define CPLD2                  0x2
#define CPLD3                  0x3
#define CPLD4                  0x4

#define CPLD_SET_PORT0(bit_num)     cpld_select(bit_num,1)
#define CPLD_CLEAR_PORT0(bit_num)   cpld_select(bit_num,0)
#define CPLD_SET_PORT1(bit_num)     cpld_changeState(CPLD1,bit_num,1)
#define CPLD_CLEAR_PORT1(bit_num)   cpld_changeState(CPLD1,bit_num,0)
#define CPLD_SET_PORT2(bit_num)     cpld_changeState(CPLD2,bit_num,1)
#define CPLD_CLEAR_PORT2(bit_num)   cpld_changeState(CPLD2,bit_num,0)
#define CPLD_SET_PORT3(bit_num)     cpld_changeState(CPLD3,bit_num,1)
#define CPLD_CLEAR_PORT3(bit_num)   cpld_changeState(CPLD3,bit_num,0)

void cpld_changeState   (int cpld_port,int bit_num,int direction);
void cpld_select      (int bit_num,int direction);
int  cpld_read        (int cpld_port);
void cpld_write       (int cpld_port,int value);
void cpld_doSelect(void);
void cpld_init(void);
int cpld_getVersion(void);
int arch_cpld_getVersion(void);

#endif
