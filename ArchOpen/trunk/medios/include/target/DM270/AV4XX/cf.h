/*
*   include/target/dm270/av400/cf.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __ARCH_CF_H
#define __ARCH_CF_H

#include <kernel/delay.h>

#include <driver/cpld.h>
#include <driver/gio.h>

#define CF_IS_CONNECTED ((cpld_read(CPLD0)&0x3)==0)
#define CF_ARCH_PLUG_INIT   {            \
    GIO_CLEAR(0x19);                         \
    GIO_CLEAR(0x1b);                         \
    GIO_SET(0x13);                           \
    mdelay(10);                              \
    GIO_SET(0x1b);                           \
    mdelay(10);                              \
    GIO_SET(0x19);                           \
    GIO_DIRECTION(0x1c,GIO_IN);              \
    GIO_DIRECTION(0x06,GIO_OUT);             \
    GIO_CLEAR(0x06);                         \
    mdelay(10);                              \
    cpld_write(CPLD0,cpld_read(CPLD0)|0x4);  \
    mdelay(10);                              \
    outw(0x88df,0x30a12);                    \
    outw(0x3441,0x30a14);                    \
    outw(inw(0x30a14)|0x4000,0x30a14);       \
    cpld_write(CPLD0,cpld_read(CPLD0)&0xFB); \
    mdelay(100);                             \
    cpld_write(CPLD0,cpld_read(CPLD0)|0x4);  \
    mdelay(10);                              \
}

#define CF_ARCH_UNPLUG_INIT { \
    GIO_SET(0x19);                           \
    GIO_SET(0x1B);                           \
    GIO_SET(0x13);                           \
    GIO_DIRECTION(0x1c,GIO_OUT);             \
    GIO_SET(0x06);                           \
    cpld_write(CPLD0,cpld_read(CPLD0)&0xFB); \
}

/*cpld_write(CPLD0,0x4); ==> PLUG*/

#endif
