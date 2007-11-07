/*
*   mem_space.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __MEM_SPACE_H
#define __MEM_SPACE_H

#include <emu.h>
#include <core/HW_node.h>
#include <core/HW_mem.h>
#include <core/bkpt_list.h>

#include <target/chip/arch/HW_cpld.h>

class HW_TI;

#include <HW/HW_ata.h>



class mem_space:public HW_node {
    public:
        mem_space(char * flash,char * sdram);
        ~mem_space();

        int do_cmd_dump(int argc,char ** argv);
        int do_cmd_add_bk_mem(int argc,char ** argv,int type);
        int do_cmd_dump_dsp(void);
        int do_cmd_write_mem(int argc,char ** argv);

        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);
        void printString(uint32_t addr);
        void printString(uint32_t addr,int size);
        
        HW_mem * dsp_mem;
        HW_mem * flash_mem;
        HW_mem * sd;
        HW_mem * iram;

        HW_ata * hw_ata;

        HW_TI * hw_TI;
        HW_cpld * hw_cpld;

    private:
        bkpt_list * bkpt;
        bkpt_list * bkptr;        
        bkpt_list * bkptw;
        
        void arch_init(void);
};

#endif
