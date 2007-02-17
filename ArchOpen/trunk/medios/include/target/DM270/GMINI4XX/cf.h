/*
*   include/target/dm270/g4xx/cf.h
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

#include <driver/cpld.h>

#define CF_IS_CONNECTED ((cpld_read(CPLD3)&0x3)==0)
#define CF_ARCH_PLUG_INIT   {}

#define CF_ARCH_UNPLUG_INIT {}



#endif
