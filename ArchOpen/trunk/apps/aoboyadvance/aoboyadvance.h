#ifndef __AOBOYADVANCE_H_
#define __AOBOYADVANCE_H_

extern "C" {

extern u32 registers[16];

extern u32 GBAContext[18];
extern u32 EmuContext[18];

void switchContext(u32 * currentContext,u32 * newContext);
void triggerLineTimer();
int getCPSR();
int getSPSR();
void printRegs();

extern void convert_line(u32 * outbuf,u32 * inbuf,u32 * pal); // implemented in convert.S

void doGBAInterrupt();
void doVBlank();

}

#endif
