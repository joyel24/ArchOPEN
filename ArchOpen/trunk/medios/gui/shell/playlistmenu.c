/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>

#include <kernel/evt.h>
#include <kernel/thread.h>
#include <kernel/malloc.h>
#include <kernel/timer.h>

#include <driver/lcd.h>

#include <gui/textmenu.h>
#include <gui/status_line.h>
#include <gui/shell.h>
#include <gui/player.h>

#include <snd/sound.h>
#include <snd/playlist.h>
#include <snd/tag.h>

#include <gui/playlistmenu.h>

TEXTMENU playlistMenu;

int playlistMenu_eventHandler;
bool playlistMenu_visible;

char playlist_itemStr[1000];

struct tmr_s playlistMenu_timer;

PLAYLIST_ITEM * playlistMenu_prevActiveItem;

static void playlistMenu_onClick(TEXTMENU m, TEXTMENU_ITEM mi){
    PLAYLIST_ITEM * item;

    item=mi->data;

    sound_activeItem=item;
    sound_play(true);
}

static void playlistMenu_itemPaint(TEXTMENU_ITEM mi){
    int x,y;
    int w,h;
    int of;
    PLAYLIST_ITEM * item;
    int foreColor,backColor;

    item=mi->data;

    widget_paint((WIDGET)mi);

    foreColor=(item==sound_activeItem)?PLAYLISTMENU_ACTIVEITEM_COLOR:mi->foreColor;
    backColor=(mi->focused)?mi->focusColor:mi->backColor;

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(mi->font);

    // playlist caption
    tag_parse(item,player_playlistFormat,playlist_itemStr,sizeof(playlist_itemStr)-1);

    gfx_getStringSize(playlist_itemStr,&w,&h);
    y=mi->y+(mi->height-h)/2;
    x=mi->x+mi->margin;

    gfx_putS(foreColor,backColor,x,y,playlist_itemStr);

    // time
    snprintf(playlist_itemStr,sizeof(playlist_itemStr)-1,"%d:%0.2d",item->tag.length/(60*HZ),(item->tag.length/HZ)%60);

    gfx_getStringSize(playlist_itemStr,&w,&h);
    y=mi->y+(mi->height-h)/2;
    x=mi->x+mi->width-mi->margin-w;

    gfx_putS(foreColor,backColor,x,y,playlist_itemStr);

    gfx_fontSet(of); // restore previous font
}

void playlistMenu_timerAction(){

    if(sound_activeItem!=playlistMenu_prevActiveItem){

        playlistMenu_refresh();

        playlistMenu_prevActiveItem=sound_activeItem;
    }
}

void playlistMenu_refresh(){
    struct evt_t eventStruct;

    if (playlistMenu_visible){

        eventStruct.evt=EVT_REDRAW;
        eventStruct.evt_class=GUI_CLASS;
        eventStruct.data=NULL;
        evt_send(&eventStruct);
    }
}

void playlistMenu_build(){
    PLAYLIST_ITEM * pi;
    TEXTMENU_ITEM mi;

    playlistMenu->clearItems(playlistMenu);

    pi=playlist_first;

    while(pi!=NULL){

        mi=textMenuItem_create();
        
        mi->setCaption(mi,pi->name);
        mi->data=pi;
        mi->paint=(WIDGET_PAINTHANDLER)playlistMenu_itemPaint;

        playlistMenu->addItem(playlistMenu,mi);

        pi=pi->next;
    }
}

void playlistMenu_eventLoop(){
    int event;

    playlistMenu_build();

    //purge events
    evt_purgeHandler(playlistMenu_eventHandler);

    playlistMenu_visible=true;

    playlistMenu_refresh();

    playlistMenu_prevActiveItem=sound_activeItem;
    tmr_start(&playlistMenu_timer);

    do{
        event=evt_getStatusBlocking(playlistMenu_eventHandler);
        if (!event){ // no new events
            yield();
            continue;
        }

        playlistMenu->handleEvent(playlistMenu,event);
        statusLine_handleEvent(event);

    }while(event!=BTN_OFF);

    tmr_stop(&playlistMenu_timer);

    playlistMenu_visible=false;

}

void playlistMenu_init(){
    int h;

    gfx_fontSet(player_playlistFont);
    gfx_getStringSize("W",NULL,&h);

    playlistMenu=textMenu_create();
    playlistMenu->setRect(playlistMenu,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    playlistMenu->onClick=(MENU_CLICKEVENT)playlistMenu_onClick;
    
    playlistMenu->setFont(playlistMenu,player_playlistFont);

    playlistMenu_eventHandler=evt_getHandler(ALL_CLASS);

    playlistMenu_visible=false;
    playlistMenu_prevActiveItem=NULL;

    tmr_setup(&playlistMenu_timer,"playlistMenu refresh");
    playlistMenu_timer.action=playlistMenu_timerAction;
    playlistMenu_timer.freeRun=1;
    playlistMenu_timer.stdDelay=HZ/5;
}

void playlistMenu_close(){
    evt_freeHandler(playlistMenu_eventHandler);
    playlistMenu->destroy(playlistMenu);
    tmr_remove(&playlistMenu_timer);
}
