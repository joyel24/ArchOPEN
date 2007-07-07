/*
*   include/kernel/ata.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ATA_H
#define __ATA_H

#include <kernel/irq.h>
#include <kernel/thread.h>

#include <driver/hardware.h>

#include <fs/disk.h>

#include <target/chip/arch/ata.h>

#define IDE_CMD_READ_SECTORS              0x20
#define IDE_CMD_WRITE_SECTORS             0x30
#define IDE_CMD_READ_MULTIPLE_SECTORS     0xc4
#define IDE_CMD_WRITE_MULTIPLE_SECTORS    0xc5
#define IDE_SET_MULTIPLE_MODE             0xc6
#define IDE_SET_FEATURES                  0xef
#define IDE_SECURITY_FREEZE_LOCK          0xf5
#define IDE_CMD_IDENTIFY                  0xec
#define IDE_CMD_SLEEP                     0xe6

#define IDE_STATUS_BSY                    0x80
#define IDE_STATUS_RDY                    0x40
#define IDE_STATUS_DF                     0x20
#define IDE_STATUS_DRQ                    0x08
#define IDE_STATUS_ERR                    0x01

#define IDE_ERROR_ABRT                    0x04

#define IDE_SEL_LBA                       0x40

#define ATA_DO_READ                       0
#define ATA_DO_WRITE                      1
#define ATA_DO_IDENT                      2

#define ATA_WITH_DMA                             1
#define ATA_NO_DMA                               0

#define ATA_GET_WORD(DATA,N) (DATA[(N*2)+1]<<8|DATA[N*2])

#define CHK_WRT_NSECTOR                    0xa5
#define CHK_WRT_SECTOR                     0x5a
#define CHK_WRT_LCYL                       0xaa
#define CHK_WRT_HCYL                       0x55
#define CHK_RD_NSECTOR                     0xa5
#define CHK_RD_SECTOR                      0x5a
#define CHK_RD_LCYL                        0xaa
#define CHK_RD_HCYL                        0x55
#define CHK_RD_MASK_NSECTOR                0xFF
#define CHK_RD_MASK_SECTOR                 0xFF
#define CHK_RD_MASK_LCYL                   0xFF
#define CHK_RD_MASK_HCYL                   0xFF

#define WAIT_XFER_TOUT                    10*HZ //10 seconds
#define WAIT_RDY_TOUT                     10*HZ //10 seconds
#define WAIT_BSY_TOUT                     30*HZ //30 seconds Yes! this cab take long
#define READ_WRITE_TOUT                   5*HZ

#define ATA_FORCE_STOP                    0  /* power off is not split in 2 parts + timer */
#define ATA_DELAY_STOP                    1  /* power off wait loop is executed in background */

#define SECTOR_SIZE      512

#define DMA_ATA                           (cur_disk==HD_DISK?DMA_ATA_HD:DMA_ATA_CF)

#define IDE_DATA                          (cur_disk==HD_DISK?HD_DATA:CF_DATA)
#define IDE_ERROR                         (cur_disk==HD_DISK?HD_ERROR:CF_ERROR)
#define IDE_FEATURES                      (cur_disk==HD_DISK?HD_ERROR:CF_ERROR)
#define IDE_NSECTOR                       (cur_disk==HD_DISK?HD_NSECTOR:CF_NSECTOR)
#define IDE_SECTOR                        (cur_disk==HD_DISK?HD_SECTOR:CF_SECTOR)
#define IDE_LCYL                          (cur_disk==HD_DISK?HD_LCYL:CF_LCYL)
#define IDE_HCYL                          (cur_disk==HD_DISK?HD_HCYL:CF_HCYL)
#define IDE_SELECT                        (cur_disk==HD_DISK?HD_SELECT:CF_SELECT)
#define IDE_CONTROL                       (cur_disk==HD_DISK?HD_CONTROL:CF_CONTROL)
#define IDE_STATUS                        (cur_disk==HD_DISK?HD_STATUS:CF_STATUS)
#define IDE_COMMAND                       (cur_disk==HD_DISK?HD_COMMAND:CF_COMMAND)
#define IDE_ALTSTATUS                     (cur_disk==HD_DISK?HD_ALTSTATUS:CF_ALTSTATUS)

/* assuming that for all arch ATA is off at start */
#define BOOT_ATA_POWER_STATE 0 

/* read write */
int ata_rwData(int disk,unsigned int lba,void * data,int count,int cmd,int use_dma);
void ide_intAction(int irq,struct pt_regs * regs);
  
/* status */
MED_RET_T ata_selectDisk(int disk);
MED_RET_T ata_waitForBusy(void);
MED_RET_T ata_waitForReady(void);
MED_RET_T ata_waitForStartXfer(void);
MED_RET_T ata_waitForEndXfer(void);
int ata_status(void);
        
/* Hard/Soft Reset */
MED_RET_T ata_doSoftReset(int disk);
MED_RET_T ata_softReset(int disk);
MED_RET_T ata_hardReset(int disk);
        
/* sleep */
MED_RET_T ata_doSleep(int disk);
void ata_sleepImmediate(int disk);
void ata_sleep(int disk);
MED_RET_T ata_doPowerOn(int disk);
MED_RET_T ata_doPowerOff(int disk);
MED_RET_T ata_StopHD(int disk);

/* config: features, multiple, freezeLock, ... */
MED_RET_T ata_setMultipleSect(int disk);
MED_RET_T ata_chkReg(int disk);
MED_RET_T ata_freezeLock(int disk);
MED_RET_T ata_setFeatures(int disk);
MED_RET_T ata_doPowerOn(int disk);
        
/* Disk config */
MED_RET_T ata_identify(int disk,struct hd_info_s * disk_info);
MED_RET_T ata_initDisk(int disk);
        
/* driver init */
void ata_init(void);

 /* print function */
struct ata_features {
    char * str;
    int disp;
    int negate;
};
void printFeat(int type,struct ata_features * features,int val);
void ata_printIdentify(int disk);
        
/* arch dependent */
void arch_ata_resetHD(void);
void arch_ata_powerUpHD(void);
void arch_ata_powerDownHD(void);
void arch_ata_selectHD(void);
void arch_ata_selectCF(void);
void arch_ata_init(void);
        
#endif
