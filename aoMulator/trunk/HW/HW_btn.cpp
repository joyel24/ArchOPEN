/*
*   HW_btn.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <HW/HW_btn.h>

HW_btn::HW_btn()
{
    for(int k=0;k<NB_BTN;k++)
        btn_var[k]=0;
    arch_init_btn();
}

HW_btn::~HW_btn()
{

}
GPIO_BTN::GPIO_BTN(int gpio_num,char * name):gpio_port(gpio_num,name)
{
    state = 1;  /* default => btn not pressed */
}

void GPIO_BTN::set_gpio(void)
{

}

void GPIO_BTN::clear_gpio(void)
{

}
