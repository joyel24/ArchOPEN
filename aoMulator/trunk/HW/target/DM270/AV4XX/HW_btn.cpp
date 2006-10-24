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

#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include <HW/HW_btn.h>

#include <HW/mem_space.h>
#include <HW/HW_TI.h>
#include <HW/HW_gpio.h>

extern mem_space * mem;

void HW_btn::arch_init_btn()
{
    ON_btn = new GPIO_BTN(GPIO_ON_NUM,"ON");
    mem->hw_TI->gpio->register_port(ON_btn);
    OFF_btn = new GPIO_BTN(GPIO_OFF_NUM,"OFF");
    mem->hw_TI->gpio->register_port(OFF_btn);
    LCD1_btn = new GPIO_BTN(GPIO_LCD_SWT_1,"LCD SWT 1");
    mem->hw_TI->gpio->register_port(LCD1_btn);
    LCD2_btn = new GPIO_BTN(GPIO_LCD_SWT_2,"LCD SWT 2");
    mem->hw_TI->gpio->register_port(LCD2_btn);
}

void HW_btn::processBtn(KeySym keysym)
{
     switch(keysym)
    {
        case XK_KP_Left:
        case XK_Left:
        case XK_KP_4:
            printf("left\n");
            btn_var[BTN_LEFT]=BTN_INIT_VAL;
            break;

        case XK_KP_Right:
        case XK_Right:
        case XK_KP_6:
            printf("right\n");
            btn_var[BTN_RIGHT]=BTN_INIT_VAL;
            break;

        case XK_KP_Up:
        case XK_Up:
        case XK_KP_8:
            printf("up\n");
            btn_var[BTN_UP]=BTN_INIT_VAL;
            break;

        case XK_KP_Down:
        case XK_Down:
        case XK_KP_2:
            printf("down\n");
            btn_var[BTN_DOWN]=BTN_INIT_VAL;
            break;
        case XK_KP_5:
            printf("ok\n");
            btn_var[BTN_OK]=BTN_INIT_VAL;
            break;
        case XK_KP_Add:
        case XK_Q:
        case XK_q:
            printf("ON %d\n",ON_btn->state);
            ON_btn->state=ON_btn->state==1?0:1;
            break;

        case XK_KP_Enter:
        case XK_A:
        case XK_a:
            printf("OFF %d\n",OFF_btn->state);
            OFF_btn->state=OFF_btn->state==1?0:1;
            break;

        case XK_Z:
        case XK_z:
            printf("LCD Switch %d\n",LCD1_btn->state);
            LCD1_btn->state=LCD1_btn->state==1?0:1;
            break;

        case XK_S:
        case XK_s:
            printf("LCD Switch %d\n",LCD2_btn->state);
            LCD2_btn->state=LCD2_btn->state==1?0:1;
            break;

        case XK_KP_Divide:
        case XK_1:
            printf("F1\n");
            btn_var[BTN_F1]=BTN_INIT_VAL;
            break;

        case XK_KP_Multiply:
        case XK_2:
            printf("F2\n");
            btn_var[BTN_F2]=BTN_INIT_VAL;
            break;

        case XK_KP_Subtract:
        case XK_3:
            printf("F3\n");
            btn_var[BTN_F3]=BTN_INIT_VAL;
            break;
    }

}


