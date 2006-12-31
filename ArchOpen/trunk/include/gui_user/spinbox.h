/*
* include/gui/spinbox.h
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

#ifndef __SPINBOX_H
#define __SPINBOX_H

#include <gui_user/widget.h>

// members of the TRACKBAR object
#define SPINBOX_MEMBERS                \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    SPINBOX_CHANGEEVENT onChange;      \
    SPINBOX_SETPARAM    setParam;      \
    SPINBOX_SETFONT     setFont;       \
    SPINBOX_SETPOS      setPos;        \
    SPINBOX_SETVALUE    setValue;      \
    SPINBOX_GETVALUE    getValue;      \
    int value;                         \
    int minimum;                       \
    int maximum;                       \
    int increment;                     \
    int nbDigits;

typedef void(*SPINBOX_CHANGEEVENT)(void *);
typedef void(*SPINBOX_SETPARAM)(void *,int,int,int,int);
typedef void(*SPINBOX_SETFONT)(void *,int);
typedef void(*SPINBOX_SETPOS)(void *,int,int);
typedef void(*SPINBOX_SETVALUE)(void *,int);
typedef int(*SPINBOX_GETVALUE)(void *);

typedef struct {
    SPINBOX_MEMBERS
} * SPINBOX;

#endif
