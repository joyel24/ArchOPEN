/*
*   include/kernel/dma.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#ifndef __DMA_H
#define __DMA_H

#include <kernel/io.h>

#include <driver/hardware.h>

#include <target/chip/dma.h>

void dma_setup(void * inAddr,int inBase,int inDev,bool inFixed,void * outAddr,int outBase,int outDev,bool outFixed,int size);
void dma_start();
bool dma_pending();

#endif
