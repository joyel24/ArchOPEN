/*
* include/gui/label.h
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

#ifndef __LABEL_H
#define __LABEL_H

#include <gui_user/widget.h>

typedef enum {LA_CUT_WORD, LA_SMART_CUT} LABEL_AUTOCUT;

//*****************************************************************************
// LABEL
//*****************************************************************************

struct label_line {
    char * str;
    int width;
};

// members of the LABEL object
#define LABEL_MEMBERS          \
    /* we inherit from MENUITEM */     \
    WIDGET_MEMBERS                     \
                                       \
    char * caption;                    \
    struct label_line * lines;         \
    char * internal_str;               \
    int nbLines;                       \
    int interline;                     \
    int frameWidth;                    \
    int frameHeight;                   \
    LABEL_AUTOCUT cutWord;             \
    LABEL_SETTEXT setText;             \
    LABEL_GETMAXSIZE getMaxSize;       \
    LABEL_GETNBLINES getNbLines;       \
    LABEL_FRAMESIZESETTER setFrameSize; \
    LABEL_FRAMESIZEGETTER getFrameSize; \
    LABEL_PARAMSETTER setParam;        \
    LABEL_PARAMGETTER getParam;

typedef void(*LABEL_SETTEXT)(void *,char *);
typedef void(*LABEL_GETMAXSIZE)(void *,int *,int *);
typedef int(*LABEL_GETNBLINES)(void*);
typedef void(*LABEL_FRAMESIZESETTER)(void *,int,int);
typedef void(*LABEL_FRAMESIZEGETTER)(void *,int *,int *);
typedef void(*LABEL_PARAMSETTER)(void*,int,int,int);
typedef void(*LABEL_PARAMGETTER)(void*,int*,int*,int*);

typedef struct {
    LABEL_MEMBERS
} * LABEL;

#endif
