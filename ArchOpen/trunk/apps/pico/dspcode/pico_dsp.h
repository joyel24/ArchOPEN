#ifndef __PICO_DSP_H
#define __PICO_DSP_H

//DAC data queue
#define DACOUTQUEUE_LENGTH 1024
#define DACOUTQUEUE_MASK (DACOUTQUEUE_LENGTH-1)
extern short ym2612_dacout_queue[DACOUTQUEUE_LENGTH];
extern short ym2612_dacout_tail;
extern short ym2612_dacout_head;

extern short buf1[SAMPLECOUNT*2];

unsigned short lfo_pm_table(unsigned short i);

#define OSC_NTSC 53693100
#define OSC_PAL  53203424

extern unsigned long rasters_total;
extern unsigned long samples_per_frame;

extern int dac_extrapolation[312];

void PsndReset();

#endif