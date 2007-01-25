/*
*   include/kernel/target/arch_AV3XX/cpld.h
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

#define CPLD_BASE              0x05100000

#define CPLD_PORT0             (CPLD_BASE+0x000)
#define CPLD_PORT1             (CPLD_BASE+0x100)
#define CPLD_PORT2             (CPLD_BASE+0x200)
#define CPLD_PORT3             (CPLD_BASE+0x300)
#define CPLD_PORT4             (CPLD_BASE+0x000)

/* init values */

#define CPLD_PORT0_INIT        0x0
#define CPLD_PORT1_INIT        0x0
#define CPLD_PORT2_INIT        0x4
#define CPLD_PORT3_INIT        0x9
#define CPLD_PORT4_INIT        0x0
/* PORT 0 */
/*** HD / CF */
#define CPLD_HD_CF             0x0
#define CPLD_SEL_HD            0x0
#define CPLD_SEL_CF            0x1

#define CPLD_FW_EXT            0x1
#define CPLD_DVR               0x2

/* PORT 1*/
#define CPLD_USB               0x0

/* PORT 2*/
#define CPLD_HD_RESET          0x0
#define CPLD2_DVR              0x1
#define CPLD_LCD               0x2

/* PORT 3*/
#define CPLD_FM                0x0
#define CPLD3_DVR              0x1
#define CPLD_HD_POWER          0x3


#endif
