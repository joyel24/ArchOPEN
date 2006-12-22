/*
*   include/kernel/target/arch_GMINI4XX/lcd.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ARCH_LCD_H
#define __ARCH_LCD_H

#include <driver/cpld.h>
#include <driver/gio.h>

#define LCD_BACK_LIGHT                    0x02600200

#define SCREEN_WIDTH                      672
#define SCREEN_REAL_WIDTH                 165
#define SCREEN_HEIGHT                     234
#define SCREEN_ORIGIN_X                   0x1e
#define SCREEN_ORIGIN_Y                   0x0

#define SCREEN_LCD_SHIFT_X                125
#define SCREEN_LCD_SHIFT_Y                22
#define SCREEN_PAL_SHIFT_X                125
#define SCREEN_PAL_SHIFT_Y                22
#define SCREEN_NTSC_SHIFT_X               125
#define SCREEN_NTSC_SHIFT_Y               22

#define LCD_WIDTH SCREEN_REAL_WIDTH
#define LCD_HEIGHT SCREEN_HEIGHT

#define OSD_BMAP_1_CFG      OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX2 | OSD_BITMAP_ZY1 | OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS
#define OSD_BMAP_2_CFG      OSD_BITMAP_RAMCLUT | OSD_BITMAP_ZX2 | OSD_BITMAP_ZY1 | OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS
#define OSD_VID_1_CFG       0
#define OSD_VID_2_CFG       0
#define OSD_CUR_1_CFG       0
#define OSD_CUR_2_CFG       0

#define OSD_CON_BMAP_CFG    OSD_BITMAP_ZX2 | OSD_BITMAP_ZY1 | OSD_BITMAP_8BIT | OSD_BITMAP_0TRANS

#endif
