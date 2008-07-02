/*
* include/gui/trackbar.h
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

#ifndef __TRACKBAR_H
#define __TRACKBAR_H

#include <gui_user/widget.h>

typedef enum {TR_VAL_LEFT, TR_VAL_RIGHT} TRACKBAR_VAL_POSITION;

// members of the TRACKBAR object
#define TRACKBAR_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    TRACKBAR_CHANGEEVENT onChange;     \
    TRACKBAR_SETVALPOSITION setValPosition; \
    TRACKBAR_GETVALPOSITION getValPosition; \
    TRACKBAR_SETVALUE setValue;        \
    TRACKBAR_GETVALUE getValue;        \
    TRACKBAR_SETPARAM setParam;        \
    TRACKBAR_GETPARAM getParam;        \
    int value;                         \
    int minimum;                       \
    int maximum;                       \
    int increment;                     \
    int numTicks;                      \
    int minSpacing;                    \
    TRACKBAR_VAL_POSITION val_position;\
    int numValueDigits;

typedef void(*TRACKBAR_CHANGEEVENT)(void *);
typedef void(*TRACKBAR_SETVALPOSITION)(void *,int);
typedef int (*TRACKBAR_GETVALPOSITION)(void *);
typedef void(*TRACKBAR_SETVALUE)(void *,int);
typedef int (*TRACKBAR_GETVALUE)(void *);
typedef void(*TRACKBAR_SETPARAM)(void *,int,int,int);
typedef void(*TRACKBAR_GETPARAM)(void *,int*,int*,int*);
      
typedef struct {
    TRACKBAR_MEMBERS
} * TRACKBAR;

#endif
