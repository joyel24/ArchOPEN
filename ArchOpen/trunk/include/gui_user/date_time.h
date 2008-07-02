/*
* include/gui/date_time.h
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

#ifndef __DATE_TIME_H
#define __DATE_TIME_H

#include <gui_user/widget.h>
#include <gui_user/widgetmenuline.h>
#include <gui_user/spinbox.h>
#include <gui_user/label.h>

#include <sys_def/time.h>

typedef enum {DATE_SETTING, TIME_SETTING} DT_TYPE;

// members of the DATE_TIME object
#define DATE_TIME_MEMBERS              \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    /*we include sub widgets*/         \
    WIDGETMENULINE menuline;           \
    SPINBOX spinA; /*hour or day*/     \
    SPINBOX spinB; /*min or month*/    \
    SPINBOX spinC; /*sec or year*/     \
    LABEL   sep_txtA;                  \
    LABEL   sep_txtB;                  \
                                       \
    DATE_TIME_SETVALUE    setValue;    \
    DATE_TIME_GETVALUE    getValue;    \
    DATE_TIME_TYPESETTER  setType;     \
    DATE_TIME_TIMEFORMATSETTER setTimeFormat;\
    DATE_TIME_DATEFORMATSETTER setDateFormat;\
    bool isPm;                         \
    int currentH;                      \
    DT_TYPE type;                      \
    DT_DATE_FORMAT date_format;        \
    DT_TIME_FORMAT time_format;

/* Specific / internal data
 currentH: save current Time in 24H format
*/

typedef void(*DATE_TIME_SETVALUE)(void *,struct med_tm *);
typedef void(*DATE_TIME_GETVALUE)(void *,struct med_tm *);
typedef void(*DATE_TIME_TYPESETTER)(void *,DT_TYPE);
typedef void(*DATE_TIME_TIMEFORMATSETTER)(void *,DT_TIME_FORMAT);
typedef void(*DATE_TIME_DATEFORMATSETTER)(void *,DT_DATE_FORMAT);

typedef struct {
    DATE_TIME_MEMBERS
} * DATE_TIME;

#endif
