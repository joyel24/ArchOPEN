/*
* include/kernel/widgetmenuline.h
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

#ifndef __K_WIDGETMENULINE_H
#define __K_WIDGETMENULINE_H

#include <gui_user/widgetmenuline.h>

#include <gui/widget.h>

WIDGETMENULINE widgetMenuLine_create();
void widgetMenuLine_destroy(WIDGETMENULINE ml);
void widgetMenuLine_init(WIDGETMENULINE ml);
bool widgetMenuLine_handleEvent(WIDGETMENULINE ml,int evt);
void widgetMenuLine_paint(WIDGETMENULINE ml);
void widgetMenuLine_autoSize(WIDGETMENULINE ml);
void widgetMenuLine_addWidget(WIDGETMENULINE ml,WIDGET w);
int  widgetMenuLine_delWidget(WIDGETMENULINE ml,WIDGET w);
void widgetMenuLine_clearWidgets(WIDGETMENULINE ml);
void widgetMenuLine_update(WIDGETMENULINE ml,WIDGET w);
void widgetMenuLine_setPos(WIDGETMENULINE ml,int x,int y);
#endif
