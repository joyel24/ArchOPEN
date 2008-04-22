/*
* status_line.c
*
* linav - http://linav.sourceforge.net
* Copyright (c) 2004 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/
#include <lib/string.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <kernel/kernel.h>
#include <kernel/version.h>
#include <kernel/evt.h>

#include <driver/time.h>
#include <driver/lcd.h>
#include <driver/usb_fw.h>
#include <driver/batDc.h>
#include <driver/cf.h>
#include <driver/speaker.h>
#include <driver/fm_remote.h>
#include <driver/osd.h>

#include <gfx/graphics.h>

#include <gui/icons.h>

int date_format;
int time_format;

#include <gui/status_line.h>
#include <gui/shell.h>

#include <fs/cfg_file.h>

BITMAP * st_fwExtIcon;
BITMAP * st_intHPIcon;
BITMAP * st_cfIcon;
BITMAP * st_usbIcon;
BITMAP * st_powerIcon;
BITMAP * st_mediosLogo;
BITMAP * st_usbIn;
BITMAP * st_usbOut;
BITMAP * st_cfIn;
BITMAP * st_cfOut;
BITMAP * st_dcIn;
BITMAP * st_dcOut;

ICON tmpIco;

int batteryRefresh=0;
int batteryRefreshValue = 10;
int pwrState=0;
int usbState=0;
int fwExtState=0;
int intHPState=0;
int cfState=0;
int power = 0;
int color = 0;
int level = 0;
int chargeProgress = 0;
int st_PopUp=0;
int st_StatusLine=0;

int initDone=0;

#define HOUR(HH)       (time_format==FORMAT_12?(HH<=12?HH:(HH-12)):HH)
#define AMPM_ADD(HH)   time_format==FORMAT_12?HH<=12?"AM":"PM":""
#define DATE1(DD,MM)   (date_format==FORMAT_DDMMYYYY?DD:MM)
#define DATE2(DD,MM)   (date_format==FORMAT_DDMMYYYY?MM:DD)

extern struct statusLine_data lineData;

int clk_w=0,clk_h=0;

/* draw the current time */
void drawTime(void)
{
    char timeSt[50];
    struct med_tm  date_time;

    if(time_get(&date_time)==MED_OK)
    {
        if(lineData.has_date)
            sprintf(timeSt,"%02d:%02d%s %02d/%02d/%04d",HOUR(date_time.tm_hour),date_time.tm_min,
                    AMPM_ADD(date_time.tm_hour),
                    DATE1(date_time.tm_mday,date_time.tm_mon),
                    DATE2(date_time.tm_mday,date_time.tm_mon),
                    date_time.tm_year);
        else
            sprintf(timeSt,"%02d:%02d%s",HOUR(date_time.tm_hour),date_time.tm_min,
                    AMPM_ADD(date_time.tm_hour));
        gfx_fillRect(lineData.bg_color,lineData.clk_x,lineData.clk_y,clk_w,clk_h);
        gfx_getStringSize(timeSt,&clk_w,&clk_h);
        gfx_fontSet(STD6X9);
        gfx_putS(lineData.clk_color,lineData.bg_color,lineData.clk_x,lineData.clk_y,timeSt);
    }

}
int first_plug_dc;
int old_level;

void drawBat(void)
{
    int w,h;
    
    if(pwrState == 0)
        batteryRefreshValue = 10;
    else
        batteryRefreshValue = 0;

    if(pwrState == 0)
    {
        power = batLevel();
        
        if(power!=-1)
        {
            if(power < 2)
                color = COLOR_DARK_RED;
            else if(power < 4)
                color = COLOR_RED;
            else if(power < 6)
                color = COLOR_ORANGE2;
            else
                color = COLOR_GREEN;
    
            if(power > 2)
                level = (int)(power - 1) * 3;
            if(level > 21)
                level = 21;
        }
    }
    else if(pwrState == 1)
    {
        if(chargeProgress == 0)
            level = 0;
        else if(chargeProgress == 1)
            level = 7;
        else if(chargeProgress == 2)
            level = 14;
        else
            level = 21;

        if(chargeProgress < 3)
            chargeProgress++;
        else
            chargeProgress = 0;

        color = COLOR_GREEN;
    }

    gfx_drawRect(COLOR_BLACK,lineData.bat_x,lineData.bat_y,23,10);
    gfx_fillRect(COLOR_BLACK,lineData.bat_x+23,lineData.bat_y+2,3,6);
    gfx_fillRect(lineData.bg_color,lineData.bat_x+1,lineData.bat_y+1,21,8);
    if(pwrState == 1 || (pwrState == 0 && power!=-1))
        gfx_fillRect(color,lineData.bat_x+1,lineData.bat_y+1,level,8);
    else
    {
        gfx_getStringSize("?", &w, &h);
        gfx_putS(COLOR_GREEN,lineData.bg_color,lineData.bat_x+(23-w)/2,lineData.bat_y+(10-h)/2,"?");
    }

    if(FM_is_connected()) /* show bat status on FM remote */
    {
        if(!pwrState)
        {
            first_plug_dc=1;
            if(level!=old_level)
            {
                old_level=level;
                if(level<7)
                    FM_setIcon(FM_BAT,1);
                else if(level<14)
                    FM_setIcon(FM_BAT,2);
                else if(level<20)
                    FM_setIcon(FM_BAT,3);
                else
                    FM_setIcon(FM_BAT,4);
            }
        }
        else
        {
            if(first_plug_dc)
            {
                FM_setIcon(FM_BAT,0);
                first_plug_dc=0;
            }
        }
    }

}

