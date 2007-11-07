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
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/checkbox.h>
#include <gui/msgBox.h>
#include <gui/icons.h>
#include <gui/chooser.h>

#include <gui/shellmenu.h>

#include <gui/settings_shell.h>

#include <fs/cfg_file.h>

#include <driver/lcd.h>

#define SHELL_GUIFONT RADONWIDE

WIDGETLIST menuList;  
CHECKBOX hasCaption;
CHOOSER iconSize;

int stop_shell_set;

char * iconSizeStr[3];

#define ICON_X 5
#define ICON_Y 5

void okBtnShell_click(BUTTON b)
{
    CFG_DATA * cfg;
    int needWrite=0;
    /* opening config file */
    
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
    
    /* setting the config */
    
    if(hasCaption->checked != shellHasCaption)
    {
        shellHasCaption=hasCaption->checked;
        cfg_writeInt(cfg,"shellHasCaption",shellHasCaption);
        needWrite=1;
    }
    
    if(iconSize->index != folderType)
    {
        folderType=iconSize->index;
        cfg_writeInt(cfg,"iconSize",folderType);
        icon_setPath();
        needWrite=1;
    }
    
    if(needWrite)
    {
        cfg_writeFile(cfg,"/medios/medios.cfg");
        /* need to reload shell */
        shellMenu_close();
        shellMenu_init();
        shell_restore();
    }
    cfg_clear(cfg);

    stop_shell_set=1;
}

void cancelBtnShell_click(BUTTON b)
{
    stop_shell_set=1;
}

void shell_setting(void)
{
    ICON logo;
        
    int evtHandle;
    int event;
    
    int minX,w,h,x,y,sepW,sepH,lineH,maxW;
    
    BUTTON mib;
    
    iconSizeStr[0]=getLangStr(STRLNG_ICONS_SMALL);
    iconSizeStr[1]=getLangStr(STRLNG_ICONS_STD);
    iconSizeStr[2]=getLangStr(STRLNG_ICONS_BIG);
    
    gfx_clearScreen(COLOR_TRSP);
    
    stop_shell_set=0;
    
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandle<0)
    {
        printk("Can't get evt handler\n");   
    }
    
    logo=icon_get("shell_setting");
    if(!logo)
        icon_load("shell_setting.ico");
    
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    
    minX = ICON_X + logo->bmap_data.width;
    
    gfx_drawLine(COLOR_BLUE,minX+3,5,minX+3,LCD_HEIGHT-5);
        
    minX+=5; 
       
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_SHELL_SETTINGS),&w,&h);
    lineH=h+5;

    gfx_putS(COLOR_DARK_GREY,COLOR_TRSP,minX+(LCD_WIDTH-minX-w)/2,ICON_Y,getLangStr(STRLNG_SHELL_SETTINGS));
    
    x=minX;    
    y=ICON_Y+2*lineH;
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    gfx_fontSet(SHELL_GUIFONT);
    
    // standardMenu
    
    hasCaption=checkbox_create();
    hasCaption->caption=getLangStr(STRLNG_SHELL_CAPTION);
    hasCaption->font=SHELL_GUIFONT;
    hasCaption->setRect(hasCaption,x,y,8,8);
    hasCaption->checked=shellHasCaption;
    
    y+=lineH;
    
    menuList->addWidget(menuList,hasCaption);
        
    gfx_getStringSize(getLangStr(STRLNG_SIZE),&w,&h);
    gfx_putS(COLOR_BLACK,COLOR_TRSP,x,y,getLangStr(STRLNG_SIZE));
    
    x=x+(w+3);
    
    gfx_getStringSize(iconSizeStr[0],&maxW,NULL);    
    gfx_getStringSize(iconSizeStr[1],&w,NULL);
    if(w>maxW) maxW=w;
    gfx_getStringSize(iconSizeStr[2],&w,&h);
    if(w>maxW) maxW=w;
        
    iconSize=chooser_create();
    iconSize->items=iconSizeStr;
    iconSize->itemCount=3;
    iconSize->index=folderType;
    iconSize->font=SHELL_GUIFONT;
    iconSize->setRect(iconSize,x,y,maxW+29,h+1);    
    iconSize->wrap=WIDGET_WRAP_ON;
    iconSize->orientation=WIDGET_ORIENTATION_HORIZ;
    menuList->addWidget(menuList,iconSize);
    
    y += lineH;
    x=minX+4;    
    
    gfx_getStringSize(getLangStr(STRLNG_OK),&sepW,&sepH);    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OK); 
    mib->font=SHELL_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnShell_click;
    menuList->addWidget(menuList,mib);
        
    x+=mib->width+4;
    gfx_getStringSize(getLangStr(STRLNG_CANCEL),&sepW,&sepH);    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_CANCEL); 
    mib->font=SHELL_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)cancelBtnShell_click;
    menuList->addWidget(menuList,mib);
    
    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,hasCaption);
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
    }while(event!=BTN_OFF && !stop_shell_set); 
       
    menuList->destroy(menuList);
    evt_freeHandler(evtHandle);
}
