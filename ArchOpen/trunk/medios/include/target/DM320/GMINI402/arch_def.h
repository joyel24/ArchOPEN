/*
 * include/kernel/G402/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __GMINI402_DEF_H
#define __GMINI402_DEF_H

#include <sys_def/arch.h>

#define SDRAM_START       0x0900000
#define SDRAM_END         0x18F0000 //HACK: archos MMU config data lies between 0x18F0000 and 0x1900000

#define IRAM_SIZE         0x4000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 101250000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  GMINI402_ARCH

#define MEDIOS_POS  0x249E00
#define MEDIOS_FILE "/medios.avi"
#define MEDIOS_EXT "avi"

/* DEBUG */
#define DEBUG_UART              UART_0
#define CMD_LINE_UART           UART_0
#define FM_REMOTE_UART          UART_0

#define FM_VER                  2

#endif  /* __HARDWARE_H */
