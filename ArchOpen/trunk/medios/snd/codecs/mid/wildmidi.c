  /*
	wildmidi.c

 	Midi Player using the WildMidi Midi Processing Library
 
 	Copyright (C)2001-2004 Chris Ison
 
	This program is free software; you can redistribute it and/or modify
	it under the terms of the GNU General Public License as published by
	the Free Software Foundation; either version 2 of the License, or
	(at your option) any later version.

	This program is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
	GNU General Public License for more details.

	You should have received a copy of the GNU General Public License
	along with this program; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Email: cisos@bigpond.net.au
		 wildcode@users.sourceforge.net
 
 	$Id: wildmidi.c,v 1.21 2004/01/28 05:45:09 wildcode Exp $
*/

#include "medios.h"
#include "wildmidi.h"
#include "wildmidi_lib.h"

extern int errno;
extern char currentFile[256];

/*
 ==============================
 Audio Output Functions
 ------------------------------
 ==============================
*/

unsigned int rate = 32072;

int audio_fd;

inline void
shutdown_output(void){
  printf("Shutting Down Sound System\n");
  if(audio_fd!=-1)
    close(audio_fd);
}

/*
 Wav Output Functions
*/

static char wav_file[15] = "/midi/test.wav";
unsigned long int wav_size;

int write_wav_output (char * output_data, int output_size);
void close_wav_output ( void );

char wav_hdr[] = {
  0x52, 0x49, 0x46, 0x46,  //RIFF
  0x00, 0x00, 0x00, 0x00,  //size
  0x57, 0x41, 0x56, 0x45, 0x66, 0x6D, 0x74, 0x20,
  0x10, 0x00, 0x00, 0x00, 0x01, 0x00, 0x02, 0x00,
  0x44, 0xAC, //rate
  0x00, 0x00, //bytes per sec
  0x10, 0xB1, 0x02, 0x00,
  0x04, 0x00, 0x10, 0x00, 0x64, 0x61, 0x74, 0x61,
  0x00, 0x00, 0x00, 0x00  //size-headerSize=size-36
};

int
open_wav_output(void){
  if (wav_file[0] == '\0'){
    printf(" -3");
    return -1;
  }

  //if ((audio_fd = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC /*EH TODO | O_BINARY*/))) < 0) {
  if(!(audio_fd = open(wav_file, O_WRONLY | O_CREAT ))){
    //if ((audio_fd = open(wav_file, (O_RDWR | O_CREAT | O_TRUNC), (S_IRUSR | S_IWUSR | S_IRGRP | S_IWGRP | S_IROTH | S_IWOTH))) < 0) {
    return -1;
  }else{
    unsigned long int bytes_per_sec;

    wav_hdr[24] = (rate) & 0xFF;
    wav_hdr[25] = (rate >> 8) & 0xFF;
    
    bytes_per_sec = rate * 4;
    wav_hdr[28] = (bytes_per_sec) & 0xFF;
    wav_hdr[29] = (bytes_per_sec >> 8) & 0xFF;
    wav_hdr[30] = (bytes_per_sec >> 16) & 0xFF;
    wav_hdr[31] = (bytes_per_sec >> 24) & 0xFF;
  }
  errno=write(audio_fd, &wav_hdr, 44);
  if ( errno< 0) {
    printf("ERROR: Writing Header %d\n", errno);
    shutdown_output();
    return -1;
  }

  wav_size = 0;
  return 0;
}

void init(void){
  open_output = open_wav_output;
  send_output = write_wav_output;
  close_output = close_wav_output;
}

int
write_wav_output (char * output_data, int output_size) {
  errno=write(audio_fd, output_data, output_size);
  if ( errno< 0) {
    printf("ERROR: Writing Wav %d\n", errno);
    shutdown_output();
    return -1;
  }

  wav_size += output_size;
  return 0;
}

