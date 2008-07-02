/*
*   kernel/gfx/gui/shell/settings_shell.c
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
#include <kernel/delay.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/icons.h>
#include <gui/widgetmenu.h>
#include <gui/settings_screens.h>

#include <gui/shellmenu.h>

#include <gui/settings_shell.h>

#include <fs/cfg_file.h>

#include <driver/lcd.h>


static WIDGETMENU widgetMenu;
static WIDGETMENU_CHECKBOX hasCaption;
static WIDGETMENU_CHOOSER iconSize;
static WIDGETMENU_TRACKBAR brightVal;

int init_bright;

char * iconSizeStr[3];

void shellSet_save(void)
{
    CFG_DATA * cfg;
    int forceWrite=0;
    int needSave=0;
    int reloadShell=0;
    /* opening config file */
    
    TEST_VAR(shellHasCaption,hasCaption->checkbox->checked,needSave);
    TEST_VAR(folderType,iconSize->chooser->index,needSave);
    if(needSave) reloadShell=1;    
    TEST_VAR(init_bright,brightVal->trackbar->value,needSave);
             
    if(!needSave)
    {
        printk("No change ==> no save\n");
        return;
    }
    
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
        forceWrite=1;
    }
    
    /* setting the config */
    
    cfg_writeInt(cfg,"shellHasCaption",shellHasCaption);
    
    cfg_writeInt(cfg,"iconSize",folderType);
    
    
    cfg_writeInt(cfg,"lcdBrightness",brightVal->trackbar->value);
    
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    /* need to reload shell */
    if(reloadShell)
    {
        icon_setPath();
        shellMenu_close();
        shellMenu_init();
        shell_restore();
    }
}

void brightVal_chg(TRACKBAR trkBar)
{
    lcd_setBrightness(trkBar->value);
}

void shell_setting(void)
{
    ICON logo;
        
    int minX,minY;
    
    iconSizeStr[0]=getLangStr(STRLNG_ICONS_SMALL);
    iconSizeStr[1]=getLangStr(STRLNG_ICONS_STD);
    iconSizeStr[2]=getLangStr(STRLNG_ICONS_BIG);
    
    logo=icon_get("shell_setting");
    if(!logo)
        logo=icon_load("shell_setting.ico");
    
    settings_initScreen(getLangStr(STRLNG_SHELL_SETTINGS),logo,&minX,&minY);
    
    // menuList
    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;
    
    // standardMenu
    
    hasCaption=widgetMenuCheckbox_create();
    hasCaption->caption=getLangStr(STRLNG_SHELL_CAPTION);
    hasCaption->checkbox->checked=shellHasCaption;
    widgetMenu->addItem(widgetMenu,hasCaption);
        
    iconSize=widgetMenuChooser_create();
    iconSize->caption=NULL;
    iconSize->chooser->items=iconSizeStr;
    iconSize->chooser->itemCount=3;
    iconSize->chooser->index=folderType;
    iconSize->chooser->wrap=WIDGET_WRAP_ON;
    iconSize->chooser->orientation=WIDGET_ORIENTATION_HORIZ;
    widgetMenu->addItem(widgetMenu,iconSize);
            
    init_bright=lcd_getBrightness();
    brightVal=widgetMenuTrackbar_create();
    brightVal->caption=NULL;
    /* max is probably different on DSC21 */
    brightVal->trackbar->onChange=(TRACKBAR_CHANGEEVENT)brightVal_chg;
    widgetMenu->addItem(widgetMenu,brightVal);
    brightVal->trackbar->setParam(brightVal->trackbar,0,100,10);
    brightVal->trackbar->setValue(brightVal->trackbar,init_bright);
    brightVal->useMaxWidth=true;
    
    // intial paint
    // set focus
    widgetMenu->setFocus(widgetMenu,hasCaption);    
    widgetMenu->paint(widgetMenu);
    
    settings_evtLoop(widgetMenu,shellSet_save,-1);
       
    widgetMenu->destroy(widgetMenu);
}
