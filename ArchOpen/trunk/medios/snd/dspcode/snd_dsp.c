#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <csl.h>
#include <csl_mcbsp.h>
#include <csl_irq.h>
#include <csl_dma.h>

#include <libdsp.h>

#include "../dspshared.h"

tDspCom dspComBuffer;
tDspCom * dspCom;

MCBSP_Handle aic23_port=INV;

DMA_Handle aic23_dma=INV;

#pragma DATA_SECTION(processBuf , ".saram_d")
unsigned short processBuf[OUTPUT_CHUNK_SIZE/2];

unsigned short outBufData[OUTPUT_CHUNK_SIZE];
unsigned short outBuf2Data[OUTPUT_CHUNK_SIZE];

unsigned short * outBuf = outBufData;
unsigned short * outBuf2 = outBuf2Data;

unsigned short outBufSize = 0;
unsigned short outBuf2Size = 0;

short dmaActive=0;

void debug(const char* msg);
void dmaEnd(void);
void startDma();

void main(){
	Uint16 dmaEventID=0;

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
	aic23_port=aic23_openPort();
	if (aic23_port==INV) debug("Data Port NOK");

	// open dma channel
 	aic23_dma = DMA_open(DMA_CHA3, DMA_OPEN_RESET);    
    dmaEventID = DMA_getEventId(aic23_dma);      
    DMA_FSET(DMPREC,INTOSEL,DMA_DMPREC_INTOSEL_CH2_CH3);

 	// setup dma irq
    IRQ_globalDisable();
    IRQ_plug(dmaEventID,&dmaEnd);
	IRQ_clear(dmaEventID);
    IRQ_enable(dmaEventID);
    IRQ_globalEnable();

	debug("DSP START");

	// simulate DMA end if DMA not active
	for(;;){
		if(!dmaActive) { debug("simdma"); dmaEnd(); }
	}
}

interrupt void dmaEnd(){
	unsigned short * tmpBuf;
	unsigned short tmpSize;
	unsigned short dmaSize;
	short hasProcess;



	// swap buffers

	tmpBuf=outBuf;
	outBuf=outBuf2;
	outBuf2=tmpBuf;

	tmpSize=outBufSize;
	outBufSize=outBuf2Size;
	outBuf2Size=tmpSize;
    
    //debug("DSPdmaEnd");
    
    
	if(outBufSize>0){
	//debug("=>stDma");
		startDma();
	}

	// request to render a buffer

    dspCom->decodeDone=0;
	dspCom->decodeRequest=1;
    armInt_trigger();

	while(dspCom->decodeRequest) /* wait for arm to finish rendering*/;

	if(dspCom->decodeDone){

		// handle buffer size changes
        //debug("DSPdone");
		outBuf2Size=dspCom->bufSize/2;

		if(!dspCom->stereo /*|| dspCom->dataSampleRate!=dspCom->outputSampleRate*/){ //TODO: sample rate conversion

			hasProcess=1;

			// output buffer will have to be twice as big as input buffer
			if(!dspCom->stereo){
				outBuf2Size*=2;
			}
		}else{
			hasProcess=0;
		}

		//get data from sdram
		
		dmaSize=(outBuf2Size*2+3)&(~3);

		if(!hasProcess){
			// get data from sdram buffer
			hpiDma_start(dspCom->bufAddr,outBuf2,dmaSize,HPI_DIR_SD2DSP);
			while(hpiDma_pending());
            //debug("Done1");
		}else{
            
			// get data from sdram buffer
			hpiDma_start(dspCom->bufAddr,processBuf,dmaSize,HPI_DIR_SD2DSP);
			while(hpiDma_pending());
            //debug("Done2");
			// process data

			// process stereo
			if(!dspCom->stereo){
				unsigned short * in, * out;
				in=processBuf;
				out=outBuf2;
				while(out<outBuf2+outBuf2Size){
					*(out++)=*in;
					*(out++)=*(in++);
				}
			}
		}

	}else{
        //debug("DSPfill0");
		outBuf2Size=OUTPUT_CHUNK_SIZE;

		// fill buffer with zeroes
		memset(outBuf2,0,outBuf2Size);
	}
}


void startDma(){
    unsigned short buffer;

	dmaActive=1;

	buffer=(unsigned short)outBuf;
	    
    /* Write configuration structure values to DMA control regs */ 
    DMA_configArgs(
	   aic23_dma,    
	   1 ,                                  /* Priority */
	   DMA_DMMCR_RMK(
			 DMA_DMMCR_AUTOINIT_OFF,
			 DMA_DMMCR_DINM_ON,
			 DMA_DMMCR_IMOD_FULL_ONLY,
			 0, /* not ABU */
			 DMA_DMMCR_SLAXS_OFF,
			 DMA_DMMCR_SIND_POSTINC,
			 DMA_DMMCR_DMS_DATA,
			 DMA_DMMCR_DLAXS_OFF,
			 DMA_DMMCR_DIND_NOMOD,
			 DMA_DMMCR_DMD_DATA
			 ),                             /* DMMCR */
	   DMA_DMSFC_RMK(
			 DMA_DMSFC_DSYN_XEVT0,
			 DMA_DMSFC_DBLW_ON,
			 DMA_DMSFC_FRAMECNT_OF(0)
			 ),                             /* DMSFC */ 
	   (DMA_AdrPtr)(buffer+2),              /* DMSRC */
	   (DMA_AdrPtr)MCBSP_ADDR(DXR20),       /* DMDST */
	   (Uint16)(outBufSize-2)/2-1      /* DMCTR = buffsize */
	);

	//debug("waitRdy");
    while(!MCBSP_xrdy(aic23_port));
    MCBSP_write32(aic23_port,outBuf[1]|outBuf[0]*0x10000);
	//debug("aicGO");
    /* Start DMA transfer */
    DMA_start(aic23_dma);
}

void debug(const char* msg){
	if (!dspCom) return;

	strcpy((char*)dspCom->dbgMsg,msg);
	dspCom->hasDbgMsg=1;

    armInt_trigger();

	while(dspCom->hasDbgMsg);
}
