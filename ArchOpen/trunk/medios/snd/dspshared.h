#ifndef __DSPSHARED_H
#define __DSPSHARED_H

#define OUTPUT_CHUNK_SIZE 4096

// warning : this file is included by both arm and dsp code (thx zig, it's a lot cleaner like this :)

// offset of the area of our control variables in the dsp
// only 2 bytes space required, the rest will be dynamically set
#define DSP_CONTROL_OFFSET 0x90

#ifdef DSPCODE
# define DSP_RAM(a) ((volatile unsigned short *)(a))
#else
# define DSP_RAM(a) ((volatile unsigned short *)(0x40000 + (a)*2))
#endif

#define SDRAM_OFFSET 0x900000L

#define DSP_COM DSP_RAM(DSP_CONTROL_OFFSET)

typedef volatile struct {
  // init
/*00*/  unsigned short armInitFinished;
/*02*/  unsigned short chipNum; 
  
/*04*/  unsigned long dataSampleRate;
/*08*/  unsigned long outputSampleRate;
/*0C*/  unsigned short stereo;

/*0E*/  unsigned short decodeRequest;
/*10*/  unsigned short decodeDone;
/*12*/  unsigned short bufSize;  
/*14*/  unsigned long  bufAddr;  

/*18*/  unsigned short dummy1;

  // debug messages
/*1a*/  unsigned short hasDbgMsg;
/*1c*/  short dbgMsg[255];
} tDspCom;


extern tDspCom * dspCom;

#endif /*__DSPSHARED_H*/

