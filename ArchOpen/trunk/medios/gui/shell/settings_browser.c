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
#include <kernel/lang.h>

#include <gfx/kfont.h>
#include <driver/lcd.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/checkbox.h>
#include <gui/trackbar.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/msgBox.h>

#include <gui/file_browser.h>
#include <gui/settings_browser.h>

#include <fs/cfg_file.h>

#define BROWSER_GUIFONT RADONWIDE

WIDGETLIST menuList;  

CHECKBOX hasBack;
CHECKBOX scrollSelected;
CHECKBOX hasStatus;
TRACKBAR txtScrollSpeed;

#define ICON_X 5
#define ICON_Y 5

int stop_brw_set;

void okBtnBrowser_click(BUTTON b)
{
    browser_has_back_entry=hasBack->checked;
    browser_scroll_only_selected=scrollSelected->checked;
    browser_has_statusbar=hasStatus->checked;
    txt_scroll_speed=(txtScrollSpeed->value)*BRW_SETTING_CST_A + BRW_SETTING_CST_B;
    
    CFG_DATA * cfg;

    /* saving to cfg file */
    
    msgBox_info(getLangStr(STRLNG_SAVE_SETTINGS));
    
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

    cfg_writeInt(cfg,"brw_back_entry",browser_has_back_entry);
    cfg_writeInt(cfg,"brw_scroll_only_selected",browser_scroll_only_selected);
    cfg_writeInt(cfg,"brw_statusBar",browser_has_statusbar);
    cfg_writeInt(cfg,"brw_scroll_speed",txt_scroll_speed);

    cfg_printItems(cfg);
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
                             
    stop_brw_set=1;

}

void cancelBtnBrowser_click(BUTTON b)
{
    stop_brw_set=1;
}

void browser_setting(void)
{
    ICON logo;
      
    int evtHandle;
    int event;
    
    int minX,lineH,x,y,w,h,sepH,sepW,tmp;
        
    stop_brw_set = 0;
    
    BUTTON mib;
        
    gfx_clearScreen(COLOR_TRSP);
        
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandle<0)
    {
        printk("Can't get evt handler\n");   
    }
    
    logo=icon_get("browser_icon");
    if(!logo)
        icon_load("browser_icon.ico");
    
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    
    minX = ICON_X + logo->bmap_data.width;        
    
    gfx_drawLine(COLOR_RED,minX+3,5,minX+3,LCD_HEIGHT-5);
        
    minX+=3; 
       
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_BROWSER_SETTINGS),&w,&h);
    gfx_putS(COLOR_DARK_GREY,COLOR_TRSP,minX+(LCD_WIDTH-minX-w)/2,ICON_Y,getLangStr(STRLNG_BROWSER_SETTINGS));
    
    gfx_fontSet(BROWSER_GUIFONT);    
    gfx_getStringSize(getLangStr(STRLNG_BROWSER_SETTINGS),&w,&lineH);    
    lineH=(lineH+4);
    
    
    x=minX+1;    
    y=ICON_Y+h+(LCD_HEIGHT-ICON_Y-h-lineH*6)/2;
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    // standardMenu
     
    hasBack=checkbox_create();
    hasBack->caption=getLangStr(STRLNG_HAS_BACK_ENTRY);
    hasBack->font=BROWSER_GUIFONT;
    hasBack->setRect(hasBack,x,y,8,8);
    hasBack->checked=browser_has_back_entry;
    menuList->addWidget(menuList,hasBack);
    
    y += lineH;
    
    scrollSelected=checkbox_create();
    scrollSelected->caption=getLangStr(STRLNG_SCROLL_ONLY_SELECTED);
    scrollSelected->font=BROWSER_GUIFONT;
    scrollSelected->setRect(scrollSelected,x,y,8,8);
    scrollSelected->checked=browser_scroll_only_selected;
    menuList->addWidget(menuList,scrollSelected);   
    
    y += lineH;

    hasStatus=checkbox_create();
    hasStatus->caption=getLangStr(STRLNG_BRW_HAS_STATUSBAR);
    hasStatus->font=BROWSER_GUIFONT;
    hasStatus->setRect(hasStatus,x,y,8,8);
    hasStatus->checked=browser_has_statusbar;
    menuList->addWidget(menuList,hasStatus);
    
    y += lineH;
    
    gfx_getStringSize(getLangStr(STRLNG_BRW_SCROLL_SPEED),&sepW,&sepH);
    gfx_putS(COLOR_BLACK,COLOR_WHITE,x,y,getLangStr(STRLNG_BRW_SCROLL_SPEED));
    y += lineH;
    
    txtScrollSpeed=trackbar_create();
    txtScrollSpeed->minimum=1;
    txtScrollSpeed->maximum=MAX_STEP;
    tmp=((txt_scroll_speed - BRW_SETTING_CST_B)/BRW_SETTING_CST_A);
    //if a wrong value is read from a previous config.cfg set default value to middle of min and max
    if (tmp < txtScrollSpeed->minimum || tmp > txtScrollSpeed->maximum)
    	tmp = (txtScrollSpeed->minimum + txtScrollSpeed->maximum) / 2;
    txtScrollSpeed->value=tmp;
    txtScrollSpeed->increment=1;
    txtScrollSpeed->setRect(txtScrollSpeed,x,y,160,sepH);
    txtScrollSpeed->font=BROWSER_GUIFONT;
    menuList->addWidget(menuList,txtScrollSpeed);
    
    y += lineH;
    
    gfx_getStringSize(getLangStr(STRLNG_OK),&sepW,&sepH);
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OK); 
    mib->font=BROWSER_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnBrowser_click;
    menuList->addWidget(menuList,mib);
        
    gfx_getStringSize(getLangStr(STRLNG_CANCEL),&sepW,&sepH);
    x+=mib->width+4;
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_CANCEL); 
    mib->font=BROWSER_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)cancelBtnBrowser_click;
    menuList->addWidget(menuList,mib);
    
    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,hasBack);
    menuList->paint(menuList);
    
    do{
        event=evt_getStatusBlocking(evtHandle);
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
    }while(event!=BTN_OFF && !stop_brw_set); 
       
    menuList->destroy(menuList);
    evt_freeHandler(evtHandle);

}

