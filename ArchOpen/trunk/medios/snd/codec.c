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

#include <snd/buffer.h>
#include <snd/codec.h>
#include <snd/playlist.h>
#include <snd/output.h>
#include <snd/sound.h>

#include <init/boot_error.h>

CODEC_INFO * codec_first = NULL;
CODEC_INFO * codec_last = NULL;
CODEC_INFO * codec_current = NULL;

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
        // make sure output params are correctly set up
        output_outputParamsChanged();
        // call trackloop if exist
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

static void codec_findCodecs(){
    DIR * codec_folder;
    int fd;
    struct dirent * entry;
    char header[4];
    CODEC_INFO * cInfo;
    char item_size;
    char * fname;

    /* searching for codecs */
    codec_folder=opendir(CODECS_DIR);
    if(codec_folder)
    {
        while((entry=readdir(codec_folder))!=NULL)
        {
            fname=(char*)malloc(strlen(entry->d_name)+strlen(CODECS_DIR)+1);
            strcpy(fname,CODECS_DIR);
            strcat(fname,entry->d_name);

            if(entry->d_name[0]=='.' || (entry->d_name[0]=='.' && entry->d_name[0]=='.'))
            {
                /* ./ or ../ folder entry ==> discard */
                continue;
            } 
            
            if(entry->type!=VFS_TYPE_FILE)
            {
                printk("discard: '%s' not a file\n",entry->d_name);
                kfree(fname);
                continue;
            }
            fd=open(fname,O_RDONLY);
            if(fd<0)
            {
                printk("Error opening file: %d\n",-fd);
                kfree(fname);
                continue; 
            }
            /* is is a codec file ?*/
            if(read(fd,header,4)!=4)
            {
                printk("Can't read Header\n");
                kfree(fname);
                close(fd);
                continue;
            }
            
            if(strncmp(header,"CDEC",4))
            {
                printk("Not a codec file: %s\n",fname);
                kfree(fname);
                close(fd);
                continue;   
            }
            
            /* created codec info */
            cInfo=codec_new();
            cInfo->filename=fname;
            cInfo->fOffset=5;
            if(read(fd,(void*)&item_size,1)!=1)
            {
                printk("Can't read name size\n");
                kfree(fname);
                close(fd);
                codec_remove(cInfo);
                continue;
            }
            cInfo->fOffset+=(item_size+1);
            cInfo->name=(char*)malloc(item_size+1);
            if(read(fd,cInfo->name,item_size)!=item_size)
            {
                printk("Can't read name\n");
                kfree(fname);
                close(fd);
                codec_remove(cInfo);
                continue;
            }
            cInfo->name[(int)item_size]='\0'; /*leading zero*/
            if(read(fd,(void*)&item_size,1)!=1)
            {
                printk("Can't read extension list size\n");
                free(fname);
                kfree(cInfo->name);
                close(fd);
                codec_remove(cInfo);
                continue;
            }
            cInfo->fOffset+=item_size;
            cInfo->extensions=(char*)malloc(item_size+1);
            if(read(fd,cInfo->extensions,item_size)!=item_size)
            {
                printk("Can't read extension list\n");
                free(fname);
                kfree(cInfo->name);
                kfree(cInfo->extensions);
                close(fd);
                codec_remove(cInfo);
                continue;
            }
            cInfo->extensions[(int)item_size]='\0'; /*leading zero*/
            close(fd);
        }
    }
    else
    {
        gui_bootError(MISSING_CODEC_ERROR,BOOT_WARN);
    }
    closedir(codec_folder);
}


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
    info->globalInfo.codecDeactivate=NULL;
    info->globalInfo.codecActivate=NULL;
    info->globalInfo.tagRequest=NULL;
    info->globalInfo.noTimeAdvance=false;
    info->globalInfo.loadOk=true;
    info->next=NULL;

    // handle linked list
    if(codec_first==NULL) codec_first=info;
    if(codec_last!=NULL) codec_last->next=info;
    codec_last=info;

    return info;
}

bool codec_remove(CODEC_INFO * info){
    CODEC_INFO * prev;

    if(info==NULL) return false;

    // find previous item
    prev=codec_first;
    while(prev!=NULL && prev->next!=info){
        prev=prev->next;
    }

    // link previous and next items
    if(prev!=NULL){
        prev->next=info->next;
    }

    // reajust first or last if necessary
    if(info==codec_first){
        codec_first=info->next;
    }

    if(info==codec_last){
        codec_last=prev;
    }

    kfree(info);

    return true;
}


