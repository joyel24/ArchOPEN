/*
* include/kernel/spinbox.h
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

#ifndef __K_SPINBOX_H
#define __K_SPINBOX_H

#include <gui_user/spinbox.h>

#include <gui/widget.h>

SPINBOX spinbox_create();
void spinbox_destroy(SPINBOX t);
void spinbox_init(SPINBOX t);
bool spinbox_handleEvent(SPINBOX t,int evt);
void spinbox_paint(SPINBOX t);
void spinbox_setValue(SPINBOX t,int value);
void spinbox_setParam(SPINBOX t,int minimum,int maximum,int increment,int nbDigits);
void spinbox_getParam(SPINBOX t,int * minimum,int * maximum,int * increment,int * nbDigits);
void spinbox_setWrap(SPINBOX t,int wrap_value);
int  spinbox_getWrap(SPINBOX t);
int  spinbox_getValue(SPINBOX t);
void spinbox_autoSize(SPINBOX t);

#endif
