/*
*   kernel/target/DM320/AV5XX/driver/time.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>

#include <sys_def/errors.h>

#include <driver/time.h>
#include <driver/rtc_ST.h>

int offset;

MED_RET_T time_archGet(struct med_tm * val_time)
{
    
    MED_RET_T ret_val=rtc_getTime(val_time);
    if(ret_val != MED_OK)
        return -ret_val;
    /*
    val_time->tm_year+=5;
    time_rtc2time_Offset(val_time,offset);*/
    return MED_OK;
}

MED_RET_T time_archSet(struct med_tm * new_time)
{
    return MED_ERROR;
}

MED_RET_T time_archInit(void)
{    
    MED_RET_T ret_val=rtc_init();;
    
    if(ret_val != MED_OK)
        return -ret_val;
    
    offset=inl(0x12ff04);
    return MED_OK;
}
