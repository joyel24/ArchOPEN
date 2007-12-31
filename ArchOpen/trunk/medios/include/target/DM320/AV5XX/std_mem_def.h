/*
 * include/kernel/AV5XX/arch_def.h
 *
 * mediOS project
 * Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
 *
 * This file contains the hardware definitions of the AV3XX series machines
 */

#define SDRAM_START   0x00900000
#define KERNEL_START  0x00900000
#define SDRAM_END     0x018F0000
#define IRAM_SIZE         0x4000

//HACK: archos MMU config data lies between 0x18F0000 and 0x1900000
