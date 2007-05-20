#include <stdio.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>

#include "../dspshared.h"
#include "pico_dsp.h"
#include "libdsp.h"
#include "sn76496.h"
#include "fm.h"
#include "fmint.h"

#define BUFFER_SIZE SAMPLECOUNT

tDspCom dspComBuffer;
tDspCom * dspCom;

short buf1[BUFFER_SIZE*2];
static short buf2[BUFFER_SIZE];
static short* buffer[2]={buf1,buf2};

short ym2612_dacout_queue[DACOUTQUEUE_LENGTH];
short ym2612_dacout_tail;
short ym2612_dacout_head;

unsigned long  rasters_total, samples_per_frame;
int dac_extrapolation[312];

void debug(const char* msg);

int n=0;

char s[50];


void main(){
    MCBSP_Handle dataPort;
    int i,c1,c2,c3,dac_samples=0;
	//char s[50];

    *DSP_COM=0;

    // dsp<>arm comm
    dspCom = &dspComBuffer;
    memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

    // wait for ARM to finish init
    while(!dspCom->armInitFinished);
       
    // libs init
    CSL_init();
    libDsp_init(dspCom->chipNum);

    // aic23
    dataPort=aic23_openPort();
    if (dataPort==INV) debug("Data Port NOK");

    PsndReset();
    
    debug("DSP START");

    for(;;){

		while(dspCom->armBusy);

		if(dspCom->sndWantReset)
		{
			PsndReset();
			dspCom->sndWantReset=0;
		}

		if(dspCom->picoStateLoad)
		{
			YM2612PicoStateLoad();
			dspCom->picoStateLoad=0;
		}

		buf1[0]=buf1[1]=0;
		c1=c2=c3=0;
		
		if ( dspCom->DACEnabled )
		{
			/*while(dspCom->DACTail!=dspCom->DACHead){
            	dac_next_shrink_stereo(dspCom->DACQueue[dspCom->DACTail]);
				dspCom->DACTail=(dspCom->DACTail+1)&SNDQUEUE_MASK;	
				c3++;			    		
			}  */

			/*while(dspCom->YM2612PicoTick>0)
			{
				//YM2612PicoTick();
				if (dspCom->YM2612PicoTick>1)
					dspCom->YM2612PicoTick-=2;
				else
					dspCom->YM2612PicoTick--;
			}*/
		}

		//while(!dspCom->Flush);


		/*while(!dspCom->Flush)
		{*/
			while(dspCom->YM2612Tail!=dspCom->YM2612Head){
		  		switch(dspCom->YM2612Queue[dspCom->YM2612Tail].type)
		  		{
					case 0 : 
						fm_write(dspCom->YM2612Queue[dspCom->YM2612Tail].address&3,dspCom->YM2612Queue[dspCom->YM2612Tail].value);
						//YM2612Write(dspCom->YM2612Queue[dspCom->YM2612Tail].address&3,dspCom->YM2612Queue[dspCom->YM2612Tail].value);
						break;
					case 1 : 
						if ( dspCom->DACEnabled )
						{
							dac_update(dspCom->YM2612Queue[dspCom->YM2612Tail].value);
						}
						YM2612PicoTick(dspCom->YM2612Queue[dspCom->YM2612Tail].value);
						break;
		  		}
		  		dspCom->YM2612Tail=(dspCom->YM2612Tail+1)&SNDQUEUE_MASK;		    		
      		}

			while(dspCom->PSGTail!=dspCom->PSGHead){
            	SN76496Write(dspCom->PSGQueue[dspCom->PSGTail]);
				dspCom->PSGTail=(dspCom->PSGTail+1)&SNDQUEUE_MASK;				    		
			}
      	/*}  */
		//dspCom->YM2612Status=YM2612Read(0);
		//dspCom->YM2612Status=fm_read(0);
		

		RefreshFm();



		//if ( dspCom->YM2612Enabled ) YM2612UpdateOne(buf1, BUFFER_SIZE);
		/*if ( dspCom->YM2612Enabled )*/ YM2612UpdateOne(buf1);
		//if ( dspCom->PSGEnabled ) SN76496Update(buf1,BUFFER_SIZE,1);

		//PsndRender((INT16 *)buffer[0]);

		//dspCom->Flush=0;
		
		/*if(n%16000==0) 
		{
			//sprintf(s, " %d - %d", ym2612_dacout_head, ym2612_dacout_tail);
			debug("here");
		}*/

		
        for(i=0;i<BUFFER_SIZE*2;i+=2){
			while(!MCBSP_xrdy(dataPort));
            MCBSP_write32(dataPort,(long)buf1[i]*0x10000+buf1[i+1]);
        }
	
		//dspCom->dacen=0;
		/*n++;
		if ( n>16000 ) n=0;*/
        //while(!(dspCom->psgEnabled));
    };
}

void debug(const char* msg){
    if (!dspCom) return;

    strcpy((char*)dspCom->dbgMsg,msg);
    dspCom->hasDbgMsg=1;

    armInt_trigger();

    while(dspCom->hasDbgMsg);
}

void PsndReset()
{
	int i;

	SN76496_init(dspCom->hardwarePal?OSC_PAL/15:OSC_NTSC/15,SAMPLE_RATE); // 3.58Mhz
	YM2612Init(dspCom->hardwarePal?OSC_PAL/7:OSC_NTSC/7,SAMPLE_RATE);
	//YM2612Init(dspCom->hardwarePal?OSC_PAL/7:OSC_NTSC/7,SAMPLE_RATE, NULL, NULL);
	//YM2612ResetChip();

	dspCom->hardwarePal?debug("sndReset - PAL"):debug("sndReset - NTSC");
	dspCom->YM2612Tail=0;
	dspCom->YM2612Head=0;
	dspCom->PSGTail=0;
	dspCom->PSGHead=0;
	ym2612_dacout_tail=0;
	ym2612_dacout_head=0;
	rasters_total=dspCom->hardwarePal?312:262;
	samples_per_frame=dspCom->hardwarePal?SAMPLE_RATE/50:SAMPLE_RATE/60;

	for ( i=0 ; i<rasters_total ; i++ )
	{
		dac_extrapolation[i]=(((samples_per_frame * (i + 1)) / rasters_total) - ((samples_per_frame * i) / rasters_total));
	}

	for ( i=0 ; i<0x200 ; i++ )
	{
		dspCom->ym2612_regs[i]=0;
	}
	dspCom->addr_A1=0;

	dspCom->sndWantReset=0;
}

unsigned short lfo_pm_table(unsigned short i)
{
	unsigned long value;
	unsigned short vs;
	
	
	//debug("hpiDma start");

	// get data from sdram buffer
	/*hpiDma_start(dspCom->lfo_pm_table_addr+(i&0xfffc),&value,4,HPI_DIR_SD2DSP);
	while(hpiDma_pending());
	//debug("hpiDma end");
	

	if(!(i&0x2))
		vs=(unsigned short)(value>>16);
	else
		vs=(unsigned short)(value&0xffff);
*/
	/*if(n%200==0)
	{
		sprintf(s, "i:%x %x %x %x", i, value, vs, (i&0x1)?vs&0xff:vs>>8);
		debug(s);
	}*/
	
	/*if(!(i&0x1))
		return (vs>>8)&0xff;
	else
		return (vs&0xff);*/
		return 0;
}