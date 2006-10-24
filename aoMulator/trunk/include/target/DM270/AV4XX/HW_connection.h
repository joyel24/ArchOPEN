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


#include <HW/gio_connection.h>

#define PUBLIC_DECLARATION_CONN \
    bool isConnected(int num); \

#define PRIVATE_DECLARATION_CONN \
    GIO_CONN * usb_conn; \
    GIO_CONN * cf_conn; \
    GIO_CONN * pw_conn;
#endif

