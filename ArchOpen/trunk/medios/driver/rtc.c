/*
*   kernel/driver/rtc.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <sys_def/errors.h>

#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/i2c.h>
#include <driver/rtc.h>

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

#define pvToUser(val)     ((((val>>4)&0xF)*10)+(val&0xF))
#define userToPv(val)     ((((val/10)<<4)&0xf0)+((val%10)&0x0F))

int month_code[12] = {6,2,2,5,0,3,5,1,4,6,2,4};

int is_leap_year(int yr)
{
    return ((yr) <= 1752 ? !((yr) % 4) : \
    (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))  ? 1:0 ;
}

#if 0
#define nbDay_year(yr) (((yr) <= 1752 ? (!((yr) % 4))?366:365 : \
            (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))  ? 366:365 )

int month_tab[12][2] = {
    {31,59,90,120,151,181,212,243,273,304,334,365},
    {31,60,91,121,152,182,213,244,274,305,335,366}};



int nbSecBtw(struct med_tm * d1,struct med_tm * d2)
{
    struct med_tm * tmp;
    int neg=0;
    int i;
    int nb_day=0;
    unsigned int res;
    
    /* compute nb day betwwen years */
    if(d1->tm_year!=d2->tm_year)
    {
        if(d1->tm_year>d2->tm_year)
        {
            neg=1;
            tmp=d1;
            d1=d2;
            d2=tmp;
        }
        for(i=d1->tm_year;i<d2->tm_year;i++)
            nb_day+=nbDay_year(i);      
    }
    
    res=((((nb_day+month_tab[d2->tm_mon][is_leap_year(d2->tm_year)]+d2->tm_mday
            -month_tab[d1->tm_mon][is_leap_year(d1->tm_year)]-d1->tm_mday)*24+
            (d2->tm_hour-d1->tm_hour))*60+
            (d2->tm_min-d1->tm_min))*60+
            (d2->tm_sec-d1->tm_sec));
    if(neg)
        return -res;
    else
        return res;
}
#endif

int getDayOfWeek(int day,int month,int year)
{
    int result;
    int y;
    
    if(month>12) month=12;
    
    y=year%100;
    result=(5*y)/4;
    result+=month_code[month-1];
    result+=day;
    if(is_leap_year(year) && (month==1 || month==2))
        result-=1;
    while(result<=0)
        result+=7;
    result%=7;
    if(result==0)
        result=7;
    return result;
}

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
    valTime->tm_wday=pvToUser(pv_dt.tm_wday);
    valTime->tm_mday=pvToUser(pv_dt.tm_mday);
    valTime->tm_mon=pvToUser(pv_dt.tm_mon);
    valTime->tm_year=2000+pvToUser(pv_dt.tm_year);
    return MED_OK;
}

#define sendRTC(addr,val)        {int __ret;int __val=val; __ret=i2c_write(RTC_DEVICE, addr, (void*)(&__val), 1); \
                                        if(retVal<0) printk("[I2C - rtc] Error, writting (err:%d)\n",retVal);}

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
    
    wday=getDayOfWeek(newTime->tm_mday,newTime->tm_mon,newTime->tm_year);
    
    
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

MED_RET_T init_rtc(void)
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

    cwd=getDayOfWeek(valTime.tm_mday,valTime.tm_mon,valTime.tm_year);
    if(cwd!=valTime.tm_wday)
    {
        sendRTC(RTC_DW,((cwd&0x7) | 0x80));
    }
    
    rtc_getTime(&valTime);
    
    printk("[init] rtc :%02d/%02d/%04d %02d:%02d:%02d\n",valTime.tm_mday,valTime.tm_mon,valTime.tm_year
                ,valTime.tm_hour,valTime.tm_min,valTime.tm_sec);
    return MED_OK;
}

