/*
* gio_mux_btn.cpp
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

#include <stdlib.h>
#include <stdio.h>

#include <hardware.h>
#include <target/chip/arch/gio_mux_btn.h>
#include <HW/HW_btn.h>

extern HW_btn * btn;

GIO_MUX_BTN::GIO_MUX_BTN(HW_gpio * gio)
{
    this->active_mux=-1;

    GIO_MUX0=new GIO_MUX(GIO_BTN_MUX0,0,this);
    GIO_MUX1=new GIO_MUX(GIO_BTN_MUX1,1,this);
    GIO_MUX2=new GIO_MUX(GIO_BTN_MUX2,2,this);

    GIO_LINE0=new GIO_MUX_LINE(GIO_BTN_LINE0,GIO_BTN_MUX0_BTN0,GIO_BTN_MUX1_BTN0,GIO_BTN_MUX2_BTN0,this);
    GIO_LINE1=new GIO_MUX_LINE(GIO_BTN_LINE1,GIO_BTN_MUX0_BTN1,GIO_BTN_MUX1_BTN1,GIO_BTN_MUX2_BTN1,this);
    GIO_LINE2=new GIO_MUX_LINE(GIO_BTN_LINE2,GIO_BTN_MUX0_BTN2,GIO_BTN_MUX1_BTN2,GIO_BTN_MUX2_BTN2,this);
    GIO_LINE3=new GIO_MUX_LINE(GIO_BTN_LINE3,GIO_BTN_MUX0_BTN3,GIO_BTN_MUX1_BTN3,GIO_BTN_MUX2_BTN3,this);

    gio->register_port(GIO_MUX0);
    gio->register_port(GIO_MUX1);
    gio->register_port(GIO_MUX2);
    gio->register_port(GIO_LINE0);
    gio->register_port(GIO_LINE1);
    gio->register_port(GIO_LINE2);
    gio->register_port(GIO_LINE3);
}

GIO_MUX::GIO_MUX(int port,int mux,GIO_MUX_BTN * parent):gpio_port(port,"BTN MUX")
{
    this->mux_num=mux;
    this->parent=parent;
}

void GIO_MUX::clear_gpio(void)
{
    if(!this->dir){
        this->parent->active_mux=mux_num;
//        printf("active mux %d\n",mux_num);
    }
}

void GIO_MUX::gpio_dir_chg(int dir)
{
    this->dir=dir;
}

GIO_MUX_LINE::GIO_MUX_LINE(int port,int btn0,int btn1,int btn2,GIO_MUX_BTN * parent):gpio_port(port,"BTN LINE")
{
    this->btn_num[0]=btn0;
    this->btn_num[1]=btn1;
    this->btn_num[2]=btn2;
    this->parent=parent;
}

bool GIO_MUX_LINE::is_set(void)
{
    int mux=this->parent->active_mux;
    bool ret_val=true;

    if(mux>=0){
        int btn_num=this->btn_num[mux];

//        printf("read btn %d mux %d\n",btn_num,mux);

        if(btn_num>=0){
            ret_val=!btn->btn_var[btn_num];

            if(btn->btn_var[btn_num]){
                btn->btn_var[btn_num]--;
            }
        }
    }

    return ret_val;
}

