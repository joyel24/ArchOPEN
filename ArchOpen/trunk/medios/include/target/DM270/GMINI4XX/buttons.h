/*
*   include/kernel/target/arch_AV3XX/buttons.h
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

#define BUTTON_BASE                       0x02600000

#define BUTTON_PORT0                      (BUTTON_BASE+0x08)
#define BUTTON_PORT1                      (BUTTON_BASE+0x0a)
#define BUTTON_PORT2                      (BUTTON_BASE+0x0c)

#endif
