#ifndef __MEDIOS_H
#define __MEDIOS_H

//#ifdef BUILD_STDALONE
#if 0 
#include <lib/string.h>
    
#include <kernel/malloc.h>
#include <kernel/kernel.h>
    
#include <fs/stdfs.h>
    
#include <snd/codec.h>
#include <snd/buffer.h>
#include <snd/output.h>
#else
    #include <api.h>
#endif

#include <sys_def/int_timer.h>
#include <sys_def/buttons.h>
#include <sys_def/section_types.h>
#include <sys_def/ctype.h>
#include <sys_def/alloca.h>
#include <sys_def/font.h>
#include <sys_def/colordef.h>
#include <sys_def/stdfs.h>

#ifdef DM320
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif

#endif
