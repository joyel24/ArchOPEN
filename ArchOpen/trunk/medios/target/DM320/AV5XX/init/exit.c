/*
*   kernel/target/arch_AV5XX/exit.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>
#include <sys_def/stddef.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/irq.h>

#include <driver/hardware.h>
#include <driver/lcd.h>
#include <driver/osd.h>
#include <driver/clkc.h>
#include <driver/ata.h>
#include <driver/cache.h>

#include <gfx/graphics.h>
#include <gfx/screens.h>

#include <gui/splash.h>

typedef struct{
    unsigned char magic[4];
    int unpackedsize;
    int packedsize;
    int checksum;
} firmware_header;

// code taken from descramble.c from the Rockbox project
__attribute__((section(".fwuncomp_code"))) void firmware_decompress(unsigned char * inbuf,unsigned char * outbuf, int length){
    int i;
    int j=0;

    for (i=0; i<length;) {
        int bit;
        int head = inbuf[i++];

        for (bit=0; bit<8 && i<length; bit++) {
            if (head & (1 << (bit))) {
                outbuf[j++] = inbuf[i++];
            }
            else {
                int x;
                int byte1 = inbuf[i];
                int byte2 = inbuf[i+1];
                int count = (byte2 & 0x0f) + 3;
                int src =
                    (j & 0xfffff000) + (byte1 | ((byte2 & 0xf0)<<4)) + 18;
                if (src > j)
                    src -= 0x1000;

                for (x=0; x<count; x++)
                    outbuf[j++] = outbuf[src+x];
                i += 2;
            }
        }
    }
}

__attribute__((section(".fwuncomp_code"))) void arch_reload_firmware(void){
    void (*firmware_start)(void) = (void (*)(void)) SDRAM_START;
    firmware_header header=*(firmware_header*)0x140000;
    unsigned char * fwdata=(unsigned char *)0x140010;

    halt_device();
#if 0
    
    // disable interrupts
    cli();

    // set default clock parameters (not reinitialized by the firmware)
    clkc_setClockParameters(CLK_ARM,15,2,2);
    clkc_setClockParameters(CLK_SDRAM,15,2,2);
    clkc_setClockParameters(CLK_ACCEL,15,2,1);
    clkc_setClockParameters(CLK_DSP,9,1,2);

    // show something on the screen (black screen for now)
    gfx_openGraphics();

    gfx_planeHide(BMAP1);
    gfx_planeHide(BMAP2);
    gfx_planeHide(VID1);
    gfx_planeHide(VID2);
    gfx_planeHide(CUR1);
    gfx_planeHide(CUR2);

    // uncompressing firmware
    firmware_decompress(fwdata,(unsigned char *)SDRAM_START,header.packedsize);

    //restore uart & timer source select
    outw(0x091e,CLKC_SOURCE_SELECT);

    // take care of the cache (I directly use macros because functions are overwritten at this point)
    CACHE_CLEAN();
    CACHE_DISABLE(CACHE_ALL);
    CACHE_INVALIDATE(CACHE_ALL);

    //jump to sdram start (where the uncompressed firmware is)
    firmware_start();
#endif
}

void arch_HaltMsg(void)
{
    splash_setString(getLangStr(STRLNG_SPLASH_SHUTDOWN));
    screens_show(SCREEN_SPLASH);
}
