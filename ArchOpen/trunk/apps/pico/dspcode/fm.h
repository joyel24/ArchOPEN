#ifndef _H_FM_FM_
#define _H_FM_FM_

int YM2612Init(UINT32 clock, UINT32 rate);
void YM2612ResetChip(UINT32 clock, UINT32 rate);
void YM2612UpdateOne(short *buffer);
void fm_write(unsigned short a, unsigned short v);
int fm_read(int address);
void RefreshFm (void);
void YM2612PicoTick(int raster);
void dac_update(int raster);
void YM2612PicoStateLoad();

#endif /* _H_FM_FM_ */
