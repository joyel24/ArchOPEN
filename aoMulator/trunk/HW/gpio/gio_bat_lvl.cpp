/* 
*   gio_bat_lvl.cpp
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

#include <HW/gio_bat_lvl.h>

#warning should add a cmdline to simulate bat low

GIO_BAT_LVL::GIO_BAT_LVL(void):gpio_port(GPIO_BAT_LVL,"JBM BAT lvl")
{
    state = 1;  /* default => bat ok */
}

void GIO_BAT_LVL::set_gpio(void)
{
    
}

void GIO_BAT_LVL::clear_gpio(void)
{
    
}

