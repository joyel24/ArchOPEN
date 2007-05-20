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
#if 0

#include <string.h>
#include <math.h>
#include "../dspshared.h"
#include "fm.h"
#include "fmint.h"
#include "fmtables.h"
#include "sn76496.h"

UINT8 addr_A1 /*__attribute__ ((aligned (4)))*/;			/* address line A1		*/

/* OPN Mode Register Write */
INLINE void set_timers(int v ) 
{
	/* b7 = CSM MODE */
	/* b6 = 3 slot mode */
	/* b5 = reset b */
	/* b4 = reset a */
	/* b3 = timer enable b */
	/* b2 = timer enable a */
	/* b1 = load b */
	/* b0 = load a */
	ST.mode = v;

	/* reset Timer b flag */
	if( v & 0x20 )
	{
		/* reset status flag */
		ST.status &=~0x02;
		if (!(ST.status & ST.irqmask) )
			ST.irq = 0;
	}
	/* reset Timer a flag */
	if( v & 0x10 )
	{
		/* reset status flag */
		ST.status &=~0x01;
		if (!(ST.status & ST.irqmask) )
			ST.irq = 0;
	}
	/* load b */
	if( v & 0x02 )
	{
		if( ST.TBC == 0 )
			ST.TBC = ( 256-ST.TB)<<4;
	}
	else
		ST.TBC = 0;
	/* load a */
	if( v & 0x01 )
	{
		if( ST.TAC == 0 )
			ST.TAC = (1024-ST.TA);
	}
	else
			ST.TAC = 0;

    /*debug("settimers");*/
}

INLINE void FM_KEYON(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( !SLOT->key )
	{
		SLOT->key = 1;
		SLOT->phase = 0;		/* restart Phase Generator */
		SLOT->state = EG_ATT;	/* phase -> Attack */
		SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
	}
}

INLINE void FM_KEYOFF(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( SLOT->key )
	{
		SLOT->key = 0;
		if (SLOT->state>EG_REL)
		{
			SLOT->state = EG_REL;/* phase -> Release */
			SLOT->eg_sh_active_mask = SLOT->eg_sh_rr_mask;
		}
	}
}

/* set detune & multiple */
INLINE void set_det_mul(FM_CH *CH,FM_SLOT *SLOT,int v)
{
	SLOT->mul = (v&0x0f)? (v&0x0f)*2 : 1;
	SLOT->DT  = (INT32 *)YMOPN_ST_dt_tab[(v>>4)&7];
	CH->SLOT[SLOT1].Incr=-1;
}

/* set total level */
INLINE void set_tl(FM_SLOT *SLOT , int v)
{
	SLOT->tl = (v&0x7f)<<(ENV_BITS-7); /* 7bit TL */
}

/* set attack rate & key scale  */
INLINE void set_ar_ksr(FM_CH *CH,FM_SLOT *SLOT,int v)
{
	UINT8 old_KSR = SLOT->KSR;

	SLOT->ar = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->KSR = 3-(v>>6);
	if (SLOT->KSR != old_KSR)
	{
		CH->SLOT[SLOT1].Incr=-1;
	}
	else
	{
		/* refresh Attack rate */
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar  = 0;
			SLOT->eg_sel_ar = 17*RATE_STEPS;
		}
	}
}

/* set decay rate */
INLINE void set_dr(FM_SLOT *SLOT,int v)
{
	SLOT->d1r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
	SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + SLOT->ksr];

}

/* set sustain rate */
INLINE void set_sr(FM_SLOT *SLOT,int v)
{
	SLOT->d2r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
	SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + SLOT->ksr];
}

/* set release rate */
INLINE void set_sl_rr(FM_SLOT *SLOT,int v)
{
	SLOT->sl = sl_table[ v>>4 ];

	SLOT->rr  = 34 + ((v&0x0f)<<2);

	SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
	SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
}

/* write a OPN mode register 0x20-0x2f */
static void OPNWriteMode(int r, int v)
{
	UINT8 c;
	FM_CH *pCH;

	switch(r){
	case 0x21:	/* Test */
		break;
	case 0x22:	/* LFO FREQ (YM2608/YM2610/YM2610B/YM2612) */
		if (v&0x08) /* LFO enabled ? */
			OPN.lfo_inc = OPN_lfo_freq[v&7];
		else
			OPN.lfo_inc = 0;
		break;
	case 0x24:	/* timer A High 8*/
		ST.TA = (ST.TA & 0x03)|(((int)v)<<2);
		break;
	case 0x25:	/* timer A Low 2*/
		ST.TA = (ST.TA & 0x3fc)|(v&3);
		break;
	case 0x26:	/* timer B */
		ST.TB = v;
		break;
	case 0x27:	/* mode, timer control */
		set_timers( v );
		break;
	case 0x28:	/* key on / off */
		c = v & 0x03;
		if( c == 3 ) break;
		if(v&0x04) c+=3;
		pCH = &CH[c];
		if(v&0x10) FM_KEYON(pCH,SLOT1); else FM_KEYOFF(pCH,SLOT1);
		if(v&0x20) FM_KEYON(pCH,SLOT2); else FM_KEYOFF(pCH,SLOT2);
		if(v&0x40) FM_KEYON(pCH,SLOT3); else FM_KEYOFF(pCH,SLOT3);
		if(v&0x80) FM_KEYON(pCH,SLOT4); else FM_KEYOFF(pCH,SLOT4);
		break;
	}
}

