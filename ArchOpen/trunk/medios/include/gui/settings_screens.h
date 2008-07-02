/*
* include/gui/settings_screens.h
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

#ifndef __SETTINGS_SCREENS_H
#define __SETTINGS_SCREENS_H

#include <gui/icons.h>
#include <gui/widgetmenu.h>

void settings_initScreen(char * title,ICON logo,int * menu_x,int * menu_y);
void settings_evtLoop(WIDGETMENU widgetMenu,void(*sav_fct)(void),int evtHandle);
void settings_stopLoop(void);

#define TEST_VAR(V1,V2,RES) {if((V1)!=(V2)){V1=(V2);RES=1;}}

#endif
