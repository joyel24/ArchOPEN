/*
*   include/kernel/exit.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __EXIT_H
#define __EXIT_H

#include <sys_def/errors.h>

void halt_device(void);
void reload_firmware(void);
MED_RET_T reload_medios(char * fname);

void arch_reload_firmware(void);
void arch_HaltMsg(void);

#endif