/* write a OPN register (0x30-0xff) */
static void OPNWriteReg(int r, int v)
{
	FM_CH *pCH;
	FM_SLOT *SLOT;

	UINT8 c = r&3;

	if (c == 3) return; /* 0xX3,0xX7,0xXB,0xXF */

	if (r >= 0x100) c+=3;

	pCH = &CH[c];

	SLOT = &(pCH->SLOT[OPN_SLOT(r)]);

	switch( r & 0xf0 ) {
	case 0x30:	/* DET , MUL */
		set_det_mul(pCH,SLOT,v);
		/*debug("set_det_mul");*/
		break;

	case 0x40:	/* TL */
		set_tl(SLOT,v);
		break;

	case 0x50:	/* KS, AR */
		set_ar_ksr(pCH,SLOT,v);
		break;

	case 0x60:	/* bit7 = AM ENABLE, DR */
		set_dr(SLOT,v);

		SLOT->AMmask = (v&0x80) ? ~0 : 0;
		break;

	case 0x70:	/*     SR */
		set_sr(SLOT,v);
		break;

	case 0x80:	/* SL, RR */
		set_sl_rr(SLOT,v);
		break;

	case 0x90:	/* SSG-EG */

		SLOT->ssg  =  v&0x0f;
		SLOT->ssgn = (v&0x04)>>1; /* bit 1 in ssgn = attack */
		break;

	case 0xa0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xa0-0xa2 : FNUM1 */
			{
				UINT32 fn = (((UINT32)( ((UINT32)ST.fn_h)&(UINT32)7))<<8) + v;
				UINT8 blk = ST.fn_h>>3;
				/* keyscale code */
				pCH->kcode = (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				pCH->fc = OPN_fn_table[fn*2]>>(7-blk);

				/* store fnum in clear form for LFO PM calculations */
				pCH->block_fnum = ((UINT32)(blk)<<11) | fn;

				pCH->SLOT[SLOT1].Incr=-1;
			}
			break;
		case 1:		/* 0xa4-0xa6 : FNUM2,BLK */
			ST.fn_h = v&0x3f;
			break;
		case 2:		/* 0xa8-0xaa : 3CH FNUM1 */
			if(r < 0x100)
			{
				UINT32 fn = (((UINT32)(SL3.fn_h&7))<<8) + v;
				UINT8 blk = SL3.fn_h>>3;
				/* keyscale code */
				SL3.kcode[c]= (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				SL3.fc[c] = OPN_fn_table[fn*2]>>(7-blk);
				SL3.block_fnum[c] = fn;
				pCH[2].SLOT[SLOT1].Incr=-1;
			}
			break;
		case 3:		/* 0xac-0xae : 3CH FNUM2,BLK */
			if(r < 0x100)
				SL3.fn_h = v&0x3f;
			break;
		}
		break;

	case 0xb0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xb0-0xb2 : FB,ALGO */
			{
				int feedback = (v>>3)&7;
				pCH->ALGO = v&7;
				pCH->FB   = feedback ? feedback+6 : 0;
			}
			break;
		case 1:		/* 0xb4-0xb6 : L , R , AMS , PMS (YM2612/YM2610B/YM2610/YM2608) */
			/* b0-2 PMS */
			pCH->pms = ((int32)v & 7) << 5; /* CH->pms = PM depth * 32 (index in lfo_pm_table) */

			/* b4-5 AMS */
			pCH->ams = lfo_ams_depth_shift[(v>>4) & 0x03];
			/* PAN :  b7 = L, b6 = R */
			OPN_pan[ c*2   ] = (v & 0x80);
			OPN_pan[ c*2+1 ] = (v & 0x40);
			break;
		}
		break;
	}
}

/* initialize YM2612 emulator(s) */
int YM2612Init()
{
	YM2612ResetChip();
	return 0;
}

