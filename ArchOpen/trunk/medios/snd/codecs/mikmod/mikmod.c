/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <api.h>
#include "mikmod.h"
#include "mikmod_internals.h"

#define TAGREQUEST_ONLYTITLE

#define SAMPLE_RATE 44100

struct MREADER bufferReader;
struct MREADER fileReader;

int currentFile=-1;

BOOL mikmod_bufferSeek(struct MREADER* r,long offset,int whence){
//    printf("[MikMod] seek(%d,%d)\n",offset,whence);
    return buffer_seek(offset,whence)!=-1;
}

long mikmod_bufferTell(struct MREADER* r){
//    printf("[MikMod] tell()\n");
    return buffer_seek(0,SEEK_CUR);
}

BOOL mikmod_bufferRead(struct MREADER* r,void* ptr,size_t size){
//    printf("[MikMod] read(%p,%d)\n",ptr,size);
    return buffer_read(ptr,size);
}
int  mikmod_bufferGet(struct MREADER* r){
//    printf("[MikMod] get()\n");
    char c=0;
    buffer_read(&c,1);
    return c;
}
BOOL mikmod_bufferEof(struct MREADER* r){
//    printf("[MikMod] eof()\n");
    char c;

    if (buffer_read(&c,1)==1){

        buffer_seek(-1,SEEK_CUR);

        return false;
    }

    return true;
}

BOOL mikmod_fileSeek(struct MREADER* r,long offset,int whence){
//    printf("[MikMod] seek(%d,%d)\n",offset,whence);
    return lseek(currentFile,offset,whence)!=-1;
}

long mikmod_fileTell(struct MREADER* r){
//    printf("[MikMod] tell()\n");
    return lseek(currentFile,0,SEEK_CUR);
}

BOOL mikmod_fileRead(struct MREADER* r,void* ptr,size_t size){
//    printf("[MikMod] read(%p,%d)\n",ptr,size);
    return read(currentFile,ptr,size);
}
int  mikmod_fileGet(struct MREADER* r){
//    printf("[MikMod] get()\n");
    char c=0;
    read(currentFile,&c,1);
    return c;
}
BOOL mikmod_fileEof(struct MREADER* r){
//    printf("[MikMod] eof()\n");
    char c;

    if (read(currentFile,&c,1)==1){

        lseek(currentFile,-1,SEEK_CUR);

        return false;
    }

    return true;
}

void mikmod_errorHandler(){
    printf("[MikMod] error : %s\n",
            MikMod_strerror(MikMod_errno));
}

void mikmod_tagRequest(char * name,TAG * tag){
    printf("[MikMod] tagRequest()\n");

    tag->sampleRate=SAMPLE_RATE;
    tag->stereo=true;
    tag->badFile=true;

    currentFile=open(name,O_RDONLY);

    if (currentFile>=0){
#ifdef TAGREQUEST_ONLYTITLE
        tag->title=Player_LoadTitleGeneric(&fileReader);
        tag->badFile=false;
#else
        MODULE *module;

        module = Player_LoadGeneric(&fileReader, 64, 0);
        if (module) {
            printf("name: %s type: %s numchn: %d time: %d\n",module->songname,module->modtype,module->numchn,module->numpos);

            tag->title=strdup(module->songname);
            tag->genre=strdup(module->modtype);
            tag->bitRate=1000*module->numchn;
            tag->length=module->numpos*HZ*60;

            tag->badFile=false;
            Player_Free(module);
        }
#endif
        close(currentFile);
    }
}

void mikmod_trackLoop(){
    PLAYLIST_ITEM * item;
    MODULE *module;
    int time;

    printf("[MikMod] trackLoop()\n");

    item=buffer_getActiveItem();

    /* load module */
    module = Player_LoadGeneric(&bufferReader, 64, 0);
    if (module) {

        item->tag.bitRate=1000*module->numchn;
        item->tag.length=module->numpos*HZ*60;

        /* start module */
        Player_Start(module);

        while (Player_Active() && codec_mustContinue()) {
            MikMod_Update();

            if (codec_mustSeek(&time)){
                Player_SetPosition(time/(HZ*60));
                codec_seekDone();
            }

            codec_setElapsed(module->sngpos*HZ*60);
        }

        Player_Stop();
        Player_Free(module);
    } else
        printf("Could not load module, reason: %s\n",
                MikMod_strerror(MikMod_errno));
}


void codec_main(CODEC_GLOBAL_INFO * info){

    printf("[MikMod] main()\n");

    info->description="MikMod, module player";
    info->seekSupported=true;
    info->trackLoop=mikmod_trackLoop;
    info->tagRequest=mikmod_tagRequest;
    info->noTimeAdvance=true;

    bufferReader.Seek=mikmod_bufferSeek;
    bufferReader.Tell=mikmod_bufferTell;
    bufferReader.Read=mikmod_bufferRead;
    bufferReader.Get=mikmod_bufferGet;
    bufferReader.Eof=mikmod_bufferEof;

    fileReader.Seek=mikmod_fileSeek;
    fileReader.Tell=mikmod_fileTell;
    fileReader.Read=mikmod_fileRead;
    fileReader.Get=mikmod_fileGet;
    fileReader.Eof=mikmod_fileEof;

    MikMod_RegisterErrorHandler(mikmod_errorHandler);

    /* register all the drivers */
    MikMod_RegisterDriver(&drv_medios);

    /* register all the module loaders */
    MikMod_RegisterAllLoaders();

    /* initialize the library */
    md_mixfreq=SAMPLE_RATE;
    md_mode|=DMODE_SOFT_MUSIC|/*DMODE_HQMIXER|*/DMODE_INTERP|DMODE_16BITS|DMODE_STEREO;
    if (MikMod_Init("")) {
        printf("Could not initialize sound, reason: %s\n",
                MikMod_strerror(MikMod_errno));
        return;
    }
}
