/*
*   kernel/target/DSC25/AV3XX/driver/time.c
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

#include <driver/time.h>
#include <driver/rtc_ST.h>

MED_RET_T time_archGet(struct med_tm * val_time)
{
    return rtc_getTime(val_time);
}

MED_RET_T time_archSet(struct med_tm * new_time)
{
    return rtc_setTime(new_time);
}

MED_RET_T time_archInit(void)
{
    return rtc_init();
}
