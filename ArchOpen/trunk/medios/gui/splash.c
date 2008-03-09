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
#include <sys_def/colordef.h>

#include <kernel/kernel.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <gfx/graphics.h>
#include <gfx/screens.h>
#include <gfx/kfont.h>

#include <gui/splash.h>

extern struct graphics_operations g32ops;
extern struct graphics_operations g8ops;

struct graphicsBuffer splash_gfxStruct = {
    offset             : 0,
    state              : OSD_SPLASH_CFG,
    enable             : 1,
    width              : SCREEN_WIDTH/2,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 32,
    gops               : &g32ops
};


struct graphicsBuffer splash_gfxBmapStruct = {
    offset             : 0,
    state              : OSD_SPLASH_BMAP_CFG,
    enable             : 1,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 8,
};

struct screen_data splash_screenData = {
    show:splash_show,
    palette:NULL,
    hide:NULL
};

extern char splash_img[SCREEN_REAL_WIDTH*4*SCREEN_HEIGHT];
extern char screen_BMAP2[SCREEN_WIDTH*SCREEN_HEIGHT+40];

char * splashStr=NULL;

void splash_init(void)
{
    screens_add(&splash_screenData,SCREEN_SPLASH);
    splash_gfxStruct.offset=(unsigned int)splash_img;
#ifdef TARGET_TYPE_STD
    gfx_initComponent(BMAP2,&splash_gfxBmapStruct,(unsigned int)screen_BMAP2);
    splashStr=getLangStr(STRLNG_SPLASH_LOADING);
#endif
}

void splash_show(void)
{  
    /* reconfigure & restore Video plane */
    int w,h;
    gfx_restoreComponent(VID1,&splash_gfxStruct);
    if(splashStr)
    {
        gfx_restoreComponent(BMAP2,&splash_gfxBmapStruct);
        gfx_fontSet(STD8X13);    
        gfx_getStringSize(splashStr,&w,&h);       
#ifdef JBMM
        /* nothing to do for now */
        osd_setComponentConfig(OSD_BITMAP2,OSD_COMPONENT_ENABLE(OSD_BITMAP2,0,0));
#else
        gfx_planeSetSize(BMAP2,w+4,h+4,8,GFX_SMODE_STD);
        osd_setComponentSize(OSD_BITMAP2, 2*(w+4), h+4);
        osd_setComponentSourceWidth(OSD_BITMAP2, (splash_gfxBmapStruct.width/32));
        osd_setComponentPosition(OSD_BITMAP2,SCREEN_ORIGIN_X,SCREEN_ORIGIN_Y+SCREEN_HEIGHT-h-4);
        g8ops.clearScreen(COLOR_TRSP,&splash_gfxBmapStruct);
        g8ops.drawString(fnt_fontFromId(STD8X13),COLOR_WHITE,COLOR_TRSP,
                         2,2,splashStr,-1,&splash_gfxBmapStruct);
        gfx_planeShow(BMAP2);
        osd_setComponentConfig(OSD_BITMAP2,OSD_COMPONENT_ENABLE(OSD_BITMAP2,OSD_SPLASH_BMAP_CFG,1));
#endif
        
        
    }
}

void splash_setString(char * str)
{
    splashStr=str;
}
