/*
*   include/kernel/target/arch_AV4XX/buttons.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __BUTTONS_ARCH_H
#define __BUTTONS_ARCH_H

#define BUTTON_BASE                       0x018BFFFC           // Virtual CPLD for buttons.

#define BUTTON_PORT0                      (BUTTON_BASE+0x00)

#endif
