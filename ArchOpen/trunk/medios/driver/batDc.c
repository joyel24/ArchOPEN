/*
*   kernel/driver/bat_power.c
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
#include <kernel/timer.h>
#include <kernel/evt.h>

#include <driver/batDc.h>
#include <driver/energy.h>

struct tmr_s pwrCableChk_tmr;

int kpwrState;

int getPowerMode(void)
{
    if(POWER_CONNECTED)
        return TIMER_MODE_DC;
    else
        return TIMER_MODE_BAT;
}

int DC_isConnected(void)
{
    return POWER_CONNECTED;
}

int batLevel(void)
{
    return GET_BAT_LEVEL;
}

void DC_chkAction(void)
{
    if(POWER_CONNECTED!=kpwrState)
    {
        struct evt_t evt;
        kpwrState=POWER_CONNECTED;
        /* change the timers */
        energy_chgMode(kpwrState?TIMER_MODE_DC:TIMER_MODE_BAT);
        evt.evt = (kpwrState)?EVT_PWR_IN:EVT_PWR_OUT;
        evt.evt_class = CONNECT_CLASS;
        evt.data = NULL;
        evt_send(&evt);
        printk("[power] DC connector %s\n",kpwrState==1?"plugged":"unplugged");
    }
}

void batDc_init(void)
{
    tmr_setup(&pwrCableChk_tmr,"Power Cable chk");
    pwrCableChk_tmr.action = DC_chkAction;
    pwrCableChk_tmr.freeRun = 1;
    pwrCableChk_tmr.stdDelay=1*HZ; /* 1s period */
    tmr_start(&pwrCableChk_tmr);

    kpwrState=POWER_CONNECTED;

    printk("[init] batDc, Bat level: %x, DC %s connected\n",GET_BAT_LEVEL,kpwrState==0?"not":"is");
}
