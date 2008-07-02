/*
* include/kernel/button.h
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

#ifndef __K_BUTTON_H
#define __K_BUTTON_H

#include <gui_user/button.h>

#include <gui/widget.h>

#define BUTTON_INTERNAL_WSPACE 3
#define BUTTON_INTERNAL_HSPACE 2

BUTTON button_create();
void button_destroy(BUTTON b);
void button_init(BUTTON b);
bool button_handleEvent(BUTTON b,int evt);
void button_paint(BUTTON b);
void button_autoSize(BUTTON b);
void button_setText(BUTTON b,char * txt);

#endif
