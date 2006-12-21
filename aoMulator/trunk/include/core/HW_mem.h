/*
*   HW_mem.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_MEM_H
#define __HW_MEM_H

#include <emu.h>
#include <core/HW_access.h>


class HW_mem : public HW_access {
    public:
        HW_mem(char * fname,uint32_t start,uint32_t end,char * name, uint32_t load_offset);
        HW_mem(char * fname,uint32_t start,uint32_t end,char * name);
        ~HW_mem();
        
        void printString(uint32_t addr);
        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);

        char *  mem;

    private:


        void init(char * fname,uint32_t start,uint32_t end,uint32_t load_offset);
};

#endif // __HW_MEM_H
