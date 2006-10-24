/*
*   main.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <HW/i2c_gpio.h>
#include <HW/mas.h>
#include <HW/HW_gpio.h>

#include <HW/i2c_RTC.h>
#include <HW/i2c_TSC.h>
#include <HW/mas.h>

extern i2c_master * i2c;

void arch_init_main(void)
{
    i2c->register_i2c((i2c_device *)new i2c_RTC());
    i2c->register_i2c((i2c_device *)new i2c_TSC());
    i2c->register_i2c((i2c_device *)new i2c_MAS());
}
