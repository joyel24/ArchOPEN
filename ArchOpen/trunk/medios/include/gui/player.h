/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PLAYER_H
#define __PLAYER_H

#include <sys_def/stddef.h>
#include <gui/shell.h>
#include <gfx/kfont.h>

#define PLAYER_CFG_FILE SHELL_DIR"/player.cfg"

#define PLAYER_DEFAULT_PLAYER_FORMAT "$if(%title%|%title%|%filename%)\n%artist%\n%album%\n\n%bitrate% kb/s %samplerate% hz"
#define PLAYER_DEFAULT_PLAYLIST_FORMAT "$if(%title%|%artist% - %title%|%filename%)"
#define PLAYER_DEFAULT_VOLUME 80
#define PLAYER_DEFAULT_FONT STD8X13
#define PLAYER_DEFAULT_PLAYLIST_FONT STD6X9

#define PLAYER_SEEK_PRESS_TIME (HZ/2)

#define PLAYER_SEEK_FIXED_TIME (20*HZ)
#define PLAYER_SEEK_LENGTH_DIVISOR 100

extern char player_playlistFormat[1000];
extern int player_playlistFont;

void player_refresh(bool full);
void player_eventLoop();
void player_init();
void player_close();

#endif
