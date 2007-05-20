#ifndef __FMTABLES_H__
#define __FMTABLES_H__


#include "../dspshared.h"

#define ENV_BITS		10
#define ENV_LEN			(1<<ENV_BITS)
#define ENV_STEP		(128.0/ENV_LEN)

#define MAX_ATT_INDEX	(ENV_LEN-1) /* 1023 */
#define MIN_ATT_INDEX	(0)			/* 0 */

#define FREQ_SH			16  /* 16.16 fixed point (frequency calculations) */
#define EG_SH			16  /* 16.16 fixed point (envelope generator timing) */
#define LFO_SH			24  /*  8.24 fixed point (LFO calculations)       */
#define TIMER_SH		16  /* 16.16 fixed point (timers calculations)    */

#define FREQ_MASK		((1<<FREQ_SH)-1)

#define EG_ATT			4
#define EG_DEC			3
#define EG_SUS			2
#define EG_REL			1
#define EG_OFF			0

#define SIN_BITS		10
#define SIN_LEN			(1<<SIN_BITS)
#define SIN_MASK		(SIN_LEN-1)

#define TL_RES_LEN		(256) /* 8 bits addressing (real chip) */


#define MAXOUT		(+32767)
#define MINOUT		(-32768)


/*	TL_TAB_LEN is calculated as:
*	13 - sinus amplitude bits     (Y axis)
*	2  - sinus sign bit           (Y axis)
*	TL_RES_LEN - sinus resolution (X axis)
*/
#define TL_TAB_LEN (13*2*TL_RES_LEN)
#define ENV_QUIET		(TL_TAB_LEN>>3)

#define RATE_STEPS (8)

extern const UINT32 sl_table[];
extern const UINT8 dt_tab[];
extern const UINT8 opn_fktable[16];
extern const UINT8 eg_rate_select[];
extern const UINT8 eg_rate_shift[];
extern const UINT8 eg_inc[19*RATE_STEPS];
extern const UINT8 lfo_pm_output[7*8][8];
extern const UINT32 lfo_samples_per_step[8];
extern const UINT8 lfo_ams_depth_shift[4];
/*extern *//*const*//* signed char lfo_pm_table[32768]; *///128*8*32
/*extern const signed short lfo_pm_table[16144];*/
extern /*const*/ signed long YMOPN_ST_dt_tab[8][32];
extern /*const*/ unsigned long OPN_fn_table[4096];	/* fnumber->increment counter */
extern /*const*/ unsigned long OPN_lfo_freq[8];	/* LFO FREQ table */
extern const signed short tl_tab[];
extern const unsigned short sin_tab[];


#endif /* __FMTABLES_H__ */


