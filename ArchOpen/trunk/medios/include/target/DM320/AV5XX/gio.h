/*
*   include/kernel/target/arch_AV3XX/gio.h
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

#define GIO_ON_BTN    0x02
#define GIO_OFF_BTN   0x03

#define GIO_I2C_CLOCK 0xd
#define GIO_I2C_DATA  0xc
/*#define GIO_I2C_CLOCK 0x4
#define GIO_I2C_DATA  0x5*/

#define GIO_BTN_SELECT_UDLR   0x11
#define GIO_BTN_SELECT_FN     0x12
#define GIO_BTN_SELECT_SQCR   0x13

#define GIO_POWER_CONNECTED   0x04
#define GIO_USB_CONNECTED     0x09
#define GIO_CF_CONNECTED      0xff


#define GIO_SPDIF     0xff
#define GIO_VID_OUT   0x0b

#define GIO_IR        0xff

#define GIO_LCD_BACKLIGHT 0xFF

#endif
