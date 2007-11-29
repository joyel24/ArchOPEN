/*
*   kernel/target/arch_AV3XX/ata.c
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
    int i;
    CPLD_SET_PORT2(CPLD_HD_RESET);
    for(i=0;i<0x100;i++) /*nothing*/;
    CPLD_CLEAR_PORT2(CPLD_HD_RESET);
}

void arch_ata_powerUpHD(void)
{
    CPLD_SET_PORT3(CPLD_HD_POWER); /* powering up HD */
}

void arch_ata_powerDownHD(void)
{
    CPLD_CLEAR_PORT3(CPLD_HD_POWER);
}

void arch_ata_selectHD(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_HD);
}

void arch_ata_selectCF(void)
{
    cpld_select(CPLD_HD_CF,CPLD_SEL_CF);
}

void arch_ata_init(void)
{
    arch_ata_resetHD();
//    outb(0x8,IDE_CONTROL);          /* enabling ints */
//    enable_irq(IRQ_IDE);
}