void YM2612ResetChip(void)
{
	int i;
	int c,s;

	OPN.eg_timer_add = 211096;
	OPN.eg_timer_overflow = 196608;
	ST.TAC_Step = 13193;
	ST.TBC_Step = 1820710;

	/* status clear */
	ST.irqmask = 0x03;
	/* reset status flag */
	ST.status = 0;
	ST.irq = 0;

	OPNWriteMode(0x27,0x30); /* mode 0 , timer reset */

	OPN.eg_timer = 0;
	OPN.eg_cnt   = 0;

	ST.mode   = 0;	/* normal mode */
	ST.TA     = 0;
	ST.TAC    = 0;
	ST.TB     = 0;
	ST.TBC    = 0;

	for( c = 0 ; c < 6 ; c++ )
	{
		CH[c].fc = 0;
		for(s = 0 ; s < 4 ; s++ )
		{
			CH[c].SLOT[s].ssg = 0;
			CH[c].SLOT[s].ssgn = 0;
			CH[c].SLOT[s].state= EG_OFF;
			CH[c].SLOT[s].eg_sh_active_mask = 0;
			CH[c].SLOT[s].volume = MAX_ATT_INDEX;
			CH[c].SLOT[s].vol_out= MAX_ATT_INDEX;
		}
	}

	for(i = 0xb6 ; i >= 0xb4 ; i-- )
	{
		OPNWriteReg(i      ,0xc0);
		OPNWriteReg(i|0x100,0xc0);
	}
	for(i = 0xb2 ; i >= 0x30 ; i-- )
	{
		OPNWriteReg(i      ,0);
		OPNWriteReg(i|0x100,0);
	}
	for(i = 0x26 ; i >= 0x20 ; i-- ) OPNWriteReg(i,0);
	/* DAC mode clear */
	dacen = 0;
}

/* YM2612 write */
/* n = number  */
/* a = address */
/* v = value   */
void fm_write(uint16 a, uint16 v)
{
	int addr;
	char s[50];

	//sprintf(s,"fm_write %x %x",(uint16)a,(uint16)v);
	//debug(s);
	v &= 0xff;	/* adjust to 8 bit bus */

	switch( a&3){
	case 0:	/* address port 0 */
		ST.address = v;
		addr_A1 = 0;
		break;

	case 1:	/* data port 0    */
		if (addr_A1 != 0)
			break;	/* verified on real YM2608 */

		addr = ST.address;
		switch( addr & 0xf0 )
		{
		case 0x20:	/* 0x20-0x2f Mode */
			switch( addr )
			{
			case 0x2a:	/* DAC data (YM2612) */
				ym2612_dacout = ((long)v - 0x80) << 6;	/* level unknown */
				break;
			case 0x2b:	/* DAC Sel  (YM2612) */
				/* b7 = dac enable */
				dacen = v & 0x80;
				break;
			default:	/* OPN section */
				/* write register */
				OPNWriteMode(addr,v);
			}
			break;
		default:	/* 0x30-0xff OPN section */
			/* write register */
			OPNWriteReg(addr,v);
		}
		break;

	case 2:	/* address port 1 */
		ST.address = v;
		addr_A1 = 1;
		break;

	case 3:	/* data port 1    */
		if (addr_A1 != 1)
			break;	/* verified on real YM2608 */

		addr = ST.address;
		OPNWriteReg(addr | 0x100,v);
		break;
	}
}

int fm_read(int address)
{
	return ST.status;
}

/* update phase increment and envelope generator */
INLINE void refresh_fc_eg_slot(FM_SLOT *SLOT , long fc , int kc )
{
	int ksr;

	/* (frequency) phase increment counter */
	SLOT->Incr = ((fc+SLOT->DT[kc])*SLOT->mul) >> 1;

	ksr = kc >> SLOT->KSR;
	if( SLOT->ksr != ksr )
	{
		SLOT->ksr = ksr;

		/* calculate envelope generator rates */
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar  = 0;
			SLOT->eg_sel_ar = 17*RATE_STEPS;
		}

		SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
		SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + SLOT->ksr];

		SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
		SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + SLOT->ksr];

		SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
		SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
	}

	SLOT->eg_sh_ar_mask = ((uint32)1<<SLOT->eg_sh_ar)-1;
	SLOT->eg_sh_d1r_mask = ((uint32)1<<SLOT->eg_sh_d1r)-1;
	SLOT->eg_sh_d2r_mask = ((uint32)1<<SLOT->eg_sh_d2r)-1;
	SLOT->eg_sh_rr_mask = ((uint32)1<<SLOT->eg_sh_rr)-1;
}

/* update phase increment counters */
INLINE void refresh_fc_eg_chan(FM_CH *CH )
{
	if( CH->SLOT[SLOT1].Incr==-1){
		long fc = CH->fc;
		long kc = CH->kcode;

		refresh_fc_eg_slot(&CH->SLOT[SLOT1] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT2] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT3] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT4] , fc , kc );
	}
}

