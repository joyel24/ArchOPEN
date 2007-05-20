#include "../dspshared.h"



/* For 16kHz sound */
#define SND_16
/* For 11kHz sound */
//#define SND_11
/* For 22kHz sound */
//#define SND_22

#ifdef SND_16
#define OUT_BUFFER_SIZE (SAMPLECOUNT)
#endif
#ifdef SND_22
#define OUT_BUFFER_SIZE (184)
#endif
#ifdef SND_11
#define OUT_BUFFER_SIZE (92)
#endif


#define MAX_OUTPUT  0x7fff
#define STEP        0x10000L
#define FB_WNOISE 0x12000L
#define FB_PNOISE   0x08000L
#define NG_PRESET   0x0F35

/*extern int32 sn_Volume[4];
extern int32 sn_Count[4];
extern int32 sn_Output[4];
extern int32 sn_Period[4];*/
//extern uint32 sn_RNG;
//extern int32 sn_NoiseFB;

#define INLINE static inline
#define FMSAMPLE signed short

#ifndef PI
#define PI 3.14159265358979323846
#endif

/* register number to channel number , slot offset */
#define OPN_SLOT(N) ((N>>2)&3)

/* slot number */
#define SLOT1 0
#define SLOT2 2
#define SLOT3 1
#define SLOT4 3

/* struct describing a single operator (SLOT) */
typedef struct
{
	UINT32 eg_sh_active_mask;
	INT32	volume;		/* envelope counter	*/
	UINT8	state;		/* phase type */
	UINT8	eg_sel_ar;	/*  (attack state) */
	UINT8	eg_sh_ar;	/*  (attack state) */
	UINT8	eg_sel_d1r;	/*  (decay state) */
	UINT8	eg_sh_d1r;	/*  (decay state) */
	UINT8	eg_sel_d2r;	/*  (sustain state) */
	UINT8	eg_sh_d2r;	/*  (sustain state) */
	UINT8	eg_sel_rr;	/*  (release state) */
	UINT8	eg_sh_rr;	/*  (release state) */
	UINT8	ssg;		/* SSG-EG waveform */
	UINT8	ssgn;		/* SSG-EG negated output */

	UINT32	sl;			/* sustain level:sl_table[SL] */
	UINT32 eg_sh_d1r_mask;
	UINT32 eg_sh_d2r_mask;
	UINT32 eg_sh_rr_mask;
	UINT32 eg_sh_ar_mask;
	UINT32	tl;			/* total level: TL << 3	*/
	UINT32	vol_out;	/* current output from EG circuit (without AM from LFO) */
	/* LFO */
	UINT32	AMmask;		/* AM enable flag */

	/* Phase Generator */
	UINT32	phase;		/* phase counter */
	UINT32	Incr;		/* phase step */

	INT32	*DT;		/* detune          :dt_tab[DT] */
	UINT32	mul;		/* multiple        :ML_TABLE[ML] */

	UINT8	KSR;		/* key scale rate  :3-KSR */
	UINT8	ksr;		/* key scale rate  :kcode>>(3-KSR) */

	UINT32	key;		/* 0=last key was KEY OFF, 1=KEY ON	*/

	UINT32	ar;			/* attack rate  */
	UINT32	d1r;		/* decay rate   */
	UINT32	d2r;		/* sustain rate */
	UINT32	rr;			/* release rate */
} FM_SLOT;

typedef struct
{
	UINT8	ALGO;		/* algorithm */
	UINT8	FB;			/* feedback shift */
	UINT8	ams;		/* channel AMS */
	UINT8	kcode;		/* key code:						*/
	INT32	op1_out[2];	/* op1 output for feedback */
	INT32	mem_value;	/* delayed sample (MEM) value */

	FM_SLOT	SLOT[4];	/* four SLOTs (operators) */

	INT32	pms;		/* channel PMS */
	UINT32	fc;			/* fnum,blk:adjusted to sample rate	*/
	UINT32	block_fnum;	/* current blk/fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
} FM_CH;


typedef struct
{
	UINT8	address;	/* address register		*/
	UINT8	irq;		/* interrupt level		*/
	UINT8	irqmask;	/* irq mask				*/
	UINT8	status;		/* status flag			*/
	UINT32	mode;		/* mode  CSM / 3SLOT	*/
	UINT8	prescaler_sel;/* prescaler selector	*/
	UINT8	fn_h;		/* freq latch			*/
	UINT8	TB;			/* timer b				*/
	long		TA;			/* timer a				*/
	long		TAC;		/* timer a counter		*/
	long 		TAC_Step;
	long		TBC;		/* timer b counter		*/
	long 		TBC_Step;
} FM_ST;

/***********************************************************/
/* OPN unit                                                */
/***********************************************************/

/* OPN 3slot struct */
typedef struct
{
	UINT32  fc[3];			/* fnum3,blk3: calculated */
	UINT8	fn_h;			/* freq3 latch */
	UINT8	kcode[3];		/* key code */
	UINT32	block_fnum[3];	/* current fnum value for this slot (can be different betweeen slots of one channel in 3slot mode) */
} FM_3SLOT;

/* OPN/A/B common state */
typedef struct
{
	/* LFO */
	UINT32	lfo_inc;
	UINT32	lfo_cnt;

	UINT32	eg_cnt;			/* global envelope generator counter */
	UINT32	eg_timer;		/* global envelope generator counter works at frequency = chipclock/64/3 */
	UINT32	eg_timer_add;	/* step of eg_timer */
	UINT32	eg_timer_overflow;/* envelope generator timer overlfows every 3 samples (on real chip) */
} FM_OPN;

extern char OPN_pan[];	/* fm channels output masks */

extern FM_3SLOT SL3;			/* 3 slot mode state */
extern FM_ST	ST;				/* general state */
extern FM_OPN OPN;				/* OPN state			*/
extern FM_CH CH[6];				/* channel state		*/
extern INT16 ym2612_dacout;
extern int dacen;


typedef int (*algo)(FM_CH *CH);

extern const algo algofuncs[8];
