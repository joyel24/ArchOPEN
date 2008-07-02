/*
* include/gui/widgetmenu.h
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

#ifndef __WIDGETMENU_H
#define __WIDGETMENU_H

#include <gui_user/menu.h>
#include <gui_user/textmenu.h>
#include <gui_user/checkbox.h>
#include <gui_user/trackbar.h>
#include <gui_user/chooser.h>
#include <gui_user/button.h>
#include <gui_user/spinbox.h>
#include <gui_user/date_time.h>
#include <gui_user/widgetmenuline.h>

//*****************************************************************************
// WIDGETMENU_ITEM
//*****************************************************************************

#define NO_ALIGN_AT -1

// members of the WIDGETMENUITEM object
#define WIDGETMENU_ITEM_MEMBERS                            \
    /* we inherit from MENUITEM */                         \
    MENU_ITEM_MEMBERS                                      \
                                                           \
    WIDGETMENU_ITEM_CFGGETTER cfgToString;                 \
    WIDGETMENU_ITEM_CFGSETTER cfgFromString;               \
    WIDGETMENU_ITEM_REPAINT   rePaint;                     \
    WIDGETMENU_ITEM_ALIGNSETTER setAlignAt;                  \
    WIDGET widget;                                         \
    int tx,ty;                                             \
    int alignAt;                                           \
    bool cfgStored;                                        \
    char * cfgName;

typedef void(*WIDGETMENU_ITEM_CFGGETTER)(void *,char *);
typedef void(*WIDGETMENU_ITEM_CFGSETTER)(void *,char *);
typedef void(*WIDGETMENU_ITEM_REPAINT)(void *);
typedef void(*WIDGETMENU_ITEM_ALIGNSETTER)(void *,int);

typedef struct {
    WIDGETMENU_ITEM_MEMBERS
} * WIDGETMENU_ITEM;

//*****************************************************************************
// WIDGETMENU_BUTTON
//*****************************************************************************

// members of the WIDGETMENU_BUTTON object
#define WIDGETMENU_BUTTON_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    BUTTON button;

typedef struct {
    WIDGETMENU_BUTTON_MEMBERS
} * WIDGETMENU_BUTTON;

//*****************************************************************************
// WIDGETMENU_MENULINE
//*****************************************************************************

// members of the WIDGETMENU_MENULINE object
#define WIDGETMENU_MENULINE_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    WIDGETMENULINE menuLine;

typedef struct {
    WIDGETMENU_MENULINE_MEMBERS
} * WIDGETMENU_MENULINE;

//*****************************************************************************
// WIDGETMENU_DATE_TIME
//*****************************************************************************

// members of the WIDGETMENU_DATE_TIME object
#define WIDGETMENU_DATE_TIME_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    DATE_TIME dateTime;

typedef struct {
    WIDGETMENU_DATE_TIME_MEMBERS
} * WIDGETMENU_DATE_TIME;

//*****************************************************************************
// WIDGETMENU_CHECKBOX
//*****************************************************************************

// members of the WIDGETMENU_CHECKBOX object
#define WIDGETMENU_CHECKBOX_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    CHECKBOX checkbox;

typedef struct {
    WIDGETMENU_CHECKBOX_MEMBERS
} * WIDGETMENU_CHECKBOX;

//*****************************************************************************
// WIDGETMENU_TRACKBAR
//*****************************************************************************

// members of the WIDGETMENU_TRACKBAR object
#define WIDGETMENU_TRACKBAR_MEMBERS                        \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    TRACKBAR trackbar;

typedef struct {
    WIDGETMENU_TRACKBAR_MEMBERS
} * WIDGETMENU_TRACKBAR;

//*****************************************************************************
// WIDGETMENU_CHOOSER
//*****************************************************************************

// members of the WIDGETMENU_CHOOSER object
#define WIDGETMENU_CHOOSER_MEMBERS                         \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    CHOOSER chooser;

typedef struct {
    WIDGETMENU_CHOOSER_MEMBERS
} * WIDGETMENU_CHOOSER;


//*****************************************************************************
// WIDGETMENU_SPINBOX
//*****************************************************************************

// members of the WIDGETMENU_SPINBOX object
#define WIDGETMENU_SPINBOX_MEMBERS                         \
    /* we inherit from WIDGETMENU_ITEM */                  \
    WIDGETMENU_ITEM_MEMBERS                                \
                                                           \
    SPINBOX spinbox;

typedef struct {
    WIDGETMENU_SPINBOX_MEMBERS
} * WIDGETMENU_SPINBOX;



//*****************************************************************************
// WIDGETMENU
//*****************************************************************************

// members of the WIDGETMENU object
#define WIDGETMENU_MEMBERS                                 \
    /* we inherit from TEXTMENU */                         \
    TEXTMENU_MEMBERS                                       \
                                                           \
    WIDGETMENU_ALIGNSETTER setAlignAt;                     \
    int alignAt;                                           \
                                                           \
    WIDGETMENU_BUTTONGETTER getButton;                     \
    WIDGETMENU_SPINBOXGETTER getSpinbox;                   \
    WIDGETMENU_CHECKBOXGETTER getCheckbox;                 \
    WIDGETMENU_TRACKBARGETTER getTrackbar;                 \
    WIDGETMENU_CHOOSERGETTER getChooser;                   \
    WIDGETMENU_CFGLOADER cfgLoad;                          \
    WIDGETMENU_CFGSAVER cfgSave;

typedef BUTTON(*WIDGETMENU_BUTTONGETTER)(void *,int);
typedef SPINBOX(*WIDGETMENU_SPINBOXGETTER)(void *,int);
typedef CHECKBOX(*WIDGETMENU_CHECKBOXGETTER)(void *,int);
typedef TRACKBAR(*WIDGETMENU_TRACKBARGETTER)(void *,int);
typedef CHOOSER(*WIDGETMENU_CHOOSERGETTER)(void *,int);
typedef bool(*WIDGETMENU_CFGLOADER)(void *,char *);
typedef bool(*WIDGETMENU_CFGSAVER)(void *,char *);
typedef void(*WIDGETMENU_ALIGNSETTER)(void *,int);

typedef struct {
    WIDGETMENU_MEMBERS
} * WIDGETMENU;

#endif
