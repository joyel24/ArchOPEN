#ifndef __SHARED_H
#define __SHARED_H

#define PICO_PATH "/pico"
#define SAVES_PATH "/pico/saves/"
#define CFG_FILE_PATH "/pico/pico.cfg"


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

#include <sys_def/colordef.h>
#include <sys_def/stddef.h>
#include <sys_def/buttons.h>


#ifdef AV300

#endif

#ifdef AV400
//#define HAVE_RESIZE
//#define SOUND_USE_DSP
//#define SOUND_USE_AIC23
#endif

#if defined(GMINI4XX) || defined(GMINI402)
#define HAVE_RESIZE
#define SOUND_USE_DSP
#define SOUND_USE_AIC23
#define GMINI_OVERCLOCKING
#endif

#include <dspshared.h>

//#define PROFILING
#include "profile.h"

extern bool bAutoFrameSkip, bScanlineRendering;
extern int FrameSkip;
extern int armFrequency, overclocking;

unsigned short pSndBuffer[SAMPLECOUNT*2];

extern char CurrentROMFile[512];

#endif
