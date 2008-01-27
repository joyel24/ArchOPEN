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

/* we are using a buffer 2x bigger tan the data send by arm
in order to work fine with MAS chip for which 16bit need to be expended
to 32bit
on AIC, buffer will be too big, and used only by half*/

#pragma DATA_SECTION(processBuf , ".saram_d")
unsigned short processBuf[OUTPUT_CHUNK_SIZE];

unsigned short outBufData[OUTPUT_CHUNK_SIZE*2];
unsigned short outBuf2Data[OUTPUT_CHUNK_SIZE*2];

unsigned short * outBuf = outBufData;
unsigned short * outBuf2 = outBuf2Data;

unsigned short outBufSize = 0;
unsigned short outBuf2Size = 0;

short dmaActive=0;

void debug(const char* msg);
void sendDbgData(unsigned short data);
void dmaEnd(void);
void startDma();

void main()
{
	Uint16 dmaEventID=0;

    *DSP_COM=0;
    aic23_port=INV;
    
    // dsp<>arm comm
    dspCom = &dspComBuffer;
	memset((void *)dspCom,0,sizeof(dspCom));
    *DSP_COM = (unsigned short) dspCom;

	// wait for ARM to finish init
	while(!dspCom->armInitFinished) /*nothing*/;
    
	// libs init
	CSL_init();
	libDsp_init(dspCom->chipNum);
    
    if(chip_num==25)
        ibDma_reset();

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
    unsigned short * in, * out;
    
	// swap buffers
	tmpBuf=outBuf;
	outBuf=outBuf2;
	outBuf2=tmpBuf;
	tmpSize=outBufSize;
	outBufSize=outBuf2Size;
	outBuf2Size=tmpSize;    
    
	if(outBufSize>0) /* launch dma on filled buffer */
        startDma();

	// request ARM to render a buffer
    dspCom->decodeDone=0;
	dspCom->decodeRequest=1;
    armInt_trigger();
	while(dspCom->decodeRequest) /* wait for arm to finish rendering*/;

	if(dspCom->decodeDone){
        //TODO: sample rate conversion
		
		dmaSize=(dspCom->bufSize+3)&(~3);

		if(dspCom->stereo)
        {
            // get data from sdram buffer
            /* DM270 DM320 code ==> using DMA btw SDRAM & DSP RAM*/
            if(chip_num==32 || chip_num==27)
            {
                hpiDma_start(dspCom->bufAddr,outBuf2,dmaSize,HPI_DIR_SD2DSP);
                while(hpiDma_pending());
                outBuf2Size=dmaSize/2;
            }
            else if(chip_num==25) /* DSC25 code ==> using ImageBuffer DMA*/
            {
				outBuf2Size=dmaSize;
                ibDma_start(dspCom->bufAddr,dmaSize,
                    0,dmaSize,
                    dmaSize,1,
                    IB_BUF_A,IB_DIR_SD2IB,0);
                while(ibDma_pending());
                ibDma_reset(); 
				in=IB_DSC25_BUFA_ADDR;
				out=outBuf2;
				while(out<outBuf2+outBuf2Size){
					*(out++)=*(in++);
					*(out++)=0;
				}
            }
		}else{            
			// get data from sdram buffer
            /* DM270 DM320 code ==> using DMA btw SDRAM & DSP RAM*/
            if(chip_num==32 || chip_num==27)
            {
				outBuf2Size=dmaSize;
                hpiDma_start(dspCom->bufAddr,processBuf,dmaSize,HPI_DIR_SD2DSP);
                while(hpiDma_pending());
                // process stereo
                if(!dspCom->stereo){
                    in=processBuf;
                    out=outBuf2;
                    while(out<outBuf2+outBuf2Size){
                        *(out++)=*in;
                        *(out++)=*(in++);
                    }
                }                
            }
            else if(chip_num==25) /* DSC25 code ==> using ImageBuffer DMA*/
            {
				outBuf2Size=dmaSize*4;
                ibDma_start(dspCom->bufAddr,dmaSize,
                    0,dmaSize,
                    dmaSize,1,
                    IB_BUF_A,IB_DIR_SD2IB,0);
                while(ibDma_pending());
                ibDma_reset();
                in=IB_DSC25_BUFA_ADDR;
				out=outBuf2; 
                while(out<outBuf2+outBuf2Size){
                	*(out++)=*in;
					*(out++)=0;
                	*(out++)=*(in++);
					*(out++)=0;
                }                
            }
		}

	}else{
        /* this should be enough blank data */
		outBuf2Size=OUTPUT_CHUNK_SIZE/2;
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

void sendDbgData(unsigned short data)
{
    if (!dspCom) return;
    
    sprintf((char*)dspCom->dbgMsg,"%x",data);
    dspCom->hasDbgMsg=1;
    armInt_trigger();
    while(dspCom->hasDbgMsg);       
}
