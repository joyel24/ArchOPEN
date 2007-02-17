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

#ifndef __DMA_DSC25_H
#define __DMA_DSC25_H

#define DMA_BASE           0x30a38

#define DMA_SRC_HI         (DMA_BASE+0x00)
#define DMA_SRC_LO         (DMA_BASE+0x02)
#define DMA_DEST_HI        (DMA_BASE+0x04)
#define DMA_DEST_LO        (DMA_BASE+0x06)
#define DMA_SIZE           (DMA_BASE+0x08)
#define DMA_DEV_SEL        (DMA_BASE+0x0A)
#define DMA_START          (DMA_BASE+0x0C)
#define DMA_STATE          (DMA_BASE+0x0C)

#define DMA_SDRAM          0x5

#define DMA_ADDR_FIXED     0x10000000

#endif
