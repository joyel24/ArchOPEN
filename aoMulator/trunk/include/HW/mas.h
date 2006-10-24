/*
*   i2c_MAS.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __I2C_MAS_H
#define __I2C_MAS_H

#include <emu.h>

#include <HW/i2c_device.h>
#include <HW/HW_gpio.h>

#define PIO_BUFFER_SIZE 256

#include <stdlib.h>
#include <stdio.h>

class i2c_MAS;

class gio_MAS_PW:public gpio_port {
    public:
        gio_MAS_PW(void);

        void set_gpio(void);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);

};

class gio_MAS_PR:public gpio_port {
    public:
        gio_MAS_PR(i2c_MAS * mas);

        void set_gpio(void);
        void clear_gpio(void);
    private:
        i2c_MAS * mas;
};

class gio_MAS_EOD:public gpio_port {
    public:
        gio_MAS_EOD(void);

        void set_gpio(void);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);

        int do_cmd_eod(void);

        int cnt,cnt_loop;;

        void chkEOD(void);

};

class gio_MAS_data:public gpio_port {
    public:
        gio_MAS_data(int num,i2c_MAS * mas);

        void set_gpio(void);
        void clear_gpio(void);
    private:
        i2c_MAS * mas;
};


class i2c_MAS:i2c_device {
    public:
        i2c_MAS();

        int read(void);
        void write(int val);

        void start(int direction);
        void stop(void);

        void set_p_data(int num,int val);

        void set_PR(void);
        void clr_PR(void);

        i2c_device * nxt;

        int address;
        char * name;

        int do_cmd_dump_D0D1(int argc,char ** argv);
        int do_cmd_dump_pio_buffer(int argc,char ** argv);
        int do_cmd_clear_pio_buffer(int argc,char ** argv);
        int do_cmd_dump_pio_to_file(int argc,char ** argv);
        int do_cmd_set_bat_val(int argc,char ** argv);

    private:
        int index;
        int cmd;
        int reg_addr;
        int xfer_size;
        int xfer_addr;

        HW_gpio * gpio;

        int mas_data;

        int control_reg,DCCF_reg,DCFR_reg;

        int codec_reg[0x30];

        int cur_p_data;

        int * D0_ram;
        int * D1_ram;

        int bat_val;

        int main_regs[256];

        char pio_buffer[PIO_BUFFER_SIZE];
        int pio_index;
        bool pio_full;
        bool dump_to_file;

        gio_MAS_data * gio_p_data[8];
        gio_MAS_PR * gio_pr;
        gpio_port * gio_rtr;
        gio_MAS_EOD * gio_eod;
        gio_MAS_PW * gio_pw;

        FILE * fd;


};




#endif /* __I2C_MAS_H */
