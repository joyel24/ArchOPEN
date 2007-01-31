/*
*   kernel/target/DM270/PMA/driver/time.c
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
    val_time->tm_sec=0;
    val_time->tm_min=0;
    val_time->tm_hour=0;
    val_time->tm_mday=0;
    val_time->tm_wday=0;
    val_time->tm_mon=0;
    val_time->tm_year=0;
    return MED_OK;
}

MED_RET_T time_archSet(struct med_tm * new_time)
{
    return MED_OK;
}

MED_RET_T time_archInit(void)
{    
    return MED_OK;
}
