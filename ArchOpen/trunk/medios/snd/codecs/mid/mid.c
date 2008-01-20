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
/*
#include <snd/output.h>
#include "../../dspshared.h"
*/
#include "wildmidi.h"
#include "wildmidi_lib.h"
#include "mid.h"

extern int rate;

#define OUTPUT_BUFFER_SIZE 16*4096//256//4096//8192 //32*4096 //old:16384
#define OUTPUT_BUFFER_SIZE4 OUTPUT_BUFFER_SIZE/4
char output_buffer[OUTPUT_BUFFER_SIZE];

int open_mid_output(void){
  return 0;
}

int write_mid_output(char * output_data, int output_size){
  output_write(output_data, output_size);
  return 0;
}

void close_mid_output ( void ) {
}

void init_mid(void){
  open_output = open_mid_output;
  send_output = write_mid_output;
  close_output = close_mid_output;
}


void mid_tagRequest(char * name,TAG * tag){
  tag->badFile=true;

  tag->sampleRate=32000;
  tag->stereo=2;
  tag->bitRate=1026*1000;

  if(isMidiFile(name)){
    tag->badFile=false;
  }
}

void mid_trackLoop(){
  allocMem=0;
  printf("\n#memLeak1=%d\n", allocMem);
  int time;
  int red;

  //struct _WM_Info * wm_info = NULL;
  //unsigned long int mixer_options = 0;
  struct _mdi* midi_ptr =  NULL;
  //unsigned char master_volume = 100;
  int output_result = 0;
  //unsigned long int perc_play = 0;
  //unsigned long int pro_mins = 0;
  //unsigned long int pro_secs = 0;
  //unsigned long int apr_mins = 0;
  //unsigned long int apr_secs = 0;
  unsigned long int count_diff;
  //static char *spinner="|/-\\";
  //static int spinpoint = 0;

  printf("[mid] trackStart()\n");

  char  * dataBuf=malloc(buffer_getActiveItem()->fileSize);
  printf("buffer_getActiveItem()->fileSize: %d", buffer_getActiveItem()->fileSize);
  if(dataBuf == NULL){
    printf("Not enough ram, exiting\n");
    //WildMidi_Shutdown();
    printf("\n#memLeak=%d\n", allocMem);
    return;
  }

  red=buffer_read(dataBuf, buffer_getActiveItem()->fileSize);
  midi_ptr = WildMidi_OpenBuffer(dataBuf, red);
  kfree(dataBuf);
  printf("\n#memLeak2=%d\n", allocMem);
  if(midi_ptr == NULL) {
    //WildMidi_Shutdown();
    printf("\n#memLeak=%d\n", allocMem);
    return;
  }
  /*
  wm_info = WildMidi_GetInfo(midi_ptr);
      
  if (wm_info == NULL) {
    WildMidi_Close(midi_ptr);
    //WildMidi_Shutdown();
    printf("\n#memLeak=%d\n", allocMem);
    return;
  }
  buffer_getActiveItem()->tag.length=(wm_info->approx_total_samples*100)/rate;
  */
  buffer_getActiveItem()->tag.length=(midi_ptr->info.approx_total_samples*100)/rate;
  //printf ("\rTime %d", buffer_getActiveItem()->tag.length);
  printf ("\rPlaying midi file");
      
  //apr_mins = wm_info->approx_total_samples / (rate * 60);
  //apr_secs = (wm_info->approx_total_samples % (rate * 60)) / rate;
      
  //mixer_options = wm_info->mixer_options;
  //WildMidi_LoadSamples(midi_ptr);//do nothing!!!!

      /*
    perc_play =  (wm_info->current_sample * 100) / wm_info->approx_total_samples;
    pro_mins = wm_info->current_sample / (rate * 60);
    pro_secs = (wm_info->current_sample % (rate * 60)) / rate;

    unsigned char modes[4] = {0};
    int mode_count = 0;
    if(mixer_options & WM_MO_LINEAR_VOLUME){
      modes[mode_count++] = 'l';
    }
    if(mixer_options & WM_MO_REVERB){
      modes[mode_count++] = 'r';
    }
    if(mixer_options & WM_MO_EXPENSIVE_INTERPOLATION){
      modes[mode_count++] = 'e';
    }
    if(mode_count !=3){
      do{
	modes[mode_count++] = ' ';
      }while(mode_count != 3);
    }
    modes[3] = '\0';
  
    printf("\r");
    printf("\t [Approx %2lum", apr_mins);
    printf(" %2lus Total]", apr_secs);
    printf(" [%s]", modes);
    printf(" [%3i]", master_volume);
    printf(" [%2lum", pro_mins);
    printf(" %2lus Processed]", pro_secs);
    printf(" [%2lu%%]", perc_play);
    printf(" %c  ",spinner[spinpoint++%4]);
    */

  // read / write loop
  red=0;
  do{    
    // do we have to seek?
    if(codec_mustSeek(&time)){
      unsigned long int pos=time*rate/100;

      //if(WildMidi_SampledSeek(midi_ptr, &pos)>=0){
      if(WildMidi_FastSeek(midi_ptr, &pos)>=0){
	codec_setElapsed(time);
      }

      codec_seekDone();
    }

    //wm_info = WildMidi_GetInfo(midi_ptr);
    //count_diff = wm_info->approx_total_samples - wm_info->current_sample;
    count_diff=midi_ptr->info.approx_total_samples-midi_ptr->info.current_sample;
    if(count_diff == 0){
      printf(" _2");
      break;
    }

    if(count_diff<OUTPUT_BUFFER_SIZE4){
      output_result=WildMidi_GetOutput(midi_ptr, output_buffer, (count_diff*4));
    }else{
      output_result=WildMidi_GetOutput(midi_ptr, output_buffer, OUTPUT_BUFFER_SIZE);
    }
    
    if(output_result == 0){
      printf(" _3");
      break;
    }
    //printf(" _%d", output_result);
    send_output(output_buffer, output_result);
  }while(codec_mustContinue());

  //WM_FreePatches();

  if(WildMidi_Close(midi_ptr) == -1){
    printf ("oops\n");
  }
  /*
  printf("Shutting down Sound System\n");
  if(WildMidi_Shutdown() == -1)
    printf("oops\n");
  */
  close_output();  
  printf("\n#memLeak4=%d\n", allocMem);
}


void codec_main(CODEC_GLOBAL_INFO * info)
{
  init_mid();

  info->description="MID Codec";
  info->seekSupported=true;
  info->trackLoop=mid_trackLoop;
  info->tagRequest=mid_tagRequest;

  unsigned long int mixer_options = 0;
  WildMidi_Init(TIMIDITY_CFG, rate, mixer_options);
    
  unsigned char master_volume = 127;
  WildMidi_MasterVolume(master_volume);
}

