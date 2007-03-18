/*
*   kernel/gfx/gui/shell/settings_misc.c
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

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/checkbox.h>

#include <gui/settings_misc.h>

#include <driver/lcd.h>
#include <driver/speaker.h>
#include <driver/fm_remote.h>

#include <fs/cfg_file.h>

#define MISC_GUIFONT RADONWIDE

WIDGETLIST menuList;  
CHECKBOX FmRemote;
CHECKBOX ExtSpkr;
int stop_misc_set;

#define ICON_X 5
#define ICON_Y 5

void okBtnMisc_click(BUTTON b)
{
    CFG_DATA * cfg;
    /* opening config file */
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
    
    /* setting the config */
    
    if(FmRemote->checked != FM_getState())
    {
        if(FmRemote->checked)
            FM_enable();
        else
            FM_disable();
        cfg_writeInt(cfg,"fmRemote",FmRemote->checked);
    }
    
    if(SPKR_AVAILABLE() && ExtSpkr->checked!=SPKR_STATE())
    {
        if(ExtSpkr->checked)
        {
            SPKR_ON();
        }
        else
        {
            SPKR_OFF();
        }
        cfg_writeInt(cfg,"ExtSpkr",ExtSpkr->checked);
    }
    
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    stop_misc_set=1;
}

void misc_setting(void)
{
    ICON logo;
        
    int evtHandle;
    int event;
    
    int minX,w,h,x,y,sepW,sepH,lineH;
    
    BUTTON mib;
    gfx_clearScreen(COLOR_WHITE);
    
    stop_misc_set=0;
    
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    
    logo=icon_get("misc");
    if(!logo)
        icon_load("misc.ico");
    
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    
    minX = ICON_X + logo->bmap_data.width;
        
    
    gfx_drawLine(COLOR_BLUE,minX+3,5,minX+3,LCD_HEIGHT-5);
        
    minX+=5; 
       
    gfx_fontSet(STD8X13);
    gfx_getStringSize("Misc Settings",&w,&h);
    lineH=h+5;
    gfx_putS(COLOR_DARK_GREY,COLOR_WHITE,minX+(LCD_WIDTH-minX-w)/2,ICON_Y,"Misc Settings");
    
    x=minX;    
    y=ICON_Y+2*lineH;
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    gfx_fontSet(MISC_GUIFONT);
    
    // standardMenu
    
    FmRemote=checkbox_create();
    FmRemote->caption="Enable FM remote";
    FmRemote->font=MISC_GUIFONT;
    FmRemote->setRect(FmRemote,x,y,8,8);
    FmRemote->checked=FM_getState();
    
    y+=lineH;
    
    menuList->addWidget(menuList,FmRemote);
    
    if(SPKR_AVAILABLE())
    {
        ExtSpkr=checkbox_create();
        ExtSpkr->caption="Enable speaker";
        ExtSpkr->font=MISC_GUIFONT;
        ExtSpkr->setRect(ExtSpkr,x,y,8,8);
        ExtSpkr->checked=SPKR_STATE();
        menuList->addWidget(menuList,ExtSpkr);
        y+=lineH;
    }
    
    
    gfx_getStringSize("OK",&sepW,&sepH);
    
    mib=button_create();
    mib->caption="OK"; 
    mib->font=MISC_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnMisc_click;
    menuList->addWidget(menuList,mib);
        
    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,FmRemote);
    menuList->paint(menuList);
    
    do{
        event=evt_getStatus(evtHandle);
        if (!event) continue; // no new events
        switch(event)
        {
            case BTN_UP:
                menuList->changeFocus(menuList,WLD_PREVIOUS);
                break;
            case BTN_DOWN:
                menuList->changeFocus(menuList,WLD_NEXT);
                break;    
            default:
                menuList->handleEvent(menuList,event);
                break;
        }
    }while(event!=BTN_OFF && !stop_misc_set); 
       
    menuList->destroy(menuList);
}

void misc_loadPref(void)
{
    CFG_DATA * cfg;
    int needWrite=0;
       
    cfg=cfg_readFile("/medios/medios.cfg");
           
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        /* creating default */
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }
        cfg_writeInt(cfg,"fmRemote",1);
        if(SPKR_AVAILABLE())
            cfg_writeInt(cfg,"ExtSpkr",0);
        needWrite=1;   
    }
    else
    {
        if(cfg_itemExists(cfg,"fmRemote"))
        {
            if(cfg_readInt(cfg,"fmRemote"))
                FM_enable();
            else
                FM_disable();
        }
        else
        {
            cfg_writeInt(cfg,"fmRemote",1);
            FM_enable();
            needWrite=1;
        }
        
        if(SPKR_AVAILABLE())
        {
            if(cfg_itemExists(cfg,"ExtSpkr"))
            {
                if(cfg_readInt(cfg,"ExtSpkr"))
                {
                    SPKR_ON();
                }
                else
                {
                    SPKR_OFF();
                }
            }
            else
            {
                cfg_writeInt(cfg,"ExtSpkr",0);
                SPKR_OFF();
                needWrite=1;
            }
        }
        
    }
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
}
