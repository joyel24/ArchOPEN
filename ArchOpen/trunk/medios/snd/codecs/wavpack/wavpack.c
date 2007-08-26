/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include "wavpack.h"

__IRAM_DATA int32_t dataBuf[DATA_BUFFER_SIZE];

int file;

int wavpack_fileRead(void * buffer,int count)
{
    return read(file,buffer,count);
}

void wavpack_tagRequest(char * name,TAG * tag)
{
    WavpackContext *wpc;
    char error [80];
    int size;

    printf("[wavpack] tagRequest()\n");

    tag->badFile=true;

    file=open(name,O_RDONLY);

    if(file>=0)
    {
        size=filesize(file);

        wpc = WavpackOpenFileInput (wavpack_fileRead, error, 0);

        if(wpc)
        {
            tag->stereo=WavpackGetReducedChannels(wpc)==2;
            tag->sampleRate=WavpackGetSampleRate(wpc);
            tag->length=HZ*WavpackGetNumSamples(wpc)/tag->sampleRate;
            tag->bitRate=(long long)size*8*WavpackGetSampleRate(wpc)/WavpackGetNumSamples(wpc);

            tag->badFile=false;
            
            WavpackClose(wpc);
        }

        close(file);
    }
}

void wavpack_trackLoop()
{
    WavpackContext *wpc;
    char error [80];

    int red;
    int time;
    int nb;
    short * dst_ptr;
    int32_t * src_ptr;
    int numChan;
    PLAYLIST_ITEM * item;

    printf("[wavpack] trackLoop()\n");

    item=buffer_getActiveItem();

    wpc = WavpackOpenFileInput (buffer_read, error, 1);

    if (!wpc)
    {
        printf("Error opening file: %s\n",error);
        return ;
    }

    numChan=WavpackGetReducedChannels (wpc);

    printf("bytes per sample: %d, bits per sample: %d\n",WavpackGetBytesPerSample(wpc),WavpackGetBitsPerSample(wpc));
    do
    {
        red= WavpackUnpackSamples (wpc, dataBuf, DATA_BUFFER_SIZE/numChan);

        dst_ptr = (short *) dataBuf;
        src_ptr = dataBuf;
        nb = red*numChan;
        while(nb--)
        {
            *dst_ptr++ = (short)(*src_ptr++);
        }

        output_write((char*)dataBuf,red*2*numChan);

        if(codec_mustSeek(&time))
        {
            int pos;

            pos=(long long)time*item->fileSize/item->tag.length;

            buffer_seek(pos,SEEK_SET);

            WavpackClose(wpc);
            wpc = WavpackOpenFileInput (buffer_read, error, 1);

            if (!wpc)
            {
                printf("Error reopening file while seeking: %s\n",error);
                return ;
            }

            time=HZ*WavpackGetSampleIndex(wpc)/item->tag.sampleRate;

            codec_setElapsed(time);
            codec_seekDone();
        }
    }
    while(codec_mustContinue() && red>0);

    WavpackClose(wpc);
}

void codec_main(CODEC_GLOBAL_INFO * info)
{
    info->description="Wav Pack Codec";
    info->seekSupported=true;
    info->trackLoop=wavpack_trackLoop;
    info->tagRequest=wavpack_tagRequest;
}
