/*
*   kernel/driver/fm_remote.c
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
#include <sys_def/evt.h>

#include <kernel/evt.h>
#include <kernel/timer.h>
#include <kernel/cmd_line.h>
#include <kernel/delay.h>

#include <driver/fm_remote.h>
#include <driver/buttons.h>
#include <driver/energy.h>
#include <driver/uart.h>
#include <driver/lcd.h>

#define INI_TXT       "--mediOS--"

int inHold=0;

int light_state=0,save_lights=0;
int lights_OnOff=1;

struct txt_data {
    char txt[256];
    int scroll;
    int pos;
    int len;
    int scroll_inc;
};

#define NORMAL_TXT   0
#define TMP_TXT      1

struct txt_data txt[2];

int cur_txt=NORMAL_TXT;
int txt_scroll_count=0;
int tmp_iter=0;
int tmp_cnt;

int txt_scroll_ctrl=1;

int contrast=0;
int bat_state=0,vol_state=0,icons=0;

int bat_state_array[]={0x0000,0x0002,0x000A,0x000E,0x001E};
int vol_state_array[]={0x0000,0x0004,0x0006,0x0007,0x0807,0x0C07,0x0D07};

int key_evt_array[NB_KEY][2] = {
    { BTN_FM_REC   , 0x20 }, /* REC */
    { -1           , 0x80 }, /* HOLD => no event*/
    { BTN_FM_MP3FM , 0x10 }, /* MP3_FM */
    { BTN_UP       , 0x08 }, /* UP */
    { BTN_DOWN     , 0x40 }, /* DOWN */
    { BTN_LEFT     , 0x04 }, /* LEFT */
    { BTN_RIGHT    , 0x01 }, /* RIGHT */
    { BTN_ON       , 0x02 }  /* PRESS */
};

struct tmr_s fmRemote_tmr;

int FM_connected=0;
int nbPingSend=0;
char radio_param[5];
int keepTmr;

char cmd=0;
int fm_index;
int nb_chk;
int nbGetV;

/***************************************
* Manages button press
****************************************/ 
void processKey(int key)
{
    int i;
    
    printk("key press: %x\n",key);
    
    /* special process for HOLD key when no event is associated with it
        => changing only the inHold variable */    
    if(key & 0x80 && key_evt_array[1][0]==-1)
    {
        inHold=~inHold;        
        printk("[FM key] hold %s\n",inHold?"enable":"disable");
        return;
    }
    
    for(i=0;i<NB_KEY;i++)
    {
        if(key_evt_array[i][0]!=-1 && (key & key_evt_array[i][1]) )
        {     
            struct evt_t _evt;           
            if(lcd_enabled()==0)
            {
                /* the lcd is off => turn on and discard the event */
                lcd_keyPress();
                break;
            }
            else
                lcd_launchTimer(); /* postpone the lcd timer */
            halt_launchTimer(); /* postpone the poweroff timer */
            _evt.evt= key_evt_array[i][0];
            _evt.evt_class=BTN_CLASS;
            _evt.data=NULL;
            evt_send(&_evt);
        }
    }
    
}

/***************************************
* Turn off backlight & button light
****************************************/
void FM_lightsOFF(void)
{
    if(lights_OnOff==1)
    {    
        save_lights=light_state;
        light_state=0;
        lights_OnOff=0;
        FM_do_setLight();
    }
}

/***************************************
* Turn on backlight & button light
****************************************/
void FM_lightsON(void)
{
    if(lights_OnOff==0)
    {    
        light_state=save_lights;
        lights_OnOff=1;
        FM_do_setLight();
    }
}

/***************************************
* Returns backlight & button light state
****************************************/
int FM_lightsState(void)
{
    return lights_OnOff;
}

/***************************************
* Change state of backlight and button light
* => use macro instead of this function
****************************************/
void FM_setLight(int type,int direction)
{
    if(direction)
        if(lights_OnOff)
            light_state |= (0x1 << type);
        else
            save_lights |= (0x1 << type);
    else
        if(lights_OnOff)
            light_state &= ~(0x1 << type);
        else
            save_lights &= ~(0x1 << type);
     if(lights_OnOff)       
        FM_do_setLight();
}

