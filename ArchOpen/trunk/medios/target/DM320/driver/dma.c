/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/types.h>

#include <driver/dma.h>

void dma_setup(void * inAddr,int inBase,int inDev,bool inFixed,void * outAddr,int outBase,int outDev,bool outFixed,int size){
    unsigned int mtcAddr,ahbAddr,ctl,dev;

    if(outDev==DMA_SDRAM){

        ahbAddr=(unsigned int)outAddr;
        mtcAddr=(unsigned int)inAddr-inBase;
        dev=inDev;
        ctl=DMA_CTL_MTC2AHB;

        if(inFixed){
            mtcAddr|=DMA_ADDR_FIXED;
        }
        if(outFixed){
            ahbAddr|=DMA_ADDR_FIXED;
        }
    }else{

        ahbAddr=(unsigned int)inAddr;
        mtcAddr=(unsigned int)outAddr-outBase;
        dev=outDev;
        ctl=DMA_CTL_AHB2MTC;

        if(inFixed){
            ahbAddr|=DMA_ADDR_FIXED;
        }
        if(outFixed){
            mtcAddr|=DMA_ADDR_FIXED;
        }
    }

    outw((ahbAddr>>16)&0xffff,DMA_AHB_HI);
    outw(ahbAddr&0xffff,DMA_AHB_LO);
    outw((mtcAddr>>16)&0xffff,DMA_MTC_HI);
    outw(mtcAddr&0xffff,DMA_MTC_LO);
    outw(size>>2,DMA_SIZE);
    outw(dev,DMA_DEV_SEL);
    outw(ctl,DMA_CONTROL);
}

void dma_start(){
    outw(inw(DMA_CONTROL)|DMA_CTL_START,DMA_CONTROL);
}

bool dma_pending(){
    return (inw(DMA_CONTROL)&DMA_CTL_START)!=0;
}
