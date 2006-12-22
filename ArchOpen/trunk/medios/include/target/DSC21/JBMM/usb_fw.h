/*
*   include/target/arch_AV3XX/usb_fw.h
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

/** USB state **/

#define USB_STATE                         0x30a24
#define kusbIsConnected()                 0 //((inw(USB_STATE) & 0x40)!=0)
#define kFWIsConnected()                  0 //((cpld_read(CPLD3) & 0x8)==0)
#define FW_enable()                       //cpld_select(CPLD_FW_EXT,0x1)
#define FW_disable()                      //cpld_select(CPLD_FW_EXT,0x0)
#define USB_enable()                      //cpld_set_port_1(CPLD_USB);
#define USB_disable()                     //cpld_clear_port_1(CPLD_USB);

#endif
