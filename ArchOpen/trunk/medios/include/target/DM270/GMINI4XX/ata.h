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

#define DMA_ATA_HD         0x3
#define DMA_ATA_CF         0x4

#define DMA_ATA_READ_ADDRESS             0x20
#define DMA_ATA_WRITE_ADDRESS            0x20

#define HD_BASE                          0x05100000
#define CF_BASE                          0x06900000

#define ATA_OUTB(DATA,REG)               {outb(DATA,REG);}
#define ATA_INB(REG)                     (inb(REG))
#define ATA_OUTW(DATA,REG)               {outw(DATA,REG);}
#define ATA_INW(REG)                     (inw(REG)) 

#define HD_DATA                          (HD_BASE+0x020)
#define HD_ERROR                         (HD_BASE+0x022)
#define HD_NSECTOR                       (HD_BASE+0x024)
#define HD_SECTOR                        (HD_BASE+0x026)
#define HD_LCYL                          (HD_BASE+0x028)
#define HD_HCYL                          (HD_BASE+0x02a)
#define HD_SELECT                        (HD_BASE+0x02c)
#define HD_CONTROL                       (HD_BASE+0x01c)
#define HD_ALTSTATUS                     (HD_BASE+0x01c)
#define HD_STATUS                        (HD_BASE+0x02e)
#define HD_COMMAND                       (HD_BASE+0x02e)

#define CF_DATA                          (CF_BASE+0x020)
#define CF_ERROR                         (CF_BASE+0x022)
#define CF_NSECTOR                       (CF_BASE+0x024)
#define CF_SECTOR                        (CF_BASE+0x026)
#define CF_LCYL                          (CF_BASE+0x028)
#define CF_HCYL                          (CF_BASE+0x02a)
#define CF_SELECT                        (CF_BASE+0x02c)
#define CF_CONTROL                       (CF_BASE+0x01c)
#define CF_ALTSTATUS                     (CF_BASE+0x01c)
#define CF_STATUS                        (CF_BASE+0x02e)
#define CF_COMMAND                       (CF_BASE+0x02e)

#endif
