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

#include "mad.h"

#define MAD_INPUT_BUFFER_SIZE 65536
#define MAD_OUTPUT_BUFFER_SIZE 4096

char inBuf[MAD_INPUT_BUFFER_SIZE];
short outBuf[MAD_OUTPUT_BUFFER_SIZE];
bool firstOutput;

static enum mad_flow mad_input(void *data,struct mad_stream *stream){
    int red;
    int left;

//    printf("[mad] mad_input()\n");

    left=0;

    if(stream->next_frame!=NULL){

        left=(int)inBuf+MAD_INPUT_BUFFER_SIZE-(int)stream->next_frame;

        memcpy(inBuf,stream->next_frame,left);

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

void mad_trackLoop(){
    struct mad_decoder decoder;

    printf("[mad] trackLoop()\n");

    firstOutput=true;

    mad_decoder_init(&decoder, 0,
		   mad_input, 0 /* header */, 0 /* filter */, mad_output,
		   mad_error, 0 /* message */);

    mad_decoder_run(&decoder, MAD_DECODER_MODE_SYNC);

    mad_decoder_finish(&decoder);
}

void codec_main(CODEC_INFO * info){

    printf("[mad] main()\n");

    info->globalInfo.description="MAD, MPEG audio codec";
    info->globalInfo.seekSupported=false;
    info->globalInfo.trackLoop=mad_trackLoop;
    info->globalInfo.tagRequest=NULL;

//    info.tagRequest=NULL;
}