/***************************************
* Send light info to FM remote
* P cmd
****************************************/
void FM_do_setLight(void)
{
    uart_out('P',FM_REMOTE_UART);
    uart_out(light_state&0xFF,FM_REMOTE_UART);
}

/***************************************
* Get state of backlight and button light
* => use macro instead of this function
****************************************/
int FM_getLight(int type)
{
    return ((light_state & (0x1 << type)) != 0);
}

void FM_setScroll(int OnOff)
{
    if(txt_scroll_ctrl && !OnOff)
        txt[cur_txt].pos=0;
    txt_scroll_ctrl=OnOff;
}

int FM_getScroll(void)
{
    return txt_scroll_ctrl;
}

void FM_getText(char * str)
{    
    strcpy(str,txt[cur_txt].txt);
}

void FM_put_iniTxt(void)
{
    FM_savText(INI_TXT,NORMAL_TXT);
    cur_txt=NORMAL_TXT;
    //FM_do_putText();
}

void FM_putText(char * str)
{
    FM_savText(str,NORMAL_TXT);
    if(cur_txt==NORMAL_TXT)
        FM_do_putText();
}

void FM_putTmpText(char * str,int iter)
{
    #warning need something for tmp txt and scroll
    FM_savText(str,TMP_TXT);
    tmp_iter=iter;
    tmp_cnt=0;
    cur_txt=TMP_TXT;
    FM_do_putText();
}

void FM_savText(char * str,int num)
{
    int i=0;
    int len=strlen(str);
    
    if(len>256)
    {
        strncpy(txt[num].txt,str,256);
        len=256;
    }
    else
        strcpy(txt[num].txt,str);
        
    if(len<11)
    {
        for(i=len;i<11;i++)
        {
            txt[num].txt[i]=' ';
        }
        txt[num].txt[11]='\0';
        txt[num].scroll=0;        
    }
    else
    {
        txt[num].len=len;
        txt[num].scroll=1;
        txt[num].scroll_inc=1;        
    }
    
    txt[num].pos=0;    
}

void FM_do_putText(void)
{
    FM_send_data('T',&txt[cur_txt].txt[txt[cur_txt].pos],11);
            
    if(txt[cur_txt].scroll && txt_scroll_ctrl)
    {
        txt[cur_txt].pos+=txt[cur_txt].scroll_inc;
        if(txt[cur_txt].pos<0 || txt[cur_txt].pos+11>txt[cur_txt].len)
        {
            txt[cur_txt].scroll_inc*=-1;
        }
    }
        
}

void FM_setContrast(int val)
{
    contrast=val;
    uart_out('C',FM_REMOTE_UART);
    uart_out(contrast&0xFF,FM_REMOTE_UART);
}

int FM_getContrast(void)
{
    return contrast;
}

void FM_setIcon(int icon,int val)
{
    switch(icon)
    {
        case FM_BAT:
            if(val>=0 && val<5)
                bat_state=val;
            break;
        case FM_PLAY:
        case FM_REC:
        case FM_PAUSE:
        case FM_LOOP:
        case FM_1PLAY:
            if(val>=0)
            {
                FMSetIcon(icon,icons);
            }
            else
            {
                FMClearIcon(icon,icons);
            }
            break;
        case FM_VOL:
            if(val>=0 && val<7)
                vol_state=val;
            break;
        default:
            return;
    }
    FM_do_setIcon();
}

int FM_getIcon(int icon)
{
    switch(icon)
    {
        case FM_BAT:
            return bat_state;
        case FM_PLAY:
        case FM_REC:
        case FM_PAUSE:
        case FM_LOOP:
        case FM_1PLAY:
            return FMIsSetIcon(icon,icons);
        case FM_VOL:
            return vol_state;
            break;
        default:
            return -1;
    }
}

