/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CODEC_H
#define __CODEC_H

#include <sys_def/types.h>

typedef struct{
    char * description;
    bool seekSupported;
    void (*trackLoop) (void);
} CODEC_GLOBAL_INFO;

typedef struct{
    bool validTrack;
    int length;
    int sampleRate;
    bool stereo;
} CODEC_TRACK_INFO;

typedef struct CODEC_INFO_STRUCT CODEC_INFO;

struct CODEC_INFO_STRUCT{
    char * name;
    char * filename;
    char * extensions;    
    bool loaded;
    int fOffset;

    CODEC_GLOBAL_INFO globalInfo;

    CODEC_INFO * next;
};

#endif
