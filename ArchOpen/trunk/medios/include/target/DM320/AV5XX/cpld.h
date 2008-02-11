/*
*   include/kernel/target/arch_AV5XX/cpld.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CPLD_ARCH_H
#define __CPLD_ARCH_H

#define CPLD_BASE                         0x40400000

#define CPLD_PORT0                        (CPLD_BASE+0x00)
#define CPLD_PORT1                        (CPLD_BASE+0x02)
#define CPLD_PORT2                        (CPLD_BASE+0x04)
#define CPLD_PORT3                        (CPLD_BASE+0x06)
#define CPLD_PORT4                        (CPLD_BASE+0x000)


/* init values */

#define CPLD_PORT0_INIT        0x3
#define CPLD_PORT1_INIT        0xF
#define CPLD_PORT2_INIT        0x2
#define CPLD_PORT3_INIT        0xf /* or 0xc, whereas 0x4 and 0x7 changes the frequency of the aic */
#define CPLD_PORT4_INIT        0x0

/* PORT 0 */

/* PORT 1*/

#define CPLD_HD_POWER          0x0
#define CPLD_HD_RESET          0x1
#define CPLD_LCD_POWER         0x2
#define CPLD_FX2_BIT0          0x3

/* PORT 2*/

#define CPLD_FX2_SELECT        0x0
#define CPLD_FX2_RESET         0x1
#define CPLD_FX2_WAKEUP        0x2
#define CPLD_FX2_PA6           0x3

/* PORT 3*/
/* AIC stuff*/
/* unused */

#define CPLD_HD_CF             0x0
#define CPLD_SEL_HD            0x0
#define CPLD_SEL_CF            0x0
#define CPLD_LCD               0x0

#endif
