/*
*   HW_lcd.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_LCD_H
#define __HW_LCD_H

class HW_lcd;

#include <HW/mem_space.h>
#include <HW/HW_OSD.h>

#include <X11/Xlib.h>
#include <X11/Xutil.h>

#define USE_CACHE
#define PX_CACHE_SIZE  1000

#define LCD_BMAP 0
#define LCD_VID  1

class mem_space;

class HW_lcd {
    public:
        HW_lcd(mem_space * mem, HW_OSD * osd);
        void updte_lcd(uint32_t base_addr,int type);
        int setPaletteRGB(int r,int g, int b, int index);
        int setPaletteYCbCr(int y,int cb, int cr, int index);
        void drawPix(uint32_t addr,uint32_t val);
        void drawVidPix(uint32_t addr,uint32_t val);
        void nxtStep(int * config,uint32_t * addr);

    private:
        mem_space * mem;
        HW_OSD * osd;

        Display* display;
        Window window1;
        Window window2;

        GC gc;
        Colormap pal;
        int screen;
        XEvent event;

        int colorTab[256];

        int skip;

        void setFullPalette(int palette[256][3],int size);
        uint32_t getColor(uint32_t color);
#ifdef USE_CACHE
        uint32_t pixel_cache[PX_CACHE_SIZE][2];
        int in_cache;
        int cache_size;    
#endif 
};

#endif // __HW_LCD_H
