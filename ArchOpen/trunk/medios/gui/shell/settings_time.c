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
#include <gui/widgetmenu.h>
#include <gui/widgetmenuline.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/settings_screens.h>
#include <gui/settings_time.h>
#include <gui/status_line.h>

#include <driver/time.h>
#include <driver/lcd.h>

#include <fs/cfg_file.h>

static WIDGETMENU widgetMenu;
static WIDGETMENU_DATE_TIME dateSetting;
static WIDGETMENU_DATE_TIME timeSetting;
static WIDGETMENU_CHECKBOX hourFormat;
static WIDGETMENU_CHOOSER dateFormat;

ICON logo;

void checkbox_12_24_onChange(CHECKBOX t)
{
    timeSetting->dateTime->setTimeFormat(timeSetting->dateTime,t->checked==1?FORMAT_12:FORMAT_24);
    timeSetting->paint(timeSetting);
}

void chooser_date_onChange(CHOOSER t)
{
    dateSetting->dateTime->setDateFormat(dateSetting->dateTime,t->index==1?FORMAT_MMDDYYYY:FORMAT_DDMMYYYY);
    dateSetting->paint(dateSetting);
}

void okBtnClk_click(BUTTON b)
{
    struct med_tm now;
    CFG_DATA * cfg;
    int needSave=0;
    
    timeSetting->dateTime->getValue(timeSetting->dateTime,&now); /* getting time */
    dateSetting->dateTime->getValue(dateSetting->dateTime,&now); /*getting date */
    
    if(time_set(&now)!=MED_OK)
    {
        msgBox_info("Write not supported");
        mdelay(1000);
    }
    
    
    TEST_VAR(time_format,hourFormat->checkbox->checked?FORMAT_12:FORMAT_24,needSave);
    TEST_VAR(date_format,dateFormat->chooser->index==1?FORMAT_MMDDYYYY:FORMAT_DDMMYYYY,needSave);
             
    if(!needSave)
    {
        printk("No change ==> no save\n");        
    }
    else
    {
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
        
        cfg_writeInt(cfg,"is12H",time_format);
        cfg_writeInt(cfg,"isMMDD",date_format);
        //cfg_printItems(cfg);
        cfg_writeFile(cfg,"/medios/medios.cfg");
        cfg_clear(cfg);
    }
    settings_stopLoop();
}   

void resetBtnClk_click(BUTTON b)
{
    struct med_tm now;
    
    time_get(&now);
    
    timeSetting->dateTime->setTimeFormat(timeSetting->dateTime,time_format);
    timeSetting->dateTime->setValue(timeSetting->dateTime,&now);
    
    dateSetting->dateTime->setDateFormat(dateSetting->dateTime,date_format);
    dateSetting->dateTime->setValue(dateSetting->dateTime,&now);
    
    dateFormat->chooser->index=date_format==FORMAT_MMDDYYYY?1:0;
    hourFormat->checkbox->checked=time_format==FORMAT_12?1:0;
    
    widgetMenu->paint(widgetMenu);
}

void clock_setting(void)
{
    struct med_tm now;
    char * dateFormStr[2]={"DD/MM/YYYY","MM/DD/YYYY"};
    
    BUTTON mib;
    WIDGETMENU_MENULINE ml;
       
    int minX,minY;
    
    time_get(&now);
    
    logo=icon_get("clock_icon");
    if(!logo)
        logo=icon_load("clock_icon.ico");
    
    settings_initScreen(getLangStr(STRLNG_CLOCK_SETTINGS),logo,&minX,&minY);
    
    printk("time format=%d (12=%d 24=%d) date format=%d (MM=%d DD=%d)\n",time_format,
           FORMAT_12,FORMAT_24,date_format,FORMAT_MMDDYYYY,FORMAT_DDMMYYYY);
    
    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;
    
    timeSetting=widgetMenuDateTime_create();
    timeSetting->caption=NULL;
    timeSetting->dateTime->setType(timeSetting->dateTime,TIME_SETTING);
    timeSetting->dateTime->setTimeFormat(timeSetting->dateTime,time_format);
    timeSetting->dateTime->setValue(timeSetting->dateTime,&now);
    widgetMenu->addItem(widgetMenu,timeSetting);
    
        
    dateSetting=widgetMenuDateTime_create();
    dateSetting->caption=NULL;
    dateSetting->dateTime->setType(dateSetting->dateTime,DATE_SETTING);
    dateSetting->dateTime->setDateFormat(dateSetting->dateTime,date_format);        
    dateSetting->dateTime->setValue(dateSetting->dateTime,&now);
    widgetMenu->addItem(widgetMenu,dateSetting);
    
    
    hourFormat=widgetMenuCheckbox_create();
    hourFormat->caption="12h format";
    hourFormat->checkbox->onChange=(CHECKBOX_CHANGEEVENT)checkbox_12_24_onChange;
    hourFormat->checkbox->checked=time_format==FORMAT_12?1:0;
    widgetMenu->addItem(widgetMenu,hourFormat);
    
    dateFormat=widgetMenuChooser_create();
    dateFormat->caption=NULL;
    dateFormat->chooser->items=dateFormStr;
    dateFormat->chooser->itemCount=2;
    dateFormat->chooser->index=date_format==FORMAT_MMDDYYYY?1:0;   
    dateFormat->chooser->onChange=(CHOOSER_CHANGEEVENT)chooser_date_onChange;
    dateFormat->chooser->wrap=WIDGET_WRAP_ON;
    dateFormat->chooser->orientation=WIDGET_ORIENTATION_HORIZ;
    widgetMenu->addItem(widgetMenu,dateFormat);
    
    ml=widgetMenuMenuLine_create();
    ml->caption=NULL;
        
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OK); 
    mib->onClick=(BUTTON_CLICKEVENT)okBtnClk_click;
    ml->menuLine->addWidget(ml->menuLine,mib);
        
    mib=button_create();
    mib->caption=getLangStr(STRLNG_RESET); 
    mib->onClick=(BUTTON_CLICKEVENT)resetBtnClk_click;
    ml->menuLine->addWidget(ml->menuLine,mib);
    
    widgetMenu->addItem(widgetMenu,ml);
    
    // intial paint
    widgetMenu->setFocus(widgetMenu,timeSetting);
    widgetMenu->paint(widgetMenu);
    
    settings_evtLoop(widgetMenu,NULL,-1);
    
    widgetMenu->destroy(widgetMenu);
}
