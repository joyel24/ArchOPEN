/*
*   include/kernel/osd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __OSD_H
#define __OSD_H

#include <sys_def/types.h>

#include <target/chip/osd.h>

int  osd_RGB2Packed(int r, int g, int b);
void osd_setCursor2Bitmap (int index, int data);
void osd_setRectCursorColor(int color,int ram_palette);
void osd_setRectCursorBorder(int horiz,int verti);
void osd_enableRectCursor(int state);
void osd_setMainConfig (int config);
void osd_setMainShift (int horizontal,int vertical);
void osd_setEntirePalette (int palette[][3],int size,bool isRgb);
void osd_setPaletteRGB(int r,int g,int b,int index);
void osd_setPalette (int Y, int Cr, int Cb, int index);
void osd_savePalette(int (*palette)[], int size);
void osd_set16CPalette (int bankN, int index, int value);
void osd_setAltOffset (int address);
void osd_setComponentOffset (int component, int address);
void osd_setComponentSize (int component, int width, int height);
void osd_setComponentPosition (int component, int x, int y);
void osd_setComponentSourceWidth (int component, int width);
void osd_setComponentConfig (int component, int config);
void osd_restorePlane(int component, unsigned int address, 
                      int x, int y, int w, int h, 
                      int bw, int bpp, int state,int enable);

void osd_init();
void arch_osd_init(void);

int osd_getTrspBit(void);
int osd_getBlendFactorBit(int factor);

#define RGB2Y(r,g,b)  ((306*r + 601*g + 117*b) >> 10)
#define RGB2Cb(r,g,b)  (((-173*r -339*g + 512*b) >> 10) + 128)
#define RGB2Cr(r,g,b)  (((512*r - 429*g - 83*b) >> 10) + 128)

#define OSD_BITMAP_BLEND_FACTOR(FACTOR)  ((FACTOR)<=OSD_BITMAP_BLEND_FACTOR_MAX? \
                            ((FACTOR)<<OSD_BITMAP_BLEND_FACTOR_SHIFT):0)
#define OSD_BITMAP_SET_BLEND_FACTOR(VAL,FACTOR) (((VAL)&(~(OSD_BITMAP_BLEND_MASK<<OSD_BITMAP_BLEND_FACTOR_SHIFT))) \
                |OSD_BITMAP_BLEND_FACTOR(FACTOR))
#define OSD_BITMAP_TRSP_ENABLE(VAL,STATE) ((STATE)==0? \
                        (VAL)&(~(1<<OSD_BITMAP_TRSP_ENABLE_SHIFT)) : \
                        (VAL)|(1<<OSD_BITMAP_TRSP_ENABLE_SHIFT))
#define OSD_BITMAP_TRSP_ENABLE_BIT (1<<OSD_BITMAP_TRSP_ENABLE_SHIFT)

#endif

