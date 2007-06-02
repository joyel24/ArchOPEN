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

int32_t dataBuf[DATA_BUFFER_SIZE];
        
void wavpack_trackLoop()
{
    /*WavpackContext *wpc;
    char error [80];

    int res;
    int red;
    int bitstream;
    int time;
    int nb,nb2;
    short * dst_ptr;
    int32_t * src_ptr;

    trackInfo.validTrack=false;
    trackInfo.sampleRate=0;
    trackInfo.length=0;
    trackInfo.stereo=false;

    printf("[wavpack] trackLoop()\n");

    wpc = WavpackOpenFileInput (buffer_read, error);

    if (!wpc)
    {
        printf("Error opening file: %s\n",error);
        return ;
    }

    trackInfo.sampleRate = WavpackGetSampleRate(wpc);
    trackInfo.stereo = WavpackGetReducedChannels (wpc)==2;

    codec_setTrackInfo(&trackInfo);
    printf("sample rate=%d, stereo=%d\n",trackInfo.sampleRate,trackInfo.stereo);
    printf("bytes per sample: %d, bits per sample: %d\n",WavpackGetBytesPerSample(wpc),WavpackGetBitsPerSample(wpc));
    do{
        printf("a\n");
        red= WavpackUnpackSamples (wpc, dataBuf, DATA_BUFFER_SIZE/2);
        printf("b\n");        
        dst_ptr = (short *) dataBuf;
        src_ptr = dataBuf;
        nb = red;
        while(nb--)
        {
            *dst_ptr++ = (short)(*src_ptr++);
            *dst_ptr++ = (short)(*src_ptr++);
        }        
        //print_data(dataBuf,red*4);
        output_write((char*)dataBuf,red*4);
    }
    while(codec_mustContinue() && red>0);*/
}

void codec_main(CODEC_GLOBAL_INFO * info)
{
    info->description="Wav Pack Codec";
    info->seekSupported=false;
    info->trackLoop=wavpack_trackLoop;
}
