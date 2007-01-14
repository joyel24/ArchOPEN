/*
 * include/kernel/AV4XX/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#ifndef __PMA_DEF_H
#define __PMA_DEF_H

#include <sys_def/arch.h>

#define SDRAM_START       0x0900000

#define SDRAM_END         0x18BFFCC

#define IRAM_SIZE         0x8000

#define MALLOC_START  ((((unsigned int)&_end_kernel) & 0xFFFFF000)+0x1000)
#define MALLOC_SIZE   (((unsigned int)&_sdend_start)-MALLOC_START)

#define CONFIG_ARM_CLK 101250000
#define CONFIG_EXT_CLK 27000000

#define CURRENT_ARCH  PMA_ARCH

/* DEBUG */
#define DEBUG_UART                        UART_0
#define CMD_LINE_UART                     UART_0
#define FM_REMOTE_UART                    UART_0

#define FM_VER                  2

/* OMAP Requests       */

#define OMAP_REQUEST_BASE                 0x018BFFCC
#define OMAP_HD_READ_REQUEST              0x1
#define OMAP_HD_WRITE_REQUEST             0x2
#define OMAP_HD_STATUS_REQUEST            0x3
#define OMAP_HD_ALTS_ERR_REQUEST          0x4
#define OMAP_HD_CMD_REQUEST               0x5
#define OMAP_HD_RESET_REQUEST             0x6
#define OMAP_HD_POWER_UP_REQUEST          0x7
#define OMAP_HD_POWER_DOWN_REQUEST        0x8
#define OMAP_HD_USB2_ENABLE_REQUEST       0x9
#define OMAP_HD_USB2_DISABLE_REQUEST      0x10
#define OMAP_COPY_FW_REQUEST              0x11
#define OMAP_HD_INIT_REQUEST              0x12

#endif  /* __HARDWARE_H */
