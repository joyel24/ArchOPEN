#ifndef __MEDIOS_H
#define __MEDIOS_H

/*
    #include <lib/string.h>

    #include <kernel/malloc.h>
    #include <kernel/evt.h>
    #include <kernel/delay.h>
    #include <kernel/timer.h>
    #include <kernel/io.h>

    #include <init/exit.h>

    #include <driver/buttons.h>
    #include <driver/timer.h>
    #include <driver/osd.h>
    #include <driver/uart.h>
    #include <driver/dsp.h>
    #include <driver/aic23.h>
    #include <driver/clkc.h>
    #include <driver/energy.h>
    #include <driver/videnc.h>
    #include <driver/lcd.h>
    #include <driver/speaker.h>
    #include <fs/stdfs.h>

    #include <gfx/graphics.h>
    #include <gfx/resize.h>

    #include <gui/icons.h>
    #include <gui/widgetlist.h>
    #include <gui/widgetmenu.h>
    #include <gui/file_browser.h>
*/
#if 1
    #include <api.h>
#else
    #include <lib/string.h>
    
    #include <kernel/malloc.h>
    #include <kernel/kernel.h>
    
    #include <fs/stdfs.h>
    
    #include <snd/codec.h>
    #include <snd/buffer.h>
    #include <snd/output.h>
#endif

#include <sys_def/int_timer.h>
#include <sys_def/buttons.h>
#include <sys_def/section_types.h>
#include <sys_def/ctype.h>
#include <sys_def/alloca.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>

#ifdef DM320
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif

#endif
