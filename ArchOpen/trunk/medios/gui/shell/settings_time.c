/*
*   kernel/gfx/gui/shell/settings/clock.c
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
#include <kernel/lang.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/spinbox.h>
#include <gui/button.h>
#include <gui/checkbox.h>
#include <gui/chooser.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/status_line.h>
#include <gui/settings_time.h>

#include <driver/time.h>
#include <driver/lcd.h>

#include <fs/cfg_file.h>

//#define TIME_DATE_GUIFONT RADONWIDE
#define TIME_DATE_GUIFONT RADONWIDE

int isPm=0;
int PmPos_x;
int PmPos_y;
int curH;
int stop_clk_set=0;
SPINBOX time[3];
SPINBOX date[3];

CHECKBOX hourFormat;
CHOOSER dateFormat;

WIDGETLIST menuList;  

void checkbox_12_24_onChange(CHECKBOX t)
{
    if(t->checked)
    {
        /*we want 12h*/
        curH=time[0]->getValue(time[0]);
        if(curH==0)
            curH=24;
        if(curH>12)
        {
            isPm=1;
            curH-=12;
            time[0]->setValue(time[0],curH);
        }
        else
        {
            isPm=0;
        }
        time[0]->setParam(time[0],1,12,1,2);
        time[0]->paint(time[0]);
        gfx_putS(time[0]->foreColor,time[0]->backColor,PmPos_x,PmPos_y+2,isPm?"PM":"AM");
    }
    else
    {
        /*we want 24h*/
        int H,W;
        time[0]->setParam(time[0],0,23,1,2);
        if(isPm)
        {         
            time[0]->setValue(time[0],time[0]->getValue(time[0])+12);            
            time[0]->paint(time[0]);
        }
        gfx_getStringSize("AM",&W,&H);
        gfx_fillRect(time[0]->backColor,PmPos_x,PmPos_y+2,W,H);
    }
}

void hour_onChange(SPINBOX t)
{
    if(hourFormat->checked)
    {
        int newH=time[0]->getValue(time[0]);
        if((newH == 12 && curH == 1)|| (newH == 1 && curH == 12))
            isPm=isPm?0:1;
        curH=newH;
        gfx_putS(time[1]->foreColor,time[1]->backColor,PmPos_x,PmPos_y+2,isPm?"PM":"AM");
    }
}

void chooser_date_onChange(CHOOSER t)
{
    int day=t->index==1?date[0]->value:date[1]->value;
    int month=t->index==1?date[1]->value:date[0]->value;
            
    date[0]->setParam(date[0],1,t->index==1?12:31,1,2);
    date[0]->value=t->index==1?month:day;
    
    date[1]->setParam(date[1],1,t->index==1?31:12,1,2);
    date[1]->value=t->index==1?day:month;
        
    date[0]->paint(date[0]);
    date[1]->paint(date[1]);
    
}

void okBtnClk_click(BUTTON b)
{
    struct med_tm now;
    CFG_DATA * cfg;
    now.tm_hour=time[0]->value;
    now.tm_min=time[1]->value;
    now.tm_sec=time[2]->value;
    if(dateFormat->index==1) /* MM/DD/YYYY */
    {
        now.tm_mday=date[1]->value;
        now.tm_mon=date[0]->value;
    }
    else
    {
        now.tm_mday=date[0]->value;
        now.tm_mon=date[1]->value;
    }
    now.tm_year=date[2]->value;
    
    if(time_set(&now)!=MED_OK)
    {
        msgBox_info("Write not supported");
        mdelay(1000);
    }
    
    msgBox_info(getLangStr(STRLNG_SAVE_SETTINGS));
    
    /* saving to cfg file */
    cfg=cfg_readFile("/medios/medios.cfg");
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }
    }
    time_format=hourFormat->checked?1:0;
    cfg_writeInt(cfg,"is12H",time_format);
    date_format=dateFormat->index==1?1:0;
    cfg_writeInt(cfg,"isMMDD",date_format);
    //cfg_printItems(cfg);
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    stop_clk_set=1;
}   

