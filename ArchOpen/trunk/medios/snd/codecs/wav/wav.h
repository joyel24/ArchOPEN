/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __WAV_H
#define __WAV_H

typedef struct{
    char id[4];
    int size;
    char type[4];
}WAV_HEADER;

typedef struct{
    char id[4];
    int size;
}WAV_CHUNK;

typedef struct{
  short          formatTag;
  unsigned short channels;
  unsigned long  samplesPerSec;
  unsigned long  avgBytesPerSec;
  unsigned short blockAlign;
  unsigned short bitsPerSample;
}WAV_FMT;

#define WAVE_FORMAT_PCM 1

#define DATA_BUFFER_SIZE 8192

#endif
