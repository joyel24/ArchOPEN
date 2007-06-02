/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SOUND_H
#define __SOUND_H

#include <snd/playlist.h>

typedef enum {SS_STOPPED,SS_PLAYING,SS_PAUSED} SOUND_STATE;

void sound_play(bool discard);

void sound_prevTrack(bool discard);
void sound_nextTrack(bool discard);

void sound_trackEnd();

void sound_pause(bool paused);
void sound_seek(int time);
void sound_setVolume(int volume);

void sound_init();
void sound_start(void);
void sound_stop(void);

void sound_playFile(char * fName);

extern PLAYLIST_ITEM * sound_activeItem;

extern SOUND_STATE sound_state;

#endif


