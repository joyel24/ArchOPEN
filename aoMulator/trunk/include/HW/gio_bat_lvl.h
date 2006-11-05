/* 
*   gio_bat_lvl.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GIO_BAT_LVL_H
#define __GIO_BAT_LVL_H

#include "emu.h"
#include "HW_gpio.h"



class GIO_BAT_LVL:public gpio_port {
    public:
        GIO_BAT_LVL(void);
        
        void set_gpio(void);
        void clear_gpio(void);
};



#endif 

