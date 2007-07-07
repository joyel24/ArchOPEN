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
#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/cpld.h>
#include <driver/ata.h>
#include <driver/gio.h>

void arch_ata_resetHD(void)
{
    printk("HD reset\n");
    CPLD_SET_PORT2(CPLD_HD_RESET);
    CPLD_CLEAR_PORT2(CPLD_HD_RESET);
}

void arch_ata_powerUpHD(void)
{
    GIO_SET(GIO_HD_POWER);

}

void arch_ata_powerDownHD(void)
{
    GIO_CLEAR(GIO_HD_POWER);
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
    GIO_DIRECTION(GIO_HD_POWER,GIO_OUT);
    
    
    GIO_SET(GIO_HD_POWER);

    outw(0x13a1,0x30a04);
    outw(0x0,0x30a08);
    outw(0x1,0x30a18);
    
    arch_ata_selectHD();
    
    arch_ata_resetHD();
}
