/*
*   init/boot_error.c
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
#include <kernel/evt.h>
#include <kernel/delay.h>

#include <init/boot_error.h>
#include <init/exit.h>

#include <gfx/graphics.h>

#include <gui/msgBox.h>

#include <sys_def/colordef.h>
#include <sys_def/evt.h>

#include <lib/string.h>

#include "boot_error_msg.h"

void gui_bootError(int errorNumber,int type)
{
    int evtHand;
    char tmp[20];
    gfx_openGraphics();
    
    gfx_clearScreen(COLOR_TRSP);
    
    msgBox_init();
    
    if((evtHand=evt_getHandler(BTN_CLASS))<0)
    {
        printk("Error getting handler\n");
        printk("MSG:%s\n",boot_error_msg[errorNumber]);        
    }
    else
    {
        printk("MSG:%s\n",boot_error_msg[errorNumber]);
        sprintf(tmp,"%s - N %d",type==BOOT_ERROR?"Boot error":"Boot warning",errorNumber);
        msgBox_show(tmp,
                    boot_error_msg[errorNumber],MSGBOX_TYPE_OK,
                    type==BOOT_ERROR?MSGBOX_ICON_ERROR:MSGBOX_ICON_WARNING,evtHand);        
    }
    
    if(type==BOOT_ERROR)
    {
        halt_device();
        printk("Waiting for halt\n");
        while(1);
    }    
    
    mdelay(100);
    
    evt_freeHandler(evtHand);
}
