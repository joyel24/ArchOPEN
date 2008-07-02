/*
*   kernel/target/DM270/AV4XX/driver/time.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <lib/string.h>

#include <kernel/io.h>
#include <kernel/kernel.h>

#include <sys_def/errors.h>

#include <driver/time.h>
#include <driver/rtc_ST.h>

int offset;



MED_RET_T time_archGet(struct med_tm * val_time)
{
    MED_RET_T ret_val=rtc_getTime(val_time);
    if(ret_val != MED_OK)
        return -ret_val;
    val_time->tm_year+=4;
    time_rtc2time_Offset(val_time,offset);
    val_time->tm_wday=time_getDayOfWeek(val_time->tm_mday,val_time->tm_mon,val_time->tm_year);
    return MED_OK;
}

MED_RET_T time_archSet(struct med_tm * new_time)
{
#if 0
    struct med_tm tmp_time;
    struct med_tm mod_time;
    memcpy(&mod_time,new_time,sizeof(struct med_tm));
    
    MED_RET_T ret_val=rtc_getTime(&tmp_time);
    
    if(ret_val != MED_OK)
        printk("Can't get cur rtc time\n");
    printk("RTC time: ");
    time_print(&tmp_time);
    printk("\n");
    
    printk("New time: ");
    time_print(new_time);
    printk("-");
    time_print(&mod_time);
    printk("\n");
    
    time_time2rtc_Offset(&mod_time,offset);
    mod_time.tm_year-=4;
    printk("New rtc: ");
    time_print(&mod_time);
    printk("\n");
#endif
    //rtc_setTime(&mod_time);
    return -MED_ERROR;
}

MED_RET_T time_archInit(void)
{
    MED_RET_T ret_val=rtc_init();       
    if(ret_val != MED_OK)
        return -ret_val;
    
    offset=(inl(0x10FF04)>>8)|(inb(0x10FF08)<<24);
    return MED_OK;
}
