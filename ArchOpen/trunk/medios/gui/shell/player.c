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
#include <kernel/lang.h>

#include <driver/lcd.h>
#include <driver/buttons.h>

#include <fs/cfg_file.h>

#include <gui/textmenu.h>
#include <gui/status_line.h>
#include <gui/shell.h>

#include <gui/widgetlist.h>
#include <gui/trackbar.h>
#include <gui/label.h>
#include <gui/playlistmenu.h>
#include <gui/player.h>

#include <snd/sound.h>
#include <snd/playlist.h>
#include <snd/tag.h>
#include <snd/codec.h>

bool player_visible;

char player_playerFormat[1000]=PLAYER_DEFAULT_PLAYER_FORMAT;
char player_playlistFormat[1000]=PLAYER_DEFAULT_PLAYLIST_FORMAT;

int player_volume=PLAYER_DEFAULT_VOLUME;

int player_font=PLAYER_DEFAULT_FONT;
int player_playlistFont=PLAYER_DEFAULT_PLAYLIST_FONT;

int player_eventHandler;

bool player_seeking;
int player_seekTime;

struct tmr_s player_timer;

WIDGETLIST player_widgetList;
TRACKBAR player_positionTrackbar;
TRACKBAR player_volumeTrackbar;
LABEL player_trackLabel;
LABEL player_volumeLabel;
LABEL player_lengthLabel;
LABEL player_elapsedLabel;
LABEL player_remainingLabel;
LABEL player_helpLabel;
LABEL player_stateLabel;

char player_trackText[1000]="";
char player_elapsedText[10]="0:00";
char player_remainingText[10]="0:00";
char player_lengthText[10]="0:00";

PLAYLIST_ITEM * player_prevActiveItem;

static void player_writeCfg();

static void player_timerAction(){
    player_refresh(false);
}

static void player_timeToString(int time, char * s,int length){
    snprintf(s,length,"%d:%0.2d",time/(60*HZ),(time/HZ)%60);
}

void player_refresh(bool full){
    int length,elapsed;
    struct evt_t eventStruct;
    bool fullRefresh;

    if (player_visible){

        fullRefresh=full || (player_prevActiveItem!=sound_activeItem);

        if(sound_activeItem!=NULL){

            length=sound_activeItem->tag.length;

            player_timeToString(length,player_lengthText,sizeof(player_lengthText)-1);

            if(codec_getElapsed(&elapsed)){

                if(player_seeking){
                    elapsed+=player_seekTime;
                }

                player_timeToString(elapsed,player_elapsedText,sizeof(player_elapsedText)-1);
                if(length>=elapsed){
                    player_timeToString(length-elapsed,player_remainingText,sizeof(player_remainingText)-1);
                }else{
                    strcpy(player_remainingText,"0:00");
                }

                player_positionTrackbar->value=elapsed;
                player_positionTrackbar->maximum=length;
            }

            if(fullRefresh){
                tag_parse(sound_activeItem,player_playerFormat,player_trackText,sizeof(player_trackText)-1);
            }

        }else{

            if(strcmp(player_trackText,"")){
                fullRefresh=true;
            }

            strcpy(player_trackText,"");
            strcpy(player_lengthText,"0:00");
            strcpy(player_elapsedText,"0:00");
            strcpy(player_remainingText,"0:00");
            player_positionTrackbar->value=0;
            player_positionTrackbar->maximum=100;
        }

        switch(sound_state){
            case SS_STOPPED:
                player_stateLabel->caption=getLangStr(STRLNG_PLAYER_STOPPED);
                break;
            case SS_PLAYING:
                player_stateLabel->caption=getLangStr(STRLNG_PLAYER_PLAYING);
                break;
            case SS_PAUSED:
                player_stateLabel->caption=getLangStr(STRLNG_PLAYER_PAUSED);
                break;
        }

        player_volumeTrackbar->value=player_volume;

        player_positionTrackbar->focused=player_seeking;

        if(fullRefresh){
            //redraw everything
            eventStruct.evt=EVT_REDRAW;
            eventStruct.evt_class=GUI_CLASS;
            eventStruct.data=NULL;
            evt_send(&eventStruct);
        }else{
            //only redraw position & volume
            player_volumeTrackbar->paint(player_volumeTrackbar);
            player_positionTrackbar->paint(player_positionTrackbar);
            player_lengthLabel->paint(player_lengthLabel);
            player_elapsedLabel->paint(player_elapsedLabel);
            player_remainingLabel->paint(player_remainingLabel);
            player_stateLabel->paint(player_stateLabel);
        }

        player_prevActiveItem=sound_activeItem;
    }
}


