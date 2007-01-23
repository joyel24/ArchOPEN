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
    curCmd = FLASH_NO_CMD ;
    step=0;
}

flash_MEM::~flash_MEM(void)
{
    
}

uint32_t flash_MEM::flash_stdRead(uint32_t addr,int size)
{
    uint32_t ret_val=HW_mem::read(addr,size);
    DEBUG_HW(FLASH_HW_DEBUG,"%s normal read @0x%08x, size %x, ret val %x\n",name,
             addr,size,ret_val);  
    return ret_val; 
}
uint32_t flash_MEM::read(uint32_t addr,int size)
{
    unsigned int ret_val;
    switch(curCmd)
    {
        case FLASH_NO_CMD:
            ret_val=flash_stdRead(addr,size);
            break;
        case FLASH_ID_ENTRY:
            if((addr-start)==0)
            {
                ret_val=0xBF;
                DEBUG_HW(FLASH_HW_DEBUG,"%s Read in cmd ID entry @0x%08x, size %x, ret val %x\n",name,
                         addr,size,ret_val);
            }
            else if((addr-start)==2)
            {
                ret_val=0x2781;
                DEBUG_HW(FLASH_HW_DEBUG,"%s Read in cmd ID entry @0x%08x, size %x, ret val %x\n",name,
                         addr,size,ret_val);
            }
            else
            {
                DEBUG_HW(FLASH_HW_DEBUG,"not reading cmd res\n");
                ret_val=flash_stdRead(addr,size);
            }
            break;
        case FLASH_ERASE_STEP_2:
            ret_val=flash_stdRead(addr,size);
            ret_val&=0x40;
            DEBUG_HW(FLASH_HW_DEBUG,"Flash - doing control read for ERASE val send = %x\n",ret_val);
            curCmd=FLASH_NO_CMD;
            step=0;
            break;
        case FLASH_WRITE:
            ret_val=flash_stdRead(addr,size);
            ret_val&=0x40;
            DEBUG_HW(FLASH_HW_DEBUG,"Flash - doing control read for WRITE val send = %x\n",ret_val);
            curCmd=FLASH_NO_CMD;
            step=0;
            break;
        case FLASH_UKN_CMD:
        default:
            ret_val=0;
            DEBUG_HW(FLASH_HW_DEBUG,"%s Read - unkn cmd: %x\n",name,curCmd);
            break;
    }
    return ret_val;
}

void flash_MEM::write(uint32_t addr,uint32_t val,int size)
{    
    DEBUG_HW(FLASH_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
    switch(step)
    {
        case 0: /* first step*/
            if((addr-start)==0xaaaa && val == 0xaa)
                step++;
            break;
        case 1: /*second step*/
            if((addr-start)==0x5554 && val == 0x55)
                step++;
            else
                step=0;
            break;
        case 2: /*third step */
            if((addr-start)==0xaaaa)
            {
                switch(val)
                {
                    case 0x90: /*ID entry cmd*/
                        curCmd=FLASH_ID_ENTRY;
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - Start cmd %x - ID entry\n",val);
                        step=0;
                        break;
                    case 0xF0: /*End of Software ID*/
                        curCmd=FLASH_NO_CMD;
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - End cmd %x\n",val);
                        step=0;
                        break;
                    case 0x80: /*start erase */
                        curCmd=FLASH_ERASE_STEP_1;
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - First step of Erase cmd %x\n",val);
                        step=0;
                        break;
                    case 0x10: /*chip erase*/
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - Chip erase\n",addr,val);
                        curCmd=FLASH_ERASE_STEP_2;
                        step=0;
                        break;
                    case 0xa0:
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - Write\n",addr,val);
                        curCmd=FLASH_WRITE;
                        step=2;
                        break;
                    default:
                        curCmd=FLASH_UKN_CMD;
                        DEBUG_HW(FLASH_HW_DEBUG,"Flash - UKN cmd %x\n",val);
                        step=0;
                        break;
                }
            }
            else if(curCmd==FLASH_ERASE_STEP_1)
            {
                DEBUG_HW(FLASH_HW_DEBUG,"Flash - Starting Erase from %x type=%x\n",addr,val);
                curCmd=FLASH_ERASE_STEP_2;
                step=0;
            }
            else if(curCmd==FLASH_WRITE)
            {
                DEBUG_HW(FLASH_HW_DEBUG,"Flash - writing %x at %x\n",val,addr);
                HW_mem::write(addr,val,2);
            }
            else
                step=0;
            break;
    }
}
