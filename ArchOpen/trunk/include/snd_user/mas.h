/*
*   snd_users/mas.h
*
*   MediOS project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MAS_USER_H
#define __MAS_USER_H

#define MAS_NO_MODE           -1
#define MAS_MP3_MODE          0
#define MAS_PCM_DIRECT_MODE   1
#define MAS_PCM_DSP_MODE      2

struct mas_sound_buffer
{
    unsigned char* data;
    unsigned long  size;
    unsigned long  read;
    int state;
    struct mas_sound_buffer * nxt;
};

#define MAS_BUFFER_PLAYING    0
#define MAS_BUFFER_END        1
#define MAS_BUFFER_FILLED     2

#endif