void player_eventLoop(){
    int event;
    bool wantQuit;
    int bt;
    int pressTick;
    int pressBt;
    int pressElapsed;
    int time;

    //purge events
    evt_purgeHandler(player_eventHandler);

    player_visible=true;

    player_refresh(true);

    tmr_start(&player_timer);

    wantQuit=false;
    pressTick=-1;
    pressBt=0;
    pressElapsed=0;

    do{
        // handle seeking & track change

        bt=btn_readState();

        if(bt&(BTMASK_LEFT|BTMASK_RIGHT)){

            if(pressTick==-1){ // was not pressed ?

                pressTick=tmr_getTick();
                pressBt=(bt&BTMASK_LEFT)?BTN_LEFT:BTN_RIGHT;

            }else{

                time=tmr_getTick()-pressTick-PLAYER_SEEK_PRESS_TIME;

                if(time>=0 && codec_getElapsed(&pressElapsed) && sound_activeItem!=NULL){

                    player_seeking=true;

                    player_seekTime=(long long)sound_activeItem->tag.length*time/(PLAYER_SEEK_FIXED_TIME-time+sound_activeItem->tag.length/PLAYER_SEEK_LENGTH_DIVISOR);

                    if(pressBt==BTN_LEFT){
                        player_seekTime=-player_seekTime;
                    }

                    if(player_seekTime+pressElapsed<0){
                        player_seekTime=-pressElapsed;
                    }

                    if(player_seekTime+pressElapsed>sound_activeItem->tag.length){
                        player_seekTime=sound_activeItem->tag.length-pressElapsed;
                    }

                    sound_pause(true);
                }
            }

        }else{

            if(pressTick!=-1){ // was pressed ?

                if(player_seeking){

                    sound_seek(player_seekTime+pressElapsed);

                    player_seeking=false;

                    sound_pause(false);

                }else{
                    if(pressBt==BTN_LEFT){
                        sound_prevTrack(true);
                    }else{
                        sound_nextTrack(true);
                    }
                }

                pressTick=-1;
            }
        }

        // handle events

        event=evt_getStatusBlocking(player_eventHandler);
        if (!event){ // no new events
            yield();
            continue;
        }

        player_widgetList->handleEvent(player_widgetList,event);
        
        statusLine_handleEvent(event);

        switch(event){
            case BTN_ON:
            case BTN_1:
                if(sound_state!=SS_STOPPED){
                    sound_pause(sound_state==SS_PLAYING);
                }
                break;
            case BTN_UP:
                player_volume=MIN(100,player_volume+1);
                sound_setVolume(player_volume);
                break;
            case BTN_DOWN:
                player_volume=MAX(0,player_volume-1);
                sound_setVolume(player_volume);
                break;
            case BTN_F2:
                player_visible=false;
                playlistMenu_eventLoop();
                player_visible=true;

                evt_purgeHandler(player_eventHandler);
                player_refresh(true);
                break;
            case BTN_F3:
                wantQuit=true;
                break;
            case BTN_OFF:
                sound_stop();
                playlist_clear();
                wantQuit=true;
                break;
        }

    }while(!wantQuit);

    tmr_stop(&player_timer);

    player_visible=false;
}

