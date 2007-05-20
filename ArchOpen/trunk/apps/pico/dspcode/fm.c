/*
**
** File: fm.c -- software implementation of Yamaha FM sound generator
**
** Copyright (C) 2001, 2002, 2003 Jarek Burczynski (bujar at mame dot net)
** Copyright (C) 1998 Tatsuyuki Satoh , MultiArcadeMachineEmulator development
**
** Version 1.4 (final beta)
**
*/

#include "../dspshared.h"
#include "pico_dsp.h"
#include "fm.h"
#include "fmint.h"
#include "fmtables.h"
#include "sn76496.h"

/*int32 sn_Volume[4];
int32 sn_Count[4];
int32 sn_Output[4];
int32 sn_Period[4];*/
//uint32 sn_RNG;
//int32 sn_NoiseFB;

#if 0

long Algo0 (FM_CH *CH);
long Algo1 (FM_CH *CH);
long Algo2 (FM_CH *CH);
long Algo3 (FM_CH *CH);
long Algo4 (FM_CH *CH);
long Algo5 (FM_CH *CH);
long Algo6 (FM_CH *CH);
long Algo7 (FM_CH *CH);

const algo algofuncs[8] /*__attribute__ ((aligned (4)))*/ = { Algo0, Algo1, Algo2, Algo3, Algo4, Algo5, Algo6, Algo7 };

FM_3SLOT SL3 /*__attribute__ ((aligned (4)))*/;			/* 3 slot mode state */

FM_ST	ST /*__attribute__ ((aligned (4)))*/;				/* general state */

UINT32 LFO_AM /*__attribute__ ((aligned (4)))*/;		/* runtime LFO calculations helper */
INT32 LFO_PM /*__attribute__ ((aligned (4)))*/;			/* runtime LFO calculations helper */

FM_OPN OPN /*__attribute__ ((aligned (4)))*/;				/* OPN state			*/
FM_CH CH[6] /*__attribute__ ((aligned (4)))*/;				/* channel state		*/
INT32 ym2612_dacout /*__attribute__ ((aligned (4)))*/;
long dacen /*__attribute__ ((aligned (4)))*/;
char OPN_pan[6*2] /*__attribute__ ((aligned (4)))*/;	/* fm channels output masks (non-zero = enable) */


