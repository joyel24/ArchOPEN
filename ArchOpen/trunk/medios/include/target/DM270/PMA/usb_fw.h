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

#include <driver/cpld.h>
#include <driver/gio.h>
#include <driver/cpld.h>

/** USB state **/

#define kusbIsConnected()                 0 //(GIO_IS_SET(GIO_USB_CONNECTED)) // I have to do something here (through OMAP), should be easy

#define USB_enable(){                                      \
                                                           \
    outb(OMAP_HD_USB2_ENABLE_REQUEST,OMAP_REQUEST_BASE);   \
    while(inb(OMAP_REQUEST_BASE));                         \
                                                           \
}

#define USB_disable(){                                     \
                                                           \
    outb(OMAP_HD_USB2_DISABLE_REQUEST,OMAP_REQUEST_BASE);  \
    while(inb(OMAP_REQUEST_BASE));                         \
                                                           \
}

/* no firewire on AV400 */
#define kFWIsConnected()                  0
#define FW_enable()                       
#define FW_disable()                       

#endif
