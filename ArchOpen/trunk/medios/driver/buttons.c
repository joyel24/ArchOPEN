/*
*   kernel/driver/buttons.c
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
#include <kernel/kernel.h>
#include <kernel/evt.h>
#include <kernel/timer.h>
#include <kernel/console.h>

#include <gfx/screens.h>

#include <init/exit.h>

#include <driver/hardware.h>
#include <driver/cpld.h>
#include <driver/gio.h>
#include <driver/energy.h>
#include <driver/lcd.h>
#include <driver/fm_remote.h>
#include <driver/buttons.h>

struct tmr_s btnChk_tmr;

__IRAM_DATA int nb_off_press;
__IRAM_DATA int btn_state;

int wait_for_release;

int mx_press[NB_BUTTONS];
struct btn_repeatParam default_repeatParam = {
    DEFAULT_INIT_DELAY,
    DEFAULT_SECOND_DELAY,
    DEFAULT_MIN_DELAY,
    DEFAULT_DEC_VALUE
};
struct btn_repeatParam * current_repeatParam;
int nb_pressed[NB_BUTTONS];
int press_step[NB_BUTTONS];


extern int inHold;

extern int btn_mask[NB_BUTTONS];

#define BTN_NOT_PRESSED(val,btn)    !(val&btn_mask[btn])
#define BTN_PRESSED(val,btn)        (val&btn_mask[btn])

int need_clean;

extern __IRAM_DATA int powering_off;

__IRAM_CODE int btn_readState(void)
{
    return btn_state;
}

__IRAM_CODE void btn_chkPress(void)
{    
    btn_state=arch_btn_readHardware();

    if(wait_for_release)
    {
      if(btn_state == 0x0)
        wait_for_release = 0;
        
      return;
    }

    if(btn_state&BTMASK_OFF)
    {
        nb_off_press++;
        if(nb_off_press>MAX_OFF)
        {
            if(!powering_off)
            {
                printk("[OFF button] => halt\n");
                halt_device();
                kernel_doCmd(CMD_HALT_DEVICE);
            }
        }
    }
    else
        nb_off_press = 0;
        
    if(btn_state!=0x0)
    {
        halt_launchTimer();
        if(!lcd_enabled())
        {
            lcd_keyPress();
            wait_for_release = 1;
        }
        else
        {
            lcd_launchTimer();
            need_clean=1;
            btn_processPress(btn_state);
        }
    }
    else if(need_clean)
    {
        need_clean=0;
        memset(nb_pressed,0x0,sizeof(int)*NB_BUTTONS);
        memset(press_step,0x0,sizeof(int)*NB_BUTTONS);
    }
}

void btn_processPress(int val)
{    
    int btn;
    struct evt_t evt;
       
    for(btn=0;btn<NB_BUTTONS;btn++)
    {
        if(BTN_NOT_PRESSED(val,btn)) /* the btn is NOT pressed */
        {
            nb_pressed[btn]=0;   /* reset nb_pressed */
            mx_press[btn] = current_repeatParam->init_delay;
            press_step[btn] = 0;
        }
        else            /* the btn i is pressed */
        {    
            if(nb_pressed[btn]==0)
            {
                if(!inHold)
                {
                    switch(press_step[btn])
                    {
                        case 0:
                            press_step[btn] = 1;
                            nb_pressed[btn] = current_repeatParam->init_delay;
                            break;
                        case 1:
                            press_step[btn] = 2;
                            nb_pressed[btn] = current_repeatParam->second_delay;
                            mx_press[btn] = current_repeatParam->second_delay;
                            break;
                        case 2:
                            mx_press[btn] -= current_repeatParam->dec_value;
                            if(mx_press[btn]<current_repeatParam->min_delay)
                                mx_press[btn]=current_repeatParam->min_delay;
                            nb_pressed[btn] = mx_press[btn];
                            break;
                        default:
                            press_step[btn] = 0;
                            mx_press[btn] = current_repeatParam->init_delay;
                    }

                    if(con_screenIsVisible())
                    {
                        if(!con_handleBtn(btn))
                        {                            
                            /* other keys : post the event */
                            evt.evt=btn+1;
                            evt.evt_class=BTN_CLASS;
                            evt.data=(void*)mx_press[btn];
                            evt_send(&evt);
                        }
                    }
                    else
                    {
                      if (val&BTMASK_F1 && btn+1==BTN_ON)
                      {
                            screens_show(SCREEN_CONSOLE);
                            nb_pressed[btn] = mx_press[btn];
                            return;
                      }
                      // post the event
                      evt.evt=btn+1;
                      evt.evt_class=BTN_CLASS;
                      evt.data=(void*)mx_press[btn];
                      evt_send(&evt);
                    }
                }
                else
                {
                    FM_putTmpText("** HOLD **",10);
                    printk("** HOLD **\n");
                }
            }
            else
                nb_pressed[btn]--;
        }
    }
}

void btn_init(void)
{
    int btn;

    current_repeatParam = & default_repeatParam;

    nb_off_press=0;
    need_clean=0;
    btn_state=0;
    wait_for_release=0;

    for(btn=0;btn<NB_BUTTONS;btn++)
    {
        nb_pressed[btn]=0;
        mx_press[btn] = current_repeatParam->init_delay;
        press_step[btn] = 0;
    }
    /* set GIO for ON/OFF to input */
    GIO_DIRECTION(GIO_ON_BTN,GIO_IN);
    GIO_DIRECTION(GIO_OFF_BTN,GIO_IN);
    
          
    tmr_setup(&btnChk_tmr,"btn chk");
    btnChk_tmr.action = btn_chkPress;
    btnChk_tmr.freeRun = 1;
    btnChk_tmr.stdDelay=1; /* 1 tick */
    tmr_start(&btnChk_tmr);
      
    printk("[init] buttons\n");
}
