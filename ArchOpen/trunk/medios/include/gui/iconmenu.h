/*
* include/kernel/iconmenu.h
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

#ifndef __K_ICONMENU_H
#define __K_ICONMENU_H

#include <gui_user/iconmenu.h>

#include <gui/menu.h>

//*****************************************************************************
// ICONMENU_ITEM
//*****************************************************************************

// paint parameters
#define ICONMENU_ITEM_SPACING 2 // space between icon and text in IMIP_LEFT

ICONMENU_ITEM iconMenuItem_create();
void iconMenuItem_destroy(ICONMENU_ITEM mi);
void iconMenuItem_init(ICONMENU_ITEM mi);
void iconMenuItem_paint(ICONMENU_ITEM mi);
void iconMenuItem_setSize(ICONMENU_ITEM mi,int width, int height);
void iconMenuItem_setPos(ICONMENU_ITEM mi,int x,int y);
void iconMenuItem_autoSize(ICONMENU_ITEM mi);
void iconMenuItem_setIPosition(ICONMENU_ITEM mi,IM_ICONPOSITION compo_type);

//*****************************************************************************
// ICONMENU
//*****************************************************************************

ICONMENU iconMenu_create();
void iconMenu_destroy(ICONMENU m);
void iconMenu_init(ICONMENU m);
bool iconMenu_handleEvent(ICONMENU m,int evt);
void iconMenu_addItem(ICONMENU m, ICONMENU_ITEM item);
void iconMenu_setCaptionType(ICONMENU m,IM_CAPTION type);
void iconMenu_paint(ICONMENU m);
void iconMenu_setItemSize(ICONMENU m, int width, int height);
void iconMenu_updatePos(ICONMENU m, bool fast);
void iconMenu_setStatusFont(ICONMENU m, int font);
        
#endif
