/*
*   kernel/driver/mas.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <sys_def/stddef.h>
#include <sys_def/section_types.h>
#include <sys_def/errors.h>

#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/delay.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>

#include <driver/hardware.h>
#include <driver/i2c.h>
#include <driver/gio.h>
#include <driver/mas.h>
#include <driver/timer.h>
#include <driver/uart.h>

#include <../../snd/dspshared.h> /* not really nice!! */

#include <snd/output.h>

int mas_curMode;

__IRAM_DATA long soundPaused = 0;     // 1 if in Pause mode

#define MAS_DELAY        {int ___i; for(___i=0;___i<20;___i++);}

#define GIO_SEND_MAS_DELAY {int ___i; for(___i=0;___i<30;___i++);}
/********************* DSP                    ***************************/

struct mas_sound_buffer * buff_1;
struct mas_sound_buffer * buff_2;

struct mas_sound_buffer * soundBuffer;

__IRAM_DATA int masPlaying;

__IRAM_DATA struct spinLock masLock;

__IRAM_DATA int cnt_send;
__IRAM_DATA int tot_send;

__IRAM_CODE void mas_dspInterrupt(int irq,struct pt_regs * regs)
{
    int toSend;
    int i;
    int data;
    char * data_buff;
    
    //printk("MAS INT\n");
    
    if(!spinLock_testAndLock(&masLock))
    {
        uart_out('A',0);
        return;
    }
    
    if(!soundBuffer || soundPaused || soundBuffer->state==MAS_BUFFER_END)
    {
        printk("Halt DSP interupt (1): buff@:%x,pause=%d,read: %x/%x\n",
            soundBuffer,soundPaused,soundBuffer->read,soundBuffer->size);
        masPlaying=0;
        irq_disable(IRQ_MAS_DATA);
        spinLock_unlock(&masLock);
        return;
    }
            
    if(soundBuffer->state==MAS_BUFFER_FILLED)
        soundBuffer->state=MAS_BUFFER_PLAYING;
    
    toSend=soundBuffer->size-soundBuffer->read;
    data_buff=soundBuffer->data+soundBuffer->read;
    for(i=0;i<toSend;i++)
    {
        if(inw(GIO_BITSET0) & (0x1<<GIO_MAS_EOD))
            break;  /* EOD set => exit */
        data=(data_buff[i] << 8) & 0xFF00;
        if(data!=0)
            outw(data,GIO_BITSET0);
        data ^= 0xFF00;
        if(data!=0)
            outw(data,GIO_BITCLEAR0);
        /* try to latch data (raise PR) */
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITSET1);
        /* wait for RTR to be set */
        while(!(inw(GIO_BITSET1) & (0x1<<(GIO_MAS_RTR-16))))
            /*nothing*/;
        /* clear latch (lower raise PR) */
        outw(0x1<<(GIO_MAS_PR-16),GIO_BITCLEAR1);
        GIO_SEND_MAS_DELAY
    }
    
    tot_send+=i;
    cnt_send++;
    soundBuffer->read+=i;
    
    if(soundBuffer->read>=soundBuffer->size)  /* we reached end of buffer => change buffer */
    {
        //uart_out('E',0);
        soundBuffer->state=MAS_BUFFER_END;
        soundBuffer=soundBuffer->nxt;
        if(soundBuffer->state!=MAS_BUFFER_FILLED)
        {
            printk("Halt DSP interupt (2): buff@:%x,pause=%d,read: %x/%x\n",
                soundBuffer,soundPaused,soundBuffer->read,soundBuffer->size);
            masPlaying=0;
            irq_disable(IRQ_MAS_DATA);
            irq_disable(IRQ_TMR_3);
            spinLock_unlock(&masLock);
            return;
        }
        soundBuffer->state=MAS_BUFFER_PLAYING;
        spinLock_unlock(&masLock);
        mas_dspInterrupt(IRQ_MAS_DATA,NULL);
    }
    else        
        spinLock_unlock(&masLock);
}

/********************* mas i2c                  ***************************/

int in_16_val(void)
{
    int ret=i2c_inb() << 8;
    i2c_ack();
    return ret|i2c_inb();
}

int in_32_val(void)
{
    int ret=i2c_inb() << 24;
    i2c_ack();
    ret|=i2c_inb() << 16;
    i2c_ack();
    ret|=i2c_inb() << 8;
    i2c_ack();
    return ret|i2c_inb();
}

