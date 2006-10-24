
/*
*   HW_IRQ.cpp
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

#include <HW/HW_IRQ.h>

char * str_irq_fiq[] = {
    "FIQ0",
    "FIQ1",
    "IRQ0",
    "IRQ1",
};

uint32_t HW_IRQ::read(uint32_t addr,int size)
{
    int ret_val = 0;
    int num=0;
    int mod_val=0xFFFF;

    switch(addr)
    {
        case INT_STATUS + FIQ_0:
        case INT_STATUS + IRQ_0:         //mod_val=0xFFF7;
        case INT_STATUS + FIQ_1:
        case INT_STATUS + IRQ_1:
            num = (addr - INT_STATUS)/2; //addr - BASE_ENTRY == FIQ_0 or FIQ_1 or ...
            ret_val=status[num];
            status[num] &= 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s STATUS @0x%08x, size %x send %x\n",name,str_irq_fiq[num],addr,size,ret_val);
            break;

        case INT_FIQ_ENTRY :
            ret_val = entry[0] & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s FIQ ENTRY0 @0x%08x, size %x send %x\n",
                name,str_irq_fiq[num],addr,size,ret_val);
            break;
        case INT_FIQ_ENTRY + 0x2:
            ret_val = (entry[0]>>16) & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s FIQ ENTRY1 @0x%08x, size %x send %x\n",
                name,str_irq_fiq[num],addr,size,ret_val);
            break;

        case INT_IRQ_ENTRY :
            ret_val = entry[1] & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s IRQ ENTRY0 @0x%08x, size %x send %x\n",
                name,str_irq_fiq[num],addr,size,ret_val);
            break;
        case INT_IRQ_ENTRY + 0x2:
            ret_val = (entry[1]>>16) & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s IRQ ENTRY1 @0x%08x, size %x send %x\n",
                name,str_irq_fiq[num],addr,size,ret_val);
            break;

        case INT_FSEL :
            ret_val = fsel[0] & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read Fsel0 @0x%08x, size %x send %x\n",
                name,addr,size,ret_val);
            break;
        case INT_FSEL + 0x2:
            ret_val = fsel[1] & 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read Fsel1 @0x%08x, size %x send %x\n",
                name,addr,size,ret_val);
            break;

        case INT_ENABLE :
            ret_val=enable[0];
            DEBUG_HW(IRQ_HW_DEBUG,"%s read ENABLE 0 @0x%08x, size %x send %x\n",name,addr,size,ret_val);
            break;
        case INT_ENABLE + 0x2:
            ret_val=enable[1];
            DEBUG_HW(IRQ_HW_DEBUG,"%s read ENABLE 1 @0x%08x, size %x send %x\n",name,addr,size,ret_val);
            break;

        case INT_EABASE:
            ret_val = (eabase & 0xFFF8) | (bloc_size & 0x3);
            DEBUG_HW(IRQ_HW_DEBUG,"%s read EABSE0 @0x%08x, size %x send %x (eabase=%x,bloc_size=%x)\n",
                name,addr,size,ret_val,eabase,bloc_size);
            break;
        case INT_EABASE+2:
            ret_val = ((eabase>>16) & 0x1FFF);
            DEBUG_HW(IRQ_HW_DEBUG,"%s read EABSE1 @0x%08x, size %x send %x (eabase=%x,bloc_size=%x)\n",
                name,addr,size,ret_val,eabase,bloc_size);
            break;

        default:
        {
            /*int prio_addr = (addr-INT_INTPRIO)/2;
            if(prio_addr>=0 && prio_addr <= (NB_INT/2))
            {
                ret_val = (prio[prio_addr*2] & 0x1F) | ((prio[prio_addr*2+1]<<8) & 0x1F);
            }
            else
            {
                ret_val=0;
                DEBUG_HW(IRQ_HW_DEBUG,"%s UKN read @0x%08x, size %x\n",name,addr,size);
            }*/
            #warning PRIO not supported
            break;
        }
    }
    return ret_val;
}

void HW_IRQ::write(uint32_t addr,uint32_t val,int size)
{
    int num=0;
    switch(addr)
    {
        case INT_STATUS+FIQ_0:
        case INT_STATUS+FIQ_1:
        case INT_STATUS+IRQ_0:
        case INT_STATUS+IRQ_1:
            num = (addr - INT_STATUS)/2;
            status[num]=(status[num]|val)&0xFFFF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s STATUS @0x%08x, size %x\n",
                name,val,str_irq_fiq[num],addr,size);
            break;
        case INT_FIQ_ENTRY:
        case INT_FIQ_ENTRY+0x2:
            num = (addr - INT_FIQ_ENTRY)/2;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to FIQ ENTRY %d @0x%08x, size %x NOT Allowed\n",
                name,val,num,addr,size);
            break;
        case INT_IRQ_ENTRY:
        case INT_IRQ_ENTRY+0x2:
            num = (addr - INT_IRQ_ENTRY)/2;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to IRQ ENTRY %d @0x%08x, size %x NOT Allowed\n",
                name,val,num,addr,size);
            break;
        case INT_FSEL:
        case INT_FSEL+0x2:
            num = (addr - INT_FSEL)/2;
            fsel[num]=val&0xFFFF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to FSEL %d @0x%08x, size %x\n",
                name,val,num,addr,size);
            break;
        case INT_ENABLE:
        case INT_ENABLE+0x2:
            num = (addr - INT_ENABLE)/2;
            enable[num]=val&0xFFFF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to ENABLE %d @0x%08x, size %x\n",
                name,val,num,addr,size);
            break;
        case INT_EABASE:
            eabase = (eabase & 0xFFFF0000) | (val & 0xFFF8);
            bloc_size = val & 0x3;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to EABSE0 @0x%08x, size %x (eabase=%x,bloc_size=%x)\n",
                name,val,addr,size,eabase,bloc_size);
            break;
        case INT_EABASE+2:
            eabase = (eabase & 0xFFF8) | ((val&0x1FFF)<<16);
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to EABSE1 @0x%08x, size %x (eabase=%x,bloc_size=%x)\n",
                name,val,addr,size,eabase,bloc_size);
            break;
        default:
        {
            /*int prio_addr = (addr-BASE_INTPRIO)/2;
            if(prio_addr>=0 && prio_addr <= (NB_INT/2))
            {
                prio[prio_addr*2] = val & 0x1F;
                prio[prio_addr*2+1] = (val>>8) & 0x1F;
                calcEntry();
            }
            else
            {
                DEBUG_HW(IRQ_HW_DEBUG,"%s UKN write %x to @0x%08x, size %x\n",
                name,val,addr,size);
            }*/
            #warning PRIO not supported
            break;
        }
    }
}

