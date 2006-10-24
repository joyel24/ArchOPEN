/*
*   HW_btn.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_BTN_H
#define __HW_BTN_H

#include <emu.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <HW/HW_gpio.h>

#include <target/chip/arch/btn.h>


#define BTN_FCT(BTN,MASK)      \
    if(BTN)                    \
        BTN--;                 \
    else                       \
        ret_val |= MASK;

class GPIO_BTN:public gpio_port {
    public:
        GPIO_BTN(int gpio_num,char * name);

        void set_gpio(void);
        void clear_gpio(void);
};

class HW_btn
{
    public:
        HW_btn();
        ~HW_btn();

        int btn_var[NB_BTN];

        void processBtn(KeySym keysym);

    private:
        void arch_init_btn();

        PRIVATE_DECLARATION
};




#endif
