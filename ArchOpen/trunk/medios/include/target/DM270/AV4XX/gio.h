/*
*   include/kernel/target/arch_AV4XX/gio.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#ifndef __GIO_ARCH_H
#define __GIO_ARCH_H

/* GIO lines */

#define GIO_ON_BTN            0x01
#define GIO_OFF_BTN           0x02
#define GIO_SCR_SWITCH1_BTN   0x0F
#define GIO_SCR_SWITCH2_BTN   0x0E

#define GIO_I2C_CLOCK         0x09
#define GIO_I2C_DATA          0x08


#define GIO_POWER_CONNECTED   0x03
#define GIO_USB_CONNECTED     0x0b
#define GIO_CF_CONNECTED      0x07


#define GIO_SPDIF     0xff
#define GIO_VID_OUT   0x12

#define GIO_IR        0xff

#define GIO_LCD_BL_AV4100 0x1e

#endif
