/* 
*   flash_mem.cpp
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

#include <emu.h>
#include <HW/flash_mem.h>

char * cmd_str = {
"NO_CMD"
};

flash_MEM::flash_MEM(char * fname,uint32_t start,uint32_t end,char * name, uint32_t load_offset):HW_mem(fname,start,end,name,load_offset)
{
    curCmd = NO_CMD;
}

flash_MEM::~flash_MEM(void)
{
    
}

uint32_t flash_MEM::read(uint32_t addr,int size)
{
    unsigned int ret_val;
    if(curCmd==NO_CMD)
    {
        ret_val=HW_mem::read(addr,size);
        DEBUG_HW(FLASH_HW_DEBUG,"%s normal read @0x%08x, size %x, sending %x\n",name,
            addr,size,ret_val);
    }
    else
    {
        ret_val=0;
        DEBUG_HW(FLASH_HW_DEBUG,"%s Read in cmd %s @0x%08x, size %x, sending %x\n",name,cmd_str[curCmd],
            addr,size,ret_val);
    }
    return ret_val;
}

void flash_MEM::write(uint32_t addr,uint32_t val,int size)
{    
    DEBUG_HW(FLASH_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
