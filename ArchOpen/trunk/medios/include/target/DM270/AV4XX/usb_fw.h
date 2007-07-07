/*
*   include/target/arch_AV4XX/usb_fw.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ARCH_USB_FW_H
#define __ARCH_USB_FW_H

#include <kernel/io.h>

#include <driver/gio.h>
#include <driver/cpld.h>

#include <fs/disk.h>

/** USB state **/

                    
#define kusbIsConnected()                 (GIO_IS_SET(GIO_USB_CONNECTED))

#define USB_enable(){                                      \
    ata_doPowerOn(HD_DISK);                                \
    mdelay(10);                                            \
    outw(1,0x0250000e);                                    \
    CPLD_CLEAR_PORT2(CPLD_FX2_WAKEUP);                     \
    mdelay(10);                                            \
    CPLD_SET_PORT2(CPLD_FX2_WAKEUP);                       \
    mdelay(10);                                            \
    CPLD_SET_PORT3(0);                                     \
}

#define USB_disable(){                                     \
    CPLD_CLEAR_PORT3(0);                                   \
    mdelay(10);                                            \
    CPLD_CLEAR_PORT2(CPLD_FX2_WAKEUP);                     \
    mdelay(10);                                            \
    outw(3,0x0250000e);                                    \
    mdelay(100);                                           \
}


/* no firewire on AV400 */
#define kFWIsConnected()                  0
#define FW_enable()                       
#define FW_disable()                       

#endif
