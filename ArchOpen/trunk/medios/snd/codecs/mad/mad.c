/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

//#include <api.h>
#include "medios.h"

#include "mad.h"

#include "id3.h"
#include "mp3_data.h"

#define MAD_INPUT_BUFFER_SIZE 4096
#define MAD_OUTPUT_BUFFER_SIZE 4096

#define BUILD_HAS_MED

char inBuf[MAD_INPUT_BUFFER_SIZE];
short outBuf[MAD_OUTPUT_BUFFER_SIZE];
bool firstOutput;
bool mustReRun;

int dataStart,dataSize;
int length;

extern unsigned char mp3buf[0x1000];

static enum mad_flow mad_input(void *data,struct mad_stream *stream){
    int red;
    int left;
    int time;
    int wasSeeking;

//    printf("[mad] mad_input()\n");

    left=0;
    wasSeeking=false;

    //do we have to seek?
    if(codec_mustSeek(&time)){
        int pos;
        int offset;
        int header;

        wasSeeking=true;

        //use track length to find where we should seek
        pos=dataStart+(long long)(dataSize-dataStart)*time/length;

        buffer_seek(pos,SEEK_SET);

        //find next frame boundary
        red=buffer_read(mp3buf,sizeof(mp3buf));

        header=stream->this_frame[0]<<24 | stream->this_frame[1]<<16 | stream->this_frame[2]<<8 | stream->this_frame[3];

        if(mem_find_next_frame(0,&offset,red,header)){

            buffer_seek(-red+offset,SEEK_CUR);
        }

        codec_setElapsed(time);
        codec_seekDone();

        mustReRun=true;

        return MAD_FLOW_STOP;
    }

    if(!wasSeeking){

        // copy what was left in the last buffer
        if(stream->next_frame!=NULL){

            left=(int)inBuf+MAD_INPUT_BUFFER_SIZE-(int)stream->next_frame;

            memcpy(inBuf,stream->next_frame,left);

        }
    }


    red=buffer_read(inBuf+left,MAD_INPUT_BUFFER_SIZE-left);

    if(red>0){

        mad_stream_buffer(stream, inBuf, red+left);

        return MAD_FLOW_CONTINUE;
    }else{

        return MAD_FLOW_STOP;
    }
}

static inline signed int mad_scale(mad_fixed_t sample){
    /* round */
    sample += (1L << (MAD_F_FRACBITS - 16));

    /* clip */
    if (sample >= MAD_F_ONE)
    sample = MAD_F_ONE - 1;
    else if (sample < -MAD_F_ONE)
    sample = -MAD_F_ONE;

    /* quantize */
    return sample >> (MAD_F_FRACBITS + 1 - 16);
}

static enum mad_flow mad_output(void *data,struct mad_header const *header,struct mad_pcm *pcm){
    unsigned int nchannels, nsamples;
    mad_fixed_t const *left_ch, *right_ch;

    short* outPtr;
    int outSize;

//    printf("[mad] mad_output()\n");

    if(firstOutput){
        PLAYLIST_ITEM * item;

        item=buffer_getActiveItem();

        item->tag.sampleRate=pcm->samplerate;
        item->tag.stereo=pcm->channels==2;

        output_outputParamsChanged();

        firstOutput=false;
    }

    /* pcm->samplerate contains the sampling frequency */

    nchannels = pcm->channels;
    nsamples  = pcm->length;
    left_ch   = pcm->samples[0];
    right_ch  = pcm->samples[1];

    outSize=nsamples*nchannels*2;

    outPtr=outBuf;

    while (nsamples--) {
        signed int sample;

        /* output sample(s) in 16-bit signed little-endian PCM */

        sample = mad_scale(*left_ch++);

        *outPtr++=sample;

        if (nchannels == 2) {
            sample = mad_scale(*right_ch++);

            *outPtr++=sample;
        }
    }

    output_write(outBuf,outSize);

    if(codec_mustContinue()){

        return MAD_FLOW_CONTINUE;
    }else{

        return MAD_FLOW_STOP;
    }
}

static enum mad_flow mad_error(void *data,struct mad_stream *stream,struct mad_frame *frame){

    printf("[mad] decoding error 0x%04x (%s) at byte offset %u\n",
	   stream->error, mad_stream_errorstr(stream),
	   stream->this_frame-(int)inBuf);

    return MAD_FLOW_CONTINUE;
}

void mad_skipId3v2(){
    unsigned char buf[4];
    int size;

    buffer_read(buf,3);

    if(buf[0]=='I' && buf[1]=='D' && buf[2]=='3'){

        //skip flags
        buffer_seek(3,SEEK_CUR);

        //read size
        buffer_read(buf,4);
        size=buf[0]<<21 | buf[1]<<14 | buf[2]<<7 | buf[3];

        printf("[mad] found id3v2 tag, size=%d\n",size+10);

        //skip tag
        buffer_seek(size,SEEK_CUR);
    }else{

        //rewind
        buffer_seek(0,SEEK_SET);
    }
}


void mad_tagRequest(char * name,TAG * tag){
    struct mp3entry entry;
    char * genre;
    char s[10];

    printf("[mad] tagRequest()\n");

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

void mad_trackLoop(){
    struct mad_decoder decoder;
    PLAYLIST_ITEM * item;

    printf("[mad] trackLoop()\n");

    firstOutput=true;
    mustReRun=false;

    //skip id3v2 tag if present
    mad_skipId3v2();

    dataStart=buffer_seek(0,SEEK_CUR);

    item=buffer_getActiveItem();
    dataSize=item->fileSize;
    length=item->tag.length;

    // mad stuff
    mad_decoder_init(&decoder, 0,
		   mad_input, 0 /* header */, 0 /* filter */, mad_output,
		   mad_error, 0 /* message */);

    do{
        mustReRun=false;
        mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);
    }while(mustReRun);

    mad_decoder_finish(&decoder);
}

#ifdef BUILD_HAS_MED
void codec_main(CODEC_GLOBAL_INFO * info){
#else
void mad_main(CODEC_INFO * info){
#endif
    int device=getArch();
    if(device==AV3XX_ARCH || device==AV1XX_ARCH || device==JBMM_ARCH)
    {
        printf("Device %s not suported\n",getArchName());
        info->loadOk=false;
        return;
    }
    
    printf("[mad] main()\n");

    info->description="MAD, MPEG audio codec";
    info->seekSupported=true;
    info->trackLoop=mad_trackLoop;
    info->tagRequest=mad_tagRequest;
}

