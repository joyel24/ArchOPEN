/*
* include/kernel/date_time.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __K_DATE_TIME_H
#define __K_DATE_TIME_H

#include <gui_user/date_time.h>

#include <gui/widget.h>

DATE_TIME dateTime_create();
void dateTime_destroy(DATE_TIME dt);
void dateTime_init(DATE_TIME dt);
bool dateTime_handleEvent(DATE_TIME dt,int evt);
void dateTime_paint(DATE_TIME dt);
void dateTime_setValue(DATE_TIME dt,struct med_tm *);
void dateTime_getValue(DATE_TIME dt,struct med_tm *);
void dateTime_autoSize(DATE_TIME dt);
void dateTime_setTimeFormat(DATE_TIME dt,DT_TIME_FORMAT format);
void dateTime_setDateFormat(DATE_TIME dt,DT_DATE_FORMAT format);
void dateTime_setType(DATE_TIME dt,DT_TYPE type);
void dateTime_setFont(DATE_TIME dt,int f);
void dateTime_setPos(DATE_TIME dt, int x, int y);
void dateTime_update(DATE_TIME dt,WIDGET w);

/*internal fction*/
void dateTime_hour_onChange(SPINBOX t);
#endif
