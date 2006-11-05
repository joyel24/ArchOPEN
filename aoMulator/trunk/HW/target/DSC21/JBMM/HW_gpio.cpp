/*
*   HW_gpio.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <HW/HW_gpio.h>

#include <HW/gio_lcd.h>
#include <HW/gio_bat_lvl.h>
//#include <HW/gio_vid_uart1RX.h>
//#include <HW/gio_spdif_uart1TX.h>


void HW_gpio::arch_init(void)
{
    register_port(new GIO_LCD());
    register_port(new GIO_BAT_LVL());    
    //register_port(new GIO_VID_UART1_RX());
    //register_port(new GIO_SPDIF_UART1_TX());
}
