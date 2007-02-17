/*
*   include/kernel/target/arch_AV3XX/dma.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __DMA_DM320_H
#define __DMA_DM320_H

#define DMA_BASE           0x30a54

#define DMA_AHB_HI         (DMA_BASE+0x00)
#define DMA_AHB_LO         (DMA_BASE+0x02)
#define DMA_MTC_HI         (DMA_BASE+0x04)
#define DMA_MTC_LO         (DMA_BASE+0x06)
#define DMA_SIZE           (DMA_BASE+0x08)
#define DMA_DEV_SEL        (DMA_BASE+0x0A)
#define DMA_CONTROL        (DMA_BASE+0x0C)

#define DMA_CTL_AHB2MTC    0
#define DMA_CTL_MTC2AHB    2

#define DMA_CTL_START      1

#define DMA_ADDR_FIXED     0x80000000

#define DMA_SDRAM          0x5

#endif
