/*
* include/gui/widget.h
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

#ifndef __WIDGET_H
#define __WIDGET_H

#include <sys_def/types.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

// Some constant
#define WIDGET_ORIENTATION_HORIZ 1
#define WIDGET_ORIENTATION_VERT  0

#define WIDGET_WRAP_ON           1
#define WIDGET_WRAP_OFF          0

typedef enum {WA_LEFT, WA_CENTER, WA_RIGHT} W_ALIGNMENT;

// default parameters
#define WIDGET_DEFAULT_FORECOLOR COLOR_BLACK
#define WIDGET_DEFAULT_BACKCOLOR COLOR_TRSP
#define WIDGET_DEFAULT_FILLCOLOR COLOR_WHITE
#define WIDGET_DEFAULT_FOCUSCOLOR COLOR_BLUE
#define WIDGET_DEFAULT_SELECTCOLOR COLOR_RED
#define WIDGET_DEFAULT_MARGIN 0

// members of the WIDGET object
#define WIDGET_MEMBERS                 \
    WIDGET_DESTROYER destroy;          \
    WIDGET_EVENTHANDLER handleEvent;   \
    WIDGET_PAINTHANDLER paint;         \
    WIDGET_RECTSETTER setRect;         \
    WIDGET_POSSETTER setPos;           \
    WIDGET_SIZESETTER setSize;         \
    WIDGET_AUTOSIZE autoSize;          \
    WIDGET_FONTSETTER setFont;         \
    WIDGET_ALIGNSETTER setAlign;       \
    WIDGET_UPDATER update;             \
    WIDGET_SIZEUPDATER updateSize;     \
    WIDGET_INTERNALSIZEGETTER getInternalSize; \
    int x;                             \
    int y;                             \
    int oldX;                          \
    int oldY;                          \
    int width;                         \
    int height;                        \
    int oldWidth;                      \
    int oldHeight;                     \
    int internalWidth;                 \
    int internalHeight;                \
    bool useMaxWidth;                  \
    int margin;                        \
    bool clearBackground;              \
    bool canFocus;                     \
    bool focused;                      \
    bool needSelection;                \
    int foreColor;                     \
    int backColor;                     \
    int fillColor;                     \
    int focusColor;                    \
    int font;                          \
    void * parent;                     \
    void *  nxt;                       \
    void * prev;                       \
    W_ALIGNMENT alignment;             \
    union { int tag; void * data; };


typedef bool(*WIDGET_EVENTHANDLER)(void *,int);
typedef void(*WIDGET_PAINTHANDLER)(void *);
typedef void(*WIDGET_DESTROYER)(void *);
typedef void(*WIDGET_RECTSETTER)(void *,int,int,int,int);
typedef void(*WIDGET_POSSETTER)(void *,int,int);
typedef void(*WIDGET_SIZESETTER)(void *,int,int);
typedef void(*WIDGET_AUTOSIZE)(void *);
typedef void(*WIDGET_FONTSETTER)(void *,int);
typedef void(*WIDGET_UPDATER)(void *, void *);
typedef void(*WIDGET_ALIGNSETTER)(void*,int);
typedef void(*WIDGET_SIZEUPDATER)(void*,int,int);
typedef void(*WIDGET_INTERNALSIZEGETTER)(void*,int*,int*);



typedef struct {
    WIDGET_MEMBERS
} * WIDGET;

#endif
