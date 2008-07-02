/*
* include/kernel/trackbar.h
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

#ifndef __K_TRACKBAR_H
#define __K_TRACKBAR_H

#include <gui_user/trackbar.h>

#include <gui/widget.h>

// paint parameters
#define TRACKBAR_SPACING 2 // space between trackbar and value
#define TRACKBAR_CURSOR_RATIO (1/1.618) // h/w ratio
#define TRACKBAR_MIN_SPACING 4 // minimum space btw 2 ticks 

TRACKBAR trackbar_create();
void trackbar_destroy(TRACKBAR t);
void trackbar_init(TRACKBAR t);
bool trackbar_handleEvent(TRACKBAR t,int evt);
void trackbar_paint(TRACKBAR t);
void trackbar_setValue(TRACKBAR t,int value);
int  trackbar_getValue(TRACKBAR t);
void trackbar_setParam(TRACKBAR t,int min,int max,int increment);
void trackbar_getParam(TRACKBAR t,int *min,int *max,int *increment);
void trackbar_setValPosition(TRACKBAR t,int val_position);
int  trackbar_getValPosition(TRACKBAR t);
void trackbar_autoSize(TRACKBAR t);

#endif