void drawStatus(void)
{
    if(fwExtState && cfState)
    {
        fwExtState=0;
        cfState=0;
        printk("FW and CF detected !!!\n");
    }

    if(fwExtState)
        gfx_drawBitmap(st_fwExtIcon, lineData.module_x, lineData.module_y);

    if(cfState)
        gfx_drawBitmap(st_cfIcon, lineData.module_x, lineData.module_y);

    if(!cfState && !fwExtState)
        gfx_fillRect(lineData.bg_color,lineData.module_x,lineData.module_y,15,6);

    if(speaker_available())
    {
        if(intHPState)
            gfx_drawBitmap(st_intHPIcon, lineData.intHP_x, lineData.intHP_y);
        else
            gfx_fillRect(lineData.bg_color,lineData.intHP_x,lineData.intHP_y,8,10);
    }

    if(pwrState)
        gfx_drawBitmap(st_powerIcon, lineData.pwr_x, lineData.pwr_y);
    else
        gfx_fillRect(lineData.bg_color,lineData.pwr_x,lineData.pwr_y,15,6);

    if(usbState)
        gfx_drawBitmap(st_usbIcon, lineData.usb_x, lineData.usb_y);
    else
        gfx_fillRect(lineData.bg_color,lineData.usb_x,lineData.usb_y,15,6);
}

void drawLogo(void)
{
    gfx_drawBitmap(st_mediosLogo, lineData.logo_x, lineData.logo_y);
}

void drawStatusLine(void)
{
    int w = 0;
    int h = 0;
    gfx_fontSet(STD6X9);

    gfx_getStringSize("M", &w, &h);

    /* blue background */
    gfx_fillRect(lineData.bg_color,lineData.x,lineData.y,lineData.w,h+6);

    /* nice little shadow */
    gfx_drawLine(COLOR_BLACK,lineData.x,lineData.y+h+5,lineData.w-1,h+5);
    gfx_drawLine(COLOR_DARK_GREY,lineData.x+0,lineData.y+h+6,lineData.w-1,h+6);
    gfx_drawLine(COLOR_LIGHT_GREY,lineData.x+0,lineData.y+h+7,lineData.w-1,h+7);

    /* show version */
    gfx_putS(lineData.ver_color,lineData.bg_color,lineData.ver_x,lineData.ver_y,VERSION_NUMBER);

    /* and time, and battery */
    drawTime();
    drawBat();
    drawStatus();
    drawLogo();
}

void drawPopup(BITMAP * cur_UsbIco)
{
    int curState;
       
        
    if(!cur_UsbIco)
    {
        printk("Missing icon\n");
        return;   
    }
    
    gfx_planeSetSize(BMAP2,cur_UsbIco->width+2,cur_UsbIco->height+2,8,0);
    gfx_planeSetPos(BMAP2,SCREEN_ORIGIN_X+(LCD_WIDTH-cur_UsbIco->width-2),
                    SCREEN_ORIGIN_Y+(LCD_HEIGHT-cur_UsbIco->height-2)/2);
    gfx_setPlane(BMAP2);
    gfx_clearScreen(COLOR_TRSP);
    gfx_drawRect(COLOR_RED,0,0,cur_UsbIco->width+2,cur_UsbIco->height+2);
    gfx_drawBitmap(cur_UsbIco,1,1);
    curState=gfx_planeGetState(BMAP2);
    gfx_planeSetState(BMAP2,OSD_BITMAP_TRSP_ENABLE(curState,1));
    curState=gfx_planeGetState(BMAP2);
    gfx_planeSetState(BMAP2,OSD_BITMAP_SET_BLEND_FACTOR(curState,OSD_BITMAP_BLEND_FACTOR_MAX));
    gfx_planeShow(BMAP2);
    st_PopUp=1;
    gfx_setPlane(BMAP1);
}