void RefreshFm ( void )
{
	int i;

	
	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		if (sn.Volume[i] == 0)
		{
			/* note that I do count += length, NOT count = length + 1. You might think */
			/* it's the same since the volume is 0, but doing the latter could cause */
			/* interferencies when the program is rapidly modulating the volume. */
			if (sn.Count[i] <= ((int32)OUT_BUFFER_SIZE<<16))
				sn.Count[i] += ((int32)OUT_BUFFER_SIZE<<16);
		}
	}

	/* refresh PG and EG */
	refresh_fc_eg_chan( &CH[0] );
	refresh_fc_eg_chan( &CH[1] );
	if( (ST.mode & 0xc0) )
	{
		/* 3SLOT MODE */
		if( CH[2].SLOT[SLOT1].Incr==-1)
		{
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT1] , SL3.fc[1] , SL3.kcode[1] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT2] , SL3.fc[2] , SL3.kcode[2] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT3] , SL3.fc[0] , SL3.kcode[0] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT4] , CH[2].fc , CH[2].kcode );
		}
	}else refresh_fc_eg_chan( &CH[2] );
	refresh_fc_eg_chan( &CH[3] );
	refresh_fc_eg_chan( &CH[4] );
	refresh_fc_eg_chan( &CH[5] );


}

#else
#include <string.h>
#include <math.h>
#include "../dspshared.h"
#include "pico_dsp.h"
#include "fm.h"
#include "fmint.h"
#include "fmtables.h" 
#include "sn76496.h"

/*UINT8 addr_A1*/ /*__attribute__ ((aligned (4)))*/;			/* address line A1		*/



/* OPN Mode Register Write */
INLINE void set_timers(int v )
{
	/* b7 = CSM MODE */
	/* b6 = 3 slot mode */
	/* b5 = reset b */
	/* b4 = reset a */
	/* b3 = timer enable b */
	/* b2 = timer enable a */
	/* b1 = load b */
	/* b0 = load a */
	ST.mode = v;

	/* reset Timer b flag */
	if( v & 0x20 )
	{
		/* reset status flag */
		ST.status &=~0x02;
		dspCom->YM2612Status=ST.status;
		if (!(ST.status & ST.irqmask) )
			ST.irq = 0;
	}
	/* reset Timer a flag */
	if( v & 0x10 )
	{
		/* reset status flag */
		ST.status &=~0x01;
		dspCom->YM2612Status=ST.status;
		if (!(ST.status & ST.irqmask) )
			ST.irq = 0;
	}
	/* load b */
	if( v & 0x02 )
	{
		if( ST.TBC == 0 )
			ST.TBC = ( 256-ST.TB)<<4;
	}
	else
		ST.TBC = 0;
	/* load a */
	if( v & 0x01 )
	{
		if( ST.TAC == 0 )
			ST.TAC = (1024-ST.TA);
	}
	else
			ST.TAC = 0;
}

INLINE void FM_KEYON(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( !SLOT->key )
	{
		SLOT->key = 1;
		SLOT->phase = 0;		/* restart Phase Generator */
		SLOT->state = EG_ATT;	/* phase -> Attack */
		SLOT->eg_sh_active_mask = SLOT->eg_sh_ar_mask;
	}
}

INLINE void FM_KEYOFF(FM_CH *CH , int s )
{
	FM_SLOT *SLOT = &CH->SLOT[s];
	if( SLOT->key )
	{
		SLOT->key = 0;
		if (SLOT->state>EG_REL)
		{
			SLOT->state = EG_REL;/* phase -> Release */
			SLOT->eg_sh_active_mask = SLOT->eg_sh_rr_mask;
		}
	}
}

INLINE void CSMKeyControll(FM_CH *CH)
{
	/* this is wrong, atm */

	/* all key on */
	FM_KEYON(CH,SLOT1);
	FM_KEYON(CH,SLOT2);
	FM_KEYON(CH,SLOT3);
	FM_KEYON(CH,SLOT4);
}

/* set detune & multiple */
INLINE void set_det_mul(FM_CH *CH,FM_SLOT *SLOT,int v)
{
	SLOT->mul = (v&0x0f)? (v&0x0f)*2 : 1;
	SLOT->DT  = (INT32 *)YMOPN_ST_dt_tab[(v>>4)&7];
	CH->SLOT[SLOT1].Incr=-1;
}

/* set total level */
INLINE void set_tl(FM_SLOT *SLOT , int v)
{
	SLOT->tl = (v&0x7f)<<(ENV_BITS-7); /* 7bit TL */
}

/* set attack rate & key scale  */
INLINE void set_ar_ksr(FM_CH *CH,FM_SLOT *SLOT,int v)
{
	UINT8 old_KSR = SLOT->KSR;

	SLOT->ar = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->KSR = 3-(v>>6);
	if (SLOT->KSR != old_KSR)
	{
		CH->SLOT[SLOT1].Incr=-1;
	}
	else
	{
		/* refresh Attack rate */
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar  = 0;
			SLOT->eg_sel_ar = 17*RATE_STEPS;
		}
	}
}

/* set decay rate */
INLINE void set_dr(FM_SLOT *SLOT,int v)
{
	SLOT->d1r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
	SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + SLOT->ksr];

}

/* set sustain rate */
INLINE void set_sr(FM_SLOT *SLOT,int v)
{
	SLOT->d2r = (v&0x1f) ? 32 + ((v&0x1f)<<1) : 0;

	SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
	SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + SLOT->ksr];
}

