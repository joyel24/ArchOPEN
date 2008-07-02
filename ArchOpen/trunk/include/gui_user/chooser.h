/*
* include/gui/chooser.h
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

#ifndef __CHOOSER_H
#define __CHOOSER_H

#include <gui_user/widget.h>

// members of the CHECKBOX object
#define CHOOSER_MEMBERS                 \
    /* we inherit from WIDGET */        \
    WIDGET_MEMBERS                      \
                                        \
    CHOOSER_CHANGEEVENT onChange;       \
    CHOOSER_ITEMLISTSETTER setItemList; \
    CHOOSER_INDEXSETTER setIndex;       \
    CHOOSER_INDEXGETTER getIndex;       \
    CHOOSER_EVTSETTER setHandleEvt;     \
    CHOOSER_EVTGETTER getHandleEvt;     \
    CHOOSER_PARAMSETTER setParam;       \
    CHOOSER_PARAMGETTER getParam;       \
    int orientation;                    \
    int wrap;                           \
    char ** items;                      \
    int evt_prev,evt_nxt;               \
    int itemCount;                      \
    int index;

typedef void(*CHOOSER_CHANGEEVENT)(void *);
typedef void(*CHOOSER_ITEMLISTSETTER)(void *,void*,int);
typedef void(*CHOOSER_INDEXSETTER)(void *,int);
typedef int(*CHOOSER_INDEXGETTER)(void*);
typedef void(*CHOOSER_EVTSETTER)(void *,int,int);
typedef void(*CHOOSER_EVTGETTER)(void *,int *,int *);
typedef void(*CHOOSER_PARAMSETTER)(void *, int, int);
typedef void(*CHOOSER_PARAMGETTER)(void *,int *,int *);


typedef struct {
    CHOOSER_MEMBERS
} * CHOOSER;

#endif
