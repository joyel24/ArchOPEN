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

#include <sys_def/errors.h>

#include <kernel/kernel.h>

#include <driver/time.h>

int month_code[12] = {6,2,2,5,0,3,5,1,4,6,2,4};

#define isLeap(yr) (((yr) <= 1752 ? !((yr) % 4) : \
    (!((yr) % 4) && ((yr) % 100)) || !((yr) % 400))  ? 1:0)

#define nbSec_year(yr) (isLeap(yr)?31622400:31536000)

int month_Sectab[2][13] = {
    {0,2678400,2419200,2678400,2592000,2678400,2592000,2678400,2678400,2592000,2678400,2592000,2678400},
    {0,2678400,2505600,2678400,2592000,2678400,2592000,2678400,2678400,2592000,2678400,2592000,2678400}
};
    
int month_tab[2][13] = {
    {0,31,28,31,30,31,30,31,31,30,31,30,31},
    {0,31,29,31,30,31,30,31,31,30,31,30,31}
};    
        
   
//#define DEBUG_TIME

#ifdef DEBUG_TIME
#define TIME_PRINT(s...)   printk(s)
#else
#define TIME_PRINT(s...)
#endif
    
void normalize(int * val,int * upperVal,int lower,int upper)
{    
    while(1)
    {
        if(*val<lower)
        {
            TIME_PRINT("below: %d %d-%d\n",*val,lower,upper);
            *val=upper-(lower-*val-1);
            *upperVal=*upperVal-1;      
        }
        else if(*val>upper)
        {
            TIME_PRINT("upper %d %d\n",*val,upper);
            *val=lower + (*val-upper-1);
            *upperVal=*upperVal+1;      
        }
        else
            break;
    }
}
    
int calcYear(int org,int * offset)
{
    int sig=1;
    if(*offset==0)
        return org;
    if(*offset<0) { sig=-1; *offset=-*offset; org--;}
    
    while(*offset>0)
    {
        *offset=*offset-nbSec_year(org);
        org=org+sig;
    }

    if(*offset<0)
    {
        org=org-sig;
        *offset+=nbSec_year(org);
    }

    if(sig==-1)
    {
        org=org-sig;
        *offset=-*offset;
    }

    return org;
}



int calcMon(int *year,int org,int * offset)
{
    int sig=1;
    if(*offset==0)
        return org;
    if(*offset<0) { sig=-1; *offset=-*offset; org--; normalize(&org,year,1,12);}
    
    while(*offset>0)
    {
        *offset=*offset-month_Sectab[isLeap(*year)][org];
        org=org+sig;
        normalize(&org,year,1,12);
    }

    if(*offset<0)
    {
        org=org-sig;
        normalize(&org,year,1,12);
        *offset+=month_Sectab[isLeap(*year)][org];        
    }

    if(sig==-1)
    {
        org=org-sig;
        normalize(&org,year,1,12);        
        *offset=-*offset;
    }

    return org;
}

int calcOther(int org, int divVal,int * offset)
{
    int nb=*offset/divVal;
    if(*offset==0)
        return org;
    *offset=*offset-(nb*divVal);
    return org+nb;    
}

void normalizeDay(struct med_tm * val_time)
{    
    int prevMon,prevYear,nxtMon,nxtYear;
    while(1)
    {
        prevMon=val_time->tm_mon-1;
        if(prevMon<1)
        {
            prevYear=val_time->tm_year-1;
            prevMon=12;
        }
        else
            prevYear=val_time->tm_year;
        
        nxtMon=val_time->tm_mon+1;
        if(nxtMon>12)
        {
            nxtYear=val_time->tm_year+1;
            nxtMon=1;
        }
        else
            nxtYear=val_time->tm_year;
        
        if(val_time->tm_mday<1)
        {
            TIME_PRINT("below: %d %d\n",val_time->tm_mday,1);
            val_time->tm_mday=month_tab[isLeap(prevYear)][prevMon]+val_time->tm_mday; // 1-day-1
            val_time->tm_mon--;      
        }
        else if(val_time->tm_mday>month_tab[isLeap(val_time->tm_year)][val_time->tm_mon])
        {
            TIME_PRINT("upper %d %d\n",val_time->tm_mday,month_tab[isLeap(val_time->tm_year)][val_time->tm_mon]);
            val_time->tm_mday=1+(val_time->tm_mday
                    -month_tab[isLeap(val_time->tm_year)][val_time->tm_mon]-1); // day-12-1
            val_time->tm_mon++;      
        }
        else
            break;
    }
}

int time_getMaxDay_month(int month,int year)
{
    if(month<1 || month >12) return 0;
    return month_tab[isLeap(year)][month];
}

void time_calcOffset(struct med_tm * val_time,int offset_val)
{
    int offset=offset_val;
    /* apply offset */
    TIME_PRINT("offset= %x\n",offset);
    val_time->tm_year=calcYear(val_time->tm_year,&offset);
    TIME_PRINT("y=%d offset= %x\n",val_time->tm_year,offset);
    val_time->tm_mon=calcMon(&val_time->tm_year,val_time->tm_mon,&offset);
    TIME_PRINT("m=%d (y=%d)offset= %x\n",val_time->tm_mon,val_time->tm_year,offset);
    val_time->tm_mday=calcOther(val_time->tm_mday,86400,&offset);
    TIME_PRINT("d=%d offset= %x\n",val_time->tm_mday,offset);
    val_time->tm_hour=calcOther(val_time->tm_hour,3600,&offset);
    TIME_PRINT("h=%d offset= %x\n",val_time->tm_hour,offset);
    val_time->tm_min=calcOther(val_time->tm_min,60,&offset);
    TIME_PRINT("m=%d offset= %x\n",val_time->tm_min,offset);
    val_time->tm_sec=val_time->tm_sec+offset;
    TIME_PRINT("s=%d\n",val_time->tm_sec);
    /* normalize */
    normalize(&val_time->tm_sec,&val_time->tm_min,0,59);
    normalize(&val_time->tm_min,&val_time->tm_hour,0,59);
    normalize(&val_time->tm_hour,&val_time->tm_mday,0,23);
    normalizeDay(val_time);   
    normalize(&val_time->tm_mon,&val_time->tm_year,1,12);
}

void time_time2rtc_Offset(struct med_tm * val_time,int offset_val)
{
    time_calcOffset(val_time,-offset_val);    
}

void time_rtc2time_Offset(struct med_tm * val_time,int offset_val)
{    
    time_calcOffset(val_time,offset_val);
}

MED_RET_T time_get(struct med_tm * val_time)
{
    return time_archGet(val_time);
}

MED_RET_T time_set(struct med_tm * new_time)
{
    return time_archSet(new_time);
}

int time_getDayOfWeek(int day,int month,int year)
{
    int result;
    int y;
    
    if(month>12) month=12;
    
    y=year%100;
    result=(5*y)/4;
    result+=month_code[month-1];
    result+=day;
    if(isLeap(year) && (month==1 || month==2))
        result-=1;
    while(result<=0)
        result+=7;
    result%=7;
    if(result==0)
        result=7;
    return result;
}
void time_print(struct med_tm * now)
{
    printk("%02d:%02d:%02d(%02d) %02d/%02d/%04d (%d)",now->tm_hour,now->tm_min,now->tm_sec,now->tm_ms,
           now->tm_mday,now->tm_mon,now->tm_year,now->tm_wday);
}

MED_RET_T time_init(void)
{
    MED_RET_T ret_val=time_archInit();
    if(ret_val<0)
        printk("[init] time - Error\n");
    else
        printk("[init] time done\n");
    return ret_val;
}