/* set release rate */
INLINE void set_sl_rr(FM_SLOT *SLOT,int v)
{
	SLOT->sl = sl_table[ v>>4 ];

	SLOT->rr  = 34 + ((v&0x0f)<<2);

	SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
	SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
}

/* write a OPN mode register 0x20-0x2f */
static void OPNWriteMode(int r, int v)
{
	UINT8 c;
	FM_CH *pCH;
	int TAnew;

	switch(r){
	case 0x21:	/* Test */
		break;
	case 0x22:	/* LFO FREQ (YM2608/YM2610/YM2610B/YM2612) */
		if (v&0x08) /* LFO enabled ? */
			OPN.lfo_inc = OPN_lfo_freq[v&7];
		else
			OPN.lfo_inc = 0;
		break;
	case 0x24:	/* timer A High 8*/
		//ST.TA = (ST.TA & 0x03)|(((int)v)<<2);
		//from notaz
		TAnew = (ST.TA & 0x03)|(((int)v)<<2);
		if(ST.TA != TAnew) {
			// we should reset ticker only if new value is written. Outrun requires this.
			ST.TA = TAnew;
			ST.TAC = (1024-TAnew)*18;
			ST.TAC_Step = 0;
		}
		break;
	case 0x25:	/* timer A Low 2*/
		//ST.TA = (ST.TA & 0x3fc)|(v&3);
		TAnew = (ST.TA & 0x3fc)|(v&3);
		if(ST.TA != TAnew) {
			ST.TA = TAnew;
			ST.TAC = (1024-TAnew)*18;
			ST.TAC_Step = 0;
		}
		break;
	case 0x26:	/* timer B */
		//ST.TB = v;
		//from notaz :
		if(ST.TB != v) {
			ST.TB = v;
			ST.TBC  = (256-v)<<4;
			ST.TBC *= 18;
			ST.TBC_Step  = 0;
		}
		break;
	case 0x27:	/* mode, timer control */
		set_timers( v );
		break;
	case 0x28:	/* key on / off */
		c = v & 0x03;
		if( c == 3 ) break;
		if(v&0x04) c+=3;
		pCH = &CH[c];
		if(v&0x10) FM_KEYON(pCH,SLOT1); else FM_KEYOFF(pCH,SLOT1);
		if(v&0x20) FM_KEYON(pCH,SLOT2); else FM_KEYOFF(pCH,SLOT2);
		if(v&0x40) FM_KEYON(pCH,SLOT3); else FM_KEYOFF(pCH,SLOT3);
		if(v&0x80) FM_KEYON(pCH,SLOT4); else FM_KEYOFF(pCH,SLOT4);
		break;
	}
}

/* write a OPN register (0x30-0xff) */
static void OPNWriteReg(int r, int v)
{
	FM_CH *pCH;
	FM_SLOT *SLOT;

	UINT8 c = r&3;

	if (c == 3) return; /* 0xX3,0xX7,0xXB,0xXF */

	if (r >= 0x100) c+=3;

	pCH = &CH[c];

	SLOT = &(pCH->SLOT[OPN_SLOT(r)]);

	switch( r & 0xf0 ) {
	case 0x30:	/* DET , MUL */
		set_det_mul(pCH,SLOT,v);
		break;

	case 0x40:	/* TL */
		set_tl(SLOT,v);
		break;

	case 0x50:	/* KS, AR */
		set_ar_ksr(pCH,SLOT,v);
		break;

	case 0x60:	/* bit7 = AM ENABLE, DR */
		set_dr(SLOT,v);

		SLOT->AMmask = (v&0x80) ? ~0 : 0;
		break;

	case 0x70:	/*     SR */
		set_sr(SLOT,v);
		break;

	case 0x80:	/* SL, RR */
		set_sl_rr(SLOT,v);
		break;

	case 0x90:	/* SSG-EG */

		SLOT->ssg  =  v&0x0f;
		SLOT->ssgn = (v&0x04)>>1; /* bit 1 in ssgn = attack */
		break;

	case 0xa0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xa0-0xa2 : FNUM1 */
			{
				UINT32 fn = (((UINT32)( (ST.fn_h)&7))<<8) + v;
				UINT8 blk = ST.fn_h>>3;
				/* keyscale code */
				pCH->kcode = (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				pCH->fc = OPN_fn_table[fn*2]>>(7-blk);

				/* store fnum in clear form for LFO PM calculations */
				pCH->block_fnum = (blk<<11) | fn;

				pCH->SLOT[SLOT1].Incr=-1;
			}
			break;
		case 1:		/* 0xa4-0xa6 : FNUM2,BLK */
			ST.fn_h = v&0x3f;
			break;
		case 2:		/* 0xa8-0xaa : 3CH FNUM1 */
			if(r < 0x100)
			{
				UINT32 fn = (((UINT32)(SL3.fn_h&7))<<8) + v;
				UINT8 blk = SL3.fn_h>>3;
				/* keyscale code */
				SL3.kcode[c]= (blk<<2) | opn_fktable[fn >> 7];
				/* phase increment counter */
				SL3.fc[c] = OPN_fn_table[fn*2]>>(7-blk);
				SL3.block_fnum[c] = fn;
				pCH[2].SLOT[SLOT1].Incr=-1;
			}
			break;
		case 3:		/* 0xac-0xae : 3CH FNUM2,BLK */
			if(r < 0x100)
				SL3.fn_h = v&0x3f;
			break;
		}
		break;

	case 0xb0:
		switch( OPN_SLOT(r) ){
		case 0:		/* 0xb0-0xb2 : FB,ALGO */
			{
				int feedback = (v>>3)&7;
				pCH->ALGO = v&7;
				pCH->FB   = feedback ? feedback+6 : 0;
			}
			break;
		case 1:		/* 0xb4-0xb6 : L , R , AMS , PMS (YM2612/YM2610B/YM2610/YM2608) */
			/* b0-2 PMS */
			pCH->pms = (v & 7) << 5; /* CH->pms = PM depth * 32 (index in lfo_pm_table) */

			/* b4-5 AMS */
			pCH->ams = lfo_ams_depth_shift[(v>>4) & 0x03];
			/* PAN :  b7 = L, b6 = R */
			OPN_pan[ c*2   ] = (v & 0x80);
			OPN_pan[ c*2+1 ] = (v & 0x40);
			break;
		}
		break;
	}
}

