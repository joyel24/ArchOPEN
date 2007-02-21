/*
*   HW_dma.cpp
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

#include <HW/HW_TI.h>
#include <HW/HW_ata.h>

#include <HW/HW_dma.h>
#include <emu.h>

HW_dma::HW_dma(mem_space * mem):HW_access(DMA_START,DMA_END,"DMA")
{
    dma_src=dma_dst=dma_size=0;
    device_sel=dma_endian=run_status=0;
    this->mem=mem;
}

uint32_t HW_dma::read(uint32_t addr,int size)
{
    int ret_val = 0;

    switch(addr)
    {
        case DMA_SRC_HI:
            ret_val = (dma_src >> 16)&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SRC HI = %x\n",ret_val);
            break;
        case DMA_SRC_LO:
            ret_val = dma_src&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SRC LO = %x\n",ret_val);
            break;
        case DMA_DST_HI:
            ret_val = (dma_dst >> 16)&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DST HI = %x\n",ret_val);
            break;
        case DMA_DST_LO:
            ret_val = dma_src&0xFFFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DST LO = %x\n",ret_val);
            break;
        case DMA_SIZE:
            ret_val = dma_size;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - SIZE = %x\n",ret_val);
            break;
        case DMA_SEL:
            ret_val = device_sel;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DEVICE SELECT = %x\n",ret_val);
            break;
        case DMA_CTL:
            ret_val = run_status | (dma_endian<<8) ;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - DMA state = %x\n",ret_val);
            break;
        default:
            DEBUG_HW(DMA_HW_DEBUG,"DMA - read - BAD address: %x\n",addr);
            break;
    }

    return ret_val;
}

void HW_dma::write(uint32_t addr,uint32_t val,int size)
{

    switch(addr)
    {
        case DMA_SRC_HI:
            dma_src = (dma_src&0xFFFF) | ((val << 16)&0xFFFF0000);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SRC HI = %x (src=%x)\n",val,dma_src);
            break;
        case DMA_SRC_LO:
            dma_src = (dma_src&0xFFFF0000) | (val & 0xFFFF);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SRC LO = %x (src=%x)\n",val,dma_src);
            break;
        case DMA_DST_HI:
            dma_dst = (dma_dst&0xFFFF) | ((val << 16)&0xFFFF0000);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DST HI = %x (dst=%x)\n",val,dma_dst);
            break;
        case DMA_DST_LO:
            dma_dst = (dma_dst&0xFFFF0000) | (val & 0xFFFF);
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DST LO = %x (dst=%x)\n",val,dma_dst);
            break;
        case DMA_SIZE:
            dma_size = val;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - SIZE = %x\n",val);
            break;
        case DMA_SEL:
            device_sel = val&0xFF;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - DEVICE SELECT = %x\n",val);
            break;
        case DMA_GO:
            dma_endian = (val>>8)&0x1;
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - START - (endian=%x)",dma_endian);
            if(val & 0x1)
            {
                int src=(device_sel & 0xF0)>>4;
                int dest=device_sel&0xF;
                DEBUG_HW(DMA_HW_DEBUG," - xfer from %x to %x, size = %x, dir : %s->%s ... ",dma_src,dma_dst,dma_size,
                        src==DMA_SDRAM?"SDRAM":src==DMA_ATA?"ATA":"CF",
                        dest==DMA_SDRAM?"SDRAM":dest==DMA_ATA?"ATA":"CF");
                switch(src)
                {
                    case DMA_SDRAM:
                        /*if(dma_src>SDRAM_START)
                            dma_src-=SDRAM_START;*/
                        for (int i = 0; i < dma_size; i++)
                            data[i+data_ptr] = mem->read(SDRAM_START + dma_src + i,1);
                        data_ptr+=dma_size;
                        dma_src+=dma_size;
                        if(data_ptr>=data_size)
                        {
                            DEBUG_HW(DMA_HW_DEBUG," (finale) \n");
                            HD->write_buffer(data,data_size);
                            //mem->hw_ata->setStatus(IDE_STATUS_RDY);
                        }
                        run_status=1;
                        DEBUG_HW(DMA_HW_DEBUG,"done");
                        break;
                    case DMA_ATA:
                    case DMA_CF:
                        /*if(dma_dst>SDRAM_START)
                            dma_dst-=SDRAM_START;*/
                        DEBUG_HW(DMA_HW_DEBUG,"real dest = %x , src val (%x/%x) %02x%02x%02x%02x  ",
                            SDRAM_START + dma_dst,data_ptr,data_size,data[data_ptr]&0xFF,data[data_ptr+1]&0xFF,
                            data[data_ptr+2]&0xFF,data[data_ptr+3]&0xFF);
                        for (int i = 0; i < dma_size; i++)
                             mem->write(SDRAM_START + dma_dst + i,data[i+data_ptr] & 0xff,1);
                        data_ptr+=dma_size;
                        dma_dst+=dma_size;
                        if(data_ptr>=data_size)
                        {
                            DEBUG_HW(DMA_HW_DEBUG," (finale) ");
                            //mem->hw_ata->setStatus(IDE_STATUS_RDY);
                        }
                        run_status=1;
                        DEBUG_HW(DMA_HW_DEBUG,"done");
                        break;
                    default:
                        DEBUG_HW(DMA_HW_DEBUG,"error bad dev select: %x",device_sel);
                        break;
                }
            }
            DEBUG_HW(DMA_HW_DEBUG,"\n");
            break;
        default:
            DEBUG_HW(DMA_HW_DEBUG,"DMA - write - BAD address: %x\n",addr);
            break;
    }

}

void HW_dma::nxtStep(void)
{
    if(run_status)
    {
        nbWait++;
        if(nbWait>40)
        {
            run_status=0;
            if(data_ptr>=data_size)
            {
                DEBUG_HW(DMA_HW_DEBUG,"Set RDY\n");         
                HD->setStatus(IDE_STATUS_RDY);
            }
            nbWait=0;
            mem->hw_TI->HW_irq->do_INT(INT_DMA);
            //CHG_RUN_MODE(STEP)
            DEBUG_HW(DMA_HW_DEBUG,"Stop DMA\n");            
        }
    }
}

void HW_dma::init_ata_xfer(char * data,int data_ptr,int data_size,HW_ata * HD)
{
    this->data = data;
    this->data_ptr = data_ptr;
    this->data_size = data_size;
    this->HD = HD;
}
