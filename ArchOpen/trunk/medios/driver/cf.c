/*
*   kernel/driver/cf.c
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

#include <kernel/kernel.h>
#include <kernel/evt.h>
#include <kernel/timer.h>

#include <driver/cf.h>
#include <driver/ata.h>

#include <fs/disk.h>

struct evt_t cf_evt;
struct tmr_s cf_tmr;
int state;

void cf_Chk(void)
{
    int new_state=CF_IS_CONNECTED;
    if(state!=new_state)
    {
        state=new_state;
        if(state)
        {
            /* mounting */
#if 0
            CF_ARCH_PLUG_INIT
            ata_softReset(CF_DISK);
            disk_add(CF_DISK);
#endif
            /* send evt */    
            cf_evt.evt=EVT_CF_IN;
            cf_evt.evt_class=CONNECT_CLASS;
            cf_evt.data=0;
            evt_send(&cf_evt);
            printk("CF plugged\n");
        }
        else
        {
            /* umounting */
#if 0
            disk_rm(CF_DISK);            
            CF_ARCH_UNPLUG_INIT
#endif
            cf_evt.evt=EVT_CF_OUT;
            cf_evt.evt_class=CONNECT_CLASS;
            cf_evt.data=0;
            evt_send(&cf_evt);
            printk("CF unplugged\n");
        }
    }
}



void cf_init(void)
{
    state = CF_IS_CONNECTED;
    
    tmr_setup(&cf_tmr,"Ext. Module Chk");
    cf_tmr.action = cf_Chk;
    cf_tmr.freeRun = 1;
    cf_tmr.stdDelay=1*HZ; /* 1s period */
    tmr_start(&cf_tmr);
    printk("[init] cf (%s)\n",state?"plugged":"not plugged");
}
