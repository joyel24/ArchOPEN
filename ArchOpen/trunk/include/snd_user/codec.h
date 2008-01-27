/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CODEC_USER_H
#define __CODEC_USER_H

#include <sys_def/types.h>

#include <snd_user/tag.h>

typedef struct{
    char * description;
    bool seekSupported;
    void (*trackLoop) (void);
    void (*tagRequest) (char *, TAG *);
    void (*codecActivate) (void);
    void (*codecDeactivate) (void);
    bool noTimeAdvance;
    bool loadOk;
} CODEC_GLOBAL_INFO;

#endif
