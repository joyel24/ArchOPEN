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
#include <stdlib.h>
#include <stdio.h>

#include <HW/HW_gpio.h>

HW_gpio::HW_gpio(void):HW_access(GPIO_START,GPIO_END,"GPIO")
{
    for(int i=0;i<NB_GPIO_REG;i++)
    {
        DIR[i]=0xFFFF;
        INV[i]=0x0;
        BITSET[i]=0x0;
        BITCLR[i]=0x0;
    }

    IRQPORT  = 0x0;
    FSEL     = 0x0;
    BITRATE  = 0x0;


    for(int i=0;i<NB_GPIO_PORT;i++)
        port_list[i]=new gpio_port(i);

    arch_init();
 }

gpio_port::gpio_port(int gpio_num)
{
    this->gpio_num=gpio_num;
    this->name = "UKN";
    state = 0;
}

gpio_port::gpio_port(int gpio_num,char * name)
{
    this->gpio_num=gpio_num;
    this->name = name;
    state = 0;
}

bool gpio_port::is_set(void)
{
    //DEBUG_HW(GPIO_HW_DEBUG,"GPIO%x reading state: %s\n",gpio_num,state?"SET":"CLR");
    /*if(gpio_num == 0 || gpio_num == 0x1c)
        return 1;*/
    return state;
}

void gpio_port::set_gpio(void)
{
    state = 1;
    DEBUG_HW(GPIO_HW_DEBUG,"GPIO%x:%s is being set\n",gpio_num,name);
}

void gpio_port::clear_gpio(void)
{
    state = 0;
    DEBUG_HW(GPIO_HW_DEBUG,"GPIO%x: %s is being clear\n",gpio_num,name);
}

void gpio_port::gpio_dir_chg(int dir)
{
    DEBUG_HW(GPIO_HW_DEBUG,"GPIO%x: %s => dir chg to %s\n",gpio_num,name,dir?"input":"output");
}

void HW_gpio::register_port(gpio_port * port)
{
    register_port(port->gpio_num,port);
}

void HW_gpio::register_port(int num,gpio_port * port)
{
    delete port_list[num];
    port_list[num] = port;
    //DEBUG_HW(GPIO_HW_DEBUG,"Registering gpio port %x for %s\n",num,port->name);
}
