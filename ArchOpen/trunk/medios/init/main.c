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
#include <init/boot_error.h>

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
#include <gui/icons.h>
#include <gui/settings_misc.h>
#include <gui/settings_lang.h>

#ifdef BUILD_LIB
extern int app_main(int argc, char * argv[]);
#endif

#ifdef TARGET_TYPE_AVLO
#include "../../apps/avlo/include/avlo.h"
#endif

unsigned int _svc_IniStack = IRAM_SIZE;
unsigned int _sys_IniStack = IRAM_SIZE-SVC_STACK_SIZE;

#if 0
#include <gui/msgBox.h>
#include <sys_def/font.h>



void test(void)
{
    int handler=evt_getHandler(BTN_CLASS);
    gfx_initGraphics();    
    gfx_clearScreen(COLOR_TRSP);

    icon_init();
    msgBox_init();
    screens_show(SCREEN_GFX);

    msgBox_show("Type OK","Text\ntype 0",MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,handler);
    msgBox_show("Type OK/Cancel","Text\ntype 1",MSGBOX_TYPE_OKCANCEL,MSGBOX_ICON_EXCLAMATION,handler);
    msgBox_show("Type Yes/No","Text\ntype 2",MSGBOX_TYPE_YESNO,MSGBOX_ICON_EXCLAMATION,handler);
    msgBox_show("Type Yes/No/Cancel","Text\ntype 3",MSGBOX_TYPE_YESNOCANCEL,MSGBOX_ICON_EXCLAMATION,handler);
    msgBox_show("Type Info","Text\ntype 4\n pas bouton",MSGBOX_TYPE_INFO,MSGBOX_ICON_EXCLAMATION,handler);
        

            
    evt_freeHandler(handler);
}


#endif


void kernel_thread(void)
{
#ifdef TARGET_TYPE_STD
    DIR * medFolder;
#endif
    
#ifdef BUILD_LIB
    char * stdalone="STDALONE";
#endif
  
    printk("[SYS thread] starting\n");
    
    if(disk_init()!=MED_OK)
    {
        printk("[init] ------ Halting\n");
        for(;;);
    }
        
#ifdef TARGET_TYPE_STD
    /* we shall have medios tree on disk */
    medFolder=opendir("/medios");
    if(!medFolder)
    {
        gui_bootError(MISSING_MEDIOS_ERROR,BOOT_ERROR);
    }
    closedir(medFolder);
#endif
        
    
#ifdef TARGET_TYPE_STD
#ifndef BUILD_LIB
    sound_init();
#endif
#endif
    
    energy_loadPref();
    
#ifdef TARGET_TYPE_STD
    misc_loadPref();
#endif
        
#ifdef TARGET_TYPE_STD
    medFolder=opendir("/medios/lang");
    if(!medFolder)
    {
        gui_bootError(MISSING_LANG_FOLDER_ERROR,BOOT_WARN);
    }
    closedir(medFolder);
    
    lang_loadLang();
#endif
        
    /* boot info */
#if 0  
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
    thread_ps();
#endif
    
       //     test();
    
#ifdef BUILD_LIB
    app_main(1,&stdalone);
    reload_firmware();
    printk("WARN: device should be already down!!!!, let's loop\n");
    while(1);
#endif
         
#ifdef TARGET_TYPE_STD
    shell_main();
#endif
    
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
    
#ifdef TARGET_TYPE_STD
    icon_kernelInit();
#endif
    /* print banner on uart */
    
#ifdef TARGET_TYPE_STD
    printk("\nMediOS " VERSION_NUMBER " - kernel loading\n\n");
#endif
    
#ifdef TARGET_TYPE_AVLO
    printk("\nAvlo " VERSION " - loading\n\n");
#endif
    
    printk("Initial SP:%08x, kernel end:%08x, size in IRAM:%x/%x\nMalloc start:%08x, Malloc size:%x\n",
           get_sp(),
        (unsigned int)&_end_kernel,
        (unsigned int)&_iram_end - (unsigned int)&_iram_start,
        IRAM_SIZE,
        (unsigned int)MALLOC_START,
        (unsigned int)MALLOC_SIZE);

    printk("Chip rev : %x\n",inw(BUS_REVR));
    printk("Current Cpu mode : %x\n",readCPUMode());
    
#ifdef TARGET_TYPE_STD    
    lang_init();    
    splash_init();
#endif
    
#ifdef BOOT_WITH_CONSOLE
    screens_show(SCREEN_CONSOLE);
#else
#ifdef TARGET_TYPE_STD
    screens_show(SCREEN_SPLASH);
#endif
#endif
    
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
    
    init_cmd_line();
   
#ifdef HAVE_EVT
    evt_init();
#endif
    btn_init();
    
    energy_init();
    batDc_init();
    
#ifdef TARGET_TYPE_STD 
    time_init();
#endif
    
#ifdef CHK_USB_FW
    init_usb_fw();
#endif

#ifdef TARGET_TYPE_STD    
    FM_init();
#endif

#if defined(HAVE_EXT_MODULE) && defined(TARGET_TYPE_STD )
    init_ext_module();
    init_dvr_module();
#endif

#if defined(HAVE_CF) && defined(TARGET_TYPE_STD)
    cf_init();
#endif

    ata_init();
    vfs_init();
        
#ifdef TARGET_TYPE_STD 
//    sound_init();
#ifdef HAVE_MAS_SOUND
   mas_init();
#endif
#ifdef HAVE_AIC23_SOUND
   aic23_init();
#endif
#endif
    printk("[init] ------------ drivers done\n");
    
    /*Load kernel thread to enable irq*/
    thread_loadContext();

    /* Err */
    printk("[init] error: back to main(), SYS thread not started\n");
    for(;;);
}
