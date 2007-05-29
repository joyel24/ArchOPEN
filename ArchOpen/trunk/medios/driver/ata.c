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
#include <kernel/pipes.h>

#include <driver/hardware.h>
#include <driver/energy.h>
#include <driver/cpld.h>
#include <driver/ata.h>
#include <driver/dma.h>
#include <driver/cache.h>

#include <fs/disk.h>

//#define NO_DMA

int ata_sectorBuffer[SECTOR_SIZE];

extern int hd_sleep_state;

extern struct tmr_s hd_timer;

int ata_stopping = 0;
THREAD_INFO * ata_thread;
struct tmr_s ataStop_tmr;

int cur_disk = HD_DISK;
struct pipe cmd_list;

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

//#define ATA_NO_TASKLIST
#define NO_MULTYSECT

int ata_rwData(int disk,unsigned int lba,void * data,int count,int cmd,int use_dma)
{
    struct ata_cmd new_cmd;
    struct ata_cmd * cur_cmd;
    MED_RET_T ret_val;
    
#ifdef ATA_NO_TASKLIST
    int i,j;
    bool unaligned;
    void * buffer;
    int xfer_size;
    int use_multiple;
    int block_size;
    int nbSector;
#endif

    new_cmd.disk=disk;
    new_cmd.lba=lba;
    new_cmd.data=data;
    new_cmd.count=count;
    new_cmd.cmd=cmd;
    new_cmd.use_dma=use_dma;
    new_cmd.cur_thread=threadCurrent;
    new_cmd.ret_val=&ret_val;
    
    cur_cmd=&new_cmd;
    
#ifndef ATA_NO_TASKLIST
    if(ata_thread->state==THREAD_STATE_DISABLE)
    {
        __cli();
        pipeWrite(&cmd_list,&cur_cmd,4);
        threadCurrent->state=THREAD_BLOCKED_BY_ATA;
        ata_thread->state=THREAD_STATE_ENABLE;
        yieldTo(ata_thread);
        __sti();   
    }
    else
    {
        __cli();
        pipeWrite(&cmd_list,&cur_cmd,4);
        threadCurrent->state=THREAD_BLOCKED_BY_ATA;
        yield(); 
        __sti();
    }
    return ret_val;
}

