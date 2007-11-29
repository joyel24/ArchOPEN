/*
*   kernel/target/arch_AV3XX/exit.c
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
#include <sys_def/font.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/io.h>

#include <init/exit.h>

#include <driver/ata.h>
#include <driver/hardware.h>
#include <driver/lcd.h>

#include <gfx/graphics.h>

#include <fs/disk.h>

void reset_device(void);

void arch_reload_firmware(void)
{
    void (*firmware_start)(void) = (void (*)(void)) 0x100004;
    printk("about to reboot\n");
    ata_softReset(HD_DISK);
    ata_StopHD(HD_DISK); /* we need to call halt_hd later to unmount all partitions */
    firmware_start();
}

void arch_HaltMsg(void)
{
    int h,w;
    char * msg = "Halting device";
    
    gfx_openGraphics();
    
    gfx_clearScreen(COLOR_TRSP);
    
    gfx_fontSet(STD8X13);
    
    gfx_getStringSize(msg,&w,&h);
    
    
    gfx_putS(COLOR_RED,COLOR_TRSP,(SCREEN_REAL_WIDTH-w)/2,(SCREEN_HEIGHT-h)/2,msg);
}
