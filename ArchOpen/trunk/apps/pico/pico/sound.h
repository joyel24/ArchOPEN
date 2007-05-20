
int sound_timers_and_dac(int raster);
int sound_render(int offset, int length);

//int YM2612PicoTick(int n);

// z80 functionality wrappers
void z80_init();
void z80_resetCycles();
void z80_int();
int  z80_run(int cycles);
void z80_exit();

void SN76496Write(int data);
void SN76496Update(short *buffer,int length,int stereo);
int  SN76496_init(int clock,int sample_rate);

void YM2612Init_(int baseclock, int rate);
void YM2612ResetChip_(void);
void YM2612UpdateOne_(short *buffer, int length, int stereo);

int  YM2612Write_(unsigned int a, unsigned int v);
unsigned char YM2612Read_(void);

int  YM2612PicoTick_(int n);
void YM2612PicoStateLoad_(void);

void *YM2612GetRegs(void);

#define YM2612Init          YM2612Init_
#define YM2612ResetChip     YM2612ResetChip_
#define YM2612UpdateOne     YM2612UpdateOne_
#define YM2612Write         YM2612Write_
#define YM2612Read          YM2612Read_
#define YM2612PicoTick      YM2612PicoTick_
#define YM2612PicoStateLoad YM2612PicoStateLoad_

extern const signed short lfo_pm_table [16144];