void FM_do_setIcon(void)
{
    char icon_state[11]={0,0,0,0,0,0,0,0,0,0,0};
    
    /* bat */
    icon_state[0]=bat_state_array[bat_state]&0xFF;
    //icon_state[1]=(bat_state_array[bat_state]>>8)&0xFF;
    /* play */
    if(FMIsSetIcon(FM_PLAY,icons))
        icon_state[3]=0x10;
    /* rec */
    if(FMIsSetIcon(FM_REC,icons))
        icon_state[4]=0x10;
    /* pause */
    if(FMIsSetIcon(FM_PAUSE,icons))
        icon_state[5]=0x02;
    /* loop */
    if(FMIsSetIcon(FM_LOOP,icons))
        icon_state[6]=0x02;
    /* 1 play */
    if(FMIsSetIcon(FM_1PLAY,icons))
        icon_state[7]=0x01;
    /* bat */
    icon_state[9]=vol_state_array[vol_state]&0xFF;
    icon_state[10]=(vol_state_array[vol_state]>>8)&0xFF;    
      
    FM_send_data('I',icon_state,11);    
}

int FM_radio_search_mode=FM_IN_SEARCH_MODE;
int FM_radio_search_dir=FM_SEARCH_DOWN;
int FM_radio_main_mute=FM_ON;
int FM_radio_mic_State=FM_OFF;
int FM_radio_stby=FM_ON;
int fm_freq=87800;

void FM_do_turn_on_radio(void)
{
    printk("in turn on radio %d\n",fm_freq);
    FM_radio_search_mode=FM_IN_SET_MODE;
    FM_radio_main_mute=FM_OFF;
    FM_radio_stby=FM_OFF;
    FM_radio_mic_State=FM_OFF;
    FM_do_setRadio();
    /* ask MAS to turn on line in */
   // mas_line_in_on();
}

void FM_do_turn_off_radio(void)
{
    printk("in turn off radio %d\n",fm_freq);
    FM_radio_main_mute=FM_ON;
    FM_radio_stby=FM_ON;
    FM_do_setRadio();
    /* ask MAS to turn off line in */
    //mas_line_in_off();
}

void FM_do_turn_on_mic(void)
{
    FM_radio_main_mute=FM_ON;
    FM_radio_stby=FM_ON;
    FM_radio_mic_State=FM_OFF;
    FM_do_setRadio();
    /* ask MAS to turn on line in */
    //mas_line_in_on();
}

void FM_do_turn_off_mic(void)
{
    FM_radio_mic_State=FM_ON;
    FM_do_setRadio();
    /* ask MAS to turn off line in */
    //mas_line_in_off();
}

void FM_do_setRadio(void)
{
    char radio_data[5]={0,0,0,0,0};
    int PLL;
    
    /* byte 0 */
    radio_data[0] |= (FM_radio_main_mute & 0x1) << 7;
    radio_data[0] |= (FM_radio_search_mode & 0x1) << 6;
    
#warning we need bound for freq
    if(fm_freq<(60*1000) || fm_freq > (200*1000))
        fm_freq=100*1000;
    
    /* compute PLL, assuming we have XTAL of 13MHz, inter freq of 225KHz and HIGH injection*/
    
    PLL=(4*(fm_freq+225))/50;
    PLL&=0x3FFF;
    radio_data[0] |= (PLL >> 8) & 0x3F;
    radio_data[1] = PLL & 0xFF;
    
    radio_data[2] |= (FM_radio_search_dir & 0x1) << 7;
    /* search stop level default to HIGH */
    radio_data[2] |= 0x0  << 5;
    /* HIGH injection */
    radio_data[2] |= 0x1  << 4;
    /* Stereo */
    radio_data[2] |= 0x0  << 3;
    /* unMute right */
    radio_data[2] |= 0x0  << 2;
    /* unMute left */
    radio_data[2] |= 0x0  << 1;
    
    radio_data[2] |= 0x1;
    //radio_data[2] |= (FM_radio_mic_State & 0x1);
    
    /* setting default val for byte 3 : 0x0E  see doc for details */
    radio_data[3] = 0x0E;
    
    /* setting default val for byte 4 : 0x40  see doc for details */
    radio_data[4] = 0x40;
    
    printk("R: %02x %02x %02x %02x %02x\n",radio_data[0],radio_data[1],radio_data[2],radio_data[3],radio_data[4]);
    
    FM_send_data('R',radio_data,5);
}