void resetBtnClk_click(BUTTON b)
{
    struct med_tm now;
    time_get(&now);
    time[0]->value=now.tm_hour;
    time[0]->setParam(time[0],time_format==FORMAT_24?0:1,time_format==FORMAT_24?23:12,1,2);
    time[1]->value=now.tm_min;
    time[2]->value=now.tm_sec;
    dateFormat->index=date_format==FORMAT_MMDDYYYY?1:0;
    hourFormat->checked=time_format;
    
    date[0]->setValue(date[0],date_format==FORMAT_DDMMYYYY?now.tm_mday:now.tm_mon);
    date[0]->setParam(date[0],1,date_format==FORMAT_DDMMYYYY?31:12,1,2);
    date[1]->setValue(date[1],date_format==FORMAT_DDMMYYYY?now.tm_mon:now.tm_mday);
    date[1]->setParam(date[1],1,date_format==FORMAT_DDMMYYYY?31:12,1,2);            
    
    date[2]->value=now.tm_year;
    menuList->paint(menuList);
}

#define ICON_X 5
#define ICON_Y 5

void clock_setting(void)
{
    struct med_tm now;
    ICON logo;
    //CFG_DATA * cfg;
    
    char * dateFormStr[2]={"DD/MM/YYYY","MM/DD/YYYY"};
    
    BUTTON mib;
            
    //FONT f=fnt_fontFromId(TIME_DATE_GUIFONT);
    
    int evtHandle;
    int event;
    int sepH,sepW,w,h;
    
    int minX,lineH,x,y,maxW;
    
    stop_clk_set=0;
    
    time_get(&now);
    printk("Cur time: ");
    time_print(&now);
    printk("\n");
   
    if(now.tm_hour>12)
        isPm=1;
        
    gfx_clearScreen(COLOR_TRSP);
    
           
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandle<0)
    {
        printk("Can't get evt handler\n");   
    }
    
    logo=icon_get("clock_icon");
    if(!logo)
        logo=icon_load("clock_icon.ico");
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    
    minX = ICON_X + logo->bmap_data.width;
    
    gfx_drawLine(COLOR_BLUE,minX+3,5,minX+3,LCD_HEIGHT-5);
    gfx_fontSet(TIME_DATE_GUIFONT);
    gfx_getStringSize("MM/DD/YYYY",&maxW,&lineH);
    minX+=3;   
    
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_CLOCK_SETTINGS),&w,&h);
    gfx_putS(COLOR_DARK_GREY,COLOR_TRSP,minX+(LCD_WIDTH-minX-w)/2,ICON_Y,getLangStr(STRLNG_CLOCK_SETTINGS));

    
    gfx_fontSet(TIME_DATE_GUIFONT);
    minX=(LCD_WIDTH-minX-(maxW+24))/2+minX;
    lineH=(lineH+10);
    printk("minX = %d(LCD=%d,max=%d,min=%d), lineH = %d\n",minX,LCD_WIDTH,maxW+24,
           ICON_X + logo->bmap_data.width+3,lineH);
    
    x=minX;
    y=ICON_Y+(LCD_HEIGHT-(5*lineH))/2;
    
    
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    // standardMenu
    
    time[0]=spinbox_create();
    time[0]->setFont(time[0],TIME_DATE_GUIFONT);
    time[0]->setParam(time[0],time_format==FORMAT_24?0:1,time_format==FORMAT_24?23:12,1,2);
    time[0]->setPos(time[0],x,y);
    time[0]->setValue(time[0],time_format==FORMAT_24?now.tm_hour:isPm?now.tm_hour-12:now.tm_hour);
    time[0]->onChange=(SPINBOX_CHANGEEVENT)hour_onChange;
    menuList->addWidget(menuList,time[0]);    
        
    gfx_getStringSize(":",&sepW,&sepH);
    
    x+=time[0]->width+1;
    gfx_putS(time[0]->foreColor,time[0]->backColor,x,y+2,":");
    x+=(sepW+1);    
        
    time[1]=spinbox_create();
    time[1]->setFont(time[1],TIME_DATE_GUIFONT);
    time[1]->setParam(time[1],0,59,1,2);
    time[1]->setPos(time[1],x,y);
    time[1]->setValue(time[1],now.tm_min);
    menuList->addWidget(menuList,time[1]);
    
    x+=time[1]->width+1;
    gfx_putS(time[1]->foreColor,time[1]->backColor,x,y+2,":");
    x+=(sepW+1);    
        
    time[2]=spinbox_create();
    time[2]->setFont(time[2],TIME_DATE_GUIFONT);
    time[2]->setParam(time[2],0,59,1,2);
    time[2]->setPos(time[2],x,y);
    time[2]->setValue(time[2],now.tm_sec);
    menuList->addWidget(menuList,time[2]);  
    
    PmPos_x=x+time[2]->width+6;       
    PmPos_y=y;
            
    if(time_format==FORMAT_12) gfx_putS(time[0]->foreColor,time[0]->backColor,PmPos_x,PmPos_y+2,isPm?"PM":"AM");
    
    y += lineH;
    x= minX;
    gfx_getStringSize("/",&sepW,&sepH);
    
    date[0]=spinbox_create();
    date[0]->setFont(date[0],TIME_DATE_GUIFONT);
    date[0]->setParam(date[0],1,date_format==FORMAT_DDMMYYYY?31:12,1,2);
    date[0]->setPos(date[0],x,y);
    date[0]->setValue(date[0],date_format==FORMAT_DDMMYYYY?now.tm_mday:now.tm_mon);
    menuList->addWidget(menuList,date[0]);    
        
    x+=date[0]->width+2;
    gfx_putS(date[0]->foreColor,date[0]->backColor,x,y+2,"/");
    x+=(sepW+1);   
        
    date[1]=spinbox_create();
    date[1]->setFont(date[1],TIME_DATE_GUIFONT);
    date[1]->setParam(date[1],1,date_format==FORMAT_DDMMYYYY?31:12,1,2);
    date[1]->setPos(date[1],x,y);
    date[1]->setValue(date[1],date_format==FORMAT_DDMMYYYY?now.tm_mon:now.tm_mday);
    menuList->addWidget(menuList,date[1]);    
    
    x+=date[1]->width+2;
    gfx_putS(date[1]->foreColor,date[1]->backColor,x,y+2,"/");    
    x+=(sepW+1);   
        
    date[2]=spinbox_create();
    date[2]->setFont(date[2],TIME_DATE_GUIFONT);
    date[2]->setParam(date[2],1900,2100,1,4);
    date[2]->setPos(date[2],x,y);
    date[2]->setValue(date[2],now.tm_year);
    menuList->addWidget(menuList,date[2]);    
    
    y += lineH;
    x= minX;
    
    hourFormat=checkbox_create();
    hourFormat->caption="12h format";
    hourFormat->font=TIME_DATE_GUIFONT;
    hourFormat->setRect(hourFormat,x,y,8,8);
    hourFormat->onChange=(CHECKBOX_CHANGEEVENT)checkbox_12_24_onChange;
    hourFormat->checked=time_format==FORMAT_12?1:0;
    menuList->addWidget(menuList,hourFormat);
    
    y += lineH;
    x= minX;
    
    dateFormat=chooser_create();
    dateFormat->items=dateFormStr;
    dateFormat->itemCount=2;
    dateFormat->index=date_format?1:0;
    dateFormat->font=TIME_DATE_GUIFONT;
    gfx_getStringSize(dateFormStr[0],&w,&h);
    dateFormat->setRect(dateFormat,x,y,w+29,h+1);    
    dateFormat->onChange=(CHOOSER_CHANGEEVENT)chooser_date_onChange;
    dateFormat->evt_prev=BTN_DOWN;
    dateFormat->evt_nxt=BTN_UP;
    dateFormat->wrap=WIDGET_WRAP_ON;
    dateFormat->orientation=WIDGET_ORIENTATION_VERT;
    menuList->addWidget(menuList,dateFormat);
    
    y += lineH;
    x= minX+4;
    
    gfx_getStringSize(getLangStr(STRLNG_OK),&sepW,&sepH);
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OK); 
    mib->font=TIME_DATE_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnClk_click;
    menuList->addWidget(menuList,mib);
        
    gfx_getStringSize(getLangStr(STRLNG_RESET),&sepW,&sepH);
    x+=mib->width+4;
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_RESET); 
    mib->font=TIME_DATE_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)resetBtnClk_click;
    menuList->addWidget(menuList,mib);
    
    // intial paint
    menuList->setFocusedWidget(menuList,time[0]);
    menuList->paint(menuList);
    
    do{
        event=evt_getStatusBlocking(evtHandle);
        if (!event) continue; // no new events
        switch(event)
        {
            case BTN_LEFT:
                menuList->changeFocus(menuList,WLD_PREVIOUS);
                break;
            case BTN_RIGHT:
                menuList->changeFocus(menuList,WLD_NEXT);
                break;    
            default:
                menuList->handleEvent(menuList,event);
                break;
        }
    }while(event!=WIDGET_BACK_BTN && !stop_clk_set);    
    menuList->destroy(menuList);
    evt_freeHandler(evtHandle);
}