void browser_loadCfg(void)
{
    CFG_DATA * cfg;
    int needWrite=0;

    browser_has_back_entry=1;
    browser_scroll_only_selected=1;
    txt_scroll_speed=3;
    
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
        cfg_writeInt(cfg,"brw_back_entry",1);
        cfg_writeInt(cfg,"brw_scroll_only_selected",1);
        cfg_writeInt(cfg,"brw_statusBar",1);
        cfg_writeInt(cfg,"brw_scroll_speed",3);
        needWrite=1;
    }
    else
    {
        if(cfg_itemExists(cfg,"brw_back_entry"))
        {
            browser_has_back_entry=cfg_readInt(cfg,"brw_back_entry");
        }
        else
        {
            cfg_writeInt(cfg,"brw_back_entry",1);
            needWrite=1;
        }

        if(cfg_itemExists(cfg,"brw_scroll_only_selected"))
        {
            browser_scroll_only_selected=cfg_readInt(cfg,"brw_scroll_only_selected");
        }
        else
        {
            cfg_writeInt(cfg,"brw_scroll_only_selected",1);
            needWrite=1;
        }

        if(cfg_itemExists(cfg,"brw_statusBar"))
        {
            browser_has_statusbar=cfg_readInt(cfg,"brw_statusBar");
        }
        else
        {
            cfg_writeInt(cfg,"brw_statusBar",1);
            needWrite=1;
        }
        //scroll speed
        if(cfg_itemExists(cfg,"brw_scroll_speed"))
        {
            txt_scroll_speed=(cfg_readInt(cfg,"brw_scroll_speed")* BRW_SETTING_CST_A) + BRW_SETTING_CST_B;
        }
        else
        {
            cfg_writeInt(cfg,"brw_scroll_speed",3);
            needWrite=1;
        }
        
    }
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
}
