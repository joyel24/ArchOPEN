/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __K_CODEC_H
#define __K_CODEC_H

#include <fs/med.h>
#include <snd_user/codec.h>

#define CODECS_DIR "/medios/codec/"

typedef struct CODEC_INFO_STRUCT CODEC_INFO;

struct CODEC_INFO_STRUCT{
    char * name;
    char * filename;
    char * extensions;
    bool loaded;
    int fOffset;

    med_t medInfo;

    CODEC_GLOBAL_INFO globalInfo;

    CODEC_INFO * next;
};

CODEC_INFO * codec_new();
bool codec_remove(CODEC_INFO * info);

bool codec_load(CODEC_INFO * info);
CODEC_INFO * codec_findCodecFor(char * name);
bool codec_setCodecFor(char * name);

void codec_trackStart();
void codec_trackStop();
bool codec_mustContinue();

bool codec_seekRequest(int time);
bool codec_mustSeek(int * time);
void codec_seekDone();

bool codec_getElapsed(int * elapsed);
void codec_setElapsed(int elapsed);

bool codec_tagRequest(char * name, TAG * tag);

void codec_init();
void codec_start(void);
void codec_stop(void);

#endif
