/*
*   kernel/init/main.c
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

#include <sys_def/colordef.h>
#include <sys_def/time.h>
#include <sys_def/font.h>

#include <init/exit.h>

#include <kernel/io.h>
#include <kernel/version.h>
#include <kernel/kernel.h>
#include <kernel/delay.h>
#include <kernel/console.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/cmd_line.h>
#include <kernel/evt.h>

#include <fs/bflat.h>
#include <fs/med.h>
#include <fs/stdfs.h>
#include <fs/vfs.h>
#include <fs/disk.h>

#include <driver/wdt.h>
#include <driver/aic23.h>
#include <driver/hardware.h>
#include <driver/uart.h>
#include <driver/cpld.h>
#include <driver/rtc.h>
#include <driver/usb_fw.h>
#include <driver/bat_power.h>
#include <driver/buttons.h>
#include <driver/ata.h>
#include <driver/fm_remote.h>
#include <driver/lcd.h>
#include <driver/bus.h>

#ifdef HAVE_EXT_MODULE
#include <driver/ext_module.h>
#include <driver/dvr_module.h>
#include <driver/cf_module.h>
#endif

#include <gfx/graphics.h>

#include <gui/shell.h>
#include <gui/splash.h>
#include <gui/screens.h>

#ifdef BUILD_LIB
extern int app_main(int argc, char * argv[]);
#endif

unsigned int _svc_IniStack = IRAM_SIZE;
unsigned int _sys_IniStack = IRAM_SIZE-SVC_STACK_SIZE;

void kernel_thread(void)
{
#ifdef BUILD_LIB
    char * stdalone="STDALONE";
#endif
  
    printk("[SYS thread] starting\n");
    
    /* boot info */
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
    thread_ps();

#ifdef BUILD_LIB
    app_main(1,&stdalone);
    reload_firmware();
#endif

    shell_main();

    printk("[SYS thread] error: back to main()\n");
    for(;;);
}

void kernel_start (void)
{
   /* sanity init */
    threadCurrent=NULL;
    
   /* need uart ok for printk init */   
    uart_init();
    printk_init();
    
    /* malloc of max space in SDRAM */
    mem_init((void*)MALLOC_START,MALLOC_SIZE);
    
    screens_init();
    
    gfx_init();
    con_init();
    splash_init();
    
#ifdef BOOT_WITH_CONSOLE
    screens_show(SCREEN_CONSOLE);
#else
    screens_show(SCREEN_SPLASH);
#endif

    /* print banner on uart */
    printk("MediOS %d.%d - kernel loading\n\n",VER_MAJOR,VER_MINOR);

    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %x/%x  Malloc start: %08x, size: %x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        IRAM_SIZE,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    printk("Chip rev : %x\n",inw(BUS_REVR));
    
    if(thread_init(kernel_thread)!=MED_OK)
    {
        /* no KERNEL thread => loop */
        printk("CAN'T BOOT\n");
        for(;;);
    }
    
    /* init the watchdog timer */
    wdt_init();
    /* init the irq */
    irq_init();
    /* init the tick timer */
    tmr_init();

    /* driver init */
    cpld_init();
    
    lcd_init();

    init_cmd_line();

#ifdef HAVE_EVT
    evt_init();
#endif
    btn_init();
#ifdef CHK_BAT_POWER
    init_power();
#endif
#ifndef PMA
    init_rtc();
#endif
#ifdef CHK_USB_FW
    init_usb_fw();
#endif

    FM_init();

#ifdef HAVE_EXT_MODULE
    init_ext_module();
    init_dvr_module();
    cf_initModule();
#endif
    ata_init();
    vfs_init();
    if(disk_init()!=MED_OK)
    {
        printk("[init] ------ Halting\n");
        for(;;);
    }
//    sound_init();

    printk("[init] ------------ drivers done\n");
    
    /*Load kernel thread to enable irq*/
    thread_loadContext();

    /* Err */
    printk("[init] error: back to main(), SYS thread not started\n");
    for(;;);
}
