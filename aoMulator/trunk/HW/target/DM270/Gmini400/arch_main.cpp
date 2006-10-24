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

#include <HW/mem_space.h>

#include <HW/i2c_gpio.h>
#include <HW/mas.h>
#include <HW/HW_gpio.h>

#include <HW/i2c_RTC.h>
#include <HW/i2c_TSC.h>
#include <HW/mas.h>

extern mem_space * mem;
extern i2c_master * i2c;

void arch_init_main(void)
{
    int i=0;

    i2c->register_i2c((i2c_device *)new i2c_RTC());

    // enable uart unlock
    mem->write(0x10600f,0x55,1);

    mem->write(0x97c3e0 ,0x1c00,2);
}
