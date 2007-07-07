/*
*   kernel/target/arch_AV4XX/ata.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/delay.h>

#include <driver/hardware.h>
#include <driver/cpld.h>
#include <driver/ata.h>

void arch_ata_resetHD(void)
{
    CPLD_CLEAR_PORT1(CPLD_HD_RESET);
    mdelay(10);
    CPLD_SET_PORT1(CPLD_HD_RESET);
}

void arch_ata_powerUpHD(void)
{
    if((cpld_read(CPLD1)&0x3)==0x3)
        return;
    CPLD_SET_PORT1(CPLD_HD_POWER); /* powering up HD */
    mdelay(10);
}

void arch_ata_powerDownHD(void)
{
    CPLD_CLEAR_PORT1(CPLD_HD_POWER);
}

void arch_ata_selectHD(void)
{
    
}

void arch_ata_selectCF(void)
{
    
}

void arch_ata_init(void)
{  
  
}
