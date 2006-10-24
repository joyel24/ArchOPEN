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

#ifndef __HW_CONNECTION_H
#define __HW_CONNECTION_H

#include <emu.h>

class HW_connection;

#include <target/chip/arch/HW_connection.h>

class HW_connection
{
    public:
        HW_connection();
        ~HW_connection();

        int power_state;
        int usb_state;
        int fw_state;
        int cf_state;
        int hdd_state;

        PUBLIC_DECLARATION_CONN

    private:
        void arch_init();

        PRIVATE_DECLARATION_CONN
};


#endif