static void reset_channels(FM_CH *CH, int num)
{
	int c,s;

	ST.mode   = 0;	/* normal mode */
	ST.TA     = 0;
	ST.TAC    = 0;
	ST.TB     = 0;
	ST.TBC    = 0;

	for( c = 0 ; c < num ; c++ )
	{
		CH[c].fc = 0;
		for(s = 0 ; s < 4 ; s++ )
		{
			CH[c].SLOT[s].state= EG_OFF;
			CH[c].SLOT[s].volume = MAX_ATT_INDEX;
		}
	}
}

/* initialize YM2612 emulator(s) */
int YM2612Init(UINT32 clock, UINT32 rate)
{
	YM2612ResetChip(clock, rate);
	return 0;
}

void YM2612ResetChip(UINT32 clock, UINT32 rate)
{
	int i,d;
	int c,s;
	double freqbase;

	//ROB'S defaults
	/*OPN.eg_timer_add = 211096;
	OPN.eg_timer_overflow = 196608;
	ST.TAC_Step = 13193;
	ST.TBC_Step = 1820710;*/

	freqbase = ( (double) clock / rate / 144 );

	OPN.eg_timer_add  = ((UINT32)1<<EG_SH)  *  freqbase;
	OPN.eg_timer_overflow = ( 3 ) * ((UINT32)1<<EG_SH); 

	//ST.TAC_Step = freqbase * 4096;
	//ST.TBC_Step = freqbase * 4096 * SAMPLECOUNT;
	ST.TAC_Step = 0;
	ST.TBC_Step = 0;

	//init time tables
	for (d = 0;d <= 3;d++){
		for (i = 0;i <= 31;i++){
			//rate = ((double)dttable[d*32 + i]) * SIN_LEN  * ST->freqbase  * ((INT32)1<<FREQ_SH) / ((double)((int32)1<<20));
			YMOPN_ST_dt_tab[d][i]   = (INT32) ((double)dt_tab[d*32 + i]) * SIN_LEN  * freqbase  * ((INT32)1<<FREQ_SH) / ((double)((uint32)1<<20));;
			YMOPN_ST_dt_tab[d+4][i] = -YMOPN_ST_dt_tab[d][i];
		}
	}

	/* there are 2048 FNUMs that can be generated using FNUM/BLK registers
        but LFO works with one more bit of a precision so we really need 4096 elements */
	/* calculate fnumber -> increment counter table */
	for(i = 0; i < 4096; i++)
	{
		/* freq table for octave 7 */
		/* OPN phase increment counter = 20bit */
		OPN_fn_table[i] = (UINT32)( (double)i * 32 * freqbase * (1<<(FREQ_SH-10)) ); /* -10 because chip works with 10.10 fixed point, while we use 16.16 */
	}

	/* LFO freq. table */
	for(i = 0; i < 8; i++)
	{
		/* Amplitude modulation: 64 output levels (triangle waveform); 1 level lasts for one of "lfo_samples_per_step" samples */
		/* Phase modulation: one entry from lfo_pm_output lasts for one of 4 * "lfo_samples_per_step" samples  */
		OPN_lfo_freq[i] = (1.0 / lfo_samples_per_step[i]) * ((long)1<<LFO_SH) * freqbase;
	}

	/* status clear */
	ST.irqmask = 0x03;
	/* reset status flag */
	ST.status = 0;
	dspCom->YM2612Status=ST.status;

	ST.irq = 0;

	OPNWriteMode(0x27,0x30); /* mode 0 , timer reset */

	OPN.eg_timer = 0;
	OPN.eg_cnt   = 0;

	ST.mode   = 0;	/* normal mode */
	ST.TA     = 0;
	ST.TAC    = 0;
	ST.TB     = 0;
	ST.TBC    = 0;

	for( c = 0 ; c < 6 ; c++ )
	{
		CH[c].fc = 0;
		for(s = 0 ; s < 4 ; s++ )
		{
			CH[c].SLOT[s].ssg = 0;
			CH[c].SLOT[s].ssgn = 0;
			CH[c].SLOT[s].state= EG_OFF;
			CH[c].SLOT[s].eg_sh_active_mask = 0;
			CH[c].SLOT[s].volume = MAX_ATT_INDEX;
			CH[c].SLOT[s].vol_out= MAX_ATT_INDEX;
		}
	}

	for(i = 0xb6 ; i >= 0xb4 ; i-- )
	{
		OPNWriteReg(i      ,0xc0);
		OPNWriteReg(i|0x100,0xc0);
	}
	for(i = 0xb2 ; i >= 0x30 ; i-- )
	{
		OPNWriteReg(i      ,0);
		OPNWriteReg(i|0x100,0);
	}
	for(i = 0x26 ; i >= 0x20 ; i-- ) OPNWriteReg(i,0);
	/* DAC mode clear */
	dacen = 0;
}

