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
#include <kernel/lang.h>

#include <fs/bflat.h>
#include <fs/med.h>
#include <fs/stdfs.h>
#include <fs/vfs.h>
#include <fs/disk.h>

#include <driver/wdt.h>

#ifdef HAVE_MAS_SOUND
#include <driver/mas.h>
#endif
#ifdef HAVE_AIC23_SOUND
#include <driver/aic23.h>
#endif

#include <snd/sound.h>

#include <driver/hardware.h>
#include <driver/uart.h>
#include <driver/cpld.h>
#include <driver/time.h>
#include <driver/usb_fw.h>
#include <driver/batDc.h>
#include <driver/energy.h>
#include <driver/buttons.h>
#include <driver/ata.h>
#include <driver/fm_remote.h>
#include <driver/lcd.h>
#include <driver/bus.h>

#ifdef HAVE_EXT_MODULE
#include <driver/ext_module.h>
#include <driver/dvr_module.h>
#endif

#include <driver/cf.h>

#include <gfx/graphics.h>
#include <gfx/screens.h>

#include <gui/shell.h>
#include <gui/splash.h>
#include <gui/settings_misc.h>
#include <gui/settings_lang.h>

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
    
    if(disk_init()!=MED_OK)
    {
        printk("[init] ------ Halting\n");
        for(;;);
    }
        
#ifndef BUILD_LIB    
    sound_init();
#endif
    energy_loadPref();
    misc_loadPref();
    lang_loadLang();
    
    /* boot info */
#if 0    
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
    thread_ps();
#endif
    
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
    printk("MediOS " VERSION_NUMBER " - kernel loading\n\n");

    printk("Initial SP: %08x, kernel end: %08x, size in IRAM: %x/%x  Malloc start: %08x, size: %x\n",get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        IRAM_SIZE,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    printk("Chip rev : %x\n",inw(BUS_REVR));
    printk("Current Cpu mode : %x\n",readCPUMode());
    
    if(thread_init(kernel_thread)!=MED_OK)
    {
        printk("no kernel thread CAN'T BOOT\n");
        for(;;);
    }
    
    /* add cmd pipe thread */
    if(thread_startFct(NULL,kernel_cmdPipeFct,"cmdPipeFct",THREAD_STATE_ENABLE,PRIO_LOW,THREAD_USE_OTHER_STACK)<0)
    {
        printk("no cmdPipe thread CAN'T BOOT\n");
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
    
    lang_init();
    
    init_cmd_line();

#ifdef HAVE_EVT
    evt_init();
#endif
    btn_init();

    energy_init();
    batDc_init();

    time_init();
    
#ifdef CHK_USB_FW
    init_usb_fw();
#endif

    FM_init();

#ifdef HAVE_EXT_MODULE
    init_ext_module();
    init_dvr_module();
#endif

#ifdef HAVE_CF
    cf_init();
#endif

    ata_init();
    vfs_init();
        
//    sound_init();
#ifdef HAVE_MAS_SOUND
   mas_init();
#endif
#ifdef HAVE_AIC23_SOUND
   aic23_init();
#endif
    
    printk("[init] ------------ drivers done\n");
    
    /*Load kernel thread to enable irq*/
    thread_loadContext();

    /* Err */
    printk("[init] error: back to main(), SYS thread not started\n");
    for(;;);
}
