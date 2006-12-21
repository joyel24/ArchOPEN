/*
*   mem_space.cpp
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

#include <HW/mem_space.h>

#include <core/HW_mem.h>
#include <target/chip/arch/HW_cpld.h>
#include <HW/HW_TI.h>
#include <HW/flash_mem.h>

#include <core/cmd_line.h>
#include <core/bkpt_list.h>
#include <core/my_print.h>

//#define EXIT_ON_ADDR_ERROR

mem_space * mem;

void init_mem_static_fct(mem_space * mem);

mem_space::mem_space(char * flash,char * sdram):HW_node(0x0,0xFFFFFFFF,16,"AVMEM")
{
    /* init bkpt_list */

    bkpt = new_bkpt_list(BKPT_MEM);

    iram=new HW_mem(NULL,IRAM_START,IRAM_END,"IRAM");
    add_item(iram);
    iram->write(RESET_VECTOR,RESET_INIT_VAL,4);

    dsp_mem = new HW_mem(NULL,0x40000,0x50000,"DSP MEM");
    add_item(dsp_mem);

    flash_mem = new flash_MEM(flash,FLASH_START,FLASH_END,"FLASH",FLASH_LOAD_OFFSET);
    add_item(flash_mem);

    sd = new HW_mem(sdram,SDRAM_START,SDRAM_END,"SDRAM",SDRAM_LOAD_OFFSET);
    add_item(sd);

    printf("Adding cpld\n");
    hw_cpld = new HW_cpld();
    add_item(hw_cpld);

    printf("Adding TI chip\n");
    hw_TI = new HW_TI(this,hw_cpld);
    add_item(hw_TI);

    printf("Adding ATA\n");
    hw_ata = new HW_ata(hw_TI);
    add_item(hw_ata);

/*
#ifdef HAS_HW_30A24
    hw_cpld->set30A24(hw_TI->hw_30a24);
#endif
    hw_ata->setDMA(hw_TI->hw_dma);
    hw_cpld->setONOFF(hw_TI->gpio);

#ifdef EXIT_ON_ADDR_ERROR
    exit_on_not_match = true;
#else
    exit_on_not_match = false;
    data_abt_on_not_match = true;
#endif
*/
    /* special init for each ARCH */


    printf("Before arch init\n");
    arch_init();

    init_mem_static_fct(this);
}

mem_space::~mem_space()
{

}

extern uint32_t old_PC;

uint32_t mem_space::read(uint32_t addr,int size)
{
    uint32_t val = HW_node::read(addr,size);

    bkpt->fct(bkpt,addr,BKPT_MEM_READ);

    return val;
}

void mem_space::write(uint32_t addr,uint32_t val,int size)
{


    bkpt->fct(bkpt,addr,BKPT_MEM_WRITE);

    HW_node::write(addr,val,size);

    hw_TI->chkMemAccess(addr,val,size);
}

void mem_space::printString(uint32_t addr)
{
    HW_node::printString(addr);
}


#include "mem_cmd_line_fct.h"
