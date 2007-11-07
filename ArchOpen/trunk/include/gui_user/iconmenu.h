/*
* include/gui/iconmenu.h
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

#ifndef __ICONMENU_H
#define __ICONMENU_H

#include<gui_user/menu.h>

#include<sys_def/graphics.h>

typedef enum {IMIP_TOP, IMIP_LEFT} IM_ICONPOSITION;
typedef enum {IM_HAS_CAPTION, IM_NO_CAPTION} IM_CAPTION;

//*****************************************************************************
// ICONMENU_ITEM
//*****************************************************************************

// members of the TEXTMENUITEM object
#define ICONMENU_ITEM_MEMBERS          \
    /* we inherit from MENUITEM */     \
    MENU_ITEM_MEMBERS                  \
                                       \
    BITMAP icon;                       \
    IM_CAPTION itemCaption;            \
    IM_ICONPOSITION iconPosition;

typedef struct {
    ICONMENU_ITEM_MEMBERS
} * ICONMENU_ITEM;

//*****************************************************************************
// ICONMENU
//*****************************************************************************

// members of the ICONMENUMENU object
#define ICONMENU_MEMBERS               \
    /* we inherit from MENU */         \
    MENU_MEMBERS                       \
                                       \
    int itemWidth;                     \
    int itemHeight;                    \
    IM_CAPTION itemCaption;            \
    int btm_line_h;

typedef struct {
    ICONMENU_MEMBERS
} * ICONMENU;

#endif
