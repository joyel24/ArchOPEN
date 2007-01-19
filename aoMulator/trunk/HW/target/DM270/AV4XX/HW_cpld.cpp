/*
*   HW_cpld.cpp
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

#include <target/chip/arch/HW_cpld.h>

#include <core/my_print.h>

#define CPLD_V_LOW
#define CPLD_V_HIGH

#include <core/cmd_line.h>
#include "cpld_cmd_line_fct.h"

#include <HW/HW_connection.h>
#include <HW/HW_btn.h>

HW_cpld::HW_cpld(void):HW_access(CPLD_START,CPLD_END,"CPLD")
{
    exit_on_not_match = false;

    init_cpld_static_fct(this);

    cpld_ata_mode = 0x0 ; /* only HD/CF is concidered => we use HD */
    cpld_module_type = 0xF; /* no modules */
    cpld_val[0] = 0x7;
    cpld_val[1] = 0;
    cpld_val[2] = 0x0;
    cpld_val[3] = 0;
    fw_connected = 1;
    hdd_on =0;
    bck_light = 0;
    ide_reset = 0;
}

HW_cpld::~HW_cpld()
{

}

extern HW_connection * hw_connection;
extern HW_btn * btn;

uint32_t HW_cpld::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;


    switch(addr)
    {
        case CPLD_START+CPLD_PORT_OFFSET+0x0:
            ret_val=cpld_val[0];
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - read, size %x: %x\n",size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x2:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - read, size %x: %x (USB %s)\n",size,cpld_val[1],cpld_val[1]&0x1?"enable":"disable");
            ret_val = cpld_val[1];
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x4:
            ret_val = cpld_val[2];
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - read, size %x: %x\n",size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x6:
            ret_val = ((~hw_connection->fw_state)&0x1)<<3;
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - read, size %x: %x\n",size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x8:
            ret_val = 0x5;
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD4 - read, size %x: %x\n",size,ret_val);
            break;
// Buttons
        case CPLD_START+CPLD_PORT_OFFSET+0xA:
            BTN_FCT(btn->btn_var[BTN_UP]   ,0x1)
            BTN_FCT(btn->btn_var[BTN_RIGHT],0x2)
            BTN_FCT(btn->btn_var[BTN_F3],0x4)
            BTN_FCT(btn->btn_var[BTN_F2] ,0x8)
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0xC:
            BTN_FCT(btn->btn_var[BTN_LEFT]   ,0x1)
            BTN_FCT(btn->btn_var[BTN_DOWN]   ,0x2)
            BTN_FCT(btn->btn_var[BTN_F1]   ,0x8)
            ret_val |= 0x4;
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - read buttons (@0x%08x), size %x => send %x\n",addr,size,ret_val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0xe:
            ret_val=0;
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD5 - read, size %x: %x\n",size,ret_val);
            break;
        default:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD - read ERROR ukn addr: @0x%08x, size %x\n",addr,size);
            ret_val=0;
            break;
    }
    return ret_val;
}

void HW_cpld::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case CPLD_START+CPLD_PORT_OFFSET+0x0:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD0 - write ata mode, size %x: %x\n",size,val);
            cpld_ata_mode = val;
            //cpld0_val = val;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x2:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD1 - write, size %x: %x (USB %s)\n",size,val,val&0x1?"enable":"disable");
            cpld_val[1] = val;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x4:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD2 - write, size %x: %x\n",size,val);
            cpld_val[2] = val;
            /*bck_light = val&0x4;
            ide_reset = val&0x1;*/
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x6:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD3 - write, size %x: %x (HD is %s)\n",size,val,(val&0x8)?"ON":"OFF");
            cpld_val[3] = val;
            hdd_on = val&0x8;
            hw_connection->hdd_state = hdd_on?1:0;
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0x8:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD4 - write, size %x: %x NOT supported\n",size,val);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0xA:
        case CPLD_START+CPLD_PORT_OFFSET+0xC:
            DEBUG_HW(CPLDBTN_HW_DEBUG,"CPLD - !!!! write buttons (@0x%08x), size %x\n",addr,size);
            break;
        case CPLD_START+CPLD_PORT_OFFSET+0xe:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD5 - write size %x: %x\n",size,val);
            break;
        default:
            DEBUG_HW(CPLD_HW_DEBUG,"CPLD - write ERROR ukn addr: @0x%08x, size %x\n",addr,size);
            break;
    }
}

void HW_cpld::cpld_print_state(void)
{
    printf("CPLD: write state:\n-0:%04x\n-1:%04x\n-2:%04x\n-3:%04x\n",cpld_val[0],cpld_val[1],cpld_val[2],cpld_val[3]);
}


