/*
*   kernel/core/kernel.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/lcd.h>
#include <driver/ata.h>

#include <kernel/thread.h>
#include <kernel/pipes.h>
#include <kernel/irq.h>

#include <init/exit.h>

MED_RET_T errno=0;

char * archName[] = {
    "AV3XX",
    "GMINI4XX",
    "AV1XX_ARCH",
    "JBMM",
    "GMINI402",
    "AV4XX",
    "PMA",
    "AV5XX"
};

int getArch(void)
{
    return CURRENT_ARCH;
}

char * getArchName(void)
{
    return archName[CURRENT_ARCH];
}

void getResolution(int * width,int * height)
{
    if(width) *width=LCD_WIDTH;
    if(height) *height=LCD_HEIGHT;
}

MED_RET_T getErrno(void)
{
    return errno;
}

char * med_iramTop=CORE_START;

int readCPUMode(void)
{
    int val;
    asm volatile(
            "mrs %0,cpsr \n"
    :"=r" (val));

    return (val&0x1F);
}

struct pipe kernel_cmdPipe;

void kernel_doCmd(int cmd)
{
    pipeWrite(&kernel_cmdPipe,&cmd,sizeof(int));
}
     
void kernel_cmdPipeFct(void)
{
    int cmd;
    pipeInitWithThread(&kernel_cmdPipe,THREAD_SELF());
    
    while(1)
    {
        if(!pipe_hasBytes(&kernel_cmdPipe))
        {
            __cli();
            THREAD_SELF()->state=THREAD_BLOCKED_BY_PIPE;
            yield();
            __sti();
        }
        else
        {
            pipeRead(&kernel_cmdPipe,&cmd,sizeof(int));
            switch(cmd)
            {
                case CMD_ATA_SLEEP:
                    ata_StopHD(HD_DISK);
                    break;
                case CMD_HALT_DEVICE:
                    halt_device();
                    break;
                case CMD_LCD_ENABLE:
                    printk("LCD enable");
                    lcd_fadeEnable();
                    printk(" done\n");
                    break;
                case CMD_LCD_DISABLE:
                    printk("LCD disable");
                    lcd_fadeDisable();
                    printk(" done\n");
                    break;
                default:
                    printk("UKN cmd: %d\n",cmd);   
            }
        }
    }
}

