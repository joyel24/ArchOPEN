/*
 * include/kernel/target/gmini4xx_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __GMINI4XX_DEF_H
#define __GMINI4XX_DEF_H

#include <sys_def/arch.h>

#include "mem_def.h"

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 101250000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  GMINI4XX_ARCH

#define MEDIOS_POS  0x9C00
#define MEDIOS_FILE "/medios.wav"
#define MEDIOS_EXT "wav"

/* DEBUG */
#define DEBUG_UART              UART_0
#define CMD_LINE_UART           UART_0
#define FM_REMOTE_UART          UART_0

#define FM_VER                  2


#endif  /* __HARDWARE_H */