void
close_wav_output ( void ) {
  char wav_count[4];
  if (audio_fd == -1)
    return;
  
  wav_count[0] = (wav_size) & 0xFF;
  wav_count[1] = (wav_size >> 8) & 0xFF;
  wav_count[2] = (wav_size >> 16) & 0xFF;
  wav_count[3] = (wav_size >> 24) & 0xFF;
  lseek(audio_fd,40,SEEK_SET);
  write(audio_fd,&wav_count,4);
  
  wav_size += 36;
  wav_count[0] = (wav_size) & 0xFF;
  wav_count[1] = (wav_size >> 8) & 0xFF;
  wav_count[2] = (wav_size >> 16) & 0xFF;
  wav_count[3] = (wav_size >> 24) & 0xFF;
  lseek(audio_fd,4,SEEK_SET);
  write(audio_fd,&wav_count,4);
  
  shutdown_output();
}

/*
 ==============================
 ==============================
 ==============================
*/
/*
void
do_help2 (void) {
  printf("  -v    --version        Display version\n");
  printf("  -h    --help           This help.\n");
  printf("Software Wavetable Options\n");
  printf("  -o W  --wavout=W       Saves the output to W in wav format\n");
  printf("                         at 44100Hz 16 bit stereo\n");
  printf("  -l    --linear_vol     Use linear volume adjustments\n");
  printf("  -r N  --rate=N         output at N samples per second\n");
  printf("  -c P  --config_file=P  P is the path and filename to your timidity.cfg\n");
  printf("                         Defaults to /etc/timidity.cfg\n\n");
  printf(" -m V  --master_volume=V Sets the master volumes, default is 100\n");
  printf("                         range is 0-127 with 127 being the loudest\n");
}
*/

int
mid2wav(int argc, char **argv){
  struct _WM_Info * wm_info = NULL;
  unsigned long int mixer_options = 0;
  static char *config_file = NULL;
  void *midi_ptr =  NULL;
  unsigned char master_volume = 100;
  int output_result = 0;
  char * output_buffer = NULL;
  unsigned long int perc_play = 0;
  unsigned long int pro_mins = 0;
  unsigned long int pro_secs = 0;
  unsigned long int apr_mins = 0;
  unsigned long int apr_secs = 0;
  unsigned long int count_diff;
  static char *spinner="|/-\\";
  static int spinpoint = 0;
  
  config_file = malloc(sizeof(TIMIDITY_CFG)+1);
  strncpy (config_file, TIMIDITY_CFG, sizeof(TIMIDITY_CFG));
  config_file[sizeof(TIMIDITY_CFG)] = '\0';

  printf("Initializing Sound System\n");
  init();
  if(open_output() == -1){
    return 0;
  }
  
  printf(" 1");

  /*    
	printf("Initializing %s\n\n", WildMidi_GetString(WM_GS_VERSION));
	printf(" +  Volume up       e  Better resampling     n  Next Midi\n");
	printf(" -  Volume down     l  Linear volume         q  Quit\n\n");
  */  
  if(WildMidi_Init(config_file, rate, mixer_options) == -1){
    return 0;
  }
  printf(" 2");
  WildMidi_MasterVolume(master_volume);
  printf(" 3");
  output_buffer = malloc(16384);
  if(output_buffer == NULL){
    printf("Not enough ram, exiting\n");
    WildMidi_Shutdown();
    return 0;
  }
  printf(" 4");
#ifdef STANDALONE
  selectInputFile();
#else
#define currentFile "test.mid\0"
#endif
  printf("current file:%s\n", currentFile);
  midi_ptr = WildMidi_Open(currentFile);//parse midi file
  //midi_ptr = WildMidi_OpenBuffer(midibuffer, size);

  if(midi_ptr == NULL) {
    if(output_buffer != NULL)
      kfree(output_buffer);
    WildMidi_Shutdown();
    return 0;
  }
  
  wm_info = WildMidi_GetInfo(midi_ptr);
      
  if (wm_info == NULL) {
    if(output_buffer != NULL)
      kfree(output_buffer);
    WildMidi_Close(midi_ptr);
    WildMidi_Shutdown();
    return 0;
  }

  printf ("\rPlaying test.mid");
      
  apr_mins = wm_info->approx_total_samples / (rate * 60);
  apr_secs = (wm_info->approx_total_samples % (rate * 60)) / rate;
      
  mixer_options = wm_info->mixer_options;
  WildMidi_LoadSamples(midi_ptr);//do nothing!!!!
  for(;;){
    count_diff = wm_info->approx_total_samples - wm_info->current_sample;
    if(count_diff == 0)
      break;

    if(count_diff<4096){
      output_result=WildMidi_GetOutput(midi_ptr, output_buffer, (count_diff*4));
    }else{
      output_result=WildMidi_GetOutput(midi_ptr, output_buffer, 16384);
    }
    wm_info = WildMidi_GetInfo(midi_ptr);
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
    
    fprintf(stderr, "\r");
    fprintf(stderr, "\t [Approx %2lum %2lus Total] [%s] [%3i] [%2lum %2lus Processed] [%2lu%%] %c  ", 
	    apr_mins, apr_secs, modes, master_volume, 
	    pro_mins, pro_secs, perc_play, spinner[spinpoint++%4]);
    
    if(output_result == 0)
      break;
    send_output(output_buffer, output_result);
  }//for

  //WM_FreePatches();

  if(WildMidi_Close(midi_ptr) == -1){
    printf ("oops\n");
  }
  memset(output_buffer, 0, 16384);
  send_output (output_buffer, 16384);
 
  //usleep(5000);
  if(WildMidi_Shutdown() == -1)
    printf("oops\n");
  //usleep(5000);
  printf("Shutting down Sound System\n");
  close_output();
  
  if(output_buffer != NULL)
    kfree(output_buffer);
  return 0;
}

