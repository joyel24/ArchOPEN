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
#include <kernel/timer.h>

#include <driver/hardware.h>
#include <driver/dsp.h>

#ifdef HAVE_AIC23_SOUND
#include <driver/aic23.h>
#endif

#ifdef HAVE_MAS_SOUND
#include <driver/mas.h>
#endif

#include <fs/stdfs.h>

#include <snd/buffer.h>
#include <snd/codec.h>
#include <snd/output.h>
#include <snd/sound.h>

#include <init/boot_error.h>

#include "dspshared.h"

#define POS2OFF(x) ((x)%OUTPUT_BUFFER_SIZE)
#define POS2PTR(x) (&output_buffer[POS2OFF(x)])

//#define BENCHMARK_MODE

static tDspCom * dspCom;

static char * output_buffer = NULL;

int output_readPos = 0;
int output_writePos = 0;

static int output_sampleRate = 0;

static bool output_active = false;
static bool output_dspWaitingData = false;
static bool output_dspNoOutput = false;
static bool output_discardingBuffer = false;
static bool output_dspRetrievingData = false;

__IRAM_CODE void output_dspInterrupt(int irq,struct pt_regs * regs){

    // debug message coming from the dsp
    if (dspCom->hasDbgMsg){
        char * str=malloc(256);
        int i;        
        for(i=0;i<256;++i){
            str[i]=dspCom->dbgMsg[i];
        }
        printf("dsp> %s\n",str);

        dspCom->hasDbgMsg=0;
        kfree(str);
    }
    // the dsp requests a decoding
    if(dspCom->decodeRequest){
        // we got a decode request -> we are now sure that the dsp has retrieved previous data
        if(output_dspRetrievingData){

            output_readPos+=dspCom->bufSize;
            output_dspRetrievingData=false;
        }

        if(output_active && (output_writePos-output_readPos)>0)
        {
            // update states
            if(!output_dspWaitingData)
                output_dspNoOutput=false;
            
            output_dspWaitingData=false;

      // buffer size is the smallest of: what's left in the buffer, what's left before the buffer boundary, chunk size
            dspCom->bufSize=MIN(MIN(output_writePos-output_readPos,
                                OUTPUT_BUFFER_SIZE-POS2OFF(output_readPos)),
                    OUTPUT_CHUNK_SIZE);
            dsp_write32(&dspCom->bufAddr,((unsigned long)POS2PTR(output_readPos))-SDRAM_START);
            output_dspRetrievingData=true;

            dspCom->decodeDone=1;
        }else
        {
            // update states
            if(output_dspWaitingData){
                output_dspNoOutput=true;
            }
            output_dspWaitingData=true;

            dspCom->decodeDone=0;
        }

        if(output_discardingBuffer){
            output_readPos=output_writePos;
        }
        dspCom->decodeRequest=0;
    }
}

void output_initDsp()
{
    *DSP_COM=0;

    if(dsp_loadProgramFromHDD("/medios/codec/snd_dsp.out")!=MED_OK)
    {
        gui_bootError(DSP_CODEC_ERROR,BOOT_WARN);
        return;
    }

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
  
    irq_changeHandler(IRQ_DSP,output_dspInterrupt);
    irq_enable(IRQ_DSP);
    
    dspCom->armInitFinished=1;    
}

void output_outputParamsChanged(){
    PLAYLIST_ITEM * item;
    int sampleRate;
    bool stereo;

    item=buffer_getActiveItem();

    sampleRate=item->tag.sampleRate;
    stereo=item->tag.stereo;
#ifdef HAVE_MAS_SOUND
    if(mas_getMode()==MAS_NO_MODE)
    {
        printk("[output_outputParamsChanged] no MAS mode define\n");
        return;   
    }
    if(mas_getMode()==MAS_MP3_MODE)        
    {
        /* MAS specific code */
    }
    else 
#endif
    {  
        if(dspCom->stereo!=stereo || sampleRate!=output_sampleRate){
            // make sure all previous data is outputted before any change
            //printk("waiting for end of buffer\n");
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
            //printk("DSP config send\n");
        }

    }
                           
}

