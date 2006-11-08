
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
        case INT_FIQ_STATUS:
            ret_val=status[0];
            status[0] &= 0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s STATUS @0x%08x, size %x send %x\n",name,str_irq_fiq[0],addr,size,ret_val);
            break;
        case INT_IRQ_STATUS + IRQ_0:         //mod_val=0xFFF7;
            ret_val=status[1];
            status[1] &= 0xFFF7;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read IRQ0 STATUS @0x%08x, size %x send %x\n",
                    name,addr,size,ret_val);
            break;
        case INT_IRQ_STATUS + IRQ_1:
            ret_val=status[2];
            status[2] &= 0x07FF;    // !!!!MOD        
#warning MOD IRQ for GIO0
            DEBUG_HW(IRQ_HW_DEBUG,"%s read IRQ1 STATUS @0x%08x, size %x send %x\n",
                    name,addr,size,ret_val);
            break;
        case INT_FIQ_ENABLE :
            ret_val=enable[0]&0x1;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENABLE @0x%08x, size %x send %x\n",
                name,str_irq_fiq[0],addr,size,ret_val);
            break;
        case INT_IRQ_ENABLE + IRQ_0:
            ret_val=enable[1]&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENABLE @0x%08x, size %x send %x\n",
                name,str_irq_fiq[1],addr,size,ret_val);
            break; 
        case INT_IRQ_ENABLE + IRQ_1:
            ret_val=enable[2]&0x07FF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read %s ENABLE @0x%08x, size %x send %x\n",
                name,str_irq_fiq[2],addr,size,ret_val);
            break;
        case INT_ID:
            ret_val=id;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read INT ID  @0x%08x, size %x send %x\n",
                name,addr,size,ret_val);
            break;
        case INT_RAW:
            ret_val=raw;
            DEBUG_HW(IRQ_HW_DEBUG,"%s read INT RAW  @0x%08x, size %x send %x\n",
                name,addr,size,ret_val);
            break;
        default:
            DEBUG_HW(IRQ_HW_DEBUG,"%s UKN read @0x%08x, size %x\n",name,addr,size);
            break;
    }
    return ret_val;
}

void HW_IRQ::write(uint32_t addr,uint32_t val,int size)
{
    int num=0;
    switch(addr)
    {
        case INT_FIQ_STATUS:
            status[0]=(status[0]|val)&0xFFFF;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to FIQ STATUS @0x%08x, size %x\n",
                name,val,addr,size);
            break;
        case INT_IRQ_STATUS + IRQ_0:         //mod_val=0xFFF7;
            status[1]=(status[1]|val)&0xFFF7;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s STATUS @0x%08x, size %x\n",
                name,val,str_irq_fiq[1],addr,size);
            break;
        case INT_IRQ_STATUS + IRQ_1:
            status[2]=(status[2]|val)&0xFFFF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to %s STATUS @0x%08x, size %x\n",
                name,val,str_irq_fiq[2],addr,size);
            break;
        case INT_FIQ_ENABLE :
            enable[0]=val&0x1;
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to FIQ ENABLE %d @0x%08x, size %x\n",
                name,val,num,addr,size);
            break;
        case INT_IRQ_ENABLE + IRQ_0:
            enable[1]=val&0xFFFF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to IRQ0 ENABLE %d @0x%08x, size %x\n",
                name,val,num,addr,size);
            break; 
        case INT_IRQ_ENABLE + IRQ_1:
            enable[2]=val&0x07FF;
            calcEntry();
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to IRQ0 ENABLE %d @0x%08x, size %x\n",
                name,val,num,addr,size);
            break;
        case INT_ID:
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to INT ID (what should we do ?) @0x%08x, size %x\n",
                name,val,addr,size);
            break;
        case INT_RAW:
            DEBUG_HW(IRQ_HW_DEBUG,"%s write %x to INT RAW 0x%08x, size %x\n",
                name,val,addr,size);
            raw=val;
            break;
        default:
            DEBUG_HW(IRQ_HW_DEBUG,"%s UKN write %x to @0x%08x, size %x\n",name,val,addr,size);
            break;
    }
}

#define CNT_MASK (raw&2)
#define ID_MASK (raw&1)

void HW_IRQ::calcEntry(void)
{
    int real_status[2];
    int tmp_status[2];
    int cnt=0;
    id=-1;
    
    
    real_status[0] = (~status[1])&enable[1];
    real_status[1] = (~status[2])&enable[2];
    tmp_status[0] = (~status[1]);
    tmp_status[1] = (~status[2]);


    for(int i=0;i<NB_INT;i++)
    {
        if(CNT_MASK)
        {
            if(tmp_status[REG_NUM(i)]) cnt++;
        }
        else
        {
            if(real_status[REG_NUM(i)]) cnt++;
        }
        
        if(id==-1)
        {
            if(ID_MASK)
            {
                if(tmp_status[REG_NUM(i)]) id=i;
            }
            else
            {
                if(real_status[REG_NUM(i)]) id=i;
            }
        }
        
    }
    
    if(id==-1) id=0;
    
    id |= (cnt<<8);
    printf("id = %x\n",id);
    
}
