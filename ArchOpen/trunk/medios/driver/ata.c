/*
*   kernel/fs/ata.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <lib/string.h>

#include <kernel/malloc.h>
#include <kernel/io.h>
#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/delay.h>
#include <kernel/timer.h>
#include <kernel/irq.h>

#include <driver/hardware.h>
#include <driver/energy.h>
#include <driver/cpld.h>
#include <driver/ata.h>
#include <driver/dma.h>

#include <fs/disk.h>

//#define USE_DMA
//#define NO_DMA

int ata_sectorBuffer[SECTOR_SIZE];

int av_cmd_array[]= {
    IDE_CMD_READ_SECTORS,
    IDE_CMD_WRITE_SECTORS
};

extern int hd_sleep_state;

extern struct tmr_s hd_timer;

int ata_stopping = 0;
struct tmr_s ataStop_tmr;

int cur_disk = HD_DISK;

#define CALC_BASE(ADDR)     (((unsigned int)(ADDR))-SDRAM_START)

#define ATA_SELECT_DISK(DISK)   ({ \
    if(DISK==HD_DISK)         \
    {                           \
        ata_stopping = 0;       \
        ata_selectHD();         \
        ata_powerUpHD();        \
        hd_sleep_state=0;       \
        hd_launchTimer();       \
    }                           \
    else                        \
    {                           \
        ata_selectCF();         \
    }                           \
    })

int ata_rwData(int disk,unsigned int lba,void * data,int count,int cmd,int use_dma)
{
    int i,j;
    bool unaligned=((unsigned long)data)&0x03;
    void * buffer=data;

    //printk("RW (cmd=%d) disk %d, lba=%x,bufer @%x, nb=%x\n",cmd,disk,lba,data,count);
    
    /* use a temporary buffer for unaligned read or writes */
    if (unaligned)
    {
        buffer=ata_sectorBuffer;
    }

    /* select the right disk */
    ATA_SELECT_DISK(disk);
        
    /* wait drive ready */
    if(ata_waitForReady()<0)
        return -1;
    /* send read/write cmd */
    switch(cmd)
    {
        case ATA_DO_IDENT:
            ATA_OUTB(0,IDE_SELECT); /* send ident. cmd */
            ATA_OUTB(IDE_CMD_IDENTIFY,IDE_COMMAND);
            /*ata_cmd->count=1;
            ata_cmd->xfer_dir=ATA_DO_READ;
            ata_cmd->use_dma=ATA_WITH_DMA;*/
            break;

        case ATA_DO_READ:
        case ATA_DO_WRITE:
            ATA_OUTB(lba,IDE_SECTOR);
            ATA_OUTB(lba>>8,IDE_LCYL);
            ATA_OUTB(lba>>16,IDE_HCYL);
            ATA_OUTB((lba>>24) | IDE_SEL_LBA,IDE_SELECT);
            ATA_OUTB(count,IDE_NSECTOR);
            ATA_OUTB(av_cmd_array[cmd],IDE_COMMAND);
            break;
    }
#ifdef PMA
    outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
   
#ifdef NO_DMA
    use_dma=ATA_NO_DMA;
#endif
#ifndef PMA
if(use_dma==ATA_NO_DMA)
    printk("No DMA\n");
#endif
    if(((unsigned int)(data) < SDRAM_START) && use_dma==ATA_WITH_DMA)
    {
        //printk("Destination buffer not in SDRAM => no DMA\n");
        use_dma=ATA_NO_DMA;
    }
    for(i=0;i<count;i++)
    {
        if (unaligned)
        {
            /* copy data to the temp buffer if unaligned and we're writing*/
            if(cmd == ATA_DO_WRITE) memcpy(buffer,data+i*SECTOR_SIZE,SECTOR_SIZE);
        }
        if(ata_waitForXfer()<0)
            return 0;
        if(use_dma==ATA_WITH_DMA)
        {
            if( DMA_RUNNING )
            {
                printk("Error dma is still running\n");
                return -1;
            }

            if((unsigned int)(data) < SDRAM_START)
            {
                printk("Error buffer not in SDRAM\n");
                return -2;
            }

            if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
            {
                DMA_SET_SRC(DMA_ATA_ADDRESS);
                DMA_SET_DEST((CALC_BASE(buffer)));
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_ATA,DMA_SDRAM)
            }
            else
            {
                DMA_SET_SRC((CALC_BASE(buffer)));
                DMA_SET_DEST(DMA_ATA_ADDRESS);
                DMA_SET_SIZE(SECTOR_SIZE);
                DMA_SET_DEV(DMA_SDRAM,DMA_ATA)
            }
            DMA_START_TRANSFER;

            while(DMA_RUNNING) /*nothing*/;
        }
        else
        {
            if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
            {
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
#ifdef PMA
                    outb(OMAP_HD_READ_REQUEST,OMAP_REQUEST_BASE);
                    while(inb(OMAP_REQUEST_BASE));
#endif
                    outw(ATA_INW(IDE_DATA),buffer+j);
                }
            }
            else
            {
                for(j=0;j<SECTOR_SIZE;j+=2)
                {
                    ATA_OUTW(inw(buffer+j),IDE_DATA);
#ifdef PMA
                    outb(OMAP_HD_WRITE_REQUEST,OMAP_REQUEST_BASE);
                    while(inb(OMAP_REQUEST_BASE));
#endif
                }
            }
        }

        if (unaligned)
        {
            /* copy temp data to the actual buffer if unaligned and we're reading*/
            if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT) memcpy(data+i*SECTOR_SIZE,buffer,SECTOR_SIZE);
        }
        else
        {
            /* else increment buffer pos */
            buffer+=SECTOR_SIZE;
        }
    }
    return 0;
}

