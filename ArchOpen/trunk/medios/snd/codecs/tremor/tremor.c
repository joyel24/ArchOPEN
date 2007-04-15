/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
/*
#include <lib/string.h>
#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/evt.h>
#include <fs/stdfs.h>

#include <snd/tremor.h>
#include <snd/codec.h>
#include <snd/buffer.h>
#include <snd/output.h>
*/

#include "medios.h"
#include <snd/tremor.h>
#include "ivorbisfile.h"
#include "ivorbiscodec.h"

char dataBuf[DATA_BUFFER_SIZE];

ov_callbacks callbacks;

size_t tremor_read(void *ptr, size_t size, size_t nmemb, void *datasource){
//    printf("[tremor] read\n");

    return buffer_read(ptr,size*nmemb);
};

int tremor_seek(void *datasource, ogg_int64_t offset, int whence){
//    printf("[tremor] seek %d %d\n",offset,whence);

    return buffer_seek(offset,whence);
};

int tremor_close(void *datasource){
//    printf("[tremor] close\n");

    return 0;
};

long tremor_tell(void *datasource){
//    printf("[tremor] tell\n");

    return buffer_seek(0,SEEK_CUR);
};

void tremor_trackLoop(){
    CODEC_TRACK_INFO trackInfo;
    vorbis_info * v_info;
    int res;
    int red;
    int bitstream;
    OggVorbis_File vf;
    int time;

    trackInfo.validTrack=false;
    trackInfo.sampleRate=0;
    trackInfo.length=0;
    trackInfo.stereo=false;

    printf("[tremor] trackLoop()\n");

    res=ov_open_callbacks((void *)1,&vf,NULL,0,callbacks);

    printf("res %d\n",res);

    if(res==0){
        trackInfo.validTrack=true;

        v_info=ov_info(&vf,-1);

        if(v_info!=NULL){
            printf("info ok %d %d\n",v_info->rate,v_info->bitrate_nominal);

            trackInfo.sampleRate=v_info->rate;
            trackInfo.stereo=v_info->channels==2;

            trackInfo.length=ov_time_total(&vf,-1)/(1000/HZ);
        }
    }

    // send track info
    codec_setTrackInfo(&trackInfo);

    if(!trackInfo.validTrack) return;

    do{
        red=ov_read(&vf,dataBuf,DATA_BUFFER_SIZE,&bitstream);

        output_write(dataBuf,red);

        if(codec_mustSeek(&time)){
            int res;

            res=ov_time_seek_page(&vf,time*(1000/HZ));

            if (res==0){

                time=ov_time_tell(&vf);

                codec_setElapsed(time/(1000/HZ));
            }

            codec_seekDone();
        }

    }while(codec_mustContinue() && red>0);
    
    ov_clear(&vf);
}

void codec_main(CODEC_INFO * info)
{
    info->globalInfo.description="Tremor, OGG Vorbis codec";
    info->globalInfo.seekSupported=true;
    info->globalInfo.trackLoop=tremor_trackLoop;
 
    printf("[tremor] main()\n");
    
    callbacks.read_func=tremor_read;
    callbacks.seek_func=tremor_seek;
    callbacks.close_func=tremor_close;
    callbacks.tell_func=tremor_tell;
}

