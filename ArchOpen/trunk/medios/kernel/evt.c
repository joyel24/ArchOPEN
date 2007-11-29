/*
*   kernel/core/evt.c
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
#include <sys_def/errors.h>

#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/pipes.h>
#include <kernel/timer.h>

#include <kernel/evt.h>

#include <driver/hardware.h>
#include <driver/buttons.h>

struct evt_pipes evt_pipe_tab[NB_EVT_PIPES];
struct tmr_s evt_timer;

int evt_getHandler(unsigned int mask)
{
    int i;
    
    /* look for a free evt_pipe */
    for(i=0;i<NB_EVT_PIPES;i++)
        if(!evt_pipe_tab[i].used)
            break;
    
    if(i==NB_EVT_PIPES)
        return -MED_EMOBJ;

    pipeInitWithThread(&(evt_pipe_tab[i].evt_pipe),THREAD_SELF());
    evt_pipe_tab[i].used = 1;
    evt_pipe_tab[i].mask = mask;
      
//    printk("[evt handling] register: %d (mask=%x)\n",i,mask);
    return i;
}

MED_RET_T evt_freeHandler(int num_evt_pipe)
{
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES)
    {
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        evt_pipe_tab[num_evt_pipe].used = 0;
 //       printk("[evt handling] UNregister: %d\n",num_evt_pipe);
    }
    else
        return -MED_EINVAL;    
    return MED_OK;
}

void evt_send(struct evt_t * evt)
{
    int i;
    for(i=0;i<NB_EVT_PIPES;i++)
    {
        if(evt_pipe_tab[i].used && (evt_pipe_tab[i].mask&evt->evt_class))
        {
            pipeWrite(&(evt_pipe_tab[i].evt_pipe), evt, sizeof(struct evt_t));
        }
    }
    
}

MED_RET_T evt_getFullStatus(int num_evt_pipe, struct evt_t * evt)
{
    //printk("evt chk: handler=%d evt @ %x\n",num_evt_pipe,evt);
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES && evt != NULL)
    {
        evt->evt=0;
        evt->evt_class=0;
        evt->data=0;
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        pipeRead(&(evt_pipe_tab[num_evt_pipe].evt_pipe), evt, sizeof(struct evt_t));
    }
    else
    {
        printk("Bad handler: %d\n",num_evt_pipe);
        return -MED_EINVAL;    
    }
    return MED_OK;
}

int evt_getStatus(int num_evt_pipe)
{
    struct evt_t evt;
    int ret_val;
    
    ret_val = evt_getFullStatus(num_evt_pipe,&evt);
    if(ret_val!=MED_OK)
    {
        return ret_val;
    }
    return evt.evt;
}

int evt_getStatusBlocking(int num_evt_pipe)
{
#ifdef HAVE_DBUG
    return evt_getStatus(num_evt_pipe);
#else
    struct evt_t evt;
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES)
    {
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        evt.evt=0;
        evt.evt_class=0;
        evt.data=0;
        while(!pipe_hasBytes(&evt_pipe_tab[num_evt_pipe].evt_pipe))
        {
            __cli();
            THREAD_SELF()->state=THREAD_BLOCKED_BY_PIPE;
            yield();
            __sti();
        }
        pipeRead(&(evt_pipe_tab[num_evt_pipe].evt_pipe), &evt, sizeof(struct evt_t));
        return evt.evt;
    }
    printk("Bad handler: %d\n",num_evt_pipe);
    return -MED_EINVAL;
#endif
}

int evt_purgeHandler(int num_evt_pipe)
{
    if(num_evt_pipe >= 0 && num_evt_pipe < NB_EVT_PIPES)
    {
        if(evt_pipe_tab[num_evt_pipe].used!=1)
            return -MED_ENBUSY;
        pipeInitWithThread(&(evt_pipe_tab[num_evt_pipe].evt_pipe),THREAD_SELF());
    }
    else
        return -MED_EINVAL;    
    return MED_OK;
}

struct evt_t evt;
void evt_timer_action(void)
{
    evt_send(&evt);
    evt_timer.expires = tick + EVT_DELAY; /* 1s timer */
    tmr_start(&evt_timer);
}

void evt_init(void)
{
    int i;
    for(i=0;i<NB_EVT_PIPES;i++)
    {
        pipeInit(&(evt_pipe_tab[i].evt_pipe));
        evt_pipe_tab[i].used=0;
    }
    
    evt.evt = EVT_TIMER;
    evt.evt_class=TMR_CLASS;
    
    tmr_setup(&evt_timer,"EVT");
    evt_timer.action = evt_timer_action;
    evt_timer.expires = tick + EVT_DELAY;
    tmr_start(&evt_timer);

    printk("[init] evt\n");
}

const char * getBtnName(int btn)
{
    return btn_name[btn-1];
}