/* events */
void statusLine_handleEvent(int evt)
{
    struct evt_t myEvt;
    switch (evt) {
        case EVT_REDRAW:
            pwrState=POWER_CONNECTED;
            batteryRefresh = 0;
            usbState=kusbIsConnected();
            fwExtState=kFWIsConnected();
            cfState=CF_IS_CONNECTED;
            intHPState=speaker_state();
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawStatusLine();
#endif
            break;
#ifndef ALWAYS_DISPLAY_ST_LINE
        case BTN_F2:
            if(st_StatusLine==0)
            {
                drawStatusLine();
                st_StatusLine=1;
            }
            break;
#endif
        case EVT_TIMER:            
            if(st_PopUp==2) /* second time we are here, hide popup*/
            {
                gfx_planeHide(BMAP2);
                st_PopUp=0;
            }
            if(st_PopUp==1) /* if it is the first time we are here with popup enable, skip hide*/
            {
                st_PopUp=2;
            }
#ifndef ALWAYS_DISPLAY_ST_LINE            
            if(st_StatusLine==2) /* second time we are here, hide popup*/
            {
                /*hiding status line with a redraw */
                myEvt.evt=EVT_REDRAW;
                myEvt.evt_class=GUI_CLASS;
                myEvt.data=(void*)0;
                evt_send(&myEvt);
                st_StatusLine=0;
            }
            if(st_StatusLine==1) /* if it is the first time we are here with popup enable, skip hide*/
            {
                st_StatusLine=2;
            }
#endif
                        
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawTime();
            if(batteryRefresh == 0)
                drawBat();
            if(batteryRefresh < batteryRefreshValue)
                batteryRefresh++;
            else
                batteryRefresh = 0;
#endif
            break;
        case EVT_PWR_IN:
        case EVT_PWR_OUT:
            pwrState=POWER_CONNECTED;
            batteryRefresh = 0;
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawStatus();
#endif
            drawPopup(evt==EVT_PWR_IN?st_dcIn:st_dcOut);
            break;
        case EVT_USB_IN:
        case EVT_USB_OUT:
            usbState=kusbIsConnected();
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawStatus();
#endif
            drawPopup(evt==EVT_USB_IN?st_usbIn:st_usbOut);
            break;
        case EVT_FW_EXT_IN:
        case EVT_FW_EXT_OUT:
            fwExtState=kFWIsConnected();
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawStatus();
#endif
            drawPopup(evt==EVT_FW_EXT_IN?st_usbIn:st_usbOut);
            break;
        case EVT_CF_IN:
        case EVT_CF_OUT:
            cfState=CF_IS_CONNECTED;
#ifdef ALWAYS_DISPLAY_ST_LINE
            drawStatus();
#endif
            drawPopup(evt==EVT_CF_IN?st_cfIn:st_cfOut);
            break;
    }
}


#define LOAD_OTHER_ICO(NAME,FNAME,VAR) { \
    tmpIco=icon_get(NAME);                \
    if(!tmpIco)                             \
        tmpIco=icon_loadOther(FNAME);  \
    if(!tmpIco) VAR=NULL;                   \
    VAR=&tmpIco->bmap_data;                 \
}

void statusLine_init(void)
{
    CFG_DATA * cfg;

    if(initDone) return;
    
    /* get icons */
    st_fwExtIcon=&icon_get("fwExtIcon")->bmap_data;
    st_intHPIcon=&icon_get("intHPIcon")->bmap_data;
    st_cfIcon=&icon_get("cfIcon")->bmap_data;
    st_usbIcon=&icon_get("usbIcon")->bmap_data;
    st_powerIcon=&icon_get("powerIcon")->bmap_data;
    st_mediosLogo=&icon_get("mediosLogo")->bmap_data;

    LOAD_OTHER_ICO("usb_in","usb_in.ico",st_usbIn);
    LOAD_OTHER_ICO("usb_out","usb_out.ico",st_usbOut);
    
    LOAD_OTHER_ICO("dc_in","dc_in.ico",st_dcIn);
    LOAD_OTHER_ICO("dc_out","dc_out.ico",st_dcOut);
    
    LOAD_OTHER_ICO("cf_in","cf_in.ico",st_cfIn);
    LOAD_OTHER_ICO("cf_out","cf_out.ico",st_cfOut);
        
    st_PopUp=0;
    st_StatusLine=0;
    
    pwrState=POWER_CONNECTED;
    usbState=kusbIsConnected();
    fwExtState=kFWIsConnected();
    intHPState=speaker_state();

    first_plug_dc = 0;
    old_level = -1;

    /* read cfg */
    cfg=cfg_readFile("/medios/medios.cfg");

    if(!cfg)
    {
        printk("Can't open cfg file\n");
        time_format=FORMAT_24;
        date_format=FORMAT_DDMMYYYY;
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }
        cfg_writeInt(cfg,"is12H",time_format);
        cfg_writeInt(cfg,"isMMDDYYYY",date_format);
        cfg_writeFile(cfg,"/medios/medios.cfg");
    }
    else
    {
        time_format=cfg_readInt(cfg,"is12H");
        date_format=cfg_readInt(cfg,"isMMDDYYYY");
    }

    cfg_clear(cfg);

    cfState=CF_IS_CONNECTED;
    initDone=1;
}