static void player_buildGui(){
    int fontW,fontH;
    int w,h;
    FONT font;

    font=fnt_fontFromId(player_font);
    fontW=font->width;
    fontH=font->height;

    player_widgetList=widgetList_create();
    player_widgetList->setRect(player_widgetList,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);

    // position

    player_positionTrackbar=trackbar_create();
    player_positionTrackbar->setRect(player_positionTrackbar,0,LCD_HEIGHT-3*fontH,LCD_WIDTH,fontH);
    player_positionTrackbar->numValueDigits=0;
    player_widgetList->addWidget(player_widgetList,player_positionTrackbar);

    player_elapsedLabel=label_create();
    player_elapsedLabel->setRect(player_elapsedLabel,0,LCD_HEIGHT-4*fontH,LCD_WIDTH/3,fontH);
    player_elapsedLabel->caption=player_elapsedText;
    player_elapsedLabel->font=player_font;
    player_widgetList->addWidget(player_widgetList,player_elapsedLabel);

    player_remainingLabel=label_create();
    player_remainingLabel->setRect(player_remainingLabel,LCD_WIDTH/3,LCD_HEIGHT-4*fontH,LCD_WIDTH/3,fontH);
    player_remainingLabel->caption=player_remainingText;
    player_remainingLabel->alignment=WA_CENTER;
    player_remainingLabel->font=player_font;
    player_widgetList->addWidget(player_widgetList,player_remainingLabel);

    player_lengthLabel=label_create();
    player_lengthLabel->setRect(player_lengthLabel,2*LCD_WIDTH/3,LCD_HEIGHT-4*fontH,LCD_WIDTH/3,fontH);
    player_lengthLabel->caption=player_lengthText;
    player_lengthLabel->alignment=WA_RIGHT;
    player_lengthLabel->font=player_font;
    player_widgetList->addWidget(player_widgetList,player_lengthLabel);

    // volume

    gfx_fontSet(player_font);
    gfx_getStringSize("Vol",&w,&h);

    player_volumeLabel=label_create();
    player_volumeLabel->setRect(player_volumeLabel,LCD_WIDTH/2,LCD_HEIGHT-2*fontH,w,fontH);
    player_volumeLabel->caption="Vol";
    player_volumeLabel->font=player_font;
    player_volumeLabel->clearBackground=false;
    player_widgetList->addWidget(player_widgetList,player_volumeLabel);

    player_volumeTrackbar=trackbar_create();
    player_volumeTrackbar->setRect(player_volumeTrackbar,LCD_WIDTH/2+w+fontW/2,LCD_HEIGHT-2*fontH,LCD_WIDTH/2-w-fontW/2,fontH);
    player_volumeTrackbar->font=player_font;
    player_widgetList->addWidget(player_widgetList,player_volumeTrackbar);

    // help

    player_helpLabel=label_create();
    player_helpLabel->setRect(player_helpLabel,0,LCD_HEIGHT-fontH,LCD_WIDTH,fontH);
    player_helpLabel->caption="F2: playlist, F3: shell";
    player_helpLabel->font=player_font;
    player_helpLabel->clearBackground=false;
    player_widgetList->addWidget(player_widgetList,player_helpLabel);

    // track

    player_trackLabel=label_create();
    player_trackLabel->setRect(player_trackLabel,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    player_trackLabel->caption=player_trackText;
    player_trackLabel->font=player_font;
    player_trackLabel->clearBackground=false;
    player_widgetList->addWidget(player_widgetList,player_trackLabel);

    // state

    player_stateLabel=label_create();
    player_stateLabel->setRect(player_stateLabel,0,LCD_HEIGHT-2*fontH,LCD_WIDTH/2,fontH);
    player_stateLabel->font=player_font;
    player_widgetList->addWidget(player_widgetList,player_stateLabel);
}


static void player_readCfg(){
    CFG_DATA * cfg;

    //open cfg file
    cfg=cfg_readFile(PLAYER_CFG_FILE);

    if(cfg!=NULL){
        //read cfg values
        if(cfg_itemExists(cfg,"player_format")){
            strncpy(player_playerFormat,cfg_readString(cfg,"player_format"),sizeof(player_playerFormat));
        }

        if(cfg_itemExists(cfg,"playlist_format")){
            strncpy(player_playlistFormat,cfg_readString(cfg,"playlist_format"),sizeof(player_playlistFormat));
        }

        if(cfg_itemExists(cfg,"volume")){
            player_volume=cfg_readInt(cfg,"volume");
        }

        if(cfg_itemExists(cfg,"player_font")){
            player_font=cfg_readInt(cfg,"player_font");
        }

        if(cfg_itemExists(cfg,"playlist_font")){
            player_playlistFont=cfg_readInt(cfg,"playlist_font");
        }

        cfg_clear(cfg);
    }
}

static void player_writeCfg(){
    CFG_DATA * cfg;

    //open cfg file
    cfg=cfg_readFile(PLAYER_CFG_FILE);

    if(cfg==NULL){
        cfg=cfg_newFile();
    }

    //write cfg values
    cfg_writeString(cfg,"player_format",player_playerFormat);
    cfg_writeString(cfg,"playlist_format",player_playlistFormat);
    cfg_writeInt(cfg,"volume",player_volume);
    cfg_writeInt(cfg,"player_font",player_font);
    cfg_writeInt(cfg,"playlist_font",player_playlistFont);

    cfg_writeFile(cfg,PLAYER_CFG_FILE);

    cfg_clear(cfg);
}

static void player_applyCfg(){
    sound_setVolume(player_volume);
}

void player_init(){

    player_readCfg();

    player_buildGui();

    player_eventHandler=evt_getHandler(ALL_CLASS);
    player_visible=false;
    player_prevActiveItem=NULL;
    player_seeking=false;
    player_seekTime=0;

    tmr_setup(&player_timer,"player refresh");
    player_timer.action=player_timerAction;
    player_timer.freeRun=1;
    player_timer.stdDelay=HZ/10;

    player_applyCfg();
}

void player_close(){

    sound_stop();
    playlist_clear();
    player_writeCfg();

    player_widgetList->destroy(player_widgetList);
    evt_freeHandler(player_eventHandler);
    tmr_remove(&player_timer);
}
