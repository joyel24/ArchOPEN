/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include "medios.h"
#include "ivorbisfile.h"
#include "ivorbiscodec.h"

#define DATA_BUFFER_SIZE 4096

char dataBuf[DATA_BUFFER_SIZE];

ov_callbacks bufferCallbacks;
ov_callbacks fileCallbacks;

size_t tremor_fileRead(void *ptr, size_t size, size_t nmemb, void *datasource){
//    printf("[tremor] read\n");

    return read((int)datasource,ptr,size*nmemb);
};

int tremor_fileSeek(void *datasource, ogg_int64_t offset, int whence){
//    printf("[tremor] seek %d %d\n",offset,whence);

    return lseek((int)datasource,offset,whence);
};

int tremor_fileClose(void *datasource){
//    printf("[tremor] close\n");

    return close((int)datasource);
};

long tremor_fileTell(void *datasource){
//    printf("[tremor] tell\n");

    return lseek((int)datasource,0,SEEK_CUR);
};


size_t tremor_bufferRead(void *ptr, size_t size, size_t nmemb, void *datasource){
//    printf("[tremor] read\n");

    return buffer_read(ptr,size*nmemb);
};

int tremor_bufferNoSeek(void *datasource, ogg_int64_t offset, int whence){
//    printf("[tremor] seek %d %d\n",offset,whence);

    return -1;
};

int tremor_bufferSeek(void *datasource, ogg_int64_t offset, int whence){
//    printf("[tremor] seek %d %d\n",offset,whence);

    return buffer_seek(offset,whence);
};

int tremor_bufferClose(void *datasource){
//    printf("[tremor] close\n");

    return 0;
};

long tremor_bufferTell(void *datasource){
//    printf("[tremor] tell\n");

    return buffer_seek(0,SEEK_CUR);
};

void tremor_tagRequest(char * name,TAG * tag){
    int res;
    int f;
    int i;
    OggVorbis_File vf;
    vorbis_comment * comment;
    vorbis_info * info;

    printf("[tremor] tagRequest()\n");

    tag->badFile=true;

    f=open(name,O_RDONLY);

    if(f>=0){

        res=ov_open_callbacks((void *)f,&vf,NULL,0,fileCallbacks);

        if(res==0){

            tag->badFile=false;

            tag->length=ov_time_total(&vf,-1)/(1000/HZ);
            tag->bitRate=ov_bitrate(&vf,-1);
            tag->sampleCount=ov_pcm_total(&vf,-1);

            info=ov_info(&vf,-1);

            tag->sampleRate=info->rate;
            tag->stereo=info->channels==2;

            comment=ov_comment(&vf,-1);

            for(i=0;i<comment->comments;++i){
                char * key;
                char * value;
                char * delim;

                delim=strchr(comment->user_comments[i],'=');

                if(delim!=NULL){
                    *delim='\0';

                    key=comment->user_comments[i];
                    value=delim+1;

                    if(!strcasecmp(key,"title")){
                        tag->title=strdup(value);
                    }

                    if(!strcasecmp(key,"artist")){
                        tag->artist=strdup(value);
                    }

                    if(!strcasecmp(key,"title")){
                        tag->title=strdup(value);
                    }

                    if(!strcasecmp(key,"album")){
                        tag->album=strdup(value);
                    }

                    if(!strcasecmp(key,"genre")){
                        tag->genre=strdup(value);
                    }

                    if(!strcasecmp(key,"tracknumber")){
                        tag->track=strdup(value);
                    }

                    if(!strcasecmp(key,"date")){
                        tag->date=strdup(value);
                    }

                    *delim='=';
                }
            }

            ov_clear(&vf);

        }else{

            close(f);
        }
    }
}


void tremor_trackLoop(){
    int res;
    int red;
    int bitstream;
    OggVorbis_File vf;
    int time;
    PLAYLIST_ITEM * item;

    ogg_int64_t vf_offsets[2];
    ogg_int64_t vf_dataoffsets;
    ogg_uint32_t vf_serialnos;
    ogg_int64_t vf_pcmlengths[2];

    printf("[tremor] trackLoop()\n");

    res=ov_open_callbacks((void *)1,&vf,NULL,0,bufferCallbacks);

    printf("res %d\n",res);

    if(res!=0) return;

    item=buffer_getActiveItem();

    // data is openned as non seekable to avoid buffering the whole file
    // now we hack through OggVorbis_File to make it seekable again
    // (idea taken from the Rockbox project)

    vf.offsets=vf_offsets;
    vf.dataoffsets=&vf_dataoffsets;
    vf.serialnos=&vf_serialnos;
    vf.pcmlengths=vf_pcmlengths;

    vf.offsets[0]=0;
    vf.offsets[1]=item->fileSize;
    vf.dataoffsets[0]=vf.offset;
    vf.pcmlengths[0]=0;
    vf.pcmlengths[1]=item->tag.sampleCount;
    vf.serialnos[0]=vf.current_serialno;
    vf.callbacks.seek_func=tremor_bufferSeek;
    vf.seekable=1;
    vf.end=item->fileSize;
    vf.ready_state=OPENED;
    vf.links=1;

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

void codec_main(CODEC_GLOBAL_INFO * info){

    printf("[tremor] main()\n");

    info->description="Tremor, OGG Vorbis codec";
    info->seekSupported=true;
    info->trackLoop=tremor_trackLoop;
    info->tagRequest=tremor_tagRequest;


    bufferCallbacks.read_func=tremor_bufferRead;
    bufferCallbacks.seek_func=tremor_bufferNoSeek;
    bufferCallbacks.close_func=tremor_bufferClose;
    bufferCallbacks.tell_func=tremor_bufferTell;

    fileCallbacks.read_func=tremor_fileRead;
    fileCallbacks.seek_func=tremor_fileSeek;
    fileCallbacks.close_func=tremor_fileClose;
    fileCallbacks.tell_func=tremor_fileTell;
}

