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

#include <snd/wav.h>

char dataBuf[DATA_BUFFER_SIZE];
char processBuf[DATA_BUFFER_SIZE];

WAV_FMT fmt;
CODEC_TRACK_INFO info;
bool is8Bit;
int dataSize,dataStart,dataPos;

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

bool wav_verifyHeader(){
    WAV_HEADER header;

    buffer_seek(0,SEEK_SET);
    buffer_read(&header,sizeof(header));

    if(!memcmp(header.id,"RIFF",4) &&
       !memcmp(header.type,"WAVE",4)){
       printf("[wav] header ok\n");
       return true;
    }else{
       printf("[wav] header nok\n");
       return false;
    }
}

int wav_findChunk(char * id){
    WAV_CHUNK chunk;
    int red;
    char idtext[5];

    idtext[4]='\0';
    chunk.size=0;
    do{
        if (buffer_seek(chunk.size,SEEK_CUR)<0) return -1;

        red=buffer_read(&chunk,sizeof(chunk));

        memcpy(idtext,chunk.id,4);
        printf("[wav] found chunk '%s'\n",idtext);

    }while(red==sizeof(WAV_CHUNK) && memcmp(chunk.id,id,4));

    if(red==sizeof(WAV_CHUNK)){
        return chunk.size;
    }else{
        return -1;
    }
}

void wav_trackLoop(){
    int size;
    int red;
    int time;

    dataPos=0;
    is8Bit=false;
    info.validTrack=false;
    info.sampleRate=0;
    info.stereo=false;

    printf("[wav] trackStart()\n");

    // parse headers

    if (wav_verifyHeader()){

        size=wav_findChunk("fmt ");
        if(size>=sizeof(fmt)){

            // parse fmt chunk
            buffer_read(&fmt,sizeof(fmt));
            if(fmt.formatTag==WAVE_FORMAT_PCM &&
               fmt.channels>0 && fmt.channels<3 &&
               (fmt.bitsPerSample==8 || fmt.bitsPerSample==16)){

                info.sampleRate=fmt.samplesPerSec;
                info.stereo=fmt.channels==2;
                is8Bit=fmt.bitsPerSample==8;

                // try to find data chunk
                dataSize=wav_findChunk("data");
                if(dataSize>=0){
                    dataStart=buffer_seek(0,SEEK_CUR);

                    info.validTrack=true;
                    info.length=dataSize*HZ/fmt.avgBytesPerSec;
                }
            }
        }
    }

    // send track info
    codec_setTrackInfo(&info);

    if(!info.validTrack) return;

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

void codec_main(){
    
    CODEC_INFO * info;
    
    info=codec_new();
    info->name="wav.c";
    info->extensions="wav";

    info->globalInfo.description="Uncompressed WAV Codec";
    info->globalInfo.seekSupported=true;
    info->globalInfo.trackLoop=wav_trackLoop;
}

