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
#ifndef __GIO_DSC21_H
#define __GIO_DSC21_H

#include <target/chip/arch/gio.h>

/* GIO addresses */

#define GIO_BASE                          0x30600
#define GIO_DIRECTION0                    (GIO_BASE+0x00)  // GIO 0-15
#define GIO_DIRECTION1                    (GIO_BASE+0x02)  // GIO 16-31
#define GIO_DIRECTION2                    0
#define GIO_INVERT0                       (GIO_BASE+0x04)  // GIO 0-15
#define GIO_INVERT1                       (GIO_BASE+0x06)  // GIO 16-31
#define GIO_INVERT2                       0
#define GIO_BITSET0                       (GIO_BASE+0x08)  // GIO 0-15
#define GIO_BITSET1                       (GIO_BASE+0x0a)  // GIO 16-31
#define GIO_BITSET2                       0
#define GIO_BITCLEAR0                     (GIO_BASE+0x0c)  // GIO 0-15
#define GIO_BITCLEAR1                     (GIO_BASE+0x0e)  // GIO 16-31
#define GIO_BITCLEAR2                     0
#define GIO_ENABLE_IRQ                    (GIO_BASE+0x10)  // GIO 0-7

#endif
