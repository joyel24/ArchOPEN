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


#include <sys_def/int_timer.h>

#include <kernel/kernel.h>
#include <kernel/timer.h>

#include <init/exit.h>

#include <driver/ata.h>
#include <driver/fm_remote.h>
#include <driver/batDc.h>
#include <driver/lcd.h>

#include <lib/string.h>

#include <fs/disk.h>
#include <fs/cfg_file.h>

struct tmr_s lcdOnOff_timer;

struct tmr_s halt_timer;
int halt_disable_timer = 0; //number of times other programs have asked to disable the halt_timer

int hd_sleep_state=0;
struct tmr_s hd_timer;

struct tmr_s pwrCableChk_tmr;

int timer_status[3][2];
int timer_delay[3][2];

int default_status[3][2] = {{ 1, 0 },{1,1},{1,0}};
int default_delay[3][2] = {{ 20, 20 },{10,10},{180,180}};

char * timer_type[]= {"LCD","HD","HALT"};
char * power_type[]= {"BAT","DC"};

/*******************************************************
* Set timer config
********************************************************/

void set_timer_status_freq(int timer_type, int power_mode, int val,int type)
{
    if(timer_type<0 || timer_type >3)
        return;
    if(power_mode!=TIMER_MODE_BAT && power_mode!=TIMER_MODE_DC)
        return;
    
    if(type == TIMER_STATUS_VAL)
    {
        if(val!=MODE_ENABLE &&val!=MODE_DISABLE)
            return;
        timer_status[timer_type][power_mode] = val;
    }
    else
    {
        timer_delay[timer_type][power_mode] = val;
    }
}

void set_timer_status(int timer_type, int power_mode, int status)
{
    set_timer_status_freq(timer_type,power_mode,status,TIMER_STATUS_VAL);
}

void set_timer_delay(int timer_type, int power_mode, int delay)
{
    set_timer_status_freq(timer_type,power_mode,delay,TIMER_DELAY_VAL);
}

/*******************************************************
* Get timer config
********************************************************/

int get_timer_status_freq(int timer_type, int power_mode,int type)
{
    if(timer_type<0 || timer_type >3)
        return -1;
    if(power_mode!=TIMER_MODE_BAT && power_mode!=TIMER_MODE_DC)
        return -1;

    if(type==TIMER_STATUS_VAL)
    {
        return timer_status[timer_type][power_mode] ;
    }
    else
    {
        return timer_delay[timer_type][power_mode] ;
    }
}

int get_timer_status(int timer_type, int power_mode)
{
    return get_timer_status_freq(timer_type,power_mode,TIMER_STATUS_VAL);
}

int get_timer_delay(int timer_type, int power_mode)
{
    return get_timer_status_freq(timer_type,power_mode,TIMER_DELAY_VAL);
}

/*******************************************************
* Launch timer
********************************************************/

void lcd_launchTimer(void)
{
    int power_mode=getPowerMode();
    if(timer_delay[LCD_TIMER][power_mode]!=0 && timer_status[LCD_TIMER][power_mode] && lcd_enabled())
    {
        lcdOnOff_timer.expires = tick + (timer_delay[LCD_TIMER][power_mode]*HZ); /* lcd_freq_rep in sec */
        tmr_start(&lcdOnOff_timer);
    }
}

void halt_launchTimer(void)
{
    int power_mode=getPowerMode();
    if(timer_delay[HALT_TIMER][power_mode]!=0 && timer_status[HALT_TIMER][power_mode])
    {
        halt_timer.expires = tick + (timer_delay[HALT_TIMER][power_mode]*HZ); /* lcd_freq_rep in sec */
        tmr_start(&halt_timer);
    }
}

void halt_disableTimer(int disable)
{
  if(disable)
    halt_disable_timer++;
  else
    if(halt_disable_timer > 0)
      halt_disable_timer--;
      
  if(halt_disable_timer == 0)
    halt_launchTimer();
}

void hd_launchTimer(void)
{
    int power_mode=getPowerMode();
    if(timer_delay[HD_TIMER][power_mode]!=0 && timer_status[HD_TIMER][power_mode] && !hd_sleep_state)
    {
        hd_timer.expires = tick + (timer_delay[HD_TIMER][power_mode]*HZ); /* hd_freq_rep in sec */
        tmr_start(&hd_timer);
    }
}

void hd_stopTimer(void)
{
    if(hd_timer.trigger)
        tmr_stop(&hd_timer);
}

/*******************************************************
* Timer action
********************************************************/

