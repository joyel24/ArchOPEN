/*
*   gio_mux_btn.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GIO_MUX_BTN_H
#define __GIO_MUX_BTN_H

#include <emu.h>
#include <HW/HW_gpio.h>

class GIO_MUX_BTN;

class GIO_MUX:public gpio_port {
    public:
        GIO_MUX(int port,int mux,GIO_MUX_BTN * parent);
        void clear_gpio(void);
        void gpio_dir_chg(int dir);

        int dir;
        int mux_num;

    private:
        GIO_MUX_BTN * parent;
};

class GIO_MUX_LINE:public gpio_port {
    public:
        GIO_MUX_LINE(int port,int btn0,int btn1,int btn2,GIO_MUX_BTN * parent);
        bool is_set(void);

        int btn_num[3];

    private:
        GIO_MUX_BTN * parent;
};

class GIO_MUX_BTN{
    public:
        GIO_MUX_BTN(HW_gpio * gio);

        GIO_MUX * GIO_MUX0;
        GIO_MUX * GIO_MUX1;
        GIO_MUX * GIO_MUX2;
        
        GIO_MUX_LINE * GIO_LINE0;
        GIO_MUX_LINE * GIO_LINE1;
        GIO_MUX_LINE * GIO_LINE2;
        GIO_MUX_LINE * GIO_LINE3;

        int active_mux;
};

#endif // __HW_ON_OFF_H
