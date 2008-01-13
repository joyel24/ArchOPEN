/*
* include/kernel/widgetmenu.h
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

#ifndef __K_WIDGETMENU_H
#define __K_WIDGETMENU_H

#include <gui_user/widgetmenu.h>

#include <gui/textmenu.h>
#include <gui/checkbox.h>
#include <gui/trackbar.h>
#include <gui/chooser.h>
#include <gui/button.h>

//*****************************************************************************
// WIDGETMENU_ITEM
//*****************************************************************************

#define WIDGETMENU_WIDGET_MARGIN 1 // margin around a widget in a menu item

WIDGETMENU_ITEM widgetMenuItem_create();
void widgetMenuItem_destroy(WIDGETMENU_ITEM mi);
void widgetMenuItem_init(WIDGETMENU_ITEM mi);
bool widgetMenuItem_handleEvent(WIDGETMENU_ITEM mi,int evt);
void widgetMenuItem_paint(WIDGETMENU_ITEM mi);
void widgetMenuItem_cfgToString(WIDGETMENU_ITEM mi,char * s);
void widgetMenuItem_cfgFromString(WIDGETMENU_ITEM mi,char * s);
void widgetMenuItem_rePaint(WIDGETMENU_ITEM mi);

//*****************************************************************************
// WIDGETMENU_BUTTON
//*****************************************************************************

WIDGETMENU_BUTTON widgetMenuButton_create();
void widgetMenuButton_destroy(WIDGETMENU_BUTTON mc);
void widgetMenuButton_init(WIDGETMENU_BUTTON mc);
void widgetMenuButton_cfgToString(WIDGETMENU_BUTTON mc,char * s);
void widgetMenuButton_cfgFromString(WIDGETMENU_BUTTON mc,char * s);

//*****************************************************************************
// WIDGETMENU_CHECKBOX
//*****************************************************************************

WIDGETMENU_CHECKBOX widgetMenuCheckbox_create();
void widgetMenuCheckbox_destroy(WIDGETMENU_CHECKBOX mc);
void widgetMenuCheckbox_init(WIDGETMENU_CHECKBOX mc);
void widgetMenuCheckbox_cfgToString(WIDGETMENU_CHECKBOX mc,char * s);
void widgetMenuCheckbox_cfgFromString(WIDGETMENU_CHECKBOX mc,char * s);

//*****************************************************************************
// WIDGETMENU_TRACKBAR
//*****************************************************************************

WIDGETMENU_TRACKBAR widgetMenuTrackbar_create();
void widgetMenuTrackbar_destroy(WIDGETMENU_TRACKBAR mt);
void widgetMenuTrackbar_init(WIDGETMENU_TRACKBAR mt);
void widgetMenuTrackbar_cfgToString(WIDGETMENU_TRACKBAR mt,char * s);
void widgetMenuTrackbar_cfgFromString(WIDGETMENU_TRACKBAR mt,char * s);

//*****************************************************************************
// WIDGETMENU_CHOOSER
//*****************************************************************************

WIDGETMENU_CHOOSER widgetMenuChooser_create();
void widgetMenuChooser_destroy(WIDGETMENU_CHOOSER mc);
void widgetMenuChooser_init(WIDGETMENU_CHOOSER mc);
void widgetMenuChooser_cfgToString(WIDGETMENU_CHOOSER mc,char * s);
void widgetMenuChooser_cfgFromString(WIDGETMENU_CHOOSER mc,char * s);

//*****************************************************************************
// WIDGETMENU
//*****************************************************************************

WIDGETMENU widgetMenu_create();
void widgetMenu_destroy(WIDGETMENU m);
void widgetMenu_init(WIDGETMENU m);
bool widgetMenu_handleEvent(WIDGETMENU m,int evt);
BUTTON widgetMenu_getButton(WIDGETMENU m,int index);
CHECKBOX widgetMenu_getCheckbox(WIDGETMENU m,int index);
TRACKBAR widgetMenu_getTrackbar(WIDGETMENU m,int index);
CHOOSER widgetMenu_getChooser(WIDGETMENU m,int index);
bool widgetMenu_cfgLoad(WIDGETMENU m,char * filename);
bool widgetMenu_cfgSave(WIDGETMENU m,char * filename);

#endif