#define OUT_WRITE_DEVICE           i2c_outb((I2C_WRITE_DEVICE(MAS_DEVICE)));
#define OUT_READ_DEVICE            i2c_outb((I2C_READ_DEVICE(MAS_DEVICE)));
#define OUT_SUBADDRESS(addr)       i2c_outb((addr));
#define OUT_16_VAL(val)            i2c_outb(((val)>>8)&0xFF); \
                                       i2c_outb((val)&0xFF);
#define OUT_32_VAL(val)            OUT_16_VAL((val) >> 16) OUT_16_VAL((val));
#define IN_16_VAL                  in_16_val()
#define IN_32_VAL                  in_32_val()

#define MAS_I2C_LOCK               {if(!i2c_getLock(I2C_LOOP,0)) {printk("[MAS-I2C] No lock\n"); return 0;}}
#define MAS_I2C_UNLOCK             i2c_release();

/********************* init mas                    ***************************/

void mas_init(void)
{
    struct mas_version version;
    printk("[init] av3xx sound driver: ");
    mas_gio_init();
    printk("M");
    mas_reset();
    printk("A");
    mas_read_version(&version);
    printk("S%x:%d:%c%d ",version.major_number,version.derivate,version.char_order_version,version.digit_order_version);
    irq_disable(IRQ_MAS_DATA);
    printk("\n");
    mas_curMode=MAS_NO_MODE;
}

int mas_reset(void)
{
    GIO_CLEAR(GIO_MAS_PWR);  // stop the MAS
    mdelay(100);
    GIO_SET(GIO_MAS_PWR);       // start the MAS
    mdelay(10);
    mas_write_direct_config(MAS_CONTROL,0x8c00);
    return 0;
}

int mas_gio_init(void)
{
    int i;
    GIO_DIRECTION(GIO_MAS_EOD,GIO_IN);
    for(i=0;i<8;i++)
        GIO_DIRECTION((GIO_MAS_D0+i),GIO_OUT);
    GIO_DIRECTION(GIO_MAS_PWR,GIO_OUT);
    GIO_DIRECTION(GIO_MAS_RTR,GIO_IN);
    GIO_DIRECTION(GIO_MAS_PR,GIO_OUT);

    GIO_INVERT(GIO_MAS_EOD,GIO_INVERTED);
    GIO_IRQ_ENABLE(GIO_MAS_EOD,GIO_IRQ);

    return 0;
}

/********************* app functions                ***************************/

MED_RET_T mas_chgMode(int mode)
{
    MED_RET_T res=MED_OK;
    if(mode!=mas_curMode)
    {
        switch(mode)
        {
            case MAS_MP3_MODE:
                res=mas_IniMp3();
                break;
            case MAS_PCM_DIRECT_MODE:
                res=-MED_ERROR;
                break;
            case MAS_PCM_DSP_MODE:
                mas_i2sInit();      
                break;                  
        }
    }
    
    if(res==MED_OK) /*everything ok, save new mode*/
        mas_curMode=mode;
    else
    {
        mas_curMode=-1;
        printk("[mas_chgMode] error while changing to mode %d\n",mode);
    }
    return res;
}

int mas_getMode(void)
{
    return mas_curMode;   
}

