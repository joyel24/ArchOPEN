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

#include <sys_def/codec.h>

CODEC_INFO * codec_new();

bool codec_load(CODEC_INFO * info);
CODEC_INFO * codec_findCodecFor(char * name);
bool codec_setCodecFor(char * name);

void codec_trackStart();
void codec_trackStop();
bool codec_mustContinue();

bool codec_seekRequest(int time);
bool codec_mustSeek(int * time);
void codec_seekDone();

bool codec_getTimes(int * length,int * elapsed);
void codec_setElapsed(int elapsed);

void codec_setGlobalInfo(CODEC_GLOBAL_INFO * info);
void codec_setTrackInfo(CODEC_TRACK_INFO * info);

void codec_init();

#endif
