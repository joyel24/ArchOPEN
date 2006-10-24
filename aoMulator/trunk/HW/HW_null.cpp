/* 
*   HW_null.cpp
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

#include <HW/HW_null.h>

HW_null::HW_null(uint32_t start,uint32_t end,char * name,int debug_aff):HW_access(start,end,name)
{
    this->debug_aff = debug_aff;
}

HW_null::HW_null(uint32_t start,uint32_t end,char * name):HW_access(start,end,name)
{
    debug_aff = 1;
}

uint32_t HW_null::read(uint32_t addr,int size)
{
    if(debug_aff)
        DEBUG_HW(NULL_HW_DEBUG,"%s read @0x%08x, size %x\n",name,addr,size);
    return 0;
}

void HW_null::write(uint32_t addr,uint32_t val,int size)
{
    if(debug_aff)
        DEBUG_HW(NULL_HW_DEBUG,"%s write %x @0x%08x, size %x\n",name,val,addr,size);
}
