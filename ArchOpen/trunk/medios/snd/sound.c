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
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/delay.h>
#include <kernel/irq.h>
#include <kernel/timer.h>

#include <driver/buttons.h>
#include <driver/dsp.h>
#include <driver/energy.h>

#ifdef HAVE_MAS_SOUND
#include <driver/mas.h>
#endif

#ifdef HAVE_AIC23_SOUND
#include <driver/aic23.h>
#endif

#include <fs/stdfs.h>
#include <gfx/graphics.h>

#include <snd/buffer.h>
#include <snd/playlist.h>
#include <snd/codec.h>
#include <snd/output.h>
#include <snd/sound.h>

#include <gui/player.h>
#include <gui/playlistmenu.h>

PLAYLIST_ITEM * sound_activeItem;

SOUND_STATE sound_state;

bool sound_started=false;

#include "dspshared.h"

void sound_play(bool discard)
{       
    if (sound_activeItem==NULL) 
    {
        printk("[sound_play] no active item to play\n");
        return; /*no active item to play ==> exit*/
    }
        
    printk("[sound] play %s\n",sound_activeItem->name);

    // make sure the codec thread is not waiting for output
    if(sound_state==SS_PAUSED)
    {
        output_discardBuffer();
    }

    codec_trackStop();

    /* NOTE: why doing discard twice ? or discard never true when sound_state==SS_PAUSED */
    if(discard)
    {
        output_discardBuffer();
    }

    buffer_setActiveItem(sound_activeItem);

    if(codec_setCodecFor(sound_activeItem->name))
    {
        output_enable(true);
        sound_state=SS_PLAYING;

        codec_trackStart();
    }
}

void sound_prevTrack(bool discard){

    if (sound_activeItem==NULL) return;

    printk("[sound] prev track\n");

    if(sound_activeItem->prev!=NULL){
        sound_activeItem=sound_activeItem->prev;
    }

    sound_play(discard);
}

void sound_nextTrack(bool discard){

    if (sound_activeItem==NULL) return;

    printk("[sound] next track\n");

    if(sound_activeItem->next!=NULL){
        sound_activeItem=sound_activeItem->next;
        sound_play(discard);
    }else{
        printk("[sound] playlist end\n");
        halt_disableTimer(TIMER_ENABLE);
    }
}

void sound_trackEnd(){

    sound_state=SS_STOPPED;

    sound_nextTrack(false);
}

void sound_pause(bool paused)
{
    if(sound_state!=SS_STOPPED)
    {
       output_enable(!paused);
        
        if(paused){
            sound_state=SS_PAUSED;
        }else{
            sound_state=SS_PLAYING;
        }
    }
}

void sound_seek(int time)
{    
    if(sound_state==SS_STOPPED) return;
    output_discardBuffer();
        
    codec_seekRequest(time);
    while(codec_mustSeek(NULL)) /*nothing*/;
}

void sound_setVolume(int volume){
    output_setVolume(volume);
}

void sound_start(void)
{
    if(!sound_started)
    {
        buffer_start();
        codec_start();
        output_start();
                    
        halt_disableTimer(TIMER_DISABLE);
        
        sound_activeItem=NULL;
        sound_state=SS_STOPPED;
        
        sound_started=true;
    }
}

void sound_stop(void)
{
    if(sound_started)
    {
        output_discardBuffer();
        codec_stop();
        
        output_stop();
        buffer_stop();
        halt_disableTimer(TIMER_ENABLE);
        
        sound_activeItem=NULL;
        sound_state=SS_STOPPED;
        sound_started=false;
    }
}

void sound_init(){
    playlist_init();
    buffer_init();
    codec_init();
    output_init();

    sound_activeItem=NULL;
    sound_state=SS_STOPPED;
    sound_started=false;
}

void sound_playFile(char * fName){
    int b;
    char s[200];
    int len,elap;
    int t,pt,pe;
    int vol;

    sound_start();

    gfx_clearScreen(COLOR_BLACK);
    gfx_fontSet(0);
    playlist_clear();

    playlist_addFile(fName,playlist_last);

    sound_activeItem=playlist_first;

    sound_play(false);

    vol=90;

    pt=tmr_getTick();
    pe=0;
    do{
        b=btn_readState();

        strcpy(s,sound_activeItem->name);
        strcat(s,"                   ");
        gfx_putS(COLOR_WHITE,COLOR_BLACK,0,0,s);

        len=elap=-1;
        len=sound_activeItem->tag.length;
        codec_getElapsed(&elap);
        sprintf(s,"%8d %8d   %d   ",len,elap,vol);
        gfx_putS(COLOR_WHITE,COLOR_BLACK,0,20,s);

        *s='\0';
        if(sound_activeItem->tag.title!=NULL){
            strcpy(s,sound_activeItem->tag.title);
        };
        strcat(s,"                   ");
        gfx_putS(COLOR_WHITE,COLOR_BLACK,0,40,s);

        *s='\0';
        if(sound_activeItem->tag.artist!=NULL){
            strcpy(s,sound_activeItem->tag.artist);
        };
        strcat(s,"                   ");
        gfx_putS(COLOR_WHITE,COLOR_BLACK,0,50,s);

        sprintf(s,"%d     ",sound_activeItem->tag.bitRate/1000);
        gfx_putS(COLOR_WHITE,COLOR_BLACK,0,60,s);

        t=tmr_getTick();
        if(t-pt>=100){
            int pc=(elap-pe)*100/(t-pt);

            sprintf(s,"%d     ",pc);
            gfx_putS(COLOR_WHITE,COLOR_BLACK,0,30,s);

            pe=elap;
            pt=tmr_getTick();
        }

        if(b&BTMASK_RIGHT){
            sound_nextTrack(true);
            mdelay(500);
        }

        if(b&BTMASK_LEFT){
            sound_prevTrack(true);
            mdelay(500);
        }

        if(b&BTMASK_UP){
            vol=MIN(100,vol+1);
            sound_setVolume(vol);
            mdelay(50);
        }

        if(b&BTMASK_DOWN){
            vol=MAX(0,vol-1);
            sound_setVolume(vol);
            mdelay(50);
        }

        if(b&BTMASK_F1){
            sound_seek(elap-500);
            mdelay(100);
        }

        if(b&BTMASK_F2){
            sound_seek(elap+500);
            mdelay(100);
        }

        if(b&BTMASK_F3){
            playlistMenu_eventLoop();
            gfx_clearScreen(COLOR_BLACK);
            mdelay(100);
        }

        if(b&BTMASK_BTN1){
            sound_pause(true);
        }

        if(b&BTMASK_BTN2){
            sound_pause(false);
        }

        yield();
    }while(!(b&BTMASK_OFF));

    sound_stop();
}