void advance_eg_channel(FM_SLOT *SLOT)
{
	unsigned long out;
	unsigned long swap_flag = 0;

	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				long temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= (((uint32)1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				long temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= (((uint32)1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				long temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= (((uint32)1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				long temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= (((uint32)1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;
}

INLINE long op_calc(UINT32 phase, unsigned long env, signed long pm)
{
	UINT32 p;
	UINT32 x;
	long z;

	x = (phase >> FREQ_SH) + pm;
	if (x&256)
		x^=0xff;

	p = (env<<3) + ((unsigned long)sin_tab[ x & 255 ]);

	if (p < TL_TAB_LEN)
	{
		z=(long)tl_tab[(p&510)>>1];
		z>>=(p>>9);
		if (x&512)
		{
			return -z;
		}
		else
		{
			return z;
		}
	}
	return 0;
}

INLINE long op_calc2(UINT32 phase, unsigned long env)
{
	UINT32 p;
	UINT32 x;
	long z;
	char s[50];

	x = phase >> FREQ_SH;
	if (x&256)
		x^=0xff;

	p = (env<<3) + ((uint32)sin_tab[ x & 255 ]);

	if (p < TL_TAB_LEN)
	{
		z=(long)tl_tab[(p&510)>>1];
		z>>=(p>>9);
		if (x&512)
		{
			/*sprintf(s, "op_calc2 r%x", -z);
			debug(s);*/
			return -z;
		}
		else
		{
			/*sprintf(s, "op_calc2 r%x", z);
			debug(s);*/
			return z;
		}
	}
	/*debug("op_calc2 r0");*/
	return 0;
}

long Algo0 (FM_CH *CH)
{
	INT32	m2,c1;		/* Phase Modulation input for operators 2,3,4 */
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	FM_SLOT *p = CH->SLOT;
	uint32 eg;
	uint32 ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	CH->op1_out[1] = 0;
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc(p->phase, eg, c1>>1);
	else
		CH->mem_value = 0;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		uint32 eg2 = p2->vol_out + (ams & p2->AMmask);
		long c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc(p2->phase, eg2, m2>>1);
		else
			c2=0;

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

long Algo1 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2;		/* Phase Modulation input for operators 2,3,4 */
	INT32	mem;			/* one sample delay memory */
	FM_SLOT *p = CH->SLOT;
	uint32 eg;
	uint32 ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;

	mem = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		mem += op_calc2(p->phase, eg>>1);

	CH->mem_value = mem;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		uint32 eg2 = p2->vol_out + (ams & p2->AMmask);
		long c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc(p2->phase, eg2, m2>>1);
		else
			c2=0;

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

long Algo2 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	uint32 eg;
	uint32 ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;
	CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc2(p->phase, eg);
	else
		CH->mem_value = 0;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		uint32 eg2 = p2->vol_out + (ams & p2->AMmask);
		long c2 = CH->op1_out[0];

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 += op_calc(p2->phase, eg2, m2>>1);

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

long Algo3 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1,c2;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	uint32 eg;
	uint32 ams = LFO_AM >> CH->ams;

	c2 = CH->mem_value;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		c2 += op_calc2(p->phase, eg);

	CH->mem_value = 0;
	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		uint32 eg2 = p2->vol_out + (ams & p2->AMmask);

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 += op_calc2(p2->phase, eg2);

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

long Algo4 (FM_CH *CH)
{
	long chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output = op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned long eg2 = p2->vol_out + (ams & p2->AMmask);
		long c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc2(p2->phase, eg2);
		else
			c2=0;

		chan_output += op_calc(p->phase, eg, c2>>1);
	}

	return chan_output;
}

long Algo5 (FM_CH *CH)
{
	long chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2,c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;
	CH->mem_value = c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc(p->phase, eg, m2>>1);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	return chan_output;
}

long Algo6 (FM_CH *CH)
{
	long chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc2(p->phase, eg);

	return chan_output;
}

long Algo7 (FM_CH *CH)
{
	long chan_output;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	chan_output = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc2(p->phase, eg);

	return chan_output;
}

INLINE void chan_calc2 (FM_CH *CH)
{
	/* update phase counters AFTER output calculations */
	if(CH->pms)
	{
		/* add support for 3 slot mode */
		UINT32 block_fnum = CH->block_fnum;

		UINT32 fnum_lfo   = ((uint32)(block_fnum & (uint32)0x7f0) << 4);

		//reego
		INT32 lfo_fn_table_index_offset;
		/*int idx=fnum_lfo + CH->pms + LFO_PM;
		if ( idx%2 )
			lfo_fn_table_index_offset=(int)lfo_pm_table[idx/2]&0x00FF;
		else
			lfo_fn_table_index_offset=(int)(lfo_pm_table[idx/2]&0xFF00)>>8;*/

		//lfo_fn_table_index_offset = (int)lfo_pm_table[ fnum_lfo + CH->pms + LFO_PM ];
		lfo_fn_table_index_offset = 0;

		if (lfo_fn_table_index_offset)	/* LFO phase modulation active */
		{
			UINT8  blk;
			UINT32 fn;
			long kc,fc;

			block_fnum = (block_fnum<<1) + lfo_fn_table_index_offset;

			blk = (block_fnum&0x7000) >> 12;
			fn  = block_fnum & 0xfff;

			/* keyscale code */
			kc = (blk<<2) | opn_fktable[fn >> 8];
			/* phase increment counter */
			fc = OPN_fn_table[fn]>>(7-blk);

			CH->SLOT[SLOT1].phase += ((fc+CH->SLOT[SLOT1].DT[kc])*CH->SLOT[SLOT1].mul) >> 1;
			CH->SLOT[SLOT2].phase += ((fc+CH->SLOT[SLOT2].DT[kc])*CH->SLOT[SLOT2].mul) >> 1;
			CH->SLOT[SLOT3].phase += ((fc+CH->SLOT[SLOT3].DT[kc])*CH->SLOT[SLOT3].mul) >> 1;
			CH->SLOT[SLOT4].phase += ((fc+CH->SLOT[SLOT4].DT[kc])*CH->SLOT[SLOT4].mul) >> 1;

			return;
		}
	}

	CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
	CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
	CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
	CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
}

void YM2612UpdateOne(INT16 **buffer)
{
	int i;
	char s[50];

	/* buffering */
	for(i=0; i < OUT_BUFFER_SIZE ; i++)
	{
		{
			if (OPN.lfo_inc)	/* LFO enabled ? */
			{
				UINT8 pos;
				OPN.lfo_cnt += OPN.lfo_inc;
				pos = (OPN.lfo_cnt >> LFO_SH) & 127;
				if (pos<64)
					LFO_AM = pos << 1;
				else
					LFO_AM = 126 - ((pos&63) << 1);

				LFO_PM = pos >> 2;
			}
			else
			{
				LFO_AM = 0;
				LFO_PM = 0;
			}

		}

		/* advance envelope generator */
		OPN.eg_timer += OPN.eg_timer_add;
		while (OPN.eg_timer >= OPN.eg_timer_overflow)
		{
			OPN.eg_timer -= OPN.eg_timer_overflow;
			OPN.eg_cnt++;

			advance_eg_channel(CH[0].SLOT);
			advance_eg_channel(CH[1].SLOT);
			advance_eg_channel(CH[2].SLOT);
			advance_eg_channel(CH[3].SLOT);
			advance_eg_channel(CH[4].SLOT);
			advance_eg_channel(CH[5].SLOT);
		}
		{
			/*unsigned long psg_out;
			{
				long vol;

				vol=0;

				if (sn.Output[0])
					vol = sn.Count[0];
				sn.Count[0] -= STEP;


				while (sn.Count[0] <= 0)
				{
					sn.Count[0] += sn.Period[0];
					if (sn.Count[0] > 0)
					{
						sn.Output[0] ^= 1;
						if (sn.Output[0])
							vol += sn.Period[0];
						break;
					}
					sn.Count[0] += sn.Period[0];
					vol += sn.Period[0];
				}
				if (sn.Output[0])
					vol -= sn.Count[0];

				psg_out = vol * sn.Volume[0];


				vol=0;
				if (sn.Output[1])
					vol = sn.Count[1];
				sn.Count[1] -= STEP;

				while (sn.Count[1] <= 0)
				{
					sn.Count[1] += sn.Period[1];
					if (sn.Count[1] > 0)
					{
						sn.Output[1] ^= 1;
						if (sn.Output[1])
							vol += sn.Period[1];
						break;
					}
					sn.Count[1] += sn.Period[1];
					vol += sn.Period[1];
				}
				if (sn.Output[1])
					vol -= sn.Count[1];

				psg_out += vol * sn.Volume[1];

				vol=0;
				if (sn.Output[2])
					vol = sn.Count[2];
				sn.Count[2] -= STEP;
				*//* Period[i] is the half period of the square wave. Here, in each */
				/* loop I add Period[i] twice, so that at the end of the loop the */
				/* square wave is in the same status (0 or 1) it was at the start. */
				/* vol[i] is also incremented by Period[i], since the wave has been 1 */
				/* exactly half of the time, regardless of the initial position. */
				/* If we exit the loop in the middle, Output[i] has to be inverted */
				/* and vol[i] incremented only if the exit status of the square */
				/* wave is 1. */
				/*while (sn.Count[2] <= 0)
				{
					sn.Count[2] += sn.Period[2];
					if (sn.Count[2] > 0)
					{
						sn.Output[2] ^= 1;
						if (sn.Output[2])
							vol += sn.Period[2];
						break;
					}
					sn.Count[2] += sn.Period[2];
					vol += sn.Period[2];
				}
				if (sn.Output[2])
					vol -= sn.Count[2];


				psg_out += vol * sn.Volume[2];

				{
					long left = STEP;

					vol=0;
					do
					{
						long nextevent;

						if (sn.Count[3] < left)
							nextevent = sn.Count[3];
						else
							nextevent = left;

						if (sn.Output[3])
							vol += sn.Count[3];

						sn.Count[3] -= nextevent;

						if (sn.Count[3] <= 0)
						{
							if (sn_RNG & 1)
								sn_RNG ^= sn_NoiseFB;
							sn_RNG >>= 1;
							sn.Output[3] = sn_RNG & 1;
							sn.Count[3] += sn.Period[3];
							if (sn.Output[3])
								vol += sn.Period[3];
						}
						if (sn.Output[3])
							vol -= sn.Count[3];

						left -= nextevent;
					}
					while (left > 0);

					psg_out += vol * sn.Volume[3];
				}
				if(psg_out > MAX_OUTPUT * STEP) psg_out = MAX_OUTPUT * STEP;
				psg_out/=STEP;
			}*/

			{
				long lt = 0;
				long rt = lt;

				/* calculate FM */
				{
					FM_CH *pCH = CH;
					long temp;

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[0])
						lt+=temp;
					if (OPN_pan[1])
						rt+=temp;
					chan_calc2( pCH++ );

					/*sprintf(s, "temp0 : %x", temp);
					if(temp!=0) debug(s);*/

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[2])
						lt+=temp;
					if (OPN_pan[3])
						rt+=temp;
					chan_calc2( pCH++ );

					//sprintf(s, "temp1 : %x", temp);
					//if(temp!=0) debug(s);

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[4])
						lt+=temp;
					if (OPN_pan[5])
						rt+=temp;
					chan_calc2( pCH++ );

					sprintf(s, "temp2 : %x", temp);
					//if(temp!=0) debug(s);

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[6])
						lt+=temp;
					if (OPN_pan[7])
						rt+=temp;
					chan_calc2( pCH++ );

					sprintf(s, "temp3 : %x", temp);
					//if(temp!=0) debug(s);

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[8])
						lt+=temp;
					if (OPN_pan[9])
						rt+=temp;
					chan_calc2( pCH++ );

					if( !dacen )
					{
						temp = algofuncs[pCH->ALGO](pCH);
						if (OPN_pan[10])
							lt+=temp;
						if (OPN_pan[11])
							rt+=temp;
						chan_calc2( pCH );
					}


					lt <<= 1;
					rt <<= 1;

					if (lt > (32767))
						lt = 32767;
					if (rt > (32767))
						rt = 32767;
					if (lt < (-32768))
						lt = -32768;
					if (rt < (-32768))
						rt = -32768;

					/* buffering */
					/*buffer[0][i] = (short)lt;
					buffer[1][i] = (short)rt;*/

        			buffer[0][i] = lt;
        			buffer[1][i] = rt;
				}
			}
		}
		/* timer A control */
		{
			if( ST.TAC )
			{
				if( (ST.TAC -= ST.TAC_Step) <= 0 )
				{
					/* set status (if enabled) */
					if(ST.mode & 0x04)
					{
						/* set status flag */
						ST.status |= 0x01;
						if (ST.status & ST.irqmask)
							ST.irq = 1;
					}
					/* clear or reload the counter */
					ST.TAC = (1024-ST.TA);
					/* CSM mode total level latch and auto key on */
					if( ST.mode & 0x80 )
					{
						FM_SLOT *pSlot = &CH[2].SLOT[SLOT1];
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
					}
				}
			}
		}
	}

	if( ST.TBC  )
	{
		if( (ST.TBC -= ST.TBC_Step) <= 0 )
		{
			/* set status (if enabled) */
			if(ST.mode & 0x08)
			{
				/* set status flag */
				ST.status |= 0x02;
				if ( ST.status & ST.irqmask)
					ST.irq = 1;
			}
			/* clear or reload the counter */
			ST.TBC = (256-ST.TB)<<4;
		}
	}
}

#else
int Algo0 (FM_CH *CH);
int Algo1 (FM_CH *CH);
int Algo2 (FM_CH *CH);
int Algo3 (FM_CH *CH);
int Algo4 (FM_CH *CH);
int Algo5 (FM_CH *CH);
int Algo6 (FM_CH *CH);
int Algo7 (FM_CH *CH);

const algo algofuncs[8] /*__attribute__ ((aligned (4)))*/ = { Algo0, Algo1, Algo2, Algo3, Algo4, Algo5, Algo6, Algo7 };

FM_3SLOT SL3 /*__attribute__ ((aligned (4)))*/;			/* 3 slot mode state */

FM_ST	ST /*__attribute__ ((aligned (4)))*/;				/* general state */

UINT32 LFO_AM /*__attribute__ ((aligned (4)))*/;		/* runtime LFO calculations helper */
INT32 LFO_PM /*__attribute__ ((aligned (4)))*/;			/* runtime LFO calculations helper */

FM_OPN OPN /*__attribute__ ((aligned (4)))*/;				/* OPN state			*/
FM_CH CH[6] /*__attribute__ ((aligned (4)))*/;				/* channel state		*/
INT16 ym2612_dacout /*__attribute__ ((aligned (4)))*/;
int dacen /*__attribute__ ((aligned (4)))*/;
char OPN_pan[6*2] /*__attribute__ ((aligned (4)))*/;	/* fm channels output masks (non-zero = enable) */


void advance_eg_channel(FM_SLOT *SLOT)
{
	unsigned int out;
	unsigned int swap_flag = 0;

	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				int temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= ((1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				int temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= ((1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				int temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= ((1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;

	SLOT++;
	if (!(OPN.eg_cnt & SLOT->eg_sh_active_mask))
	{
		switch(SLOT->state)
		{
		case EG_ATT:		/* attack phase */
			SLOT->volume += (~SLOT->volume * (eg_inc[SLOT->eg_sel_ar + ((OPN.eg_cnt>>SLOT->eg_sh_ar)&7)])) >>4;
			if (SLOT->volume <= MIN_ATT_INDEX)
			{
				SLOT->volume = MIN_ATT_INDEX;
				SLOT->state = EG_DEC;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d1r_mask;
			}
			break;
		case EG_DEC:	/* decay phase */
			{
				int temp;
				temp = eg_inc[SLOT->eg_sel_d1r + ((OPN.eg_cnt>>SLOT->eg_sh_d1r)&7)];
				if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
					SLOT->volume += temp << 2;
				else
					SLOT->volume += temp;
			}
			if ( SLOT->volume >= SLOT->sl )
			{
				SLOT->state = EG_SUS;
				SLOT->eg_sh_active_mask = SLOT->eg_sh_d2r_mask;
			}
			break;

		case EG_SUS:	/* sustain phase */
			if (SLOT->ssg&0x08)	/* SSG EG type envelope selected */
			{
				SLOT->volume += (eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)]) << 2;
				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					if (SLOT->ssg&0x01)	/* bit 0 = hold */
					{
						if (!(SLOT->ssgn&1))	/* have we swapped once ??? */
							swap_flag = (SLOT->ssg&0x02) | 1 ; /* bit 1 = alternate */
					}
					else
					{
						SLOT->state = EG_ATT;
						SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
						swap_flag = (SLOT->ssg&0x02); /* bit 1 = alternate */
					}
				}
			}
			else
			{
				SLOT->volume += eg_inc[SLOT->eg_sel_d2r + ((OPN.eg_cnt>>SLOT->eg_sh_d2r)&7)];

				if ( SLOT->volume >= MAX_ATT_INDEX )
				{
					SLOT->volume = MAX_ATT_INDEX;
					/* do not change SLOT->state (verified on real chip) */
				}
			}
			break;

		case EG_REL:	/* release phase */
			SLOT->volume += eg_inc[SLOT->eg_sel_rr + ((OPN.eg_cnt>>SLOT->eg_sh_rr)&7)];
			if ( SLOT->volume >= MAX_ATT_INDEX )
			{
				SLOT->volume = MAX_ATT_INDEX;
				SLOT->state = EG_OFF;
				SLOT->eg_sh_active_mask = 0;
			}
			break;
		}
	}

	out = SLOT->tl + ((UINT32)SLOT->volume);

	if ((SLOT->ssg&0x08) && (SLOT->ssgn&2))	/* negate output (changes come from alternate bit, init comes from attack bit) */
		out ^= ((1<<ENV_BITS)-1); /* 1023 */

	SLOT->vol_out = out;
	SLOT->ssgn ^= swap_flag;
}

INLINE int op_calc(UINT32 phase, unsigned long env, signed long pm)
{
	UINT32 p;
	UINT32 x;
	int z;

	x = (phase >> FREQ_SH) + pm;
	if (x&256)
		x^=0xff;

	p = (env<<3) + ((unsigned int)sin_tab[ x & 255 ]);

	if (p < TL_TAB_LEN)
	{
		z=(int)tl_tab[(p&510)>>1];
		z>>=(p>>9);
		if (x&512)
			return -z;
		else
			return z;
	}
	return 0;
}

INLINE int op_calc2(UINT32 phase, unsigned long env)
{
	UINT32 p;
	UINT32 x;
	int z;

	x = phase >> FREQ_SH;
	if (x&256)
		x^=0xff;

	p = (env<<3) + ((unsigned int)sin_tab[ x & 255 ]);

	if (p < TL_TAB_LEN)
	{
		z=(int)tl_tab[(p&510)>>1];
		z>>=(p>>9);
		if (x&512)
			return -z;
		else
			return z;
	}
	return 0;
}

int Algo0 (FM_CH *CH)
{
	INT32	m2,c1;		/* Phase Modulation input for operators 2,3,4 */
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	CH->op1_out[1] = 0;
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc(p->phase, eg, c1>>1);
	else
		CH->mem_value = 0;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned int eg2 = p2->vol_out + (ams & p2->AMmask);
		int c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc(p2->phase, eg2, m2>>1);
		else
			c2=0;

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

int Algo1 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2;		/* Phase Modulation input for operators 2,3,4 */
	INT32	mem;			/* one sample delay memory */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;

	mem = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		mem += op_calc2(p->phase, eg>>1);

	CH->mem_value = mem;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned long eg2 = p2->vol_out + (ams & p2->AMmask);
		int c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc(p2->phase, eg2, m2>>1);
		else
			c2=0;

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

int Algo2 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;
	CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc2(p->phase, eg);
	else
		CH->mem_value = 0;

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned long eg2 = p2->vol_out + (ams & p2->AMmask);
		long c2 = CH->op1_out[0];

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 += op_calc(p2->phase, eg2, m2>>1);

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

int Algo3 (FM_CH *CH)
{
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1,c2;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	c2 = CH->mem_value;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		c2 += op_calc2(p->phase, eg);

	CH->mem_value = 0;
	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		CH->mem_value = op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned long eg2 = p2->vol_out + (ams & p2->AMmask);

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 += op_calc2(p2->phase, eg2);

		return op_calc(p->phase, eg, c2>>1);
	}

	return 0;
}

int Algo4 (FM_CH *CH)
{
	int chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output = op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
	{
		FM_SLOT *p2 = &(CH->SLOT[SLOT3]);
		unsigned long eg2 = p2->vol_out + (ams & p2->AMmask);
		int c2;

		if( eg2 < ENV_QUIET )		/* SLOT 3 */
			c2 = op_calc2(p2->phase, eg2);
		else
			c2=0;

		chan_output += op_calc(p->phase, eg, c2>>1);
	}

	return chan_output;
}

int Algo5 (FM_CH *CH)
{
	int chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	m2,c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	m2 = CH->mem_value;
	CH->mem_value = c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc(p->phase, eg, m2>>1);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	return chan_output;
}

int Algo6 (FM_CH *CH)
{
	int chan_output = 0;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	INT32	c1;		/* Phase Modulation input for operators 2,3,4 */
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	c1 = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc(p->phase, eg, c1>>1);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc2(p->phase, eg);

	return chan_output;
}

int Algo7 (FM_CH *CH)
{
	int chan_output;
	INT32 out = CH->op1_out[0] + CH->op1_out[1];
	FM_SLOT *p = CH->SLOT;
	unsigned long eg;
	unsigned long ams = LFO_AM >> CH->ams;

	chan_output = CH->op1_out[0] = CH->op1_out[1];

	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )	/* SLOT 1 */
	{
		if (!CH->FB)
			out=0;
		CH->op1_out[1] = op_calc2(p->phase + (out<<CH->FB), eg );
	}
	else
		CH->op1_out[1] = 0;

	p=&(CH->SLOT[SLOT3]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 3 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT2]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 2 */
		chan_output += op_calc2(p->phase, eg);

	p=&(CH->SLOT[SLOT4]);
	eg = p->vol_out + (ams & p->AMmask);
	if( eg < ENV_QUIET )		/* SLOT 4 */
		chan_output += op_calc2(p->phase, eg);

	return chan_output;
}

INLINE void chan_calc2 (FM_CH *CH)
{
	/* update phase counters AFTER output calculations */
	if(CH->pms)
	{
		/* add support for 3 slot mode */
		UINT32 block_fnum = CH->block_fnum;

		UINT32 fnum_lfo   = ((block_fnum & 0x7f0) << 4);

		//INT32  lfo_fn_table_index_offset = 0;
		//INT32  lfo_fn_table_index_offset = (int)lfo_pm_table[ fnum_lfo + CH->pms + LFO_PM ];
		INT32 lfo_fn_table_index_offset = lfo_pm_table( fnum_lfo + CH->pms + LFO_PM);

		if (lfo_fn_table_index_offset)	/* LFO phase modulation active */
		{
			UINT8  blk;
			UINT32 fn;
			int kc;
			long fc;

			block_fnum = (block_fnum<<1) + lfo_fn_table_index_offset;

			blk = (block_fnum&0x7000) >> 12;
			fn  = block_fnum & 0xfff;

			/* keyscale code */
			kc = (blk<<2) | opn_fktable[fn >> 8];
			/* phase increment counter */
			fc = OPN_fn_table[fn]>>(7-blk);

			CH->SLOT[SLOT1].phase += ((fc+CH->SLOT[SLOT1].DT[kc])*CH->SLOT[SLOT1].mul) >> 1;
			CH->SLOT[SLOT2].phase += ((fc+CH->SLOT[SLOT2].DT[kc])*CH->SLOT[SLOT2].mul) >> 1;
			CH->SLOT[SLOT3].phase += ((fc+CH->SLOT[SLOT3].DT[kc])*CH->SLOT[SLOT3].mul) >> 1;
			CH->SLOT[SLOT4].phase += ((fc+CH->SLOT[SLOT4].DT[kc])*CH->SLOT[SLOT4].mul) >> 1;

			return;
		}
	}

	CH->SLOT[SLOT1].phase += CH->SLOT[SLOT1].Incr;
	CH->SLOT[SLOT2].phase += CH->SLOT[SLOT2].Incr;
	CH->SLOT[SLOT3].phase += CH->SLOT[SLOT3].Incr;
	CH->SLOT[SLOT4].phase += CH->SLOT[SLOT4].Incr;
}

void YM2612UpdateOne(INT16 *buffer)
{
	int i;

	/* buffering */
	for(i=0; i < OUT_BUFFER_SIZE ; i++)
	{
		{
			if (OPN.lfo_inc)	/* LFO enabled ? */
			{
				UINT8 pos;
				OPN.lfo_cnt += OPN.lfo_inc;
				pos = (OPN.lfo_cnt >> LFO_SH) & 127;
				if (pos<64)
					LFO_AM = pos << 1;
				else
					LFO_AM = 126 - ((pos&63) << 1);

				LFO_PM = pos >> 2;
			}
			else
			{
				LFO_AM = 0;
				LFO_PM = 0;
			}

		}

		/* advance envelope generator */
		OPN.eg_timer += OPN.eg_timer_add;
		while (OPN.eg_timer >= OPN.eg_timer_overflow)
		{
			OPN.eg_timer -= OPN.eg_timer_overflow;
			OPN.eg_cnt++;

			advance_eg_channel(CH[0].SLOT);
			advance_eg_channel(CH[1].SLOT);
			advance_eg_channel(CH[2].SLOT);
			advance_eg_channel(CH[3].SLOT);
			advance_eg_channel(CH[4].SLOT);
			advance_eg_channel(CH[5].SLOT);
		}
		{
			long psg_out=0;
			if ( dspCom->PSGEnabled )
			{
				long vol;

				vol=0;
				if (sn.Output[0])
					vol = sn.Count[0];
				sn.Count[0] -= STEP;

				while (sn.Count[0] <= 0)
				{
					sn.Count[0] += sn.Period[0];
					if (sn.Count[0] > 0)
					{
						sn.Output[0] ^= 1;
						if (sn.Output[0])
							vol += sn.Period[0];
						break;
					}
					sn.Count[0] += sn.Period[0];
					vol += sn.Period[0];
				}
				if (sn.Output[0])
					vol -= sn.Count[0];

				psg_out = vol * sn.Volume[0];

				vol=0;
				if (sn.Output[1])
					vol = sn.Count[1];
				sn.Count[1] -= STEP;

				while (sn.Count[1] <= 0)
				{
					sn.Count[1] += sn.Period[1];
					if (sn.Count[1] > 0)
					{
						sn.Output[1] ^= 1;
						if (sn.Output[1])
							vol += sn.Period[1];
						break;
					}
					sn.Count[1] += sn.Period[1];
					vol += sn.Period[1];
				}
				if (sn.Output[1])
					vol -= sn.Count[1];

				psg_out += vol * sn.Volume[1];

				vol=0;
				if (sn.Output[2])
					vol = sn.Count[2];
				sn.Count[2] -= STEP;
				/* Period[i] is the half period of the square wave. Here, in each */
				/* loop I add Period[i] twice, so that at the end of the loop the */
				/* square wave is in the same status (0 or 1) it was at the start. */
				/* vol[i] is also incremented by Period[i], since the wave has been 1 */
				/* exactly half of the time, regardless of the initial position. */
				/* If we exit the loop in the middle, Output[i] has to be inverted */
				/* and vol[i] incremented only if the exit status of the square */
				/* wave is 1. */
				while (sn.Count[2] <= 0)
				{
					sn.Count[2] += sn.Period[2];
					if (sn.Count[2] > 0)
					{
						sn.Output[2] ^= 1;
						if (sn.Output[2])
							vol += sn.Period[2];
						break;
					}
					sn.Count[2] += sn.Period[2];
					vol += sn.Period[2];
				}
				if (sn.Output[2])
					vol -= sn.Count[2];

				psg_out += vol * sn.Volume[2];

				{
					long left = STEP;

					vol=0;
					do
					{
						long nextevent;

						if (sn.Count[3] < left)
							nextevent = sn.Count[3];
						else
							nextevent = left;

						if (sn.Output[3])
							vol += sn.Count[3];

						sn.Count[3] -= nextevent;

						if (sn.Count[3] <= 0)
						{
							if (sn.RNG & 1)
								sn.RNG ^= sn.NoiseFB;
							sn.RNG >>= 1;
							sn.Output[3] = sn.RNG & 1;
							sn.Count[3] += sn.Period[3];
							if (sn.Output[3])
								vol += sn.Period[3];
						}
						if (sn.Output[3])
							vol -= sn.Count[3];

						left -= nextevent;
					}
					while (left > 0);

					psg_out += vol * sn.Volume[3];
				}


			}
			{
				long lt = (psg_out>>16);
				long rt = lt;

				if ( dspCom->YM2612Enabled )
				/* calculate FM */
				{
					FM_CH *pCH = CH;
					long temp;

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[0])
						lt+=temp;
					if (OPN_pan[1])
						rt+=temp;
					chan_calc2( pCH++ );

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[2])
						lt+=temp;
					if (OPN_pan[3])
						rt+=temp;
					chan_calc2( pCH++ );

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[4])
						lt+=temp;
					if (OPN_pan[5])
						rt+=temp;
					chan_calc2( pCH++ );

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[6])
						lt+=temp;
					if (OPN_pan[7])
						rt+=temp;
					chan_calc2( pCH++ );

					temp = algofuncs[pCH->ALGO](pCH);
					if (OPN_pan[8])
						lt+=temp;
					if (OPN_pan[9])
						rt+=temp;
					chan_calc2( pCH++ );

					if( !dacen )
					{
						temp = algofuncs[pCH->ALGO](pCH);
						if (OPN_pan[10])
							lt+=temp;
						if (OPN_pan[11])
							rt+=temp;
						chan_calc2( pCH );
					}
				}


				/*if (dacen && dspCom->DACEnabled)
				{*/
					/*if ( ym2612_dacout_tail!=ym2612_dacout_head ){
						if(dacen && ym2612_dacout_queue[ym2612_dacout_tail]!=0)
						{
							lt+=ym2612_dacout_queue[ym2612_dacout_tail];
							rt+=ym2612_dacout_queue[ym2612_dacout_tail];
							last_dac_sample=ym2612_dacout_queue[ym2612_dacout_tail];
						}
						else
						{
							if (dacen)
							{
								lt+=last_dac_sample;
								rt+=last_dac_sample;
							}
						}

						ym2612_dacout_tail=(ym2612_dacout_tail+1)&DACOUTQUEUE_MASK;
					}*/
				/*}*/

				/*if (dacen && dspCom->DACEnabled)
				{*/
				if(dspCom->DACEnabled)
					if ( ym2612_dacout_tail!=ym2612_dacout_head ){
						
						if (OPN_pan[10]) lt+=ym2612_dacout_queue[ym2612_dacout_tail];
						if (OPN_pan[11]) rt+=ym2612_dacout_queue[ym2612_dacout_tail];

						ym2612_dacout_tail=(ym2612_dacout_tail+1)&DACOUTQUEUE_MASK;
					}
					else
					{
						if (dacen) 
						{
							lt+=ym2612_dacout;
							rt+=ym2612_dacout;
						}
					}
					
					/*dspCom->dacen=ym2612_dacout;
					dspCom->dacen1=ym2612_dacout_queue[ym2612_dacout_tail];*/

					//lt=rt=ym2612_dacout;
					
					
				/*}*/

				lt >>= 1;
				rt >>= 1;



				if (lt > (32767L))
					lt = 32767L;
				if (rt > (32767L))
					rt = 32767L;
				if (lt < (-32768L))
					lt = -32768L;
				if (rt < (-32768L))
					rt = -32768L;

				/* buffering */
				/**(buffer[0]+i) = (short)lt;
				*(buffer[1]+i) = (short)rt;*/
				*buffer++ = (short)lt;
				*buffer++ = (short)rt;
			}
		}
		/*if ( !dspCom->DACEnabled )*/
		/* timer A control */
		{
			#if 0
			if( ST.TAC )
			{
				if( (ST.TAC -= ST.TAC_Step) <= 0 )
				{
					/* set status (if enabled) */
					if(ST.mode & 0x04)
					{
						/* set status flag */
						ST.status |= 0x01;
						dspCom->YM2612Status=ST.status;
						if (ST.status & ST.irqmask)
							ST.irq = 1;
					}
					/* clear or reload the counter */
					ST.TAC = (1024-ST.TA);
					/* CSM mode total level latch and auto key on */
					#if 0
					if( ST.mode & 0x80 )
					{
						FM_SLOT *pSlot = &CH[2].SLOT[SLOT1];
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
						pSlot++;
						if( !pSlot->key )
						{
							pSlot->key = 1;
							pSlot->phase = 0;		/* restart Phase Generator */
							pSlot->state = EG_ATT;	/* phase -> Attack */
							pSlot->eg_sh_active_mask = pSlot->eg_sh_ar_mask;
						}
					}
					#endif
				}
			}
			#endif
		}
	}
	
	#if 0
	/*if ( !dspCom->DACEnabled )*/
	if( ST.TBC  )
	{
		if( (ST.TBC -= ST.TBC_Step) <= 0 )
		{
			/* set status (if enabled) */
			if(ST.mode & 0x08)
			{
				/* set status flag */
				ST.status |= 0x02;
				dspCom->YM2612Status=ST.status;
				if ( ST.status & ST.irqmask)
					ST.irq = 1;
			}
			/* clear or reload the counter */
			ST.TBC = ( 256-ST.TB)<<4;
		}
	}
	#endif
}





#endif
