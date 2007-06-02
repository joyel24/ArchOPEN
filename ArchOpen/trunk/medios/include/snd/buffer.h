/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BUFFER_H
#define __BUFFER_H

#include <snd/playlist.h>

#define BUFFER_SIZE 12*1024*1024

#define BUFFER_CRITICAL_ZONE 512*1024


int buffer_seek(int offset,int whence);
int buffer_read(void * buf,int count);

void buffer_setActiveItem(PLAYLIST_ITEM * item);
PLAYLIST_ITEM * buffer_getActiveItem();

void buffer_init();
void buffer_start(void);
void buffer_stop(void);

#endif
