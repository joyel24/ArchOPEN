/*
* include/gui/textmenu.h
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

#ifndef __TEXTMENU_H
#define __TEXTMENU_H

#include <gui_user/menu.h>

//*****************************************************************************
// TEXTMENU_ITEM
//*****************************************************************************

// members of the TEXTMENUITEM object
#define TEXTMENU_ITEM_MEMBERS          \
    /* we inherit from MENUITEM */     \
    MENU_ITEM_MEMBERS                  \
                                       \
    int txtX,txtY;

typedef struct {
    TEXTMENU_ITEM_MEMBERS
} * TEXTMENU_ITEM;

//*****************************************************************************
// TEXTMENU
//*****************************************************************************

// members of the TEXTMENU object
#define TEXTMENU_MEMBERS               \
    /* we inherit from MENU */         \
    MENU_MEMBERS                       \
                                       \
    TEXTMENU_FOCUSSETTER setFocus;

typedef void(*TEXTMENU_FOCUSSETTER)(void *,void *);

typedef struct {
    TEXTMENU_MEMBERS
} * TEXTMENU;

#endif