MED_RET_T mas_IniMp3(void)
{
    mas_reset();
    irq_disable(IRQ_MAS_DATA);
    
    /* NOTE: all this config is bit too much, as we are changing sound levels too ... */
    mas_codecWrite(MAS_REG_AUDIO_CONF,MAS_INPUT_AD | MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
                        | 0xf  << 4 // mic gain
                        | 0xf << 8  // adc gain right
                        | 0xf <<12  // adc gain left
                        );
    mas_codecWrite(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_MONO);
    mas_codecWrite(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
    mas_codecWrite(MAS_REG_MIX_DSP_SCALE,0x40 << 8);
    mas_codecWrite(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_codecCtrlConf(MAS_SET,MAS_BALANCE,50);    
    if(output_volume!=-1)
        mas_codecCtrlConf(MAS_SET,MAS_VOLUME,output_volume);
    else
        mas_codecCtrlConf(MAS_SET,MAS_VOLUME,70);
    
    MAS_DELAY
    
    /* we could almost start here */
            
    if(mas_stopApps()!=MED_OK)
    {
        printk("[mas_IniMp3] Error while stoping all app\n");
        return -MED_ERROR;
    }

    mas_setD0(MAS_INTERFACE_CONTROL,0x04);
    mas_setClkSpeed(0x4800);
#if 0
    mas_setD0(MAS_MAIN_IO_CONTROL,0x125);
#else
    mas_setD0(MAS_MAIN_IO_CONTROL,0x125);
#endif
    MAS_DELAY

    if(mas_startMp3App()!=MED_OK)
    {
        printk("[mas_IniMp3]Error while starting MP3 app\n");
        return -MED_ERROR;
    }

    masPlaying=0;
    
    printk("[mas_IniMp3] done\n");
    
    return MED_OK;
}

void mas_setMp3Buffer(struct mas_sound_buffer * b1,struct mas_sound_buffer * b2)
{
    buff_1=b1;
    buff_2=b2;
    /*setting up the buffer loop*/
    b1->nxt=b2;
    b2->nxt=b1;
    
    printk("[mas_setMp3Buffer] b1=%x b2=%x\n",b1,b2);
    
    soundBuffer=b1;   
}

int mas_mp3DecodeState(void)
{
    return masPlaying;
}

void mas_mp3LaunchDecode(void)
{
    soundPaused=0;
    masPlaying=1;
    
    /* stat */
    cnt_send=0;
    tot_send=0;
     
    irq_enable(IRQ_MAS_DATA);
    spinLock_ini(&masLock);
    mas_dspInterrupt(IRQ_MAS_DATA,NULL); 
}

void mas_mp3StopDecode(void)
{
    soundPaused=1;
    irq_disable(IRQ_MAS_DATA);
    masPlaying=0;
}

void mas_clearMp3Buffer(void)
{
    soundBuffer=buff_1=buff_2=NULL;
}

MED_RET_T mas_stopApps(void)
{
    int i;
    mas_appSelect(MASS_APP_NONE);
    while(1)
    {
        i=mas_appIsRunning(MASS_APP_ANY);
        if(i<0)
        {
            printk("error getting app status (trying to stop)\n");
            return -MED_ERROR;
        }
        if(i==0)
            break;
    }
    return MED_OK;
}

MED_RET_T mas_startMp3App(void)
{
    int i;
    int nbTry=100;
    mas_appSelect(MASS_APP_MPEG3_DEC | MASS_APP_MPEG2_DEC);
    while(1)
    {
        i=mas_getD0(MAS_APP_SELECT);
        if((i & MASS_APP_MPEG3_DEC) && (i & MASS_APP_MPEG2_DEC))
            break;
        if(nbTry<0)
        {
            printk("[mas_startMp3App] failed\n");
            return -MED_ERROR;
        }
        nbTry--;
    }
    return MED_OK;
}

int mas_appSelect(int app)
{
    return mas_setD0(MAS_APP_SELECT,app);
}

int mas_appIsRunning(int app)
{
    unsigned int val=mas_getD0(MAS_APP_SELECT);
    if(val<0)
        return -1;
    return (val & app);
}

int mas_setClkSpeed(int spd)
{
    return mas_setD0(MAS_OSC_FREQ,spd);
}

int mas_getFrameCount(void)
{
    return (mas_getD0(MAS_MPEG_FRAME_COUNT)&0xFFFFF);
}

/********************* PIO read/write               ***************************/
int mas_pio_read(void * buffer,int maxSize)
{
    return 0;
}

/********************* Direct config i2c read/write ***************************/
int mas_readBat(void)
{
    int base_val;
    int threshold=0xF;
    
    base_val=mas_read_direct_config(MAS_DCFR)&0x3FF;
    while(1)
    {
        mas_write_direct_config(MAS_DCFR,base_val|(threshold<<10));
        mdelay(2);
        if(mas_read_direct_config(MAS_DCFR)&0x8000)
            break;
        threshold--;
        if(threshold==0)
            break;
    }
    
    mas_write_direct_config(MAS_DCFR,base_val);
    //printk("[mas] readBat: %d (base=%x)\n",threshold,base_val);
    return threshold;
}


/********************* Direct config i2c read/write ***************************/

int mas_read_direct_config(int reg)
{
    int ret=0;
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(reg)
    i2c_start();
    OUT_READ_DEVICE
    ret=IN_16_VAL;
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    return ret;
}

int mas_write_direct_config(int reg,int val)
{
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(reg)
    OUT_16_VAL(val)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}


/********************* Register i2c read/write ***************************/

int mas_read_register(int reg)
{
    int ret=0;
    int ret2=0;
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0xa000 | ((reg<<4)  & 0x0FF0))
    i2c_stop();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_READ)
    i2c_start();
    OUT_READ_DEVICE
        ret = IN_16_VAL;
        ret = (ret&0xF)<<16;
        i2c_ack();
        ret2 = IN_16_VAL;
        ret |= (ret2 & 0xFFFF);
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    return ret;
}

