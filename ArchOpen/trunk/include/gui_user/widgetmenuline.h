/*
* include/gui/widgetmenuline.h
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

#ifndef __WIDGETMENULINE_H
#define __WIDGETMENULINE_H

#include <gui_user/widget.h>
#include <gui_user/widgetlist.h>

// members of the WIDGETMENULINE object
#define WIDGETMENULINE_MEMBERS              \
    /* we inherit from WIDGET */       \
    WIDGETLIST_MEMBERS                 \
                                       \
                                       \
    /*we include sub widgets*/         \
    int spacing;                       \
    bool hasFocus;                     \
    bool hasSelect;

/* Specific / internal data
 hasFocus: true when the widget is selected => widget catches more events
false => widget waits for ON key to be pressed to be seleced
*/

typedef struct {
    WIDGETMENULINE_MEMBERS
} * WIDGETMENULINE;

#endif
