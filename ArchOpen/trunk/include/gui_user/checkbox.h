/*
* include/gui/checkbox.h
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

#ifndef __CHECKBOX_H
#define __CHECKBOX_H

#include <gui_user/widget.h>

// members of the CHECKBOX object
#define CHECKBOX_MEMBERS               \
    /* we inherit from WIDGET */       \
    WIDGET_MEMBERS                     \
                                       \
    CHECKBOX_CHANGEEVENT onChange;     \
    CHECKBOX_STATESETTER setState;     \
    CHECKBOX_STATEGETTER getState;     \
    int clickOnRightLeft;              \
    bool checked;

typedef void(*CHECKBOX_CHANGEEVENT)(void *);
typedef void(*CHECKBOX_STATESETTER)(void *,int);
typedef int(*CHECKBOX_STATEGETTER)(void *);


typedef struct {
    CHECKBOX_MEMBERS
} * CHECKBOX;

#endif