int mas_write_register(int reg,unsigned int val)
{
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0xB000 | ((reg&0xFF)<<4) | ((val>>16)&0xf))
    OUT_16_VAL(val&0xFFFF)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_freeze(void)
{
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0x0000)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_run(void)
{
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(0x1000)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_setD0(int addr,unsigned int val)
{
    return mas_write_Di_register(MAS_REGISTER_D0,addr,&val,1);
}

unsigned int mas_getD0(int addr)
{
    unsigned int ret;
    if( mas_read_Di_register(MAS_REGISTER_D0,addr,&ret,1)<0)
        return -1;

    return ret;
}

int mas_read_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values stored as 32 bit
{
    int ret=0;
    int j;
    char * buffer = (char*) buf;
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_READ_Di(i))
    OUT_16_VAL(size)
    OUT_16_VAL(addr)
    i2c_stop();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_READ)
    i2c_start();
    OUT_READ_DEVICE
    for(j=0;j<size*4;j+=4)
    {
        ret=IN_32_VAL;
        buffer[j+3]=0;
        buffer[j+2]=(ret>>16)&0x0F;
        buffer[j+1]=(ret>>8)&0xFF;
        buffer[j]=ret&0xFF;
        if(j+4!=size)
            i2c_ack();
    }
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_shortRead_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
    int ret=0;
    int j;
    char * buffer = (char*) buf;
    MAS_I2C_LOCK
        //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)

    OUT_16_VAL(MAS_SHORTREAD_Di(i))
    OUT_16_VAL(size)
    OUT_16_VAL(addr)
    i2c_stop();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_READ)
    i2c_start();
    OUT_READ_DEVICE
    for(j=0;j<size*2;j+=2)
    {
        ret=IN_16_VAL;
        buffer[j+1]=(ret>>8)&0xFF;
        buffer[j]=(ret)&0xFF;
        if(j+1!=size)
            i2c_ack();
    }
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_write_Di_regFromData(struct mas_data_struct * data)
{
    return mas_write_Di_register(data->reg,data->addr,(void *)data->buffer,data->length);
}