int ata_sleep(void)
{
    if(ata_waitForReady()<0)
        return -1;
    ATA_OUTB(0,IDE_SELECT);
    ATA_OUTB(IDE_CMD_SLEEP,IDE_COMMAND);
#ifdef PMA
    outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
    return 0;
}

void ata_stopHD(int mode)
{
    printk("[ide sleep] beg\n");
    if(hd_sleep_state)
    {
        printk("[ide sleep] HD already stopped => nothing to do\n");
        return;
    }
    ata_selectHD();
    ata_sleep();
    if(mode == ATA_FORCE_STOP)
    {
        int i,status;
        ata_stopping = 0;
        for(i=0;i<100;i++)
        {
            mdelay(50);
            status=ata_status();
            if((status & IDE_STATUS_BSY)==0 && (status & IDE_STATUS_RDY)!=0)
                break;
        }
        ata_stopHDEnd();
    }
    else
    {
        ata_stopping = 1;
        tmr_start(&ataStop_tmr);
    }

}

void ata_stopHDEnd(void)
{
    ata_powerDownHD();

    tmr_stop(&hd_timer);

    hd_sleep_state=1;
    ata_stopping = 0;
    printk("[ide sleep] end\n");
}

void ata_softReset(int disk)
{
    if(disk==HD_DISK)
        ata_selectHD();
    else
        ata_selectCF();
    ATA_OUTB(0xe,IDE_CONTROL);
    mdelay(5);
    ATA_OUTB(0xa,IDE_CONTROL);
#ifdef PMA
    outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
}

int ata_waitForXfer(void)
{
    int val,t;
    t=tmr_getTick();
    do
    {
#ifdef PMA
        outb(OMAP_HD_ALTS_ERR_REQUEST,OMAP_REQUEST_BASE);
        while(inb(OMAP_REQUEST_BASE));
#endif
        val=ATA_INB(IDE_ALTSTATUS);
        if((val & IDE_STATUS_BSY)==0 && (val & IDE_STATUS_DRQ)!=0)
            return 0;
    }
    while((tmr_getTick()-t)<WAIT_XFER_TIMEOUT);

    return -1; /* if we are here => we have a timeout */
}

int ata_waitForReady(void)
{
    int val,t;
    t=tmr_getTick();
    do
    {
#ifdef PMA
        outb(OMAP_HD_ALTS_ERR_REQUEST,OMAP_REQUEST_BASE);
        while(inb(OMAP_REQUEST_BASE));
#endif
        val=ATA_INB(IDE_ALTSTATUS);
        if((val & IDE_STATUS_BSY)==0 && (val & IDE_STATUS_RDY)!=0)
            return 0;
    }
    while((tmr_getTick()-t)<WAIT_READY_TIMEOUT);

    return -1; /* if we are here => we have a timeout */
}

int ata_status(void)
{
#ifdef PMA
    outb(OMAP_HD_ALTS_ERR_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
    return ATA_INB(IDE_ALTSTATUS);
}

void ata_resetHD(void)
{
  arch_ata_resetHD();
}

void ata_powerUpHD(void)
{
    arch_ata_powerUpHD();
}

void ata_powerDownHD(void)
{
    arch_ata_powerDownHD();
}

void ata_selectHD(void)
{
    cur_disk=HD_DISK;
    arch_ata_selectHD();
}

void ata_selectCF(void)
{
    cur_disk=CF_DISK;
    arch_ata_selectCF();
}

void ata_stopTmrFct(void)
{
    int __status;

    if(!ata_stopping)
        tmr_stop(&ataStop_tmr);

    if(ata_stopping)
    {
        ata_stopping++;
        __status=ata_status();
        if(((__status & IDE_STATUS_BSY)==0 && (__status & IDE_STATUS_RDY)!=0) || ata_stopping > 1000)
        {
            if(ata_stopping > 1000) printk("ata_stopping timout\n");
            ata_stopHDEnd();
            ata_stopping = 0;
            tmr_stop(&ataStop_tmr);
        }
    }
}

void ata_init(void)
{
    ata_stopping = 0;
    cur_disk = HD_DISK;

    tmr_setup(&ataStop_tmr,"ata Stop");
    ataStop_tmr.action   = ata_stopTmrFct;
    ataStop_tmr.freeRun  = 1;
    ataStop_tmr.stdDelay = 1; /* 1 tick delay */

    ata_reset();
    printk("[ATA init] done\n");
}

void ata_reset(void)
{
    arch_ata_init();
}

void ide_intAction(int irq,struct pt_regs * regs)
{
    //arch_ide_intAction(irq,regs);
}

