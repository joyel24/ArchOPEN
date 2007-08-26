/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include "mpcdec.h"

MPC_SAMPLE_FORMAT dataBuf[MPC_DECODER_BUFFER_LENGTH];

mpc_reader bufferReader;
mpc_reader fileReader;

__IRAM_DATA mpc_decoder decoder;

mpc_int32_t musepack_bufferRead(void *data, void *ptr, mpc_int32_t size){
//    printf("[musepack] read %d\n",size);

    return buffer_read(ptr,size);
}

mpc_bool_t musepack_bufferSeek(void *data, mpc_int32_t offset){
//    printf("[musepack] seek %d\n",offset);

    return buffer_seek(offset,SEEK_SET)!=-1;
}

mpc_int32_t musepack_bufferTell(void *data){
//    printf("[musepack] tell\n");

    return buffer_seek(0,SEEK_CUR);
}

mpc_int32_t musepack_bufferGetSize(void *data){
//    printf("[musepack] get_size\n");

    return buffer_getActiveItem()->fileSize;
}


mpc_int32_t musepack_fileRead(void *data, void *ptr, mpc_int32_t size){
//    printf("[musepack] read %d\n",size);
    int f=*((int *)data);

    return read(f,ptr,size);
}

mpc_bool_t musepack_fileSeek(void *data, mpc_int32_t offset){
//    printf("[musepack] seek %d\n",offset);
    int f=*((int *)data);

    return lseek(f,offset,SEEK_SET)!=-1;
}

mpc_int32_t musepack_fileTell(void *data){
//    printf("[musepack] tell\n");
    int f=*((int *)data);

    return lseek(f,0,SEEK_CUR);
}

mpc_int32_t musepack_fileGetSize(void *data){
//    printf("[musepack] get_size\n");
    int f=*((int *)data);

    return filesize(f);
}


mpc_bool_t musepack_canSeek(void *data){
//    printf("[musepack] canseek\n");

    return true;
}

void musepack_tagRequest(char * name,TAG * tag){
    mpc_streaminfo info;
    int res;
    int f;

    printf("[musepack] tagRequest()\n");

    tag->badFile=true;

    f=open(name,O_RDONLY);

    if(f>=0){

        fileReader.data=&f;

        res=mpc_streaminfo_read(&info, &fileReader);

        if(res!=ERROR_CODE_OK){
            printf("[musepack] error: couldn't read streaminfo, code: %d\n",res);
        }else{

            tag->sampleRate=info.sample_freq;
            tag->stereo=info.channels==2;
            tag->bitRate=info.average_bitrate;
            tag->length=HZ*mpc_streaminfo_get_length(&info);

            //printf("sr %d st %d br %d l %d\n",tag->sampleRate,tag->stereo,tag->bitRate,tag->length);

            tag->badFile=false;
        }

        close(f);
    }else{
        printf("[musepack] error : can't open file\n");
    }
}

void musepack_trackLoop(){
    mpc_streaminfo info;
    int red;
    int res;
    int time;

    MPC_SAMPLE_FORMAT sample;
    MPC_SAMPLE_FORMAT * ip;
    short * op;
    int nb;

    printf("[musepack] trackLoop()\n");

    mpc_streaminfo_init(&info);

    res=mpc_streaminfo_read(&info, &bufferReader);

    if(res!=ERROR_CODE_OK){
        printf("[musepack] error: couldn't read streaminfo, code: %d\n",res);
        return;
    }

    mpc_decoder_setup(&decoder, &bufferReader);

    mpc_decoder_set_seeking(&decoder, &info, 0);

    if (!mpc_decoder_initialize(&decoder, &info)) {
        printf("[musepack] error: couldn't initialize decoder\n");
        return;
    }

    do{
        red=mpc_decoder_decode(&decoder,dataBuf,NULL,NULL);

        if(red==-1){
            printf("[musepack] error: decoding failed\n");
            break;
        }

        // convert samples from 32bit fixed point to 16bit
        ip=dataBuf;
        op=(short *)dataBuf;
        nb=red*info.channels;
        while(nb--){
            sample=(*(ip++))>>(MPC_FIXED_POINT_SCALE_SHIFT-16);

            // clipping
            sample=MIN(32767,sample);
            sample=MAX(-32768,sample);

            *(op++)=sample;
        }

        output_write(dataBuf,red*2*info.channels);

        if(codec_mustSeek(&time)){

            mpc_decoder_seek_seconds(&decoder,1.0*time/HZ);

            codec_setElapsed(time);
            codec_seekDone();
        }

    }while(codec_mustContinue() && red>0);
}

void codec_main(CODEC_GLOBAL_INFO * info){

    info->description="Musepack Codec";
    info->seekSupported=true;
    info->trackLoop=musepack_trackLoop;
    info->tagRequest=musepack_tagRequest;

    bufferReader.read=musepack_bufferRead;
    bufferReader.seek=musepack_bufferSeek;
    bufferReader.tell=musepack_bufferTell;
    bufferReader.get_size=musepack_bufferGetSize;
    bufferReader.canseek=musepack_canSeek;
    bufferReader.data=NULL;

    fileReader.read=musepack_fileRead;
    fileReader.seek=musepack_fileSeek;
    fileReader.tell=musepack_fileTell;
    fileReader.get_size=musepack_fileGetSize;
    fileReader.canseek=musepack_canSeek;
    fileReader.data=NULL;
}
