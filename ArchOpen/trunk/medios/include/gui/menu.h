/*
* include/kernel/menu.h
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

#ifndef __K_MENU_H
#define __K_MENU_H

#include <gui_user/menu.h>

#include <gui/widget.h>
#include <gui/widgetlist.h>



//*****************************************************************************
// MENU_ITEM
//*****************************************************************************

MENU_ITEM menuItem_create();
void menuItem_destroy(MENU_ITEM mi);
void menuItem_init(MENU_ITEM mi);
bool menuItem_isVisible(MENU_ITEM mi);
void menuItem_setPos(MENU_ITEM mi,int x,int y);
void menuItem_setSize(MENU_ITEM mi,int width,int height);
void menuItem_setCaption(MENU_ITEM mi,char * caption);
void menuItem_autoSize(MENU_ITEM mi);

//*****************************************************************************
// MENU
//*****************************************************************************

MENU menu_create();
void menu_destroy(MENU m);
void menu_init(MENU m);
bool menu_handleEvent(MENU m,int evt);
void menu_paint(MENU m);
void menu_addItem(MENU m, MENU_ITEM item);
int menu_delItem(MENU m, MENU_ITEM item);
void menu_clearItems(MENU m);
int menu_indexOf(MENU m, MENU_ITEM item);
int menu_indexFromCaption(MENU m, char * caption);
MENU_ITEM menu_itemFromCaption(MENU m,char * caption);
void menu_update(MENU parent,MENU_ITEM child);
void menu_setSize(MENU m,int width,int height);
void menu_setFont(MENU m, int font);
void menu_setPos(MENU m,int x,int y);
void menu_updatePos(MENU m,bool fast);
void menu_autoSize(MENU m);
void menu_setAlign(MENU m, W_ALIGNMENT alignment);

#endif
