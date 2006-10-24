/*
*   btn.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_BTN_ARCH_H
#define __HW_BTN_ARCH_H

#include <HW/HW_gpio.h>

#define BTN_UP    0x0
#define BTN_LEFT  0x1
#define BTN_RIGHT 0x2
#define BTN_DOWN  0x3
#define BTN_F3    0x4
#define BTN_F1    0x5
#define BTN_F2    0x6
#define BTN_ON    0x7
#define BTN_OFF   0x8
#define BTN_SQUARE 0x9
#define BTN_CROSS  0xa

#define BTN_INIT_VAL  0x10

#define NB_BTN 11

// gio multiplexed buttons (cpld v5)

#define GIO_BTN_MUX0_BTN0 BTN_UP
#define GIO_BTN_MUX0_BTN1 BTN_DOWN
#define GIO_BTN_MUX0_BTN2 BTN_LEFT
#define GIO_BTN_MUX0_BTN3 BTN_RIGHT

#define GIO_BTN_MUX1_BTN0 BTN_F1
#define GIO_BTN_MUX1_BTN1 BTN_F2
#define GIO_BTN_MUX1_BTN2 BTN_F3
#define GIO_BTN_MUX1_BTN3 -1

#define GIO_BTN_MUX2_BTN0 BTN_SQUARE
#define GIO_BTN_MUX2_BTN1 BTN_CROSS
#define GIO_BTN_MUX2_BTN2 -1
#define GIO_BTN_MUX2_BTN3 -1


#define PRIVATE_DECLARATION \
GPIO_BTN * ON_btn;          \
GPIO_BTN * OFF_btn;

#endif
