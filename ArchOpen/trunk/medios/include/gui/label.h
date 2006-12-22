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

LABEL label_create();
void label_init(LABEL l);
void label_paint(LABEL l);

#endif
