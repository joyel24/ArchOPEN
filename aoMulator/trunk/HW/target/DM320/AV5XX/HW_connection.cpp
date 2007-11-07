/*
*   HW_connection.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <emu.h>

#include <HW/HW_connection.h>
#include <HW/gio_connection.h>

#include <HW/mem_space.h>
#include <HW/HW_TI.h>
#include <HW/HW_gpio.h>

extern mem_space * mem;

void HW_connection::arch_init()
{
    usb_conn=new GIO_CONN(this,GIO_USB_CONNECTED,"GIO USB");
    mem->hw_TI->gpio->register_port(usb_conn);
    /*cf_conn=new GIO_CONN(this,GIO_CF_CONNECTED,"GIO CF");
    mem->hw_TI->gpio->register_port(cf_conn);*/
    pw_conn=new GIO_CONN(this,GIO_POWER_CONNECTED,"GIO PW");
    mem->hw_TI->gpio->register_port(pw_conn);
}

bool HW_connection::isConnected(int num)
{
    switch(num)
    {
        case GIO_USB_CONNECTED:
            return usb_state;
        /*case GIO_CF_CONNECTED:
            return cf_state;*/
        case GIO_POWER_CONNECTED:
            return power_state;
    }
    return 0;
}