void FM_do_ini_call(void)
{
    FM_do_setLight();
    FM_do_putText();
    FM_setContrast(contrast);
    FM_do_setIcon();
}

void FM_send_data(char cmd,char * data,int size)
{
    int i;
    uart_out(cmd,FM_REMOTE_UART);
    for(i=0;i<size;i++)
        uart_out(data[i]&0xFF,FM_REMOTE_UART);
}

int FM_is_connected(void)
{
    return FM_connected;
}

void fm_remote_INT(int irq_num,struct pt_regs * reg)
{
    unsigned char c;
    while(uart_in(&c,FM_REMOTE_UART))
    {
        if(FM_connected)
        {
            /* in connected state */
            switch(c)
            {        
                case 'V':
                    /* received a pong => reset nbPingSet*/
                    fm_index=0;
                    nbPingSend=0;
                    cmd=0;
                    break;
                case 'K':
                    /* button press => cmd 1*/
                    cmd=1;
                    fm_index=0;
                    break;
                case 'R':
                    /* radio => cmd 2*/
                    cmd=2;
                    fm_index=0;
                    break;
                default:
                    switch(cmd)
                    {
                        /* if cmd was set, we are now managing
                         the data/parameter of the cmd*/
                        case 1:   
                            /* button press */                         
                            if(c!=0)
                                processKey(c);
                            cmd=0;
                            break;
                        case 2:
                            /* radio data */
                            radio_param[fm_index++]=c; /* storing the data*/
                            if(fm_index==5)
                            {
                                /* we get the 5 char of data*/
                                printk("[FM Remote] get radio cmd: %x%x%x%x%x\n",radio_param[0],radio_param[1]
                                    ,radio_param[2],radio_param[3],radio_param[4]);
                                cmd=0;
                            }
                            break;
                        default:
                            /*no cmd set*/
                            if(c!=0 && c!=0x3)
                            { /* printing UKN char, char 0x3 is send at the end of cmd */
                                printk("NO cmd - UNK char : %x\n",c);
                            }
                            cmd=0;
                            break;
                    }
                    break;
            }
        }
        else
        {
            /* not in connected state
            and we've received a char*/
            if(fmRemote_tmr.trigger!=1)
            {
                /*ping/pong timer not runing
                => launch it and init some var*/
                nbGetV=0;
                nbPingSend=0;
                tmr_start(&fmRemote_tmr);
            }
            switch(c)
            {
                case 'V':
                    FM_connected=0;
                    /* we've received a pong
                    => inc nbGetV & set nbPingSend=0*/
                    nbGetV++; 
                default:
                    /* nothing to do*/
                    break;
            }
        }
    }        
}

