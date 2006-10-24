/*
*   HW_connection.cpp
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

#include <core/cmd_line.h>
#include <HW/HW_connection.h>

extern HW_connection * hw_connection;

int do_plug_pw_s(int argc,char ** argv)
{
    hw_connection->power_state=1;
    return 0;
}

int do_unplug_pw_s(int argc,char ** argv)
{
    hw_connection->power_state=0;
    return 0;
}

int do_plug_usb_s(int argc,char ** argv)
{
    hw_connection->usb_state=1;
    return 0;
}

int do_unplug_usb_s(int argc,char ** argv)
{
    hw_connection->usb_state=0;
    return 0;
}

int do_plug_fw_s(int argc,char ** argv)
{
    hw_connection->fw_state=1;
    return 0;
}

int do_unplug_fw_s(int argc,char ** argv)
{
    hw_connection->fw_state=0;
    return 0;
}

int do_plug_cf_s(int argc,char ** argv)
{
    hw_connection->cf_state=1;
    return 0;
}

int do_unplug_cf_s(int argc,char ** argv)
{
    hw_connection->cf_state=0;
    return 0;
}

HW_connection::HW_connection()
{
    power_state=0;
    usb_state=0;
    fw_state=0;
    cf_state=0;
    hdd_state=0;

    arch_init();

    hw_connection=this;
    add_cmd_fct("plug_usb",do_plug_usb_s,"Emulate usb plugged");
    add_cmd_fct("unplug_usb",do_unplug_usb_s,"Emulate usb unplugged");
    
    add_cmd_fct("plug_fw",do_plug_usb_s,"Emulate fw plugged");
    add_cmd_fct("unplug_fw",do_unplug_fw_s,"Emulate fw unplugged");
    
    add_cmd_fct("plug_power",do_plug_pw_s,"Emulate power plugged");
    add_cmd_fct("unplug_power",do_unplug_pw_s,"Emulate power unplugged");
    
    add_cmd_fct("plug_cf",do_plug_cf_s,"Emulate cf plugged");
    add_cmd_fct("unplug_cf",do_unplug_cf_s,"Emulate cf unplugged");


}

HW_connection::~HW_connection()
{

}


