/*
*   include/kernel/mas.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MAS_H
#define __MAS_H

#include <sys_def/errors.h>
#include <snd_user/mas.h>
#include <kernel/irq.h>

/******************** Sound playback related ******************************/
MED_RET_T mas_chgMode(int mode);
int mas_getMode(void);

/******************** Mp3 related high level ******************************/

MED_RET_T mas_IniMp3(void);

void mas_mp3LaunchDecode(void);
int mas_mp3DecodeState(void);
void mas_mp3StopDecode(void);
void mas_clearMp3Buffer(void);
void mas_setMp3Buffer(struct mas_sound_buffer * b1,struct mas_sound_buffer * b2);

/******************** Mp3 related low level ******************************/
int mas_appSelect(int app);
int mas_appIsRunning(int app);
int mas_setClkSpeed(int spd);
int mas_getFrameCount(void);
void mas_dspInterrupt(int irq,struct pt_regs * regs);
MED_RET_T mas_startMp3App(void);

/********************* init mas                    ***************************/
void mas_init(void);
int mas_reset(void);
int mas_gio_init(void);

int mas_readBat(void);

MED_RET_T mas_stopApps(void);

/********************* Direct config function       ***************************/
/* register */
#define MAS_CONTROL                0x6a
#define MAS_DCCF                   0x76
#define MAS_DCFR                   0x77

/********************* Direct config i2c read/write ***************************/
int mas_read_direct_config(int reg);
int mas_write_direct_config(int reg,int val);

/********************* Register function       ***************************/
/* subaddress */
#define MAS_DATA_READ              0x69
#define MAS_DATA_WRITE             0x68

/* Register D0, D */
#define MAS_REGISTER_D0            0x00
#define MAS_REGISTER_D1            0x01

/* function addresses */
#define MAS_READ_Di(reg)           (((0xc + reg ) << 12) & 0xF000)
#define MAS_SHORTREAD_Di(reg)      ((((0xc + reg )<< 12)|0x0400) & 0xFF00)
#define MAS_WRITE_Di(reg)          (((0xe + reg ) << 12) & 0xF000)
#define MAS_SHORTWRITE_Di(reg)     ((((0xe + reg )<< 12)|0x0400) & 0xFF00)

#define MAS_CLEAR_SYNC             0x5000
#define MAS_IC_VERSION             0x7000

/* dsp memory cells */
#define MAS_VARIABLE_RAM_AREAS     0x6B
#define MAS_SPDIF_INPUT_STATUS     0x56
#define MAS_APP_SELECT             0x7F6
#define MAS_APP_RUNNING            0x7F7
#define MAS_ENCODER_CONTROL        0x7F0
#define MAS_MAIN_IO_CONTROL        0x7F1
#define MAS_INTERFACE_CONTROL      0x7F2
#define MAS_OSC_FREQ               0x7F3
#define MAS_OUTPUT_CLK_CONF        0x7F4
#define MAS_SPDIF_OUPUT_STATUS     0x7F8
#define MAS_SOFT_MUTE              0x7F9
#define MAS_LEFT_LEFT_GAIN         0x7FC
#define MAS_LEFT_RIGHT_GAIN        0x7FD
#define MAS_RIGHT_LEFT_GAIN        0x7FE
#define MAS_RIGHT_RIGHT_GAIN       0x7FF
#define MAS_MPEG_FRAME_COUNT       0xFD0
#define MAS_MPEG_HEADER_1          0xFD1
#define MAS_MPEG_HEADER_2          0xFD2
#define MAS_MPEG_CRC_ERR_COUNT     0xFD3
#define MAS_NB_BITS_IN_AUCILLARY   0xFD4

#define MASS_APP_MPEG3_ENC         0x040
#define MASS_APP_MPEG3_DEC         0x008
#define MASS_APP_MPEG2_DEC         0x004
#define MASS_APP_ANY               (MASS_APP_MPEG3_ENC | MASS_APP_MPEG3_DEC | MASS_APP_MPEG2_DEC)
#define MASS_APP_NONE              0x000

struct mas_version {
    int major_number;
    int derivate;
    char char_order_version;
    int digit_order_version;
};

/********************* DSP                    ***************************/
/* dev functions */
void    dsp_ctl           (unsigned int cmd, void * arg);

