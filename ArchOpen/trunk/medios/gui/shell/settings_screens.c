/*
*   medios/gui/shell/settings_screens.c
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

#include <lib/string.h>

#include <gui/settings_screens.h>
#include <gui/label.h>
#include <gui/icons.h>
#include <gui/widgetmenu.h>

#include <driver/lcd.h>

#include <kernel/evt.h>

#define ICON_X 5
#define ICON_Y 5

void settings_initScreen(char * title,ICON logo,int * menu_x,int * menu_y)
{
    LABEL title_label;
    int smallScreen;
    int h,x,y;

    if(LCD_HEIGHT<240 || LCD_WIDTH<320)
       smallScreen=1;
    else
       smallScreen=0;

    gfx_clearScreen(COLOR_TRSP);
    if(logo)
        gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    if(!smallScreen && logo)
        gfx_drawLine(COLOR_RED,ICON_X+logo->bmap_data.width+3,5,ICON_X+logo->bmap_data.width+3,LCD_HEIGHT-5);

    x=logo!=NULL?ICON_X + logo->bmap_data.width + 8:2;
    y=0;

    title_label=label_create();
    title_label->setRect(title_label,x, ICON_Y,LCD_WIDTH-x-1,0);
    title_label->font=STD8X13;
    title_label->alignment=WA_CENTER;
    title_label->cutWord=LA_SMART_CUT;
    title_label->setText(title_label,title);
    title_label->paint(title_label);
    title_label->getMaxSize(title_label,NULL,&h);
    title_label->destroy(title_label);

    if(smallScreen)
    {
        x=2;
        if(logo!=NULL)
            y=ICON_Y+MAX(logo->bmap_data.height,h)+1;
        else
            y=ICON_Y;
    }
    else
    {
        y=ICON_Y+h+2;
    }
    if(menu_x)
        *menu_x=x;
    if(menu_y)
        *menu_y=y;
}

int stop_screen=0;

void settings_stopLoop(void)
{
    stop_screen=1;
}

void settings_evtLoop(WIDGETMENU widgetMenu,void(*sav_fct)(void),int evtHandle)
{
    int evtHandler;
    int hasHandler=0;
    int event;

    stop_screen=0;

    if(evtHandle<0)
    {
        evtHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);
        if(evtHandler<0)
        {
            printk("Can't get evt handler\n");
            return;
        }
    }
    else
    {
        evtHandler=evtHandle;
        hasHandler=1;
    }

    do
    {
        event=evt_getStatusBlocking(evtHandler);
        if (!event) continue; // no new events

        if(widgetMenu->handleEvent(widgetMenu,event))
            continue;
        switch(event)
        {
            case WIDGET_BACK_BTN:
                stop_screen=1;
                if(sav_fct)
                    sav_fct();
                break;
        }
    }
    while(!stop_screen);

    if(!hasHandler)
        evt_freeHandler(evtHandler);
}

