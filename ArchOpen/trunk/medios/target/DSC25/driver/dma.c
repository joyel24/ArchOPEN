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

#include <kernel/io.h>

#include <driver/dma.h>

void dma_setup(void * inAddr,int inBase,int inDev,bool inFixed,void * outAddr,int outBase,int outDev,bool outFixed,int size){
    unsigned int in,out;

    in=(unsigned int)inAddr-inBase;
    out=(unsigned int)outAddr-outBase;

    if(inFixed){
        in|=DMA_ADDR_FIXED;
    }

    if(outFixed){
        out|=DMA_ADDR_FIXED;
    }

    outw((in>>16)&0xFFFF,DMA_SRC_HI);
    outw(in&0xFFFF,DMA_SRC_LO);

    outw((out>>16)&0xFFFF,DMA_DEST_HI);
    outw(out&0xFFFF,DMA_DEST_LO);

    outw(size&0xFFFF,DMA_SIZE);

    outw(((inDev<<4)&0x00F0) | (outDev&0x000F),DMA_DEV_SEL);
}

void dma_start(){
    outw(inw(DMA_START) | 0x1,DMA_START);
}

bool dma_pending(){
    return (inw(DMA_STATE)&0x1)==0x1;
}
