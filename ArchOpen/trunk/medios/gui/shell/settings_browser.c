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
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/widgetmenu.h>
#include <gui/file_browser.h>
#include <gui/settings_browser.h>
#include <gui/settings_screens.h>

#include <fs/cfg_file.h>

static WIDGETMENU widgetMenu;
static WIDGETMENU_CHECKBOX hasBack;
static WIDGETMENU_CHECKBOX scrollSelected;
static WIDGETMENU_CHECKBOX hasStatus;
static WIDGETMENU_TRACKBAR txtScrollSpeed;

void browserSetting_sav(void)
{
    int needSave=0;
    CFG_DATA * cfg;
    
    TEST_VAR(browser_has_back_entry,hasBack->checkbox->checked,needSave);
    TEST_VAR(browser_scroll_only_selected,scrollSelected->checkbox->checked,needSave);
    TEST_VAR(browser_has_statusbar,hasStatus->checkbox->checked,needSave);
    TEST_VAR(txt_scroll_speed,(txtScrollSpeed->trackbar->value)*BRW_SETTING_CST_A + BRW_SETTING_CST_B,needSave);
    
    if(!needSave)
    {
        printk("No change ==> no save\n");
        return;
    }
    
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

//    cfg_printItems(cfg);
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
}

void browser_setting(void)
{
    ICON logo;
    
    int minX,minY;
    int tmp;
    WIDGETMENU_ITEM it;
    
    logo=icon_get("browser_icon");
    if(!logo)
        logo=icon_load("browser_icon.ico");
    
    settings_initScreen(getLangStr(STRLNG_BROWSER_SETTINGS),logo,&minX,&minY);

    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;
    
    hasBack=widgetMenuCheckbox_create();
    hasBack->setCaption(hasBack,getLangStr(STRLNG_HAS_BACK_ENTRY));
    hasBack->checkbox->checked=browser_has_back_entry;
    widgetMenu->addItem(widgetMenu,hasBack);
        
    scrollSelected=widgetMenuCheckbox_create();
    scrollSelected->setCaption(scrollSelected,getLangStr(STRLNG_SCROLL_ONLY_SELECTED));
    scrollSelected->checkbox->checked=browser_scroll_only_selected;
    widgetMenu->addItem(widgetMenu,scrollSelected);
    
    hasStatus=widgetMenuCheckbox_create();
    hasStatus->setCaption(hasStatus,getLangStr(STRLNG_BRW_HAS_STATUSBAR));
    hasStatus->checkbox->checked=browser_has_statusbar;
    widgetMenu->addItem(widgetMenu,hasStatus);
        
    it=widgetMenuItem_create();
    it->setCaption(it,getLangStr(STRLNG_BRW_SCROLL_SPEED));
    it->canFocus=0;
    widgetMenu->addItem(widgetMenu,it);
    
    txtScrollSpeed=widgetMenuTrackbar_create();
    txtScrollSpeed->caption=NULL;
    tmp=((txt_scroll_speed - BRW_SETTING_CST_B)/BRW_SETTING_CST_A);
    //if a wrong value is read from a previous config.cfg set default value to middle of min and max
    if (tmp < txtScrollSpeed->trackbar->minimum || tmp > txtScrollSpeed->trackbar->maximum)
        tmp = (txtScrollSpeed->trackbar->minimum + txtScrollSpeed->trackbar->maximum) / 2;
    txtScrollSpeed->trackbar->value=tmp;
    txtScrollSpeed->trackbar->minimum=1;
    txtScrollSpeed->trackbar->maximum=MAX_STEP; /* mas is probably different on DSC21 */
    txtScrollSpeed->trackbar->increment=1;
    widgetMenu->addItem(widgetMenu,txtScrollSpeed);
            
    // intial paint
    // set focus
    widgetMenu->setFocus(widgetMenu,hasBack);    
    widgetMenu->paint(widgetMenu);
    
    settings_evtLoop(widgetMenu,browserSetting_sav,-1);
       
    widgetMenu->destroy(widgetMenu);
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
