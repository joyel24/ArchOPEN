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

#ifndef __HW_FLASH_MEM_H
#define __HW_FLASH_MEM_H

#include <emu.h>
#include <core/HW_mem.h>

#define FLASH_NO_CMD       0
#define FLASH_UKN_CMD      1
#define FLASH_ID_ENTRY     2
#define FLASH_ERASE_STEP_1 3
#define FLASH_ERASE_STEP_2 4
#define FLASH_WRITE        5


class flash_MEM : public HW_mem {
    public:
        flash_MEM(char * fname,uint32_t start,uint32_t end,char * name, uint32_t load_offset);
        ~flash_MEM();

        uint32_t read(uint32_t addr,int size);
        void write(uint32_t addr,uint32_t val,int size);

    private:
        int curCmd;
        int step;
        uint32_t flash_stdRead(uint32_t addr,int size);
       
};

#endif // __HW_MEM_H
