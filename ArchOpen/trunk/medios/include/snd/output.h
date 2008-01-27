/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OUTPUT_H
#define __OUTPUT_H

#define OUTPUT_DEFAULT_SAMPLERATE 44100

#define OUTPUT_NUM_CHUNKS 32

#define OUTPUT_BUFFER_SIZE (OUTPUT_NUM_CHUNKS*OUTPUT_CHUNK_SIZE)

extern int output_readPos;
extern int output_writePos;
extern int output_volume;

void output_outputParamsChanged();

bool output_setSampleRate(int rate);
void output_enableAudioOutput(bool enabled);
void output_setVolume(int volume);

void output_enable(bool enabled);

void output_write(void * buffer, int size);
void output_discardBuffer();

void output_init();
void output_stop(void);
void output_start(void);

#endif


