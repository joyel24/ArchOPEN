// SIDEmu.h: Schnittstelle für die Klasse CSIDEmu.
//
//////////////////////////////////////////////////////////////////////

// Quick cut'n paste from TinySID sources
//
// The c64-emulation and the synth-renderer was written by Tammo Hinrichs (kb)
//
// Combining of the emulation and the renderer as well as improvements and bugfixes 
// by Rainer Sinsch (sinsch@stud.uni-frankfurt.de)
//
// spreading of the source code is FORBIDDEN!!!


#if !defined(AFX_SIDEMU_H__32219DE5_C202_440C_991E_44A7A8136E44__INCLUDED_)
#define AFX_SIDEMU_H__32219DE5_C202_440C_991E_44A7A8136E44__INCLUDED_

#include "defines.h"


void sidPoke(int reg, unsigned char val);

extern byte memory[65536];

extern void sidReset(void);
extern void cpuReset();

extern void cpuJSR(word npc, byte na);
extern void c64Init(int nSampleRate);
extern word LoadSID(
	const char *filename, word *load_addr, word *init_addr,
	word *play_addr);
extern void synth_init(dword mixfrq);
extern void synth_render(signed short *buffer, dword len);
extern word LoadSIDFromMemory(void *pSidData, word *load_addr,
							  word *init_addr, word *play_addr, byte *subsongs, byte *startsong, unsigned char *speed, word size);

#endif //!defined(AFX_SIDEMU_H__32219DE5_C202_440C_991E_44A7A8136E44__INCLUDED_)