bool codec_load(CODEC_INFO * info){
    int use_iram;

    if(info->loaded) return true;

    //load codec

#ifdef DM320
    // don't use DM320 iram, too small and not faster than sdram
    use_iram=0;
#else
    use_iram=1;
#endif

    if(med_loadMed(info->filename,&info->medInfo,info->fOffset,use_iram)==MED_OK)
    {
        ((void (*)(CODEC_GLOBAL_INFO * info))info->medInfo.entry)(&info->globalInfo);
        if(info->globalInfo.loadOk)
        {
            printk("[codec] loaded codec %s, description=%s\n",info->name,info->globalInfo.description);
            info->loaded=true;
            return true;
        }
        else
        {
            printk("[codec] loaded codec %s, error during init\n");
            info->loaded=false;
            return false;
        }
    }
    else
    {
        printk("[codec] error loading codec %s\n",info->name);
        return false;
    }
}

CODEC_INFO * codec_findCodecFor(char * name){
    char * ext;
    char * extlist;
    char * tok;
    char * context;
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
        extlist=strdup(info->extensions);

        // scan through | separated extensions
        tok=strtok_r(extlist,"|",&context);
        while(tok!=NULL){

            if(!strcmp(ext,tok)){
                found=true;
            }

            tok=strtok_r(NULL,"|",&context);
        }

        kfree(extlist);

        if(found){
            break;
        }

        info=info->next;
    }

    if(!found)
        info=NULL;

    kfree(ext);

    return info;
}

bool codec_setCodecFor(char * name){
    CODEC_INFO * info;

    // find suitable codec
    info=codec_findCodecFor(name);
    if (info==NULL){
        return false;
    }

    // call codec deactivate function if it exists  
    if(codec_current && codec_current!=info)
    {
        if(codec_current->globalInfo.codecDeactivate)
            codec_current->globalInfo.codecDeactivate();
    }
    
    // set current codec  
    codec_current=info;

    // make sure it is loaded
    if(codec_load(info))
    {    
        // call codec activation if it exists
        if(codec_current->globalInfo.codecActivate)
            codec_current->globalInfo.codecActivate();        
    }
    else
        return false;

    return true;
}

void codec_trackStart(){
    codec_trackStop();
    codec_startOutPos=output_writePos;
    codec_timeDelta=0;
    
    codec_seekRequested=false;
    
    // we need to have codec iram stuff in iram before the track loop
    med_copyToIram(&codec_current->medInfo);
    // set start request, wait for ack from thread if we're not in thread
    codec_startRequested=true;
    while(THREAD_SELF()!=codec_thread && codec_startRequested){
        yield();
    }
}

void codec_trackStop(){
    //loop not running -> nothing to stop
    if(!codec_loopRunning)
        return;

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

    if(codec_loopRunning && codec_current->globalInfo.seekSupported){

        codec_seekTime=MAX(0,time);
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


bool codec_getElapsed(int * elapsed){

    if(!codec_loopRunning) return false;

    if(elapsed){
        if(codec_current->globalInfo.noTimeAdvance){

            *elapsed=codec_timeDelta;

        }else{

            int pos;
            PLAYLIST_ITEM * item;
    
            item=buffer_getActiveItem();
    
            pos=MAX(output_readPos-codec_startOutPos,0);
    
            *elapsed=codec_timeDelta+(long long)pos*HZ/(item->tag.sampleRate*((item->tag.stereo)?4:2));

        }
    }

    return true;
}

void codec_setElapsed(int elapsed){
    int prevElap;

    codec_getElapsed(&prevElap);

    codec_timeDelta+=elapsed-prevElap;
}

bool codec_tagRequest(char * name, TAG * tag){
    CODEC_INFO * info;

    info=codec_findCodecFor(name);

    if(info==NULL) return false;

    codec_load(info);

    if(info->globalInfo.tagRequest==NULL){
        printk("[codec] error: tagRequest not assigned\n");
        return false;
    }

    info->globalInfo.tagRequest(name,tag);
    
    return true;
}

void codec_start(void){

    printk("[Codec] starting\n");

    codec_startRequested=false;
    codec_stopRequested=false;
    codec_loopRunning=false;

    codec_seekRequested=false;
    codec_seekTime=0;

    codec_startOutPos=0;
    codec_timeDelta=0;

    codec_current=NULL;
    
    thread_enable(codec_thread->pid);
}

void codec_stop(void){

    codec_trackStop();

    thread_disable(codec_thread->pid);
}

void codec_init(){
    codec_first=NULL;
    codec_last=NULL;

    codec_thread=NULL;

    // scan codec folder
    codec_findCodecs();

    // create codec thread
    thread_startFct(&codec_thread,codec_threadFunction,"Codec thread",
                     THREAD_STATE_DISABLE,PRIO_HIGH,THREAD_USE_SYS_STACK);
}
