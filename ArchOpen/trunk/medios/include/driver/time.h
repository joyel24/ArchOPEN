/*
*   include/kernel/time.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __DRIVER_TIME_H
#define __DRIVER_TIME_H

#include <sys_def/time.h>

MED_RET_T time_init(void);
void time_print(struct med_tm * now);

int time_getDayOfWeek(int day,int month,int year);
int time_getMaxDay_month(int month,int year);

void time_time2rtc_Offset(struct med_tm * val_time,int offset_val);
void time_rtc2time_Offset(struct med_tm * val_time,int offset_val);
        
MED_RET_T time_get(struct med_tm * val_time);
MED_RET_T time_set(struct med_tm * new_time);

/* arch specific function */
MED_RET_T time_archGet(struct med_tm * val_time);
MED_RET_T time_archSet(struct med_tm * new_time);
MED_RET_T time_archInit(void);

#endif