/* YM2612 write */
/* n = number  */
/* a = address */
/* v = value   */
void fm_write(uint16 a, uint16 v)
{
	int addr;

	v &= 0xff;	/* adjust to 8 bit bus */

	switch( a&3){
	case 0:	/* address port 0 */
		ST.address = v;
		dspCom->addr_A1 = 0;
		break;

	case 1:	/* data port 0    */
		if (dspCom->addr_A1 != 0)
			break;	/* verified on real YM2608 */

		addr = ST.address;
		dspCom->ym2612_regs[addr] = v;
		//addr%2? dspCom->ym2612_regs[addr>>1] = (dspCom->ym2612_regs[addr>>1]&0x00ff)+(v<<8) : (dspCom->ym2612_regs[addr>>1]&0xff00)+v;
		switch( addr & 0xf0 )
		{
		case 0x20:	/* 0x20-0x2f Mode */
			switch( addr )
			{
			case 0x2a:	/* DAC data (YM2612) */
				ym2612_dacout = ((int)v - 0x80) << 6;	/* level unknown */
				break;
			case 0x2b:	/* DAC Sel  (YM2612) */
				/* b7 = dac enable */
				dacen = v & 0x80;
				break;
			default:	/* OPN section */
				/* write register */
				OPNWriteMode(addr,v);
			}
			break;
		default:	/* 0x30-0xff OPN section */
			/* write register */
			OPNWriteReg(addr,v);
		}
		break;

	case 2:	/* address port 1 */
		ST.address = v;
		dspCom->addr_A1 = 1;
		break;

	case 3:	/* data port 1    */
		if (dspCom->addr_A1 != 1)
			break;	/* verified on real YM2608 */

		addr = ST.address;
		dspCom->ym2612_regs[addr|0x100] = v;
		//addr%2? dspCom->ym2612_regs[(addr>>1)|0x80] = (dspCom->ym2612_regs[(addr>>1)|0x80]&0x00ff)+(v<<8) : (dspCom->ym2612_regs[(addr>>1)|0x80]&0xff00)+v;
		OPNWriteReg(addr | 0x100,v);
		break;
	}
}

int fm_read(int address)
{
	return ST.status;
}

/* update phase increment and envelope generator */
INLINE void refresh_fc_eg_slot(FM_SLOT *SLOT , long fc , int kc )
{
	int ksr;

	/* (frequency) phase increment counter */
	SLOT->Incr = ((fc+SLOT->DT[kc])*SLOT->mul) >> 1;

	ksr = kc >> SLOT->KSR;
	if( SLOT->ksr != ksr )
	{
		SLOT->ksr = ksr;

		/* calculate envelope generator rates */
		if ((SLOT->ar + SLOT->ksr) < 32+62)
		{
			SLOT->eg_sh_ar  = eg_rate_shift [SLOT->ar  + SLOT->ksr ];
			SLOT->eg_sel_ar = eg_rate_select[SLOT->ar  + SLOT->ksr ];
		}
		else
		{
			SLOT->eg_sh_ar  = 0;
			SLOT->eg_sel_ar = 17*RATE_STEPS;
		}

		SLOT->eg_sh_d1r = eg_rate_shift [SLOT->d1r + SLOT->ksr];
		SLOT->eg_sel_d1r= eg_rate_select[SLOT->d1r + SLOT->ksr];

		SLOT->eg_sh_d2r = eg_rate_shift [SLOT->d2r + SLOT->ksr];
		SLOT->eg_sel_d2r= eg_rate_select[SLOT->d2r + SLOT->ksr];

		SLOT->eg_sh_rr  = eg_rate_shift [SLOT->rr  + SLOT->ksr];
		SLOT->eg_sel_rr = eg_rate_select[SLOT->rr  + SLOT->ksr];
	}

	SLOT->eg_sh_ar_mask = (1<<SLOT->eg_sh_ar)-1;
	SLOT->eg_sh_d1r_mask = (1<<SLOT->eg_sh_d1r)-1;
	SLOT->eg_sh_d2r_mask = (1<<SLOT->eg_sh_d2r)-1;
	SLOT->eg_sh_rr_mask = (1<<SLOT->eg_sh_rr)-1;
}

