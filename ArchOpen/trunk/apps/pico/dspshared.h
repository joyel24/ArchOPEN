#ifndef __DSPSHARED_H
#define __DSPSHARED_H

#define SAMPLECOUNT		    1
#define SAMPLE_RATE 16000

#ifdef SOUND_USE_DSP
// warning : this file is included by both arm and dsp code (thx zig, it's a lot cleaner like this :)

//#define DSP_VID_PROFILE



// offset of the area of our control variables in the dsp
// only 2 bytes space required, the rest will be dynamically set
#define DSP_CONTROL_OFFSET 0x90

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

typedef char INT8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef long INT32;
typedef unsigned long UINT32;

#ifdef DSPCODE

typedef char int8;
typedef unsigned char uint8;
typedef short int16;
typedef unsigned short uint16;
typedef long int32;
typedef unsigned long uint32;

typedef char INT8;
typedef unsigned char UINT8;
typedef short INT16;
typedef unsigned short UINT16;
typedef long INT32;
typedef unsigned long UINT32;

# define DSP_RAM(a) ((volatile unsigned short *)(a))

#else
# define DSP_RAM(a) ((volatile unsigned short *)(0x40000 + (a)*2))
#endif

#define SDRAM_OFFSET 0x900000L

#define DSP_COM DSP_RAM(DSP_CONTROL_OFFSET)

#define SNDQUEUE_LENGTH 512
#define SNDQUEUE_MASK (SNDQUEUE_LENGTH-1)

typedef struct {
  uint16 test;
  uint16 type;
  uint16 address;
  uint16 value;
} sndItem;

typedef volatile struct {
  // init
  uint16 armInitFinished;
  uint16 port280Interrupt;
  uint16 armBusy;
  uint16 chipNum;

  uint32 lfo_pm_table_addr;

  //Pico hardware
  //

  // YM2612 registers writes & timer updates queue
  sndItem YM2612Queue[SNDQUEUE_LENGTH];
  uint16 YM2612Head;
  uint16 YM2612Tail;

  uint16 PSGQueue[SNDQUEUE_LENGTH];
  uint16 PSGHead;
  uint16 PSGTail;

  uint16 YM2612Enabled;
  uint16 PSGEnabled;
  uint16 DACEnabled;

  uint16 sndWantReset;

  uint16 Flush;

  uint16 YM2612PicoTick;

  uint16 hardwarePal;
  uint16 YM2612Status;

  uint16 ym2612_regs[0x200];   //We need 0x200 chars
  uint16 addr_A1;
  uint32 sn76496_regs[8];

  uint16 picoStateLoad;

  // debug messages
  uint16 hasDbgMsg;
  int16 dbgMsg[255];
} tDspCom;


extern tDspCom * dspCom;

#endif

#endif /*__DSPSHARED_H*/

