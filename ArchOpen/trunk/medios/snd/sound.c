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

#ifdef HAVE_AIC23_SOUND
#include <driver/aic23.h>
#endif

#include <fs/stdfs.h>
#include <gfx/graphics.h>

#include <snd/buffer.h>
#include <snd/playlist.h>
#include <snd/codec.h>
#include <snd/output.h>

PLAYLIST_ITEM * sound_activeItem;

void sound_play(bool discard){

    if (sound_activeItem==NULL) return;

    printk("[sound] play %s\n",sound_activeItem->name);

    codec_trackStop();

    if(discard){
        output_discardBuffer();
    }

    buffer_setActiveItem(sound_activeItem);
    codec_setCodecFor(sound_activeItem->name);
    codec_trackStart();
}

void sound_prevTrack(bool discard){
    printk("[sound] prev track\n");

    if(sound_activeItem->prev){
        sound_activeItem=sound_activeItem->prev;
    }

    sound_play(discard);
}

void sound_nextTrack(bool discard){

    printk("[sound] next track\n");

    if(sound_activeItem->next){
        sound_activeItem=sound_activeItem->next;
        sound_play(discard);
    }else{
        printk("[sound] playlist end\n");
    }
}

void sound_trackEnd(){
    sound_nextTrack(false);
};

void sound_start(void)
{
    buffer_start();
    codec_start();
    output_start();
    
}

void sound_stop(void)
{
    codec_stop();
    output_stop();
    buffer_stop();

}

void sound_init()
{   
    playlist_init();
    buffer_init();
    codec_init();
    output_init();    

}

void sound_playFile(char * fName)
{
    int b;
    char s[100];
    int len,elap;
    int t,pt,pe;
    
    sound_start();   
    
    gfx_clearScreen(COLOR_BLACK);
        
#ifdef HAVE_AIC23_SOUND
    aic23_setOutputVolume(100,AIC23_CHANNEL_BOTH);
#endif
    playlist_clear();
    playlist_addFile(fName);

    sound_activeItem=playlist_first;

    output_enable(true);
    sound_play(false);

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
        sprintf(s,"%8d %8d",len,elap);
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

        if(b&BTMASK_F1){
            output_discardBuffer();
            codec_seekRequest(elap-500);
            while(codec_mustSeek(NULL));
            mdelay(100);
        }

        if(b&BTMASK_F2){
            output_discardBuffer();
            codec_seekRequest(elap+500);
            while(codec_mustSeek(NULL));
            mdelay(100);
        }

        yield();
    }while(!(b&BTMASK_OFF));

    sound_stop();

}