/* update phase increment counters */
INLINE void refresh_fc_eg_chan(FM_CH *CH )
{
	if( CH->SLOT[SLOT1].Incr==-1){
		long fc = CH->fc;
		int kc = CH->kcode;
		refresh_fc_eg_slot(&CH->SLOT[SLOT1] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT2] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT3] , fc , kc );
		refresh_fc_eg_slot(&CH->SLOT[SLOT4] , fc , kc );
	}
}

void RefreshFm ( void )
{
	int i;

	/* If the volume is 0, increase the counter */
	for (i = 0;i < 4;i++)
	{
		if (sn.Volume[i] == 0)
		{
			/* note that I do count += length, NOT count = length + 1. You might think */
			/* it's the same since the volume is 0, but doing the latter could cause */
			/* interferencies when the program is rapidly modulating the volume. */
			if (sn.Count[i] <= ((int32)OUT_BUFFER_SIZE<<16))
				sn.Count[i] += ((int32)OUT_BUFFER_SIZE<<16);
		}
	}

	/* refresh PG and EG */
	refresh_fc_eg_chan( &CH[0] );
	refresh_fc_eg_chan( &CH[1] );
	if( (ST.mode & 0xc0) )
	{
		/* 3SLOT MODE */
		if( CH[2].SLOT[SLOT1].Incr==-1)
		{
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT1] , SL3.fc[1] , SL3.kcode[1] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT2] , SL3.fc[2] , SL3.kcode[2] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT3] , SL3.fc[0] , SL3.kcode[0] );
			refresh_fc_eg_slot(&CH[2].SLOT[SLOT4] , CH[2].fc , CH[2].kcode );
		}
	}else refresh_fc_eg_chan( &CH[2] );
	refresh_fc_eg_chan( &CH[3] );
	refresh_fc_eg_chan( &CH[4] );
	refresh_fc_eg_chan( &CH[5] );
}

void dac_update(int raster)
{
  static int dac_cnt;

	short *dac_out;

	int i;

	//int length=(((267 * (raster + 1)) / 262) - ((267 * raster) / 262));
	//int length=(((samples_per_frame * (raster + 1)) / rasters_total) - ((samples_per_frame * raster) / rasters_total));
	int length=dac_extrapolation[raster];

	for ( i=0 ; i<length ; i++ )
	{
		if ( dacen )
			ym2612_dacout_queue[ym2612_dacout_head]=ym2612_dacout;
		else
			ym2612_dacout_queue[ym2612_dacout_head]=(int16)0;

		ym2612_dacout_head=(ym2612_dacout_head+1)&DACOUTQUEUE_MASK;
	}

}


void YM2612PicoTick(int raster)
{
	int n=dac_extrapolation[raster];

	#if 1
	// timer A
	if(ST.mode & 0x01 && (ST.TAC_Step+=64*n) >= ST.TAC) {
		ST.TAC_Step -= ST.TAC;
		if(ST.mode & 0x04) ST.status |= 1;
		// CSM mode total level latch and auto key on
		if(ST.mode & 0x80) CSMKeyControll( &(CH[2]) ); // ???
		
	}

	// timer B
	if(ST.mode & 0x02 && (ST.TBC_Step+=64*n) >= ST.TBC) {
		ST.TBC_Step -= ST.TBC;
		if(ST.mode & 0x08) ST.status |= 2;
	}
	#endif

	dspCom->YM2612Status=ST.status;

}

void YM2612PicoStateLoad(void)
{
    int i, old_A1 = dspCom->addr_A1;

	reset_channels( &CH[0], 6 );

	// feed all the registers and update internal state
	/*for(i = 0; i < 0x100; i++) {
		fm_write(0, i);
		fm_write(1, i%2?dspCom->ym2612_regs[i>>1]>>8:dspCom->ym2612_regs[i>>1]&0xff);
	}
	for(i = 0; i < 0x100; i++) {
		fm_write(2, i);
		fm_write(3, i%2?dspCom->ym2612_regs[(i>>1)|0x80]>>8:dspCom->ym2612_regs[(i>>1)|0x80]&0xff);
	}*/
	for(i = 0; i < 0x100; i++) {
		fm_write(0, i);
		fm_write(1, dspCom->ym2612_regs[i]);
	}
	for(i = 0; i < 0x100; i++) {
		fm_write(2, i);
		fm_write(3, dspCom->ym2612_regs[i|0x100]);
	}

	dspCom->addr_A1 = old_A1;
}

#endif