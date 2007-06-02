/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __TAG_USER_H
#define __TAG_USER_H

#include <sys_def/types.h>

typedef struct TAG_STRUCT TAG;

struct TAG_STRUCT{
    char * title;
    char * artist;
    char * album;
    char * genre;
    char * track;
    char * date;
    int length;
    int bitRate;
    int sampleRate;
    bool stereo;
    int sampleCount;
    bool badFile;
};

#endif
