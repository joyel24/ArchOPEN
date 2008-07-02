/*
*   kernel/driver/rtc.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

/**********************************************
Driver for the STMicroelectronics chip (M41ST84W and similar)
it implements standard functions to read/write time/date
from the RTC
**********************************************/


#include <sys_def/errors.h>

#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/i2c.h>
#include <driver/rtc_ST.h>
#include <driver/time.h>

/* structure representing time for ST chip*/
struct tm_pv {
    unsigned char tm_ms;
    unsigned char tm_sec;
    unsigned char tm_min;
    unsigned char tm_hour;
    unsigned char tm_wday;
    unsigned char tm_mday;
    unsigned char tm_mon;
    unsigned char tm_year;
};

/* conversion Macro: ST chip uses hex representation of dec:
0x22 from chip is in fact 22 not 34*/
#define pvToUser(val)     ((((val>>4)&0xF)*10)+(val&0xF))
#define userToPv(val)     ((((val/10)<<4)&0xf0)+((val%10)&0x0F))

/* Macro that put a data in ST chip,
print an error if one occurs */
#define sendRTC(addr,val)        {int __ret;int __val=val; __ret=i2c_write(RTC_DEVICE, addr, (void*)(&__val), 1); \
                                        if(retVal<0) printk("[I2C - rtc] Error, writting (err:%d)\n",retVal);}

MED_RET_T rtc_getTime(struct med_tm * valTime)
{
    struct tm_pv pv_dt;
    int retVal;
    
    retVal=i2c_read(RTC_DEVICE, 0, (void*)(&pv_dt), 8);
            
    if(retVal<0)
    {
        printk("[I2C - rtc] Error, reading (err:%d)\n",retVal);
        return retVal;
    }
    
    pv_dt.tm_wday&=0x7;

    valTime->tm_ms=pvToUser(pv_dt.tm_ms);
    valTime->tm_sec=pvToUser(pv_dt.tm_sec);
    valTime->tm_min=pvToUser(pv_dt.tm_min);
    valTime->tm_hour=pvToUser(pv_dt.tm_hour);
    
    valTime->tm_mday=pvToUser(pv_dt.tm_mday);
    valTime->tm_mon=pvToUser(pv_dt.tm_mon);
    valTime->tm_year=2000+pvToUser(pv_dt.tm_year);
    
    valTime->tm_wday=pvToUser(pv_dt.tm_wday);
    //valTime->tm_wday=time_getDayOfWeek(valTime->tm_mday,valTime->tm_mon,valTime->tm_year);
    return MED_OK;
}


MED_RET_T rtc_setTime(struct med_tm *newTime)
{
    struct tm_pv pv_dt;
    struct tm_pv pv_dt_ini;
    int year;
    int wday;
    int retVal;
    
    retVal=i2c_read(RTC_DEVICE, 0, (void*)(&pv_dt_ini), 8);
            
    if(retVal<0)
    {
            printk("[I2C - rtc] Error, reading (err:%d) can't save the value\n",retVal);
            return retVal;
    }
    
    year=newTime->tm_year-2000;
    
    pv_dt.tm_ms=userToPv(newTime->tm_ms);
    pv_dt.tm_sec=userToPv(newTime->tm_sec);
    pv_dt.tm_min=userToPv(newTime->tm_min);
    pv_dt.tm_hour=userToPv(newTime->tm_hour);
    pv_dt.tm_wday=userToPv(newTime->tm_wday);
    pv_dt.tm_mday=userToPv(newTime->tm_mday);
    pv_dt.tm_mon=userToPv(newTime->tm_mon);
    pv_dt.tm_year=userToPv(year);
    
    
    
    /* only writting h,m,s and d,m,y */
    
    wday=time_getDayOfWeek(newTime->tm_mday,newTime->tm_mon,newTime->tm_year);
    
    
    pv_dt.tm_ms=0;
    pv_dt.tm_sec=RTC_MK_S(pv_dt_ini.tm_sec,pv_dt.tm_sec);
    pv_dt.tm_min=RTC_MK_M(pv_dt_ini.tm_min,pv_dt.tm_min);
    pv_dt.tm_hour=RTC_MK_H(pv_dt_ini.tm_hour,pv_dt.tm_hour);
    pv_dt.tm_wday=(wday&0x7);
    pv_dt.tm_mday=RTC_MK_DM(pv_dt_ini.tm_mday,pv_dt.tm_mday);
    pv_dt.tm_mon=RTC_MK_MO(pv_dt_ini.tm_mon,pv_dt.tm_mon);
    pv_dt.tm_year=RTC_MK_Y(pv_dt_ini.tm_year,pv_dt.tm_year);
    
    retVal=i2c_write(RTC_DEVICE, 0, (void*)(&pv_dt), 8);

    if(retVal<0)
    {
        printk("[I2C - rtc] Error, writting (err:%d)\n",retVal);
        return retVal;
    }
           
    return MED_OK;
}

MED_RET_T rtc_init(void)
{
    unsigned char cb;
    int retVal;
    int cwd;
    
    struct med_tm valTime;
        
    if((retVal=i2c_read(RTC_DEVICE, 0x04, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 0: Error, reading (err:%d)\n",retVal);
        return -1;
    }
    if(cb & 0x80)
    {
        cb&= ~0x80;        
        if((retVal=i2c_write(RTC_DEVICE, 0x04, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 1: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }

    /* Clear the Stop bit if it is set */
    if((retVal=i2c_read(RTC_DEVICE, 0x01, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 2: Error, reading (err:%d)\n",retVal);
        return -1;
    }        
    if(cb & 0x80)
    {
        cb&= ~0x80;
        if((retVal=i2c_write(RTC_DEVICE, 0x01, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 3: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }
        
    if((retVal=i2c_read(RTC_DEVICE, 0x0c, (void*)&cb, 1))<0)
    {
        printk("[I2C-rtc-ini] Step 4: Error, reading (err:%d)\n",retVal);
        return -1;
    }
    if(cb & 0x40)
    {
        cb &= ~0x40;                         // Make sure HT bit is not set
        if((retVal=i2c_write(RTC_DEVICE, 0x0c, (void*)&cb, 1))<0)
        {
            printk("[I2C-rtc-ini] Step 5: Error, writting (err:%d)\n",retVal);
            return -1;
        }
    }
    
    rtc_getTime(&valTime); 

    cwd=time_getDayOfWeek(valTime.tm_mday,valTime.tm_mon,valTime.tm_year);
    if(cwd!=valTime.tm_wday)
    {
        sendRTC(RTC_DW,((cwd&0x7) | 0x80));
    }
    
    rtc_getTime(&valTime);
    
    printk("[init] rtc :current time : %02d/%02d/%04d %02d:%02d:%02d\n",valTime.tm_mday,valTime.tm_mon,valTime.tm_year
                ,valTime.tm_hour,valTime.tm_min,valTime.tm_sec);
    
    return MED_OK;
}