//============================
//============================
//============================

void app_main2(int argc,char ** argv){
  mid2wav(argc, argv);
}


/*
#ifdef READ_STDIN //EH TODO
	ch = 0;
//EH TODO read on stdin!!!
	if (read(my_tty, &ch, 1) != 1)
	  ch = 0; 

	if (ch) {
	  switch (ch) {
	  case 'l':
	    WildMidi_SetOption(midi_ptr, WM_MO_LINEAR_VOLUME, ((mixer_options & WM_MO_LINEAR_VOLUME) ^ WM_MO_LINEAR_VOLUME));
	    mixer_options ^= WM_MO_LINEAR_VOLUME;
	    break;
#ifdef EXPERIMENT_626
	  case 'r':
	    WildMidi_SetOption(midi_ptr, WM_MO_REVERB, ((mixer_options & WM_MO_REVERB) ^ WM_MO_REVERB));	
	    mixer_options ^= WM_MO_REVERB;	
	    break;
#endif
	  case 'e':
	    WildMidi_SetOption(midi_ptr, WM_MO_EXPENSIVE_INTERPOLATION, ((mixer_options & WM_MO_EXPENSIVE_INTERPOLATION) ^ WM_MO_EXPENSIVE_INTERPOLATION));	
	    mixer_options ^= WM_MO_EXPENSIVE_INTERPOLATION;	
	    break;
	  case 'n':
	    goto NEXTMIDI;
	  case 'q':	
	    printf("\n");
	    WildMidi_Close(midi_ptr);
	    WildMidi_Shutdown();
	    printf("Shutting down Sound System\n");
	    close_output();

	    printf("\r\n");
	    exit (0);
	  case '-':	
	    if (master_volume > 0) {
	      master_volume--;
	      WildMidi_MasterVolume(master_volume);
	    }
	    break;
	  case '+':
	    if (master_volume < 127) {
	      master_volume++;
	      WildMidi_MasterVolume(master_volume);
	    }
	    break;
	  }
	}
#endif //READ_STDIN	
*/
