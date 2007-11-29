/*
 * include/kernel/JBMM/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __JBMM_DEF_H
#define __JBMM_DEF_H

#include <sys_def/arch.h>

#include "mem_def.h"

#define SDRAM_START   0x08000000
#define SDRAM_END     0x09000000

#define IRAM_SIZE         0x8000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 37125000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  JBMM_ARCH

#define MEDIOS_POS  0x0
#define MEDIOS_FILE "/medios.bin"
#define RELOAD_CP_BASE
#define MEDIOS_EXT "bin"

/* DEBUG */
#define DEBUG_UART              UART_1
#define CMD_LINE_UART           UART_1
#define FM_REMOTE_UART          UART_1

#define FM_VER                  1

#endif  /* __HARDWARE_H */
