/*
*   HW_connection.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_CONN_ARCH_H
#define __HW_CONN_ARCH_H

#include <target/chip/arch/HW_30a24.h>

#define PUBLIC_DECLARATION_CONN

#define PRIVATE_DECLARATION_CONN \
HW_30a24 * hw_30a24;

#endif

