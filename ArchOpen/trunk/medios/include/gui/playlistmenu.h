/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PLAYLISTMENU_H
#define __PLAYLISTMENU_H

#define PLAYLISTMENU_ACTIVEITEM_COLOR COLOR_RED

void playlistMenu_refresh();
void playlistMenu_build();
void playlistMenu_eventLoop();
void playlistMenu_init();
void playlistMenu_close();

#endif /* __PLAYLISTMENU_H */
