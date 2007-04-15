/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/delay.h>
#include <kernel/irq.h>
#include <kernel/io.h>
#include <driver/hardware.h>
#include <driver/dsp.h>
#include <driver/aic23.h>
#include <fs/stdfs.h>

#include <snd/codec.h>
#include <snd/output.h>
#include <snd/sound.h>

#include "dspshared.h"

#define POS2OFF(x) ((x)%OUTPUT_BUFFER_SIZE)
#define POS2PTR(x) (&output_buffer[POS2OFF(x)])

static tDspCom * dspCom;

static char * output_buffer = NULL;

int output_readPos = 0;
int output_writePos = 0;

static int output_sampleRate = 0;

static bool output_active = false;
static bool output_dspWaitingData = false;
static bool output_dspNoOutput = false;
static bool output_discardingBuffer = false;

__IRAM_CODE void output_dspInterrupt(int irq,struct pt_regs * regs){
    //printf("I");
    // debug message coming from the dsp
    if (dspCom->hasDbgMsg){
        char * str=malloc(256);
        int i;

        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
        free(str);
    }

    // the dsp requests a decoding
    if(dspCom->decodeRequest){
       // printk("D");
        if(output_active && (output_writePos-output_readPos)>0){
            // update states
            if(!output_dspWaitingData){
                output_dspNoOutput=false;
            }
            output_dspWaitingData=false;

            // buffer size is the smallest of: what's left in the buffer, what's left before the buffer boundary, chunk size
            dspCom->bufSize=MIN(MIN(output_writePos-output_readPos,OUTPUT_BUFFER_SIZE-POS2OFF(output_readPos)),OUTPUT_CHUNK_SIZE);

            dsp_write32(&dspCom->bufAddr,((unsigned long)POS2PTR(output_readPos))-SDRAM_START);

            output_readPos+=dspCom->bufSize;

            dspCom->decodeDone=1;
        }else{
            // update states
            if(output_dspWaitingData){
                output_dspNoOutput=true;
            }
            output_dspWaitingData=true;

            dspCom->decodeDone=0;
        }

        if(output_discardingBuffer){
            output_readPos=output_writePos;
        };

        dspCom->decodeRequest=0;
    }
    //printf("F");
}

void output_initDsp(){
    extern char _binary_apps_aodoom_dspcode_doom_dsp_out_start;
    extern char _binary_apps_aodoom_dspcode_doom_dsp_out_end;
    unsigned char * dspcode=&_binary_apps_aodoom_dspcode_doom_dsp_out_start;
    int len=&_binary_apps_aodoom_dspcode_doom_dsp_out_end-&_binary_apps_aodoom_dspcode_doom_dsp_out_start;

    // dsp irq handler
    irq_changeHandler(IRQ_DSP,output_dspInterrupt);
    irq_enable(IRQ_DSP);

    *DSP_COM=0;
#if 0
    dsp_loadProgramFromMemory(dspcode,len);
#else
    dsp_loadProgramFromHDD("/medios/codec/snd_dsp.out");
#endif

    // setup dma
#ifdef DM270
    outw((inw(0x309a8)&0x03ff)|0x1000,0x309a8); //REFCTL : DMA 2 is used for DSP<>SDRAM
#endif

    dsp_run();

    while(!(*DSP_COM)); // wait for the dsp to finish init

    dspCom = (tDspCom *) DSP_RAM(*DSP_COM);

    dspCom->chipNum=dsp_getChipNum();

    dspCom->decodeRequest=0;
    dspCom->decodeDone=0;
    dspCom->bufAddr=0;

    dspCom->armInitFinished=1;
};

void output_outputParamsChanged(int sampleRate, bool stereo){

    if(dspCom->stereo!=stereo || sampleRate!=output_sampleRate){
        // make sure all previous data is outputted before any change
        while(!output_dspNoOutput){
            yield();
        }
    }

    // stereo

    dspCom->stereo=stereo;

    // sample rate

    dspCom->dataSampleRate=sampleRate;

    if(sampleRate!=output_sampleRate){

        if(output_setSampleRate(sampleRate)){
            output_sampleRate=sampleRate;
        }else{
            output_sampleRate=OUTPUT_DEFAULT_SAMPLERATE;
            output_setSampleRate(output_sampleRate);
        }

        dspCom->outputSampleRate=output_sampleRate;
    }
}

bool output_setSampleRate(int rate){
    return aic23_setSampleRate(rate);
}

void output_enableAudioOutput(bool enabled){
    aic23_enableOutput(enabled);
}

void output_enable(bool enabled){
    output_active=enabled;
}

void output_write(void * buffer, int size){
    int free;
    int continuous;

//    printk("[output] write %d\n",size);
//    printk("buffer %d\n",output_writePos-output_readPos);

    // make sure we have room for the data, else wait
    for(;;){
        free=OUTPUT_BUFFER_SIZE-(output_writePos-output_readPos);

        if(size<=free){
            break;
        }else{
            //wait for some data to be outputted
           // printk("c");
            yield();
        }
    }

    // wait if we're discarding the buffer
    while(output_discardingBuffer){
      //  printk("b");
        yield();
    }

    // copy data, handle buffer boundary crossing
    continuous=OUTPUT_BUFFER_SIZE-POS2OFF(output_writePos);
    if(size<=continuous){
        //printk("d");
        memcpy(POS2PTR(output_writePos),buffer,size);
    }else{
       // printk("e");
        memcpy(POS2PTR(output_writePos),buffer,continuous);
        memcpy(output_buffer,((char *)buffer)+continuous,size-continuous);
    }

    output_writePos+=size;
}

void output_discardBuffer(){

    output_discardingBuffer=true;

    while(!output_dspNoOutput){
      //  printk("a");
        yield();
    }

    output_discardingBuffer=false;
}

void output_init(){

    output_buffer=malloc(OUTPUT_BUFFER_SIZE);

    output_readPos=0;
    output_writePos=0;

    output_active=false;
    output_dspWaitingData=false;
    output_dspNoOutput=false;
    output_discardingBuffer=false;

    output_initDsp();

    output_sampleRate=OUTPUT_DEFAULT_SAMPLERATE;
    output_setSampleRate(output_sampleRate);
    output_enableAudioOutput(true);
}

void output_close(){
    output_enableAudioOutput(false);

    free(output_buffer);
}
