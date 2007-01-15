/*
*   kernel/core/timer.c
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

#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>

unsigned long tick __IRAM_DATA;

struct tmr_s * tmr_head __IRAM_DATA;


__IRAM_CODE void tmr_intAction(int irq,struct pt_regs * regs)
{
    struct tmr_s * ptr=tmr_head;
        
    tick++;
    
    while(ptr!=NULL)
    {
        if(ptr->trigger && ptr->expires<=tick)
        {
            if(!ptr->freeRun)
                ptr->trigger=0;
                
            if(ptr->action)
                ptr->action();
                
            if(ptr->freeRun && ptr->trigger)
            {
                ptr->expires=ptr->stdDelay+tick;
                if(!(ptr->expires>tick))
                    ptr->trigger=0;
            }                
        }
        ptr=ptr->nxt;
    }  
    /* calling thread switch */
    thread_nxt();
}

unsigned int tmr_getTick(void)
{
    return tick;
}

unsigned int tmr_getMicroTick(void)
{
    return arch_tmr_getMicroTick();
}

void tmr_setup(struct tmr_s * tmr_data,char * name)
{
    tmr_data->expires=0;
    tmr_data->trigger=0;
    tmr_data->name=name;
    tmr_data->freeRun=0;
    tmr_data->stdDelay=0;
    
    tmr_data->nxt=tmr_head;
    tmr_data->prev=NULL;
    if(tmr_head)
        tmr_head->prev=tmr_data;
    tmr_head=tmr_data;

}

void tmr_remove(struct tmr_s * tmr_data)
{

    if(!tmr_data->prev)
        tmr_head=tmr_data->nxt;
    else
        tmr_data->prev->nxt=tmr_data->nxt;

    if(tmr_data->nxt)
        tmr_data->nxt->prev=tmr_data->prev;
}

void tmr_stop(struct tmr_s * tmr_data)
{
    tmr_data->trigger=0;
}

void tmr_start(struct tmr_s * tmr_data)
{
    if(tmr_data->freeRun)
    {
        tmr_data->expires=tmr_data->stdDelay+tick;
    }    
        
    if(tmr_data->expires>tick)
        tmr_data->trigger=1;
    else
        tmr_data->trigger=0;
}

void tmr_init(void)
{
    /* init the timer structure */
    tmr_head=NULL;
    tick=0;

    /* disable all timer */
    TMR_SET_MODE(TMR_MODE_STOP,TMR0);
    TMR_SET_MODE(TMR_MODE_STOP,TMR1);
    TMR_SET_MODE(TMR_MODE_STOP,TMR2);
    TMR_SET_MODE(TMR_MODE_STOP,TMR3);

    arch_tmr_init();
    
    printk("[init] timer\n");
}


void tmr_print(void)
{
    struct tmr_s * ptr=tmr_head;
    int tmrnr=0;
    printk("Timer list: (cur tick:%d)\n",tick);
    while(ptr!=NULL)
    {
        printk("%d: %s, expire:%d trigger=%d,freeRun=%d\n",
            tmrnr,ptr->name!=NULL?ptr->name:"UNDEF",ptr->expires,ptr->trigger,ptr->freeRun);
        tmrnr++;
        ptr=ptr->nxt;
    }
}

