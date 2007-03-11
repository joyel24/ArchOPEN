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

#ifdef HAVE_MAS_SOUND
#include <driver/mas.h>
#endif
#ifdef HAVE_AIC23_SOUND
#include <driver/aic23.h>
#endif

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


#ifdef BUILD_LIB
extern int app_main(int argc, char * argv[]);
#endif

unsigned int _svc_IniStack = IRAM_SIZE;
unsigned int _sys_IniStack = IRAM_SIZE-SVC_STACK_SIZE;

#if 0
void tst_1(void)
{
    int fd=open("/test1.bin",O_RDWR);
    int ok=1;
    int nb=0;
    char buffer[512];
    printk("Thread 1 file open\n");
    if(fd<0)
    {
        printk("Error opening file\n");
        return;
    }
    while(ok)
    {
        nb=read(fd,buffer,0x200);
        printk("Thread 1 read: %d\n",nb);
        if(nb!=0x200) ok=0;
    }
    close(fd);
    printk("Thread 1 file closed\n");
}

void tst_2(void)
{
    int fd=open("/test2.bin",O_RDWR);
    int ok=1;
    int nb=0;
    char buffer[512];
    printk("Thread 2 file open\n");
    if(fd<0)
    {
        printk("Error opening file\n");
        return;
    }
    while(ok)
    {
        nb=read(fd,buffer,0x200);
        printk("Thread 2 read: %d\n",nb);
        if(nb!=0x200) ok=0;
    }
    close(fd);
    printk("Thread 2 file closed\n");
}

void tst_3(void)
{
    int fd=open("/test3.bin",O_RDWR);
    int ok=1;
    int nb=0;
    char buffer[512];
    printk("Thread 3 file open\n");
    if(fd<0)
    {
        printk("Error opening file\n");
        return;
    }
    while(ok)
    {
        nb=read(fd,buffer,0x200);
        printk("Thread 3 read: %d\n",nb);
        if(nb!=0x200) ok=0;
    }
    close(fd);
    printk("Thread 3 file closed\n");
}

void tst_fct(void)
{
    THREAD_INFO *th1,*th2,*th3;
    int pid1,pid2,pid3;
    pid1=thread_startFct(&th1,tst_1,"Test1",THREAD_STATE_DISABLE,PRIO_MED);
    pid2=thread_startFct(&th2,tst_2,"Test2",THREAD_STATE_DISABLE,PRIO_MED);
    pid3=thread_startFct(&th3,tst_3,"Test3",THREAD_STATE_DISABLE,PRIO_MED);
    thread_ps();
    thread_enable(pid1);
    thread_enable(pid2);
    thread_enable(pid3);
    while(1) /*nothing*/;
}
#endif

#if 0

#include <fs/csv_file.h>

struct testStruct {
    char * st1;
    int val1;
    int val2;
    char * st2;
};

void test_csv(void)
{
    printk("trying to close before start: get %d\n",-csv_end());
    
    if(csv_newFile("/test.csv")==MED_OK)
    {
        int ret_val;
        int stop=0;
        struct testStruct s1;
        while(!stop)
        {
            ret_val=csv_readLine(&s1,"sdds",';');
            //ret_val=csv_readLine(&s1,"sddA",';'); /*test with bad format string*/
            //ret_val=csv_readLine(&s1,"sddss",';'); /*test with too much item in format string*/
            switch(ret_val)
            {
                case MED_OK:
                    printk("Everything is fine, found:\n");
                    printk("%s\n%d\n%d\n%s\n",s1.st1,s1.val1,s1.val2,s1.st1);                    
                    print_data((void*)&s1,sizeof(struct testStruct));
                    break;
                case -MED_EINVAL:
                case -MED_EMOBJ:
                    printk("Error\n");
                    break;
                case -MED_EOF:
                    stop=1;
                    printk("EOF\n");
                    break;
                default:
                    printk("UKN return code: %d\n",ret_val);
                    break;            
            }
        }
    }
    csv_end();
    printk("end of test let's loop\n");
    while(1) ;
}


#endif

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
    
    energy_loadPref();
    
    /* boot info */
    printk("SP: %08x\n",get_sp());
    irq_print();
    tmr_print();
    thread_ps();

#ifdef BUILD_LIB
    app_main(1,&stdalone);
    reload_firmware();
#endif
    //tst_fct();
    //test_csv();
    //FM_enable();
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
