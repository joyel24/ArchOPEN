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

static long dataBuf [DATA_BUFFER_SIZE];
        
void wavpack_trackLoop()
{
/*
    CODEC_TRACK_INFO trackInfo;
    WavpackContext *wpc;
    char error [80];

    int res;
    int red;
    int bitstream;
    int time;
    int nb,nb2;

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

    trackInfo.sampleRate = WavpackGetSampleRate;
    trackInfo.stereo = WavpackGetReducedChannels (wpc)==2;
    printf("sample rate=%d,stereo=%d\n",trackInfo.sampleRate,trackInfo.stereo);
    nb2=WavpackGetBytesPerSample (wpc)*4;
    nb=DATA_BUFFER_SIZE/nb2;
    printf("nb samples per buffer=%d\n",nb);
    do{
        red= WavpackUnpackSamples (wpc, dataBuf, nb);
        output_write(dataBuf,red*nb2);
        printf("red:%d\n",red);
    }while(codec_mustContinue() && red>0);
*/   
}

void codec_main(CODEC_INFO * info)
{
    info->globalInfo.description="Wav Pack Codec";
    info->globalInfo.seekSupported=false;
    info->globalInfo.trackLoop=wavpack_trackLoop;
}
