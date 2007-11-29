/*
 * include/kernel/AV4XX/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __AV4XX_DEF_H
#define __AV4XX_DEF_H

#include <sys_def/arch.h>

// RQ: exploit is at 0xDA0000 at end of avi load

#include "mem_def.h"

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 101250000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  AV4XX_ARCH

#define MEDIOS_POS  0x3d1c00
#define MEDIOS_FILE "/medios.avi"
#define MEDIOS_EXT "avi"

/* DEBUG */
#define DEBUG_UART                        UART_0
#define CMD_LINE_UART                     UART_0
#define FM_REMOTE_UART                    UART_0

#define FM_VER                  2

#endif  /* __HARDWARE_H */
