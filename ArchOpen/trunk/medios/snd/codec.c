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
#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <fs/stdfs.h>
#include <fs/med.h>

#include <snd/codec.h>
#include <snd/playlist.h>
#include <snd/output.h>
#include <snd/sound.h>

//#include <snd/wav.h>
#include <snd/tremor.h>

CODEC_INFO * codec_first = NULL;
CODEC_INFO * codec_last = NULL;
CODEC_INFO * codec_current = NULL;

CODEC_TRACK_INFO codec_trackInfo;

int codec_startOutPos = 0;
int codec_timeDelta = 0;

bool codec_startRequested = false;
bool codec_stopRequested = false;

bool codec_loopRunning = false;

bool codec_seekRequested = false;
int codec_seekTime = 0;

static THREAD_INFO * codec_thread = NULL;

static void codec_threadFunction(){

    for(;;){
        // wait for a start new track request
        while(!codec_startRequested){
            yield();
        }
        codec_startRequested=false;

        // call trackloop
        if(codec_current->globalInfo.trackLoop!=NULL){
            codec_loopRunning=true;
            codec_current->globalInfo.trackLoop();
        }else{
            printk("[codec] error: trackLoop not assigned\n");
        }
        codec_loopRunning=false;

        // track end was reached?
        if(!codec_stopRequested){
            sound_trackEnd();
        }
    }
};

CODEC_INFO * codec_new(){
    CODEC_INFO * info;

    // alloc data
    info=malloc(sizeof(CODEC_INFO));

    // default values
    info->name="";
    info->extensions="";
    info->loaded=false;
    info->globalInfo.description="";
    info->globalInfo.seekSupported=false;
    info->globalInfo.trackLoop=NULL;
    info->next=NULL;

    // handle linked list
    if(codec_first==NULL) codec_first=info;
    if(codec_last!=NULL) codec_last->next=info;
    codec_last=info;

    return info;
}

bool codec_load(CODEC_INFO * info){

    if(info->loaded) return true;
    
    //load codec

    //TODO: I need med codec system to implement this
    //if(!strcmp(info->name,"wav.c")) wav_main();
    //if(!strcmp(info->name,"tremor.c")) tremor_main();

    printk("[codec] loaded codec %s, description=%s\n",info->name,info->globalInfo.description);

    info->loaded=true;

    return true;
}

CODEC_INFO * codec_findCodecFor(char * name){
    char * ext;
    char * tok;
    bool found;
    CODEC_INFO * info;

    // get extension
    ext=strrchr(name,'.');
    if(ext==NULL){
        return NULL;
    }

    // skip .
    ext++;

    // force lowercase
    ext=strdup(ext);
    strlwr(ext);
    
    // scan through codecs
    found=false;
    info=codec_first;
    while(info!=NULL){

        // scan through | separated extensions
        tok=strtok(info->extensions,"|");
        while(tok!=NULL){

            if(!strcmp(ext,tok)){
                found=true;
            }
            tok=strtok(NULL,"|");
        }

        if(found){
            break;
        }

        info=info->next;
    }

    free(ext);

    return info;
}

bool codec_setCodecFor(char * name){
    CODEC_INFO * info;

    // find suitable codec
    info=codec_findCodecFor(name);
    if (info==NULL){
        return false;
    }

    // set current codec
    codec_current=info;

    // make sure it is loaded
    if(!codec_load(info)){
        return false;
    }

    return true;
}

void codec_trackStart(){

    codec_trackStop();

    // init trackInfo
    codec_trackInfo.validTrack=false;
    codec_trackInfo.length=0;
    codec_trackInfo.sampleRate=0;
    codec_trackInfo.stereo=false;

    codec_startOutPos=output_writePos;
    codec_timeDelta=0;
    codec_seekRequested=false;

    // set start request, wait for ack from thread if we're not in thread
    codec_startRequested=true;
    while(THREAD_SELF()!=codec_thread && codec_startRequested){
        yield();
    }
};

void codec_trackStop(){

    //loop not running -> nothing to stop
    if(!codec_loopRunning){
        return;
    }

    // set stop request, wait for loop to stop running
    codec_stopRequested=true;
    while(codec_loopRunning){
        yield();
    }
    codec_stopRequested=false;
}

bool codec_mustContinue(){
    return !codec_stopRequested;
}

bool codec_seekRequest(int time){

    if(codec_current->globalInfo.seekSupported){

        codec_seekTime=time;
        codec_seekRequested=true;

        return true;
    }else{
        return false;
    }
}

bool codec_mustSeek(int * time){

    if(codec_seekRequested && time!=NULL){
        *time=codec_seekTime;
    }

    return codec_seekRequested;
}

void codec_seekDone(){
    codec_seekRequested=false;
}


bool codec_getTimes(int * length,int * elapsed){

    if(!codec_loopRunning || !codec_trackInfo.validTrack) return false;

    if(length){
        *length=codec_trackInfo.length;
    }

    if(elapsed){
        int pos;

        pos=MAX(output_readPos-codec_startOutPos,0);

        *elapsed=codec_timeDelta+pos*HZ/(codec_trackInfo.sampleRate*((codec_trackInfo.stereo)?4:2));
    }

    return true;
}

void codec_setElapsed(int elapsed){
    int prevElap;

    codec_getTimes(NULL,&prevElap);

    codec_timeDelta+=elapsed-prevElap;
}

void codec_setGlobalInfo(CODEC_GLOBAL_INFO * info){
    codec_current->globalInfo=*info;
}

void codec_setTrackInfo(CODEC_TRACK_INFO * info){
    codec_trackInfo=*info;

    // inform output
    output_outputParamsChanged(codec_trackInfo.sampleRate,codec_trackInfo.stereo);
}

void codec_init(){

    med_t medInfo;
    
    codec_first=NULL;
    codec_last=NULL;
    codec_current=NULL;

    memset(&codec_trackInfo,0,sizeof(codec_trackInfo));

    codec_startRequested=false;
    codec_stopRequested=false;
    codec_loopRunning=false;

    codec_seekRequested=false;
    codec_seekTime=0;

    codec_startOutPos=0;
    codec_timeDelta=0;

    codec_thread=NULL;

    //start codec thread
    thread_startFct(&codec_thread,codec_threadFunction,"Codec thread",THREAD_STATE_ENABLE,PRIO_HIGH);

    if(med_loadMed("/medios/codec/wav.cod",&medInfo)==MED_OK)
        ((void (*)(void))medInfo.entry)();
    
    if(med_loadMed("/medios/codec/tremor.cod",&medInfo)==MED_OK)
        ((void (*)(void))medInfo.entry)();
    
    /*{
        CODEC_INFO * info;
        info=codec_new();
        info->name="wav.c";
        info->extensions="wav";
    }*/

    /*{
        CODEC_INFO * info;
        info=codec_new();
        info->name="tremor.c";
        info->extensions="ogg";
    }*/

}
