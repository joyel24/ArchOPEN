/*
*   kernel/gui/splash.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/kernel.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <gfx/graphics.h>

#include <gui/splash.h>
#include <gui/screens.h>

extern struct graphics_operations g32ops;

struct graphicsBuffer splash_gfxStruct = {
    offset             : 0,
    state              : OSD_VIDEO_ZX1,
    enable             : 1,
    width              : SCREEN_WIDTH/2,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 32,
    gops               : &g32ops
};

struct screen_data splash_screenData = {
    show:splash_show,
    palette:NULL
};

extern char splash_img[SCREEN_REAL_WIDTH*4*SCREEN_HEIGHT];

void splash_init(void)
{
    splash_gfxStruct.offset=(unsigned int)splash_img;
    screens_add(&splash_screenData,SCREEN_SPLASH);
}

void splash_show(void)
{
   
    /* reconfigure & restore Video plane */
    gfx_restoreComponent(VID1,&splash_gfxStruct);    
}
