/*
*   kernel/target/arch_AV4XX/exit.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/stddef.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/irq.h>

#include <driver/hardware.h>
#include <driver/lcd.h>
#include <driver/osd.h>
#include <driver/clkc.h>
#include <driver/ata.h>
#include <driver/cache.h>
#include <driver/cpld.h>

#include <gfx/graphics.h>

#include <init/exit.h>

__attribute__((section(".fwuncomp_code"))) void arch_reload_firmware(void){
    void (*DM_restart)(void) = (void (*)(void)) 0x0;
    
    __cli();
    __clf();

    gfx_closeGraphics();
    printk("About to return to Linux...\n");

    outb(OMAP_COPY_FW_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));

    DM_restart();
}

void arch_HaltMsg(void)
{
   reload_firmware();      // Yes, that's not a message but it should be OK for now...
}