void fmRemote_chk(void)
{
    if(FM_connected)
    {
        /* manage TMP txt: if tmp txt is set,
        show/hide the txt every time the fction is
        called, tmp_iter is used to measure the time,
        the tmp txt is shown*/
        if(cur_txt==TMP_TXT)
        { 
            tmp_iter--;
            if(tmp_iter<0)
            {
                /* we have shown tmp txt enough time,
                put back the saved txt*/
                cur_txt=NORMAL_TXT;
                FM_do_putText();
            }
            else
            {
                if(tmp_cnt)
                {
                    tmp_cnt=0;
                    FM_do_putText();
                }
                else
                {
                    tmp_cnt=1;
                    FM_send_data('T',"           ",11);
                }
            }            
        }
    }
    
    /* timer is running at a higher freq to manage
    tmp text, we need to send/check ping/pong only 
    every N times this fction is called*/ 
    if(nb_chk>2)
    {
        nb_chk=0;
        if(FM_connected)
        {
            nbPingSend++;
            if(nbPingSend>MAX_PING)
            {
                /* we didn't get pong answer */
                FM_connected=0;
                nbPingSend=0;
                inHold=0;
                nbGetV=0;
                if(fmRemote_tmr.trigger==1)
                    tmr_stop(&fmRemote_tmr);
                printk("[FM Remote] disconnected\n");
            }
        }
        else
        {
            /*not connected yet */
            if(nbGetV>1)
            {
                /* we received enough pong (V)
                => initial call, going to connected state*/
                cmd=0;
                fm_index=0;
                FM_connected=1;
                nbPingSend=0;
                inHold=0;   
                nbGetV=0;
                FM_do_ini_call();   
                printk("[FM Remote] connected\n");
            }
            nbPingSend++;
            if(nbPingSend>2)
            {
                /*we've received a least 1 char from remote,
                but not enough pong (V)
                => stop sending ping (v) until next char*/ 
                nbPingSend=0;
                if(fmRemote_tmr.trigger==1)
                    tmr_stop(&fmRemote_tmr);
                nbGetV=0;
            }
        }
        uart_out('v',FM_REMOTE_UART);
    }
    else
        nb_chk++;
}

int fm_state;

void FM_enable(void)
{
    char c;
    
    if(FM_REMOTE_UART==CMD_LINE_UART || FM_REMOTE_UART== DEBUG_UART)
    {
        cmd_line_disable();
        printk_uartDisable();
    }
    
    uart_changeSpeed(9600,FM_REMOTE_UART);
    
    /* initiale state */
    
    FM_connected=0;
    nbGetV=0;
    nb_chk=0;
    irq_changeHandler(UART_IRQ_NUM(FM_REMOTE_UART),fm_remote_INT);
    /* clear uart buffer in */
    while(uart_in(&c,FM_REMOTE_UART)) /*nothing*/;
    
    /*trying to see if remote is already here*/
    uart_out('v',FM_REMOTE_UART);
    mdelay(5);
    irq_enable(UART_IRQ_NUM(FM_REMOTE_UART));
    fm_state=1;
}

void FM_disable(void)
{
    fm_state=0;
    if(fmRemote_tmr.trigger==1)
        tmr_stop(&fmRemote_tmr);
    uart_restoreIrqHandler(UART_IRQ_NUM(FM_REMOTE_UART));
    irq_disable(UART_IRQ_NUM(FM_REMOTE_UART));
    if(FM_REMOTE_UART==CMD_LINE_UART || FM_REMOTE_UART== DEBUG_UART)
    {
        cmd_line_enable();
        printk_uartEnable();
    }
}

int FM_getState(void)
{
    return fm_state;
}

void FM_init(void)
{
    inHold=0;
    fm_state=0;
    fm_index=0;
    FM_connected=0;
    keepTmr = 0;
    
    tmr_setup(&fmRemote_tmr,"FM remote remove chk");
    fmRemote_tmr.action = fmRemote_chk;
    fmRemote_tmr.freeRun = 1;
    fmRemote_tmr.stdDelay=HZ>>2; /* 0.5s period */
    
#if FM_VER==1
        CPLD_SET_PORT3(CPLD_FM);
#endif

    light_state=0x1;
    FM_put_iniTxt();    
    contrast=0x00;
    printk("[init] fm remote\n");
}


/***************************************
* Test function to show in loop all icons
****************************************/
void test_icons(void)
{
    static int count=0;
    static int icon_num=0;
    count++;
    if(count > 20)
    {
        count=0;
        bat_state++;
        if(bat_state==5)
            bat_state=0;
        vol_state++;
        if(vol_state==7)
            vol_state=0;
        
        if(icon_num==0)
        {
            FMClearIcon(0x4,icons);
        }
        else
        {
            FMClearIcon(icon_num-1,icons);
        }
            
        FMSetIcon(icon_num,icons);
        icon_num++;
        if(icon_num==5)
            icon_num=0;
        FM_do_setIcon();
    }    
}
