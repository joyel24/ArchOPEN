/*
* include/kernel/chooser.h
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

#ifndef __K_CHOOSER_H
#define __K_CHOOSER_H

#include <gui_user/chooser.h>

#include <gui/widget.h>

CHOOSER chooser_create();
void chooser_destroy(CHOOSER c);
void chooser_init(CHOOSER c);
bool chooser_handleEvent(CHOOSER c,int evt);
void chooser_paint(CHOOSER c);
void chooser_autoSize(CHOOSER c);
void chooser_setItemList(CHOOSER c,char ** items,int itemCount);
void chooser_setIndex(CHOOSER c,int index);
int chooser_getIndex(CHOOSER c);
void chooser_setHandleEvt(CHOOSER c, int evt_prev, int evt_nxt);
void chooser_getHandleEvt(CHOOSER c, int * evt_prev, int * evt_nxt);
void chooser_setParam(CHOOSER c, int orientation, int wrap);
void chooser_getParam(CHOOSER c, int * orientation, int * wrap);

#endif