int mas_write_Di_register(int i,int addr,void * buf,int size) // !!! 20 bit values read as 32 bit
{
    char * buffer = (char*) buf;
    int j;
    unsigned long outval;
    MAS_I2C_LOCK
        //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_WRITE_Di(i))
    OUT_16_VAL(size)
    OUT_16_VAL(addr)
    for(j=0;j<size*4;j+=4)
    {
        outval=((buffer[j+3]<<24) & 0x00000000)
                  | ((buffer[j+2]<<16) & 0x000F0000)
              | ((buffer[j+1]<<8) & 0x0000FF00)
              | ((buffer[j]) & 0x000000FF);
        OUT_32_VAL(outval)
    }
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_shortWrite_Di_register(int i,int addr,void * buf,int size) // no problem here, 16 bit values used
{
    char * buffer = (char*) buf;
    int j;
    MAS_I2C_LOCK
        //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_SHORTWRITE_Di(i))
    OUT_16_VAL(size)
    OUT_16_VAL(addr)
    for(j=0;j<size*2;j+=2)
        OUT_32_VAL(((buffer[j+1]<<8) & 0x0000FF00) | ((buffer[j]) & 0x000000FF))
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_clear_sync(void)
{
    MAS_I2C_LOCK
    //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_CLEAR_SYNC)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

int mas_read_version(struct mas_version * ptr)
{
    int ret;
    MAS_I2C_LOCK
        //i2c_ini_xfer();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_WRITE)
    OUT_16_VAL(MAS_IC_VERSION)
    i2c_stop();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_DATA_READ)
    i2c_start();
    OUT_READ_DEVICE
    ptr->major_number=IN_16_VAL;
    i2c_ack();
    ret=IN_16_VAL;
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    ptr->derivate=(ret >> 12) & 0xF;
    ptr->char_order_version=((ret>>4) & 0xFF)+0x41; // it's a char !
    ptr->digit_order_version=ret&0xF;
    return 0;
}

/********************* Codec function       ***************************/

void mas_lineInOn(void)
{
    mas_codecWrite(MAS_REG_AUDIO_CONF, MAS_L_AD_CONVERTER | MAS_R_AD_CONVERTER | MAS_DA_CONVERTER
                        | 0x0  << 4 // mic gain
                        | 0xf << 8  // adc gain right
                        | 0xf <<12  // adc gain left
                        );
    mas_codecWrite(MAS_REG_INPUT_MODE,MAS_CONFIG_INPUT_STEREO);
    mas_codecWrite(MAS_REG_MIX_ADC_SCALE,0x40 << 8);
    mas_codecWrite(MAS_REG_MIX_DSP_SCALE,0x00 << 8);
    mas_codecWrite(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_codecCtrlConf(MAS_SET,MAS_BALANCE,50);
    mas_codecCtrlConf(MAS_SET,MAS_VOLUME,70);
}

void mas_lineInOff(void)
{
    mas_codecWrite(MAS_REG_MIX_ADC_SCALE,0x00 << 8);
    mas_codecCtrlConf(MAS_SET,MAS_VOLUME,0x0);
}


int reg_addr[]={
    MAS_REG_VOLUME,
    MAS_REG_BASS,
    MAS_REG_TREBLE,
    MAS_REG_LOUDNESS,
    MAS_REG_BALANCE,
    MAS_REG_AUDIO_CONF,
    MAS_REG_AUDIO_CONF,
    MAS_REG_AUDIO_CONF
};

int mas_codecCtrlConf(int action,int control,int val)
{
    int ret=0;
    switch (action)
    {
        case MAS_SET:
            if(val<0 || val >100)
                return -1;
            ret=convertVal(val,control,action);
            if(ret<0)
                return ret;
            return mas_codecCtrlWrite(control,ret);
        case MAS_GET:
            ret=mas_codecCtrlRead(control);
            if(ret<0)
                return -1;
            return convertVal(ret,control,action);
        default:
            printk("[mas_codec_config_bit] Wrong action\n");
    }
    return -1;
}

int mas_codecCtrlWrite(int control,int val)
{
    int tmpVal;
    switch(control)
    {
        case MAS_LOUDNESS:
            if((tmpVal=mas_codecRead(control))<0)
                return -1;
            val=(val<<8) | (tmpVal & 0x00FF);
            break;
        case MAS_VOLUME:
        case MAS_BASS:
        case MAS_TREBLE:
        case MAS_BALANCE:
            val<<=8;
            break;
        case MAS_MICRO_GAIN:
            if((tmpVal=mas_codecRead(control))<0)
                return -1;
            val=((val&0xF)<<12)  | (tmpVal & 0x0FFF);
            break;
        case MAS_ADC_L_GAIN:
            if((tmpVal=mas_codecRead(control))<0)
                return -1;
            val=((val&0xF)<<8)  | (tmpVal & 0xF0FF);
            break;
        case MAS_ADC_R_GAIN:
            if((tmpVal=mas_codecRead(control))<0)
                return -1;
            val=((val&0xF)<<4)  | (tmpVal & 0xFF0F);
            break;
        default:
            return -1;
    }
    return mas_codecWrite(reg_addr[control],val);
}

int mas_codecCtrlRead(int control)
{
    int ret;
    switch(control)
    {
        case MAS_VOLUME:
        case MAS_BASS:
        case MAS_TREBLE:
        case MAS_BALANCE:
        case MAS_LOUDNESS:
            if((ret=mas_codecRead(reg_addr[control]))<0)
                return -1;
            ret=(ret>>8)&0xFF;
            break;
        case MAS_MICRO_GAIN:
            if((ret=mas_codecRead(reg_addr[control]))<0)
                return -1;
            ret=(ret>>12)&0xF;
            break;
        case MAS_ADC_L_GAIN:
            if((ret=mas_codecRead(reg_addr[control]))<0)
                return -1;
            ret=(ret>>8)&0xF;
            break;
        case MAS_ADC_R_GAIN:
            if((ret=mas_codecRead(reg_addr[control]))<0)
                return -1;
            ret=(ret>>4)&0xF;
            break;
        default:
            return -1;
    }
    return ret;
}


int convertVal(int val,int control,int action)
{
    int a,b;
    switch(control)
    {
        case MAS_VOLUME:
            a=0x7F;
            b=0;
            break;
        case MAS_BASS:
        case MAS_TREBLE:
            if(val <50)
            {
                a=180;
                b=160;
            }
            else
            {
                a=192;
                b=-96;
            }
            break;
        case MAS_LOUDNESS:
            a=0x44;
            b=0;
            break;
        case MAS_BALANCE:
            if(val <50)
            {
                a=260;
                b=128;
            }
            else
            {
                a=254;
                b=-127;
            }
            break;
        case MAS_MICRO_GAIN:
        case MAS_ADC_L_GAIN:
        case MAS_ADC_R_GAIN:
            a=0xF;
            b=0;
            break;
        default:
            return -1;
    }
    if(action == MAS_SET)
    {
        val=(val*a)/100+b;
    }
    else
    {
        val=((val-b)/a)*100;
        if(val>100)
            val=100;
        if(val<0)
            val=0;
    }

    return val;
}

/********************* Codec i2c read/write ***************************/

int mas_codecRead(int reg)
{
    int ret=0;
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_CODEC_WRITE)
    OUT_16_VAL(reg)
    i2c_stop();
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_CODEC_READ)
    i2c_start();
    OUT_READ_DEVICE
    ret=IN_16_VAL;
    i2c_notAck();
    i2c_stop();
    MAS_I2C_UNLOCK
    return ret;
}

int mas_codecWrite(int reg,int val)
{
    MAS_I2C_LOCK
    i2c_start();
    OUT_WRITE_DEVICE
    OUT_SUBADDRESS(MAS_CODEC_WRITE)
    OUT_16_VAL(reg)
    OUT_16_VAL(val)
    i2c_stop();
    MAS_I2C_UNLOCK
    return 0;
}

#include "mas_codecI2S.h"

int mas_stop_data[8][2] = {
    { 0x3B,0x00318 },
    { 0x43,0x00300 },
    { 0x4B,0x00000 },
    { 0x53,0x00318 },
    { 0x6B,0x00000 },
    { 0xBB,0x00318 },
    { 0xC3,0x00300 },
    { 0x06,0x00000 }
};

unsigned int sample_rate_cfg[][2]= {
{0x4E5E,0x106}, /*  5.0125 */
{0x4800,0x106}, /*  8      */
{0x4E5E,0x186}, /* 11.025  */
{0x4800,0x186}, /* 12      */
{0x4800,0x206}, /* 16      */
{0x4E5E,0x286}, /* 22.05   */
{0x4800,0x286}, /* 24      */
{0x6C00,0x306}, /* 32      */
{0x4E5E,0x306}, /* 44.1    */
{0x4800,0x306}, /* 48      */
};
                  
void mas_i2sInit(void)
{
    int i;
    //mas_reset();
    mas_stopApps();
    mas_freeze();
    for(i=0;i<8;i++)
    {
        mas_write_register(mas_stop_data[i][0],mas_stop_data[i][1]);
    }

    mas_write_Di_regFromData(&I2S_codec);
    
    mas_write_register(0x6B,0xC0000);
    mas_run();

    mas_setD0(0x7f8,0x00008300);    
    mas_setD0(0x7f2,0x04);  

    mas_codecWrite(0x1,sample_rate_cfg[DEFAULT_SRATE][1]);
    mas_setD0(0x347,0x0);
    mas_setD0(0x348,sample_rate_cfg[DEFAULT_SRATE][0]);
    mas_setD0(0x346,0x1a1);
    
    mas_codecWrite(MAS_REG_AUDIO_CONF,0x7);
    mas_codecWrite(MAS_REG_INPUT_MODE,0x0);
    mas_codecWrite(MAS_REG_MIX_ADC_SCALE,0x0);
    mas_codecWrite(MAS_REG_MIX_DSP_SCALE,0x4000);
    mas_codecWrite(MAS_REG_DA_OUTPUT_MODE,0x0);
    mas_codecCtrlConf(MAS_SET,MAS_BALANCE,50);
    if(output_volume!=-1)
        mas_codecCtrlConf(MAS_SET,MAS_VOLUME,output_volume);
    else
        mas_codecCtrlConf(MAS_SET,MAS_VOLUME,70);
    
    printk("[MAS-I2S init] done\n");
    
}


bool mas_i2sChgSRate(int sample_rate)
{
    if(sample_rate<SRATE_5012 || sample_rate>SRATE_48000)
    {
        printk("[MAS-SRATE] Error: unsupported sample rate: %d\n",sample_rate);
        return false;
    }
    else
    {
        mas_codecWrite(0x1,sample_rate_cfg[sample_rate][1]);
        mas_setD0(0x347,0x0);
        mas_setD0(0x348,sample_rate_cfg[sample_rate][0]); 
        mas_setD0(0x346,0x1a1);
        printk("[MAS-SRATE] wrote %x,%x for %d\n",sample_rate_cfg[sample_rate][0],
            sample_rate_cfg[sample_rate][1],sample_rate);
        return true;
    }
}
