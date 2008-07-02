/*
* include/gui/menu.h
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

#ifndef __MENU_H
#define __MENU_H

#include <gui_user/widget.h>
#include <gui_user/widgetlist.h>
#include <gui/scrollbar.h>

// forward declaration needed because MENU_ITEM has a MENU member and vice-versa
typedef struct MENU_STRUCT * MENU;

//*****************************************************************************
// MENU_ITEM
//*****************************************************************************

// members of the MENU_ITEM object
#define MENU_ITEM_MEMBERS                   \
    /* we inherit from WIDGET */            \
    WIDGET_MEMBERS                          \
                                            \
    char * caption;                         \
    MENU_ITEM_CAPTIONSETTER setCaption;     \
    MENU_ITEM_VISIBILITYTESTER isVisible;   \
    MENU parentMenu;                        \
    MENU subMenu;

typedef void(*MENU_ITEM_CAPTIONSETTER)(void *,char *);
typedef bool(*MENU_ITEM_VISIBILITYTESTER)(void*);

typedef struct {
    MENU_ITEM_MEMBERS
} * MENU_ITEM;

//*****************************************************************************
// MENU
//*****************************************************************************

// members of the MENU object
#define MENU_MEMBERS                   \
    /* we inherit from WIDGET */       \
    WIDGETLIST_MEMBERS                 \
                                       \
    MENU_CLICKEVENT onClick;           \
    MENU_ITEMADDER addItem;            \
    MENU_ITEMDEL   delItem;            \
    MENU_ITEMSCLEARER clearItems;      \
    MENU_INDEXFROMCAPTIONGETTER indexFromCaption;\
    MENU_ITEMFROMCAPTIONGETTER itemFromCaption; \
    MENU_POSUPDATER updatePos;         \
    MENU_ITEM index;                         \
    MENU_ITEM previousIndex;                 \
    MENU_ITEM topIndex;                      \
    int visibleCount;                  \
    WIDGETLIST menuList;               \
    int hasScrollBar;                  \
    struct scroll_bar scrollBar;       \
    bool packed;                       \
    MENU parentMenu;


typedef void(*MENU_CLICKEVENT)(void *,void *);
typedef void(*MENU_ITEMADDER)(void *,void *);
typedef int(*MENU_ITEMDEL)(void *,void *);
typedef void(*MENU_ITEMSCLEARER)(void *);
typedef int(*MENU_INDEXFROMCAPTIONGETTER)(void *,char *);
typedef void*(*MENU_ITEMFROMCAPTIONGETTER)(void *,char *);
typedef void (*MENU_POSUPDATER)(void*,bool);

struct MENU_STRUCT {
    MENU_MEMBERS
};

#endif