/********************* Register i2c read/write ***************************/

struct mas_data_struct {
    char * name;
    int reg;
    int addr;
    int length;
    int * buffer;
};

int mas_read_register(int reg);
int mas_write_register(int reg,unsigned int val);
int mas_freeze(void);
int mas_run(void);
int mas_setD0(int addr,unsigned int val);
unsigned int mas_getD0(int addr);

int mas_write_Di_regFromData(struct mas_data_struct * data);
int mas_read_Di_register(int i,int addr,void * buf,int size); // !!! 20 bit values stored as 32 bit
int mas_shortRead_Di_register(int i,int addr,void * buf,int size); // no problem here, 16 bit values used
int mas_write_Di_register(int i,int addr,void * buf,int size); // !!! 20 bit values read as 32 bit
int mas_shortWrite_Di_register(int i,int addr,void * buf,int size); // no problem here, 16 bit values used
int mas_clear_sync(void);
int mas_read_version(struct mas_version * ptr);

/********************* Codec function       ***************************/
/* subaddress */
#define MAS_CODEC_READ             0x6d
#define MAS_CODEC_WRITE            0x6c

/* register */
#define MAS_REG_AUDIO_CONF         0x0000
#define MAS_REG_INPUT_MODE         0x0008
#define MAS_REG_MIX_ADC_SCALE      0x0006
#define MAS_REG_MIX_DSP_SCALE      0x0007
#define MAS_REG_DA_OUTPUT_MODE     0x000e
#define MAS_REG_BASS               0x0014
#define MAS_REG_TREBLE             0x0015
#define MAS_REG_LOUDNESS           0x001e
#define MAS_REG_MDB_STRENGTH       0x0022
#define MAS_REG_MDB_HARMONICS      0x0024
#define MAS_REG_MDB_CENTER_FRE     0x0021
#define MAS_REG_VOLUME             0x0010
#define MAS_REG_BALANCE            0x0011
#define MAS_REG_AVC                0x0012
#define MAS_REG_BASS               0x0014
#define MAS_REG_INPEAK_LEFT        0x000a
#define MAS_REG_INPEAK_RIGHT       0x000b
#define MAS_REG_OUTPEAK_LEFT       0x000c
#define MAS_REG_OUTPEAK_RIGHT      0x000d

/* values */
#define MAS_INPUT_AD               0x08
#define MAS_L_AD_CONVERTER         0x04
#define MAS_R_AD_CONVERTER         0x02
#define MAS_DA_CONVERTER           0x01

#define MAS_CONFIG_INPUT_STEREO              0
#define MAS_CONFIG_INPUT_MONO                0x8000
#define MAS_CONFIG_INPUT_DEEMPHASIS_OFF      0
#define MAS_CONFIG_INPUT_DEEMPHASIS_50us     1
#define MAS_CONFIG_INPUT_DEEMPHASIS_75us     2

/* controls list */
#define MAS_VOLUME                0x00
#define MAS_BASS                  0x01
#define MAS_TREBLE                0x02
#define MAS_LOUDNESS              0x03
#define MAS_BALANCE               0x04
#define MAS_MICRO_GAIN            0x05
#define MAS_ADC_L_GAIN            0x06
#define MAS_ADC_R_GAIN            0x07

#ifndef MAS_SET
#define MAS_SET                   0x00
#define MAS_GET                   0x01
#endif

void mas_lineInOn(void);
void mas_lineInOff(void);

int mas_codecCtrlConf(int action,int control,int val);
int convertVal(int val,int control,int action);
int mas_codecCtrlWrite(int control,int val);
int mas_codecCtrlRead(int control);

/********************* Codec i2c read/write ***************************/
int mas_codecRead(int reg);
int mas_codecWrite(int reg,int val);

/********************* PCM decoding        ***************************/

void mas_i2sInit(void);
bool mas_i2sChgSRate(int sample_rate);

#define SRATE_5012     0
#define SRATE_8000     1
#define SRATE_11025    2
#define SRATE_12000    3
#define SRATE_16000    4
#define SRATE_22050    5
#define SRATE_24000    6
#define SRATE_32000    7
#define SRATE_44100    8
#define SRATE_48000    9

#define DEFAULT_SRATE  SRATE_44100

#endif

