/*
*   gfx/bmp.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BMP_H
#define __BMP_H

typedef struct  {
   short type;
   unsigned long size;
   short reserved1;
   short reserved2;
   unsigned long offsetbits;
} bmp_file_header ;


typedef struct {
    char Y;
    char Cr;
    char Cb;
} bmpColor;

#define fileHeader_size 14

typedef struct  {
    unsigned long size;
    unsigned long width;
    unsigned long height;
    unsigned short planes;
    unsigned short bitcount;
    unsigned long compression;
    unsigned long sizeimage;
    long xpelspermeter;
    long ypelspermeter;
    unsigned long colorsused;
    unsigned long colorsimportant;
} bmp_info_header ;

MED_RET_T gfx_loadBmp(char * filename);

#endif
