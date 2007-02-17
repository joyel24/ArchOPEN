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

#ifndef __ATA_ARCH_H
#define __ATA_ARCH_H

//#define NO_DMA

#define DMA_ATA_HD         0x3
#define DMA_ATA_CF         0x3

#define DMA_ATA_READ_ADDRESS             0xa0
#define DMA_ATA_WRITE_ADDRESS            0x20

#define HD_BASE                          0x50000000
#define CF_BASE                          0x50000000

#define ATA_OUTB(DATA,REG)               {outb(DATA,REG);}
#define ATA_INB(REG)                     (inb((REG)|0x80))
#define ATA_OUTW(DATA,REG)               {outw(DATA,REG);}
#define ATA_INW(REG)                     (inw((REG)|0x80))      

#define HD_DATA                          (HD_BASE+0x20)
#define HD_ERROR                         (HD_BASE+0x22)
#define HD_NSECTOR                       (HD_BASE+0x24)
#define HD_SECTOR                        (HD_BASE+0x26)
#define HD_LCYL                          (HD_BASE+0x28)
#define HD_HCYL                          (HD_BASE+0x2a)
#define HD_SELECT                        (HD_BASE+0x2c)
#define HD_CONTROL                       (HD_BASE+0x1c)
#define HD_ALTSTATUS                     (HD_BASE+0x1c)
#define HD_STATUS                        (HD_BASE+0x2e)
#define HD_COMMAND                       (HD_BASE+0x2e)

#define CF_DATA                          (CF_BASE+0xa0)
#define CF_ERROR                         (CF_BASE+0xa2)
#define CF_NSECTOR                       (CF_BASE+0x24)
#define CF_SECTOR                        (CF_BASE+0x26)
#define CF_LCYL                          (CF_BASE+0x28)
#define CF_HCYL                          (CF_BASE+0x2a)
#define CF_SELECT                        (CF_BASE+0x2c)
#define CF_CONTROL                       (CF_BASE+0x9c)
#define CF_ALTSTATUS                     (CF_BASE+0x9c)
#define CF_STATUS                        (CF_BASE+0xae)
#define CF_COMMAND                       (CF_BASE+0x2e)

#endif
