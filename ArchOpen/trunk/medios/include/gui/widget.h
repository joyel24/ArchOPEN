/*
* include/kernel/widget.h
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

#ifndef __K_WIDGET_H
#define __K_WIDGET_H

#include <gui/arch_widget.h>

#include <gui_user/widget.h>

#include <gfx/graphics.h>

//#define DEBUG_WIDGET
//#define DEBUG_WIDGET_CREATE_DESTROY

#ifdef DEBUG_WIDGET
#define DEBUGWI(s...)   printk(s)
#else
#define DEBUGWI(s...)
#endif

#ifdef DEBUG_WIDGET_CREATE_DESTROY
#define DEBUGWI_CD(s...)   printk(s)
#else
#define DEBUGWI_CD(s...)
#endif

WIDGET widget_create();
void widget_destroy(WIDGET w);
void widget_init(WIDGET w);
bool widget_handleEvent(WIDGET w,int evt);
void widget_paint(WIDGET w);
void widget_setRect(WIDGET w,int x,int y,int width,int height);
void widget_setPos(WIDGET w,int x,int y);
void widget_setSize(WIDGET w,int width,int height);
void widget_getInternalSize(WIDGET w,int * width,int * height);
void widget_updateSize(WIDGET w,int width,int height);
void widget_autoSize(WIDGET w);
void widget_setFont(WIDGET w, int f);
void widget_update(WIDGET parent, WIDGET child);
void widget_setAlign(WIDGET w, W_ALIGNMENT alignment);

#endif
