/*
*   snd/codecs/mas_mp3/masMp3.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include "masMp3.h"
#include "id3.h"
#include "mp3_data.h"

char dataBuf1[DATA_BUFFER_SIZE];
char dataBuf2[DATA_BUFFER_SIZE];
struct mas_sound_buffer buff_1;
struct mas_sound_buffer buff_2;
        
void mas_skipId3v2(void)
{
    unsigned char buf[4];
    int size;

    buffer_read(buf,3);

    if(buf[0]=='I' && buf[1]=='D' && buf[2]=='3'){

        //skip flags
        buffer_seek(3,SEEK_CUR);

        //read size
        buffer_read(buf,4);
        size=buf[0]<<21 | buf[1]<<14 | buf[2]<<7 | buf[3];

        printf("[masMp3] found id3v2 tag, size=%d ==> skipping\n",size+10);

        //skip tag
        buffer_seek(size,SEEK_CUR);
    }else{

        //rewind
        buffer_seek(0,SEEK_SET);
    }
}
        
void mas_tagRequest(char * name,TAG * tag)
{
    struct mp3entry entry;
    char * genre;
    char s[10];

    printf("[masMp3] tagRequest()\n");

    if(!mp3info(&entry,name,0)){

        if(entry.title!=NULL){
            tag->title=strdup(entry.title);
        }

        if(entry.artist!=NULL){
            tag->artist=strdup(entry.artist);
        }

        if(entry.album!=NULL){
            tag->album=strdup(entry.album);
        }

        if(entry.track_string!=NULL){
            tag->track=strdup(entry.track_string);
        }else{
            sprintf(s,"%d",entry.tracknum);
            tag->track=strdup(s);
        }

        genre=id3_get_genre(&entry);
        if(genre!=NULL){
            tag->genre=strdup(genre);
        }

        sprintf(s,"%d",entry.year);
        tag->date=strdup(s);

        tag->length=entry.length/(1000/HZ);
        tag->bitRate=entry.bitrate*1000;
        tag->sampleRate=entry.frequency;
    }
}

void mp3_trackLoop(void)
{
    int red=0;
    struct mas_sound_buffer * curBuff;
    int time;
    
    printf("[mp3_trackLoop] start\n");
    
#if 1    
    buff_1.read=buff_2.read=0;
    buff_2.state=buff_2.state=MAS_BUFFER_END;
    
    mas_setMp3Buffer(&buff_1,&buff_2);
    
    curBuff=&buff_1;
    
    /*NOTE: after last read the fction returns while there is still some data to be send to mas*/    
       
    //mas_skipId3v2();
    
//    buffer_seek(0x1a1,SEEK_SET);
        
    red=buffer_read(curBuff->data,DATA_BUFFER_SIZE);
    curBuff->size=red;
    curBuff->state=MAS_BUFFER_FILLED;
    curBuff=curBuff->nxt;
    
    printf("before launch (%d)\n",red);
    
    mas_mp3LaunchDecode();
    
    while(codec_mustContinue() && red>0)    
    {        
        if(curBuff->state==MAS_BUFFER_END)
        {
            red=buffer_read(curBuff->data,DATA_BUFFER_SIZE);
            curBuff->size=red;
            curBuff->read=0;
            curBuff->state=MAS_BUFFER_FILLED;
            curBuff=curBuff->nxt;
            //printf("masMp3 red=%d\n",red);
            /* NOTE: we might need to relaunch decode*/
            if(!mas_mp3DecodeState())
            {
                mas_mp3LaunchDecode();
            }
        }
           
        if(codec_mustSeek(&time))
        {
            printf("Need to seek %d\n",time);
            codec_setElapsed(time);
            codec_seekDone();
        }
    }
    mas_mp3StopDecode();
    mas_clearMp3Buffer();
#else
    printf("Starting playback");
    do
    {
        red=buffer_read(dataBuf1,DATA_BUFFER_SIZE);
        output_write(dataBuf1,red);
        if(codec_mustSeek(&time))
        {
            printf("Need to seek %d\n",time);
            codec_setElapsed(time);
            codec_seekDone();
        }
    }while(codec_mustContinue() && red>0);
#endif
    printf("[mas MP3] exit loop\n");
                   
}

void mas_activate(void)
{
    mas_chgMode(MAS_MP3_MODE);
}

void codec_main(CODEC_GLOBAL_INFO * info)
{
    int arch;
    
    arch=getArch();    
    if(arch!=AV3XX_ARCH && arch!=AV1XX_ARCH)
    {
        info->loadOk=false;
        printf("%s not supported by Mp3 Codec for MAS chip\n",getArchName());
    }
    else
    {    
        printf("Starting Mp3 Codec for MAS chip (device=%s)\n",getArchName());
        info->description="Mp3 Codec for MAS chip";
        info->seekSupported=false;
        info->trackLoop=mp3_trackLoop;
        info->tagRequest=mas_tagRequest;
        info->codecActivate=mas_activate;
        
        buff_1.data=dataBuf1;
        buff_1.size=DATA_BUFFER_SIZE;
        buff_1.read=0;
        buff_1.state=MAS_BUFFER_END;
        
        buff_2.data=dataBuf2;
        buff_2.size=DATA_BUFFER_SIZE;
        buff_2.read=0;
        buff_2.state=MAS_BUFFER_END;
        
        buff_1.nxt=&buff_2;
        buff_2.nxt=&buff_1;
    }
}
