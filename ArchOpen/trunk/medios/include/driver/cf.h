/*
*   include/kernel/cf.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CF_H
#define __CF_H

#ifdef HAVE_EXT_MODULE
#include <driver/ext_module.h>
#else
#ifdef HAVE_CF
#include <target/chip/arch/cf.h>
#endif
#endif

#ifndef CF_IS_CONNECTED
#define CF_IS_CONNECTED 0
#endif

void cf_init(void);


#endif
