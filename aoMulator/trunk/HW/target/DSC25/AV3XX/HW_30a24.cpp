/*
*   HW_30A24.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <stdlib.h>
#include <stdio.h>


#include <emu.h>

#include <target/chip/arch/HW_30a24.h>
#include <HW/HW_connection.h>

extern HW_connection * hw_connection;

HW_30a24::HW_30a24(void):HW_access(0x30a24,0x30a25,"@30A24")
{

}

uint32_t HW_30a24::read(uint32_t addr,int size)
{
    int ret_val = ((hw_connection->hdd_state & 0x1) << 1) | ((hw_connection->power_state & 0x1) << 5 )
        | ((hw_connection->usb_state & 0x1) << 6);

    DEBUG_HW(HW_30A24_DEBUG,"read 30A24\n");
    DEBUG_HW(HW_30A24_DEBUG,"%s read, size %x, send : %x: %s %s %s\n",name,size,ret_val,
        hw_connection->hdd_state?"HDD powered ON":"HDD powered OFF",
        hw_connection->power_state?"power connected":"power NOT connected",
        hw_connection->usb_state?"usb connected":"usb NOT connected"
    );
    return ret_val;
}

void HW_30a24::write(uint32_t addr,uint32_t val,int size)
{
    DEBUG_HW(HW_30A24_DEBUG,"write 30A24\n");
    DEBUG_HW(HW_30A24_DEBUG,"%s write, size %x, val= %x\n",name,size,val);
}

