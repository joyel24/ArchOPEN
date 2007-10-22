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

#include <fs/disk.h>

extern int cur_disk;

int ATA_INB(int reg) {
    //int __res;
    if(reg==IDE_ALTSTATUS) {
        outb(OMAP_HD_ALTS_ERR_REQUEST,OMAP_REQUEST_BASE);
        while(inb(OMAP_REQUEST_BASE));
    }
    return inb(reg);
}


void arch_ata_resetHD(void)
{
    // outb(OMAP_HD_POWER_UP_REQUEST,OMAP_REQUEST_BASE);
    // while(inb(OMAP_REQUEST_BASE));
     outb(OMAP_HD_RESET_REQUEST,OMAP_REQUEST_BASE);
     while(inb(OMAP_REQUEST_BASE));
  //   mdelay(2000);
}

void arch_ata_powerUpHD(void)
{
     outb(OMAP_HD_POWER_UP_REQUEST,OMAP_REQUEST_BASE);
     while(inb(OMAP_REQUEST_BASE));
     //printk("Power UP done!\n");
}

void arch_ata_powerDownHD(void)
{
     outb(OMAP_HD_POWER_DOWN_REQUEST,OMAP_REQUEST_BASE);
     while(inb(OMAP_REQUEST_BASE));
}

void arch_ata_selectHD(void)
{
}

void arch_ata_selectCF(void)
{
}

void arch_ata_init(void)
{
  outb(OMAP_HD_INIT_REQUEST,OMAP_REQUEST_BASE);
  while(inb(OMAP_REQUEST_BASE));  
}
