/*
*   i2c_gpio.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_GPIO_H
#define __I2C_GPIO_H

#include <emu.h>
#include <HW/HW_gpio.h>

class i2c_master;

#include <HW/i2c_device.h>

class i2c_gpio : gpio_port {
    public:
        i2c_gpio(int num,i2c_master * master);
        i2c_gpio(int num,char * name,i2c_master * master);
        bool is_set(void);
        void set_gpio(void);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);

        void init_i2c_gpio(int num,i2c_master * master);
        void i2c_gpio_force_state(int state);
        void i2c_gpio_set_state(int state);

        int new_state;
        int old_state;

    private:
        void arch_init();
        int state;
        int gpio_num;
        i2c_master * master;


};

class i2c_master {
    public:
        i2c_master(HW_gpio * gpio);
        void i2c_state_has_changed(void);
        void register_i2c(i2c_device * device);
        void print_i2c_list(void);

    private:
        i2c_gpio * clk;
        i2c_gpio * data;

        HW_gpio * gpio;

        bool active;
        bool do_ack;
        bool wait_ack;
        int step;
        int clock;
        int address;

        void arch_init();

        i2c_device * cur_device;
        int val,sav_val;

        i2c_device * i2c_head;

        i2c_device * find_device(int address);


};

#endif // __I2C_GPIO_H