void lcd_off_action(void)
{
    int power_mode=getPowerMode();
    if(timer_status[LCD_TIMER][power_mode])
    {
        //lcd_disable();
        kernel_doCmd(CMD_LCD_DISABLE);
        if(FM_is_connected())
            FM_lightsOFF();
        
    }
}

void halt_action(void)
{
    int power_mode=getPowerMode();
    if(timer_status[HALT_TIMER][power_mode] && halt_disable_timer == 0)
    {
        printk("[power] Power off timer => halt\n");
        halt_device();
    }
}

extern int kusb_fw_status;
void hd_sleep_fct(void)
{
    int power_mode;
    
    if(kusb_fw_status)
    {
        hd_launchTimer(); // we have enable the usb => keep the timer running
        return;
    }
    
    power_mode=getPowerMode();
       
    if(timer_status[HD_TIMER][power_mode])
    {
        kernel_doCmd(CMD_ATA_SLEEP);
    }
}

void energy_lowPower(void)
{
    tmr_stop(&lcdOnOff_timer);
    //lcd_disable();
    kernel_doCmd(CMD_LCD_DISABLE);
    if(FM_is_connected())
        FM_lightsOFF();
    if(!kusb_fw_status)
    {
        tmr_stop(&hd_timer);
        kernel_doCmd(CMD_ATA_SLEEP);
    }
}

void energy_chgMode(int power_mode)
{
    /* first stop all timer */
    tmr_stop(&halt_timer);
    tmr_stop(&lcdOnOff_timer);
    tmr_stop(&hd_timer);
    
    /* launch any timer according to new power mode*/
    halt_launchTimer();
    lcd_launchTimer();
    hd_launchTimer();

    printk("[power] DC changed (tick=%d)=> changing timers (LCD:%d,HALT:%d,HD:%d)\n",
            tick,
            timer_status[LCD_TIMER][power_mode],
            timer_status[HALT_TIMER][power_mode],
            timer_status[HD_TIMER][power_mode]);
}

#ifdef TARGET_TYPE_STD
void energy_loadPref(void)
{
    CFG_DATA * cfg;
    int i,j;
    int needWrite=0;
    char tmpStr[50];
       
    cfg=cfg_readFile("/medios/medios.cfg");
           
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        /* creating default */
        for(i=0;i<3;i++)
            for(j=0;j<2;j++)
            {
                timer_status[i][j]=default_status[i][j];
                timer_delay[i][j]=default_delay[i][j];
            }
        
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            halt_launchTimer();
            lcd_launchTimer();
            hd_launchTimer();
            return;
        }
        
        for(i=0;i<3;i++)
            for(j=0;j<2;j++)
            {   
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,timer_status[i][j]);
                sprintf(tmpStr,"energy_%s_%s_delay",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,timer_delay[i][j]);
            } 
        needWrite=1;   
    }
    else
    {
        for(i=0;i<3;i++)
            for(j=0;j<2;j++)
            {   
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                if(cfg_itemExists(cfg,tmpStr))
                {
                    timer_status[i][j]=cfg_readInt(cfg,tmpStr);
                }
                else
                {
                    timer_status[i][j]=default_status[i][j];
                    cfg_writeInt(cfg,tmpStr,timer_status[i][j]);
                    needWrite=1;
                }
                    
                sprintf(tmpStr,"energy_%s_%s_delay",timer_type[i],power_type[j]);
                if(cfg_itemExists(cfg,tmpStr))
                {
                    timer_delay[i][j]=cfg_readInt(cfg,tmpStr);
                }
                else
                {
                    timer_delay[i][j]=default_delay[i][j];
                    cfg_writeInt(cfg,tmpStr,timer_delay[i][j]);
                    needWrite=1;
                }
            }
    }
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    halt_launchTimer();
    lcd_launchTimer();
    hd_launchTimer();
}
#endif

#ifdef TARGET_TYPE_AVLO
void energy_loadPref(void)
{
    int i,j;
    for(i=0;i<3;i++)
        for(j=0;j<2;j++)
        {
            timer_status[i][j]=default_status[i][j];
            timer_delay[i][j]=default_delay[i][j];
        }
    halt_launchTimer();
    lcd_launchTimer();
    hd_launchTimer();
}
#endif

void energy_init(void)
{
    int i,j;
    tmr_setup(&lcdOnOff_timer,"lcdOnOff");
    lcdOnOff_timer.action = lcd_off_action;
    tmr_setup(&halt_timer,"halt");
    halt_timer.action = halt_action;
    tmr_setup(&hd_timer,"HD");
    hd_timer.action = hd_sleep_fct;
    
    for(i=0;i<3;i++)
        for(j=0;j<2;j++)
            timer_status[i][j]=timer_delay[i][j]=0;

    printk("[init] energy\n");
}
