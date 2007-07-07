/*
	wildmidi_lib.h

 	Midi Wavetable Processing library
 
 	Copyright (C)2001-2004 Chris Ison
 
	This library is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	This library is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with this library; if not, write to the Free Software
	Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA  02111-1307  USA

	Email: cisos@bigpond.net.au
		 wildcode@users.sourceforge.net
 
 	$Id: wildmidi_lib.h,v 1.6 2004/01/26 02:24:33 wildcode Exp $
*/
#ifndef __WILDMIDI_LIB__
#define __WILDMIDI_LIB__

#define WILDMIDI_VERSION 1
#define TIMIDITY_CFG "timidity.cfg\0"
#define WILDMIDILIB_VERSION "1"


#define WILDMIDI_VERSION 1

#define WM_MO_LINEAR_VOLUME	0x0001  
#define WM_MO_EXPENSIVE_INTERPOLATION 0x0002
#define WM_MO_REVERB		0x0004
#define WM_MO_BIG_ENDIAN_OUTPUT	0x0020  

#define WM_GS_VERSION		0x0001

struct _WM_Info {
	unsigned long int current_sample;
	unsigned long int approx_total_samples;
	unsigned short int mixer_options;
};

typedef void midi;

struct _lowpass {
  signed long int in[2];
  signed long int out[2];
};

struct _filter {
  signed long int *delay[4][2];
  unsigned long int delay_pos[4][2];
  struct _lowpass lowpass[4][2];
  signed long int in[2][2];
  signed long int out[2][2];
};

struct _env {
  float time;
  float level;
  unsigned char set;
};

struct _sample {
  unsigned long int data_length;
  unsigned long int loop_start;
  unsigned long int loop_end;
  unsigned long int loop_size;
  unsigned char loop_fraction;
  unsigned short int rate;
  unsigned long int freq_low;
  unsigned long int freq_high;
  unsigned long int freq_root;
  unsigned char modes;
  unsigned long int env_rate[7];
  unsigned long int env_target[7];
  unsigned long int inc_div;
  signed short *data;
  signed short max_peek;
  signed short min_peek;
  signed long int peek_adjust;
  struct _sample *next;
};

struct _patch {
  unsigned short patchid;
  unsigned char loaded;
  char *filename;
  signed short int amp;
  unsigned char keep;
  unsigned char remove;
  struct _env env[6];
  unsigned char note;
  unsigned long int inuse_count;
  struct _sample *first_sample;
  struct _patch *next;
};

struct _channel {
  unsigned char bank;
  struct _patch *patch;
  unsigned char hold;
  unsigned char volume;
  unsigned char pressure;
  unsigned char expression;
  signed char balance;
  signed char pan;
  signed short int left_adjust;
  signed short int right_adjust;
  signed short int pitch;
  signed short int pitch_range;
  signed long int pitch_adjust;
  unsigned short reg_data;
};

#define HOLD_OFF 0x02

struct _note {
  unsigned short noteid;
  unsigned char velocity;
  struct _patch *patch;
  struct _sample *sample;
  unsigned long int sample_pos;
  unsigned long int sample_inc;
  signed long int env_inc;
  unsigned char env;
  unsigned long int env_level;
  unsigned char modes;
  unsigned char hold;
  unsigned char active;
  struct _note *next;
  signed short int vol_lvl;
};

struct _miditrack {
  unsigned long int length;
  unsigned long int ptr;
  unsigned long int delta;
  unsigned char running_event;
  unsigned char EOT;
};

struct _mdi_patches {
  struct _patch *patch;
  struct _mdi_patch *next;
};

struct _mdi_index {
  unsigned long int offset;
  unsigned char event;
  unsigned long int delta;
};

struct _mdi {
  int lock;
  unsigned char *data;
  unsigned long int size;
  unsigned short int divisions ;
  unsigned short midi_master_vol;
  unsigned long int samples_per_delta;
  unsigned long int samples_to_mix;
  struct _mdi_index * index;
  unsigned long int index_count;
  unsigned long int index_size;
  struct _WM_Info info;
  struct _WM_Info *tmp_info;
  unsigned char recalc_samples;
  struct _channel channel[16];
  struct _note *note[128];
  struct _note **last_note;
  struct _note note_table[2][16][128];
  
  struct _patch **patches;
  unsigned long int patch_count;
  unsigned long int sample_count;
  signed short int amp;
  
  // setup data for auto amp
  signed long int log_cur_vol;
  signed long int lin_cur_vol;
  signed long int log_max_vol;
  signed long int lin_max_vol;
  
  unsigned char ch_vol[16];
  unsigned char ch_exp[16];
  unsigned char note_vel[16][128];
  
  struct _filter filter;
};

extern const char * WildMidi_GetString (unsigned short int info);
extern int WildMidi_Init (const char * config_file, unsigned short int rate, unsigned short int options);
extern int WildMidi_MasterVolume (unsigned char master_volume);
extern midi * WildMidi_Open (const char *midifile);
extern midi * WildMidi_OpenBuffer (unsigned char *midibuffer, unsigned long int size);
extern int WildMidi_LoadSamples ( midi * handle);

int (*WildMidi_GetOutput) (midi * handle, char * buffer, unsigned long int size);
int WildMidi_GetOutput_Gauss (midi * handle, char * buffer, unsigned long int size);
int WildMidi_GetOutput_Linear (midi * handle, char * buffer, unsigned long int size);
//extern int WildMidi_GetOutput (midi * handle, char * buffer, unsigned long int size);
extern int WildMidi_SetOption (midi * handle, unsigned short int options, unsigned short int setting);
extern struct _WM_Info * WildMidi_GetInfo ( midi * handle );
extern int WildMidi_FastSeek ( midi * handle, unsigned long int *sample_pos);
extern int WildMidi_SampledSeek ( midi * handle, unsigned long int *sample_pos);
extern int WildMidi_Close (midi * handle);
extern int WildMidi_Shutdown ( void );
// extern void WildMidi_ReverbSet(midi * handle, float width, float wet, float dry, float damp, float roomsize);

int isMidiFile(char* midifile);

#define DEBUGMEM
#ifdef DEBUGMEM

void *myRealloc(void* y, size_t x);
void *myCalloc(size_t x1, size_t x2);
void *myMalloc(size_t x);
void myFree(void* x);

//#define realloc(y, x) myRealloc(y, x)
//#define calloc(x1, x2) myCalloc(x1, x2)
//#define malloc(x) myMalloc(x)
//#define kfree(x) myFree(x)
#endif //DEBUGMEM

#endif //__WILDMIDI_LIB__
