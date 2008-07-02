/*
* include/kernel/label.h
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

#ifndef __K_LABEL_H
#define __K_LABEL_H

#include <gui_user/label.h>

#include <gui/widget.h>

//*****************************************************************************
// LABEL
//*****************************************************************************

#define LABEL_MAX_LINE 20

LABEL label_create();
void label_init(LABEL l);
void label_destroy(LABEL l);
void label_paint(LABEL l);
int label_getNbLines(LABEL l);
void label_setText(LABEL l,char * txt);
void label_getMaxSize(LABEL l,int * w,int * h);
void label_setFrameSize(LABEL l,int width, int height);
void label_getFrameSize(LABEL l,int * width, int * height);
void label_setParam(LABEL l, int alignment,int cutWord,int interline);
void label_getParam(LABEL l, int * alignment,int * cutWord,int * interline);
void label_setPos(LABEL l, int x, int y);

int label_cutBigLine(LABEL l,int charWidth,char ** endPtr);

void label_autoSize(LABEL l);

#endif
