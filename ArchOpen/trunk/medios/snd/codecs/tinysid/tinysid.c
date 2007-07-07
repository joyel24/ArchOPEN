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
#include "sidemu.h"

#define SID_SAMPLERATE 44100

#define DATA_BUFFER_SIZE 4096

typedef __attribute__ ((packed)) struct {
    char __attribute__ ((packed)) magic[4];
    word __attribute__ ((packed)) version;
    word __attribute__ ((packed)) dataOffset;
    word __attribute__ ((packed)) loadAddress;
    word __attribute__ ((packed)) initAddress;
    word __attribute__ ((packed)) playAddress;
    word __attribute__ ((packed)) songs;
    word __attribute__ ((packed)) startSong;
    dword __attribute__ ((packed)) speed;
    char __attribute__ ((packed)) title[32];
    char __attribute__ ((packed)) author[32];
    char __attribute__ ((packed)) released[32];
} __attribute__ ((packed)) SID_HEADER;


short dataBuf[DATA_BUFFER_SIZE];

word loadAddr, initAddr, playAddr;
byte subSongsMax, subSong, songSpeed;

void tinysid_tagRequest(char * name,TAG * tag){
    SID_HEADER header;
    int f;
    int red;
    int numSongs;

    printf("[tinysid] tagRequest()\n");

    tag->badFile=true;

    f=open(name,O_RDONLY);

    if(f>=0){

        red=read(f,&header,sizeof(SID_HEADER));

        if(red==sizeof(SID_HEADER)){

            if(!memcmp(header.magic,"RSID",4) || !memcmp(header.magic,"PSID",4)){

                numSongs=((header.songs<<8)&0xff00) | ((header.songs>>8)&0xff);

                tag->sampleRate=44100;
                tag->stereo=false;
                tag->length=numSongs*HZ;
                tag->title=strdup(header.title);
                tag->artist=strdup(header.author);
                tag->bitRate=0;

                tag->badFile=false;
            }
        }

        close(f);
    }
}

void tinysid_trackLoop(){
    PLAYLIST_ITEM * item;
    char * data;
    int time;
    int curSong;

    printf("[tinysid] trackLoop()\n");

    int nSamplesRendered = 0;
    int nSamplesPerCall = 882;  /* This is PAL SID single speed (44100/50Hz) */
    int nSamplesToRender = 0;

    item=buffer_getActiveItem();
    
    data=malloc(item->fileSize);

    buffer_read(data,item->fileSize);

	c64Init(SID_SAMPLERATE);

 	LoadSIDFromMemory(data, &loadAddr, &initAddr, &playAddr, &subSongsMax, &subSong, &songSpeed, item->fileSize);

    cpuJSR(initAddr, subSong);
    
    curSong=subSong;

    do{

        nSamplesRendered = 0;
        while (nSamplesRendered < DATA_BUFFER_SIZE)
        {
            if (nSamplesToRender == 0)
            {
                cpuJSR(playAddr, 0);

                /* Find out if cia timing is used and how many samples
                   have to be calculated for each cpujsr */
                int nRefreshCIA = (int)(20000*(memory[0xdc04]|(memory[0xdc05]<<8))/0x4c00);	
                if ((nRefreshCIA==0) || (songSpeed == 0))
                    nRefreshCIA = 20000;
                nSamplesPerCall = SID_SAMPLERATE*nRefreshCIA/1000000;
          
                nSamplesToRender = nSamplesPerCall;
            }
            if (nSamplesRendered + nSamplesToRender > DATA_BUFFER_SIZE)
            {
                synth_render(dataBuf+nSamplesRendered, DATA_BUFFER_SIZE-nSamplesRendered);
                nSamplesToRender -= DATA_BUFFER_SIZE-nSamplesRendered;
                nSamplesRendered = DATA_BUFFER_SIZE;
            }
            else
            {
                synth_render(dataBuf+nSamplesRendered, nSamplesToRender);
                nSamplesRendered += nSamplesToRender;
                nSamplesToRender = 0;
            }
        }
        
        output_write(dataBuf,DATA_BUFFER_SIZE*2);

        if(codec_mustSeek(&time)){

            curSong=time/HZ;

            if(curSong==0){
                curSong=subSong;
            }else{
                --curSong;
            }

        	c64Init(SID_SAMPLERATE);
         	LoadSIDFromMemory(data, &loadAddr, &initAddr, &playAddr, &subSongsMax, &subSong, &songSpeed, item->fileSize);
            cpuJSR(initAddr, curSong);

            codec_seekDone();
        }

        codec_setElapsed((curSong+1)*HZ);

    }while(codec_mustContinue());


 	kfree(data);
}


void codec_main(CODEC_GLOBAL_INFO * info){

    printf("[tinysid] main()\n");

    info->description="TinySID, C64 SID Codec";
    info->seekSupported=true;
    info->trackLoop=tinysid_trackLoop;
    info->tagRequest=tinysid_tagRequest;
    info->noTimeAdvance=true;
}
