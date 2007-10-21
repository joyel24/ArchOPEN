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

#ifndef __BMP_K_H
#define __BMP_K_H

#include <sys_def/bmp.h>

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
    char index;
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

/* Helper function for OSD */
MED_RET_T gfx_loadBmp_toPlane_center(char * filename,int plane);
MED_RET_T gfx_loadBmp_toPlane_coord(char * filename,int plane,int x,int y);

/* helper function for mem buffer */
MED_RET_T gfx_loadBmp_toMem_center(char * filename,char * dest,int w,int h,int output_mode);
MED_RET_T gfx_loadBmp_toMem_coord(char * filename,char * dest,int w,int h,int output_mode,int x,int y);

/* intermediate functions */
MED_RET_T gfx_loadBmp_toMem(char * filename,char * dest,int w,int h,int output_mode,int center,int x,int y);
MED_RET_T gfx_loadBmp_toPlane(char * filename,int plane,int center,int x,int y);

/* main loader */
MED_RET_T gfx_loadBmpMain(char * filename, char * dest,int plane,int dest_type,
                      int output_mode,int center,int width,int height,int x, int y);
int get_nearest(unsigned char r,unsigned char g,unsigned char b);
#endif