bool output_setSampleRate(int rate){
#ifdef HAVE_AIC23_SOUND
    return aic23_setSampleRate(rate);
#endif
#ifdef HAVE_MAS_SOUND
    /*printk("Changing S rate\n");
    if(mas_getMode()==MAS_PCM_DSP_MODE)
        return mas_i2sChgSRate(rate);
    else
        return false;*/
    return true;
#endif
    return false;
}

void output_enableAudioOutput(bool enabled){
#ifdef HAVE_AIC23_SOUND
    aic23_enableOutput(enabled);
#endif
}

int output_volume;

void output_setVolume(int volume)
{
    output_volume=volume;
#ifdef HAVE_AIC23_SOUND
    aic23_setOutputVolume(volume+AIC23_MAX_OUTPUT_VOLUME-100,AIC23_CHANNEL_BOTH);
#endif
#ifdef HAVE_MAS_SOUND
    mas_codecCtrlConf(MAS_SET,MAS_VOLUME,volume); /*this function expect a value btw 0 & 100*/
#endif
}

void output_enable(bool enabled)
{
#ifdef HAVE_MAS_SOUND
    if(mas_getMode()==MAS_MP3_MODE)        
    {
        /* MAS specific code */
#if 1
        if(enabled)
            mas_mp3LaunchDecode();
        else
            mas_mp3StopDecode();
#else        
        output_active=enabled;
#endif
    }
    else
#endif
    {
        output_active=enabled;
    }
}

#ifdef BENCHMARK_MODE
int prevTime=0;
int bytesWritten=0;
#endif

void output_write(void * buffer, int size){

#ifndef BENCHMARK_MODE

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
            yield();
        }
    }

    // wait if we're discarding the buffer
    while(output_discardingBuffer){

        yield();
    }

    // copy data, handle buffer boundary crossing
    continuous=OUTPUT_BUFFER_SIZE-POS2OFF(output_writePos);
    if(size<=continuous){
        memcpy(POS2PTR(output_writePos),buffer,size);
    }else{

        memcpy(POS2PTR(output_writePos),buffer,continuous);
        memcpy(output_buffer,((char *)buffer)+continuous,size-continuous);
    }

    output_writePos+=size;

#else

    int t;
    int ref;

    bytesWritten+=size;
    t=tmr_getTick();

    if(t-prevTime>100){

        ref=(t-prevTime)*2*output_sampleRate*(dspCom->stereo?2:1)/HZ;

        printk("speed: %d%%\n",100*bytesWritten/ref);

        bytesWritten=0;
        prevTime=t;
    }

#endif
}

void output_discardBuffer()
{
#ifdef HAVE_MAS_SOUND
    if(mas_getMode()==MAS_NO_MODE)
    {
        printk("[output_outputParamsChanged] no MAS mode define\n");
        return;   
    }
#if 1
    if(mas_getMode()==MAS_MP3_MODE)        
    {
        /* MAS specific code */
        mas_mp3StopDecode();
    }
    else
#endif
#endif
    {
        //printk("doing discard (%d %d %d)\n",output_dspNoOutput,output_readPos,output_writePos);
        output_discardingBuffer=true;
    
        while(!output_dspNoOutput || output_readPos!=output_writePos){    
            yield();
        }
    
        output_discardingBuffer=false;
    }
}

void output_start(void)
{
    printk("[Output] starting\n");
    
    output_buffer=malloc(OUTPUT_BUFFER_SIZE);

    output_readPos=0;
    output_writePos=0;

    output_active=false;
    output_dspWaitingData=false;
    output_dspNoOutput=false;
    output_discardingBuffer=false;
    output_dspRetrievingData=false;

    output_sampleRate=OUTPUT_DEFAULT_SAMPLERATE;
    output_setSampleRate(output_sampleRate);
    output_enableAudioOutput(true);
}

void output_init()
{
    dspCom=NULL;
    output_volume=-1;
#ifndef HAVE_DBUG
    output_initDsp();
#endif
}

void output_stop(void)
{
    output_discardBuffer();

    output_enableAudioOutput(false);

    kfree(output_buffer);
}
