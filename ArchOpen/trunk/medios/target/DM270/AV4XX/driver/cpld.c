/*
*   kernel/target/arch_AV4XX/cpld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>

#include <driver/hardware.h>
#include <driver/cpld.h>

int arch_cpld_getVersion(void){
    return (cpld_read(CPLD4) & 0x000F);
}

