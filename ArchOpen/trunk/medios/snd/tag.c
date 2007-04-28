/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/thread.h>
#include <kernel/delay.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <driver/buttons.h>
#include <driver/dsp.h>
#include <driver/aic23.h>
#include <fs/stdfs.h>
#include <gfx/graphics.h>

#include <snd/buffer.h>
#include <snd/playlist.h>
#include <snd/codec.h>
#include <snd/output.h>
#include <snd/tag.h>


void tag_clear(TAG * tag){
    tag->title=NULL;
    tag->artist=NULL;
    tag->album=NULL;
    tag->genre=NULL;
    tag->track=NULL;
    tag->year=-1;
    tag->length=-1;
    tag->bitRate=-1;
    tag->sampleRate=-1;
    tag->stereo=false;
    tag->sampleCount=-1;
    tag->badFile=false;
}

void tag_free(TAG * tag){
    if(tag->title!=NULL) free(tag->title);
    if(tag->artist!=NULL) free(tag->artist);
    if(tag->album!=NULL) free(tag->album);
    if(tag->genre!=NULL) free(tag->genre);
    if(tag->track!=NULL) free(tag->track);
}
