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

#include "wav.h"

char dataBuf[DATA_BUFFER_SIZE];
char processBuf[DATA_BUFFER_SIZE];

int wav_8Bit(){
    int red;
    char * ip;
    short * op;

    red=buffer_read(processBuf,DATA_BUFFER_SIZE/2);

    ip=processBuf;
    op=(short *)dataBuf;
    while(op<(short *)(dataBuf+DATA_BUFFER_SIZE)){
        *(op++)=(*(ip++)<<8)-0x8000;
    }

    return red;
}

bool wav_verifyHeader(bool onBuffer,int f){
    WAV_HEADER header;

    if(onBuffer){
        buffer_seek(0,SEEK_SET);
        buffer_read(&header,sizeof(header));
    }else{
        lseek(f,0,SEEK_SET);
        read(f,&header,sizeof(header));
    }

    if(!memcmp(header.id,"RIFF",4) &&
       !memcmp(header.type,"WAVE",4)){
       printf("[wav] header ok\n");
       return true;
    }else{
       printf("[wav] header nok\n");
       return false;
    }
}

int wav_findChunk(char * id,bool onBuffer,int f){
    WAV_CHUNK chunk;
    int red;
    char idtext[5];

    idtext[4]='\0';
    chunk.size=0;
    do{
        if(onBuffer){
    
            if (buffer_seek(chunk.size,SEEK_CUR)<0) return -1;

            red=buffer_read(&chunk,sizeof(chunk));
        }else{

            if (lseek(f,chunk.size,SEEK_CUR)<0) return -1;

            red=read(f,&chunk,sizeof(chunk));
        }

        memcpy(idtext,chunk.id,4);
        printf("[wav] found chunk '%s'\n",idtext);

    }while(red==sizeof(WAV_CHUNK) && memcmp(chunk.id,id,4));

    if(red==sizeof(WAV_CHUNK)){
        return chunk.size;
    }else{
        return -1;
    }
}

void wav_tagRequest(char * name,TAG * tag){
    int f;
    int size;
    WAV_FMT fmt;

    tag->badFile=true;

    f=open(name,O_RDONLY);

    if(f>=0){

        if (wav_verifyHeader(false,f)){

            size=wav_findChunk("fmt ",false,f);
            if(size>=sizeof(fmt)){

                // parse fmt chunk
                read(f,&fmt,sizeof(fmt));

                if(fmt.formatTag==WAVE_FORMAT_PCM &&
                   fmt.channels>0 && fmt.channels<3 &&
                   (fmt.bitsPerSample==8 || fmt.bitsPerSample==16)){

                    tag->sampleRate=fmt.samplesPerSec;
                    tag->stereo=fmt.channels==2;
                    tag->bitRate=fmt.samplesPerSec*fmt.channels*fmt.bitsPerSample;

                    // try to find data chunk
                    size=wav_findChunk("data",false,f);
                    if(size>=0){
                        tag->badFile=false;
                        tag->length=(long long)size*HZ/fmt.avgBytesPerSec;
                    }
                }
            }
        }

        close(f);
    }
}

void wav_trackLoop(){
    int size;
    int red;
    int time;
    
    WAV_FMT fmt;
    bool is8Bit;
    int dataSize,dataStart,dataPos;

    dataSize=dataStart=dataPos=0;
    is8Bit=false;

    printf("[wav] trackStart()\n");

    // parse headers
    if (wav_verifyHeader(true,0)){

        size=wav_findChunk("fmt ",true,0);
        if(size>=sizeof(fmt)){

            // parse fmt chunk
            buffer_read(&fmt,sizeof(fmt));

            is8Bit=fmt.bitsPerSample==8;

            // try to find data chunk
            dataSize=wav_findChunk("data",true,0);
            if(dataSize>=0){
                dataStart=buffer_seek(0,SEEK_CUR);
            }
        }
    }
    
    // read / write loop
    red=0;
    do{

        if(!is8Bit){
            red=buffer_read(dataBuf,DATA_BUFFER_SIZE);
        }else{
            red=wav_8Bit();
        }

        // make sure we didn't read past chunk end
        dataPos+=red;
        if(dataPos>dataSize){
            red-=(dataPos-dataSize);
            dataPos=dataSize;
        }

        // 8bit -> 16Bit = twice as big
        if(is8Bit){
            red=red*2;
        }

        output_write(dataBuf,red);

        // do we have to seek?
        if(codec_mustSeek(&time)){
            int pos;
            int offset;

            pos=time*fmt.avgBytesPerSec/HZ;

            // make sure we are on a sample boundary
            pos-=pos%fmt.blockAlign;

            offset=buffer_seek(dataStart+pos,SEEK_SET);

            if(offset>=0){
                dataPos=offset-dataStart;

                codec_setElapsed(time);
            }

            codec_seekDone();
        }

    }while(codec_mustContinue() && red>0);

}

void wav_activate(void)
{
    int arch;
    arch=getArch();
    printf("Wav activation\n");
    if(arch==AV3XX_ARCH || arch==AV1XX_ARCH) /* for device with MAS chip we need specific init*/
        mas_chgMode(MAS_PCM_DSP_MODE);
}        

void codec_main(CODEC_GLOBAL_INFO * info)
{
    info->description="Uncompressed WAV Codec";
    info->seekSupported=true;
    info->trackLoop=wav_trackLoop;
    info->tagRequest=wav_tagRequest;
    info->codecActivate=wav_activate;
}