void ata_rwThread(void)
{
    int i,j;
    bool unaligned;
    void * buffer;
    int xfer_size;
    int use_multiple;
    int block_size;
    MED_RET_T ret_val=-1;
    int nbSector;
    struct ata_cmd * cur_cmd;
    
    printk("Starting ata thread\n");
    
    while(1)
    {
        while(cmd_list.nIN==cmd_list.nOUT)
        {
            __cli();
            threadCurrent->state=THREAD_STATE_DISABLE;
            yield();
            __sti();
        }
       
        __cli();
        pipeRead(&cmd_list,&cur_cmd,4);
        __sti();
#endif        
        /*printk("RW (cmd=%d) disk %d, lba=%x,bufer @%x, nb=%x thread=%d\n",cur_cmd->cmd,cur_cmd->disk,
               cur_cmd->lba,cur_cmd->data,cur_cmd->count,cur_cmd->cur_thread->pid);*/
        
        block_size=SECTOR_SIZE;
        unaligned=((unsigned long)cur_cmd->data)&0x03;
        buffer=cur_cmd->data;
        xfer_size=cur_cmd->count;
        use_multiple=0;
        nbSector=1;
        
        /* use a temporary buffer for unaligned read or writes */
        if (unaligned)
        {
            buffer=ata_sectorBuffer;
        }
#ifndef NO_MULTYSECT    
        if(disk_info[cur_cmd->disk]->has_multi_sector && !unaligned && cur_cmd->cmd==ATA_DO_READ)
        {
            use_multiple=1;
            nbSector=disk_info[cur_cmd->disk]->multi_sector;
        }
#endif            
        /* select the right disk */
        ATA_SELECT_DISK(cur_cmd->disk);
            
        /* wait drive ready */
        if(ata_waitForReady()<0)
        {
            ret_val=-1;
            goto end;
        }
        block_size=nbSector*SECTOR_SIZE;
        
        /* send read/write cmd */
        switch(cur_cmd->cmd)
        {
            case ATA_DO_IDENT:
                ATA_OUTB(0,IDE_SELECT); /* send ident. cmd */
                ATA_OUTB(IDE_CMD_IDENTIFY,IDE_COMMAND);
                break;
    
            case ATA_DO_READ:
                /* change multiple mode setting*/
                ATA_OUTB(cur_cmd->lba,IDE_SECTOR);
                ATA_OUTB(cur_cmd->lba>>8,IDE_LCYL);
                ATA_OUTB(cur_cmd->lba>>16,IDE_HCYL);
                ATA_OUTB((cur_cmd->lba>>24) | IDE_SEL_LBA,IDE_SELECT);
                ATA_OUTB(cur_cmd->count,IDE_NSECTOR);
#ifndef NO_MULTYSECT 
                if(!use_multiple)
                {
                    ATA_OUTB(IDE_CMD_READ_SECTORS,IDE_COMMAND);
                }
                else
                {
                    ATA_OUTB(IDE_CMD_READ_MULTIPLE_SECTORS,IDE_COMMAND);
                }
#else
                ATA_OUTB(IDE_CMD_READ_SECTORS,IDE_COMMAND);
#endif
                break;
            case ATA_DO_WRITE:
                ATA_OUTB(cur_cmd->lba,IDE_SECTOR);
                ATA_OUTB(cur_cmd->lba>>8,IDE_LCYL);
                ATA_OUTB(cur_cmd->lba>>16,IDE_HCYL);
                ATA_OUTB((cur_cmd->lba>>24) | IDE_SEL_LBA,IDE_SELECT);
                ATA_OUTB(cur_cmd->count,IDE_NSECTOR);
                ATA_OUTB(IDE_CMD_WRITE_SECTORS,IDE_COMMAND);
                break;
        }
        
    #ifdef PMA
        outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
        while(inb(OMAP_REQUEST_BASE));
    #endif
    
    #ifdef NO_DMA
    cur_cmd->use_dma=ATA_NO_DMA;
    #endif
    
    if(((unsigned int)(cur_cmd->data) < SDRAM_START) && cur_cmd->use_dma==ATA_WITH_DMA)
        {
            cur_cmd->use_dma=ATA_NO_DMA;
        }
        i=0;
        while(i<cur_cmd->count)
        {
            if (unaligned)
            {
                /* copy data to the temp buffer if unaligned and we're writing*/
                if(cur_cmd->cmd == ATA_DO_WRITE) memcpy(buffer,cur_cmd->data+i*SECTOR_SIZE,SECTOR_SIZE);
            }
            
            if(ata_waitForXfer()<0)
            {
                ret_val=0;
                goto end;
            }
#ifndef NO_MULTYSECT             
            if(use_multiple && xfer_size<nbSector)
                block_size=xfer_size*SECTOR_SIZE;
#endif
            if(cur_cmd->use_dma==ATA_WITH_DMA)
            {
                if(dma_pending())
                {
                    printk("Error dma is still running\n");
                    ret_val=-1;
                    goto end;
                }
    
                if((unsigned int)(buffer) < SDRAM_START)
                {
                    printk("Error buffer not in SDRAM\n");
                    ret_val=-2;
                    goto end;
                }
    
                if(cur_cmd->cmd == ATA_DO_READ || cur_cmd->cmd == ATA_DO_IDENT)
                {
                    dma_setup((void *)DMA_ATA_READ_ADDRESS,0,DMA_ATA,true,
                            buffer,SDRAM_START,DMA_SDRAM,false,
                            block_size);
                }
                else
                {
                    dma_setup(buffer,SDRAM_START,DMA_SDRAM,false,
                            (void *)DMA_ATA_WRITE_ADDRESS,0,DMA_ATA,true,
                            block_size);
                }
                
                
    
#ifdef DM320
    cache_invalidateRange(CACHE_DATA,buffer,block_size);
#endif
    
                
                /* ata can't work without irq */
#ifndef ATA_NO_TASKLIST
                __cli();
                dma_start();
                threadCurrent->state=THREAD_BLOCKED_BY_DMA;
                yield();
                __sti();
#else
                dma_start();
                while(dma_pending()) /*nothing*/;
#endif
            }
            else
            {
                if(cur_cmd->cmd == ATA_DO_READ || cur_cmd->cmd == ATA_DO_IDENT)
                {
                    for(j=0;j<block_size;j+=2)
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
                    for(j=0;j<block_size;j+=2)
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
                if(cur_cmd->cmd == ATA_DO_READ || cur_cmd->cmd == ATA_DO_IDENT) 
                    memcpy(cur_cmd->data+i*SECTOR_SIZE,buffer,SECTOR_SIZE);
            }
            else
            {
                /* else increment buffer pos */
                buffer+=block_size;
            }
            i+=nbSector;
            if(i>cur_cmd->count) i=cur_cmd->count;
            xfer_size-=nbSector;
        }
        ret_val=0;
end:
#ifndef ATA_NO_TASKLIST
        *(cur_cmd->ret_val)=ret_val;
        cur_cmd->cur_thread->state=THREAD_STATE_ENABLE;
    }
#else
    return ret_val;
#endif
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

    cmd_list.nIN=cmd_list.nOUT=0;
    
#ifndef ATA_NO_TASKLIST    
    thread_startFct(&ata_thread,ata_rwThread,"ATA rw",THREAD_STATE_ENABLE,PRIO_HIGH,THREAD_USE_OTHER_STACK);
#endif

    tmr_setup(&ataStop_tmr,"ata Stop");
    ataStop_tmr.action   = ata_stopTmrFct;
    ataStop_tmr.freeRun  = 1;
    ataStop_tmr.stdDelay = 1; /* 1 tick delay */
    
    irq_enable(IRQ_IDE);
    
    ata_reset();
    printk("[ATA init] done\n");
}

void ata_reset(void)
{
    arch_ata_init();
}

void ide_intAction(int irq,struct pt_regs * regs)
{
    if(ata_thread->state==THREAD_BLOCKED_BY_DMA)
    {
        ata_thread->state=THREAD_STATE_ENABLE;
        threadCurrent=ata_thread; 
    }
    else
    {
        //printk("DMA INT\n");
    }
}

