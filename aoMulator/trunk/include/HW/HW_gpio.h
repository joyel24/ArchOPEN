/*
*   HW_gpio.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_GPIO_H
#define __HW_GPIO_H

#include <emu.h>
#include <core/HW_access.h>

#define IS_GPIO_SET(NUM)        (port_list[NUM]->is_set())
#define SET_GPIO(NUM)           {port_list[NUM]->set_gpio();}
#define CLR_GPIO(NUM)           {port_list[NUM]->clear_gpio();}
#define GPIO_DIR_CHG(NUM,DIR)   {port_list[NUM]->gpio_dir_chg(DIR);}

class gpio_port {
    public:
        gpio_port(int num,char * name);
        gpio_port(int num);
        virtual bool is_set(void);
        virtual void set_gpio(void);
        virtual void clear_gpio(void);
        virtual void gpio_dir_chg(int dir);

        const char * name;

        int state;
        int gpio_num;
};

#define NB_GPIO_PORT NB_GPIO_REG*16

class HW_gpio : public HW_access {
    public:
        HW_gpio(void);

        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);

        void register_port(int num,gpio_port * port);
        void register_port(gpio_port * port);

        int DIR[NB_GPIO_REG];
        int INV[NB_GPIO_REG];
        int BITSET[NB_GPIO_REG];
        int BITCLR[NB_GPIO_REG];

        gpio_port * port_list[NB_GPIO_PORT];

    private:
        int FSEL,BITRATE;
        int IRQPORT;
        void arch_init(void);

};


#endif // __HW_GPIO_H
