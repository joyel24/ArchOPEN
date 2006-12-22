/*
*   kernel/target/arch_AV3XX/osd.c
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

#include <kernel/io.h>
#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/osd.h>
#include <driver/lcd.h>
#include <driver/videnc.h>

void arch_osd_init(void)
{
    osd_setMainConfig(0x1);
    osd_setMainShift(SCREEN_LCD_SHIFT_X, SCREEN_LCD_SHIFT_Y);
    
    outw(0x2182,VIDENC_BASE); /* preview engine enable/diasble */
    outw(0x0400,VIDENC_BASE+0x2);
}
