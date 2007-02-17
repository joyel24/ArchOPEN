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

#define DMA_ATA_HD         0x1
#define DMA_ATA_CF         0x1

#define DMA_ATA_READ_ADDRESS             0x400000
#define DMA_ATA_WRITE_ADDRESS            0x400000

#define HD_BASE                          0x02500000
#define CF_BASE                          0x02500000

#define ATA_OUTB(DATA,REG)               {outb(DATA,REG);}
#define ATA_INB(REG)                     (inb(REG))
#define ATA_OUTW(DATA,REG)               {outw(DATA,REG);}
#define ATA_INW(REG)                     (inw(REG)) 

#define HD_DATA                          (HD_BASE+0x000)
#define HD_ERROR                         (HD_BASE+0x080)
#define HD_NSECTOR                       (HD_BASE+0x100)
#define HD_SECTOR                        (HD_BASE+0x180)
#define HD_LCYL                          (HD_BASE+0x200)
#define HD_HCYL                          (HD_BASE+0x280)
#define HD_SELECT                        (HD_BASE+0x300)
#define HD_CONTROL                       (HD_BASE+0x340)
#define HD_ALTSTATUS                     (HD_BASE+0x340)
#define HD_STATUS                        (HD_BASE+0x380)
#define HD_COMMAND                       (HD_BASE+0x380)

#define CF_DATA                          (CF_BASE+0x000)
#define CF_ERROR                         (CF_BASE+0x080)
#define CF_NSECTOR                       (CF_BASE+0x100)
#define CF_SECTOR                        (CF_BASE+0x180)
#define CF_LCYL                          (CF_BASE+0x200)
#define CF_HCYL                          (CF_BASE+0x280)
#define CF_SELECT                        (CF_BASE+0x300)
#define CF_CONTROL                       (CF_BASE+0x340)
#define CF_ALTSTATUS                     (CF_BASE+0x340)
#define CF_STATUS                        (CF_BASE+0x380)
#define CF_COMMAND                       (CF_BASE+0x380)

#endif
