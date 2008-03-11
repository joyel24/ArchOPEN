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
*
* Part of the code is inspired from rockbox ata driver: www.rockbox.org
*
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

#include "ata_featuresList.h"

#ifdef HAVE_DBUG
#warning Debug mode => ATA in low speed
#define ATA_USE_POLL
#define ATA_NO_MULTY
#endif

#ifdef JBMM
#warning JBMM needs DMA support
#ifndef ATA_USE_POLL
#define ATA_USE_POLL
#endif
#ifndef ATA_NO_MULTY
#define ATA_NO_MULTY
#endif
#define NO_DMA
#endif

int ata_sectorBuffer[SECTOR_SIZE];

THREAD_INFO * ata_RW_thread;
struct spinLock ata_lock;

int cur_disk;
int ata_sleeping;
int ata_spinup;
int ata_powered;

int ata_rwData(int disk,unsigned int lba,void * inData,int inCount,int cmd,int use_dma)
{
    int i,j;
    bool unaligned;
    void * buffer;
    int xfer_size;
    int block_size;
    int nbSector;
    int useInt;
    int count;
    void * data;
    int status;
    int tOut;
    int retCount;
    int use_multiple;
    MED_RET_T ret_val;
    
    //printk("[ata-rw] disk=%x buffer=%x lba=%x count=%x cmd=%x use_dma=%x\n",disk,inData,lba,inCount,cmd,use_dma);
    
    spinLock_lock(&ata_lock);   
         
    use_multiple=1;
    useInt=1;
    ret_val=-MED_ERROR;
    
    ata_RW_thread=threadCurrent;
    
    block_size=SECTOR_SIZE;
    unaligned=((unsigned long)inData)&0x03;
#ifndef ATA_USE_POLL    
    if(readCPUMode()==0x13)
#endif
    {
        //printk("Not using INT for DMA\n");
        useInt=0;
        ata_RW_thread=NULL;
    }
    
    if(cmd==ATA_DO_WRITE)
    {
        use_multiple=0;
        if(lba==0)
        {
            printk("[ata_rwData] Error trying to write lba 0\n");
            ata_RW_thread=NULL;
            spinLock_unlock(&ata_lock);
            return -MED_ERROR;
        }
    }
    
#ifdef ATA_NO_MULTY
    use_multiple=0;
#endif

    /* use a temporary buffer for unaligned read or writes */
    if (unaligned)
    {
        buffer=ata_sectorBuffer;
        use_multiple=0;
    }

    if(use_multiple)
        nbSector=disk_info[disk]->multi_sector;
    else
        nbSector=1;
    
    if(ata_sleeping)
    {
        ata_spinup=1;
        if(!ata_powered)
        {
            if((ret_val=ata_doPowerOn(disk))!=MED_OK)
            {
                printk("[ata_rwData] error powering ON disk %d (err=%d)\n",disk,-ret_val);
                ata_RW_thread=NULL;
                spinLock_unlock(&ata_lock);
                return -MED_ERROR;   
            }            
        }
        else
        {
            if((ret_val=ata_doSoftReset(disk))!=MED_OK)
            {
                printk("[ata_rwData] error doing softReset for disk %d (err=%d)\n",disk,-ret_val);
                ata_RW_thread=NULL;
                spinLock_unlock(&ata_lock);
                return -MED_ERROR;   
            }  
        }
    }
    
    /* select the right disk */
    ata_selectDisk(disk);
        
    /* wait drive ready */
    if((ret_val=ata_waitForReady())!=MED_OK)
    {
        printk("[ata_rwData] error in wait RDY (err=%d)\n",ret_val);
        goto end;
    }    
        
    
    
    tOut=tick+READ_WRITE_TOUT;
    retCount=0;
retry:   
    data=inData;
    xfer_size=count=inCount;
    block_size=nbSector*SECTOR_SIZE;
    if (unaligned)
        buffer=ata_sectorBuffer;
    else
        buffer=data;
    
    if(!useInt)
        retCount++;
    
    while(tOut>tick && retCount<10)
    {        
        /* send read/write cmd */
        switch(cmd)
        {
            case ATA_DO_IDENT:
                ATA_OUTB(0,IDE_SELECT); /* send ident. cmd */
                ATA_OUTB(IDE_CMD_IDENTIFY,IDE_COMMAND);
                break;
    
            case ATA_DO_READ:
                /* change multiple mode setting*/
                ATA_OUTB(lba,IDE_SECTOR);
                ATA_OUTB(lba>>8,IDE_LCYL);
                ATA_OUTB(lba>>16,IDE_HCYL);
                ATA_OUTB((lba>>24) | IDE_SEL_LBA,IDE_SELECT);
                ATA_OUTB(count,IDE_NSECTOR);
                if(!use_multiple)
                {
                    ATA_OUTB(IDE_CMD_READ_SECTORS,IDE_COMMAND);
                }
                else
                {
                    ATA_OUTB(IDE_CMD_READ_MULTIPLE_SECTORS,IDE_COMMAND);
                }
                
                break;
            case ATA_DO_WRITE:
                ATA_OUTB(lba,IDE_SECTOR);
                ATA_OUTB(lba>>8,IDE_LCYL);
                ATA_OUTB(lba>>16,IDE_HCYL);
                ATA_OUTB((lba>>24) | IDE_SEL_LBA,IDE_SELECT);
                ATA_OUTB(count,IDE_NSECTOR);
                ATA_OUTB(IDE_CMD_WRITE_SECTORS,IDE_COMMAND);
                break;
        }

#ifdef PMA
    outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
        __asm__ volatile ("nop");
#ifdef NO_DMA
#warning forcing no DMA
    use_dma=ATA_NO_DMA;
#endif
    
        if(((unsigned int)(data) < SDRAM_START) && use_dma==ATA_WITH_DMA)
        {
            use_dma=ATA_NO_DMA;
        }
        i=0;
        
        while(i<count)
        {
            if (unaligned)
            {
                /* copy data to the temp buffer if unaligned and we're writing*/
                if(cmd == ATA_DO_WRITE) memcpy(buffer,data+i*SECTOR_SIZE,SECTOR_SIZE);
            }
            
            if(ata_waitForStartXfer()!=MED_OK)
            {
                ret_val=-MED_ERROR;
                if(cmd==ATA_DO_WRITE)
                    goto end;
                ata_doSoftReset(disk);
                goto retry;
            }
            
            if(ata_spinup)
            {
                ata_spinup=0;
                ata_sleeping=0;
                ata_powered=1;
            }
                
            if(use_multiple && xfer_size<nbSector)
                block_size=xfer_size*SECTOR_SIZE;
    
            if(cmd==ATA_DO_WRITE && use_multiple)
                printk("Write in multiple mode\n");
            
            status=ata_status();
            
            if(use_dma==ATA_WITH_DMA)
            {
                if(dma_pending())
                {
                    printk("[ata_rwData] Error dma is still running\n");
                    goto retry;;
                }
                
                if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
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
                if(useInt)
                {
                    __cli();
                    dma_start();
                    ata_RW_thread->state=THREAD_BLOCKED_BY_DMA;
                    yield();
                    __sti();
                }
                else
                {
                    dma_start();
                    while(dma_pending()) /*nothing*/;
                }
            }
            else
            {
                if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT)
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
            
            if(cmd!=ATA_DO_WRITE && (status & (IDE_STATUS_BSY|IDE_STATUS_ERR|IDE_STATUS_DF)))
            {
                if((ret_val=ata_doSoftReset(disk))!=MED_OK)
                {
                    printk("[ata_rwData] Error doing softRest after read (err=%d)\n");
                    goto retry;
                }
            }
            /* read/write finished*/
            if (unaligned)
            {
                /* copy temp data to the actual buffer if unaligned and we're reading*/
                if(cmd == ATA_DO_READ || cmd == ATA_DO_IDENT) 
                    memcpy(data+i*SECTOR_SIZE,buffer,SECTOR_SIZE);
            }
            else
            {
                /* else increment buffer pos */
                buffer+=block_size;
            }
            i+=nbSector;
            if(i>count) i=count;
            xfer_size-=nbSector;            
        }
#ifdef JBMM
        if(cmd!=ATA_DO_WRITE && ata_waitForEndXfer()!=MED_OK)
#else        
        if(ata_waitForEndXfer()!=MED_OK)
#endif
        {
            ret_val=-MED_ERROR;            
            printk("[ata_rwData] Error doing softRest after bad EndOf Xfer\n");
            if(cmd==ATA_DO_WRITE)
                goto end;
            ata_doSoftReset(disk);
            goto retry;
        }
        break;
    }
    ret_val=MED_OK;
end:
    ata_RW_thread=NULL;
    hd_launchTimer();
    spinLock_unlock(&ata_lock);
    return ret_val;
}

/*****************************************************
*  Selecting disk (CF=1 or HD=0)
******************************************************/

void (*selectFctTab[]) (void)= {arch_ata_selectHD,arch_ata_selectCF};

MED_RET_T ata_selectDisk(int disk)
{
    if(disk!=HD_DISK && disk!=CF_DISK)
        return -MED_ERROR;
    if(cur_disk!=disk)
    {
        cur_disk=disk;
        selectFctTab[disk]();
    }
    return MED_OK;
}

/*****************************************************
*  Group of function managing ATA status
*  BSY, RDY, xFer start/end
******************************************************/

MED_RET_T ata_waitForBusy(void)
{    
    int tOut=tick+WAIT_BSY_TOUT;
    /*NOTE: we'll have to test perf with a yield*/ 
    //int useYield=(readCPUMode()==0x13)?0:1;
    
    while(tick<tOut && (ATA_INB(IDE_ALTSTATUS)&IDE_STATUS_BSY)) {
        /*yield()*/;
        
    }
    
    if(tick<tOut)
        return MED_OK;
    else
    {
        printk("[ATA-waitBSY] TimeOut\n");
        return -MED_ERROR;
    }
}

MED_RET_T ata_waitForReady(void)
{
    int tOut;    
    /*NOTE: we'll have to test perf with a yield*/ 
    //int useYield=(readCPUMode()==0x13)?0:1;
    if(ata_waitForBusy()<0)
        return -MED_ERROR;
    tOut=tick+WAIT_RDY_TOUT;
    
    while(tick<tOut && !(ATA_INB(IDE_ALTSTATUS)&IDE_STATUS_RDY)) {
        /*yield()*/;
    }
    
    if(tick<tOut)
        return MED_OK;
    else
    {
        printk("[ATA-waitRDY] TimeOut\n");
        return -MED_ERROR;
    }
}

MED_RET_T ata_waitForStartXfer(void)
{
    if(ata_waitForBusy()<0)
        return -MED_ERROR;
    
    if((ATA_INB(IDE_ALTSTATUS) & (IDE_STATUS_BSY|IDE_STATUS_DRQ))==IDE_STATUS_DRQ)
        return MED_OK;
    else
    {
        printk("[ATA-waitStartXfer] not started\n");
        return -MED_ERROR;
    }
}

MED_RET_T ata_waitForEndXfer(void)
{
    if(ata_waitForBusy()<0)
        return -MED_ERROR;
    
    mdelay(2);
    
    /*printk("[ATA-waitForEndXfer]: status=%x (masked %x|%x)\n",ATA_INB(IDE_ALTSTATUS),
           ATA_INB(IDE_ALTSTATUS) & (IDE_STATUS_RDY|IDE_STATUS_DRQ),IDE_STATUS_RDY);
    */
    
    if((ATA_INB(IDE_ALTSTATUS) & (IDE_STATUS_RDY|IDE_STATUS_DRQ))==IDE_STATUS_RDY)
        return MED_OK;
    else
    {
        printk("[ATA-waitForEndXfer] not ended\n");
        return -MED_ERROR;
    }
}

int ata_status(void)
{
    return ATA_INB(IDE_ALTSTATUS);
}


/*****************************************************
*  Hard and Soft Reset
*  Hard reset uses arch specific function
******************************************************/

MED_RET_T ata_doSoftReset(int disk)
{
    int i;
    MED_RET_T ret_val=MED_OK;
    ata_selectDisk(disk);
    ATA_OUTB(0xe,IDE_CONTROL);
    mdelay(1);
    ATA_OUTB(0xa,IDE_CONTROL);
    mdelay(1);
#ifdef PMA
    outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
    while(inb(OMAP_REQUEST_BASE));
#endif
    for(i=0;i<8;i++)
    {
        ret_val=ata_waitForReady();
        if(ret_val==MED_OK)
            break;
    }
    
    return ret_val;
}

MED_RET_T ata_softReset(int disk)
{
    MED_RET_T ret_val;
    spinLock_lock(&ata_lock);
    ret_val=ata_doSoftReset(disk);
    spinLock_unlock(&ata_lock);
    return ret_val;
}

MED_RET_T ata_hardReset(int disk)
{
    arch_ata_resetHD();
    ata_selectDisk(disk);
    ATA_OUTB(0,IDE_SELECT);
    return ata_waitForBusy();
}


/*****************************************************
*  Sleep / Stdby
******************************************************/

MED_RET_T ata_doSleep(int disk)
{
    if(!ata_spinup && !ata_sleeping && !spinLock_isLocked(&ata_lock))
    {
        spinLock_lock(&ata_lock);
        printk("[ata_doSleep] start\n");
        ata_selectDisk(disk);
        ATA_OUTB(0,IDE_SELECT);
        if(ata_waitForReady()!=MED_OK)
        {
            printk("[ata-doSleep] failed: not rdy\n");
            spinLock_unlock(&ata_lock);
            return - MED_ERROR;
        }
        ATA_OUTB(IDE_CMD_SLEEP,IDE_COMMAND);
    #ifdef PMA
        outb(OMAP_HD_CMD_REQUEST,OMAP_REQUEST_BASE);
        while(inb(OMAP_REQUEST_BASE));
    #endif
    
        if(ata_waitForReady()!=MED_OK)
        {
            printk("[ata-doSleep] failed: error on cmd\n");
            spinLock_unlock(&ata_lock);
            return - MED_ERROR;
        }
        ata_sleeping=1;
        printk("[ata_doSleep] end\n");
        spinLock_unlock(&ata_lock);
    }
    else
        printk("[ata_doSleep] bad State (spin=%d, sleep=%d lock=%d)\n",
               ata_spinup,ata_sleeping,spinLock_isLocked(&ata_lock));
    return MED_OK;
}

MED_RET_T ata_doPowerOff(int disk)
{
    if(!ata_spinup && ata_sleeping && ata_powered)
    {
        spinLock_lock(&ata_lock);
        arch_ata_powerDownHD();
        spinLock_unlock(&ata_lock);
        ata_powered=0;
    }
    else
        printk("[ata_doPowerOff] bad State (spin=%d, sleep=%d powered=%d)\n",ata_spinup,ata_sleeping,ata_powered);
    return MED_OK;
}

MED_RET_T ata_StopHD(int disk)
{
    if(ata_doSleep(disk)!=MED_OK)
        return -MED_ERROR;
    if(ata_doPowerOff(disk)!=MED_OK)
        return -MED_ERROR;
    return MED_OK;
}

/*****************************************************
*  Init / powerOn
******************************************************/

MED_RET_T ata_setMultipleSect(int disk)
{
    ATA_OUTB(0,IDE_SELECT);
    if(ata_waitForReady()!=MED_OK)
    {
        printk("[ata-Set multy] failed: not rdy\n");
        return - MED_ERROR;
    }
    ATA_OUTB(disk_info[disk]->multi_sector,IDE_NSECTOR);
    ATA_OUTB(IDE_SET_MULTIPLE_MODE,IDE_COMMAND);
    if(ata_waitForReady()!=MED_OK)
    {
        printk("[ata-Set multy] failed: error on CMD\n");
        return - MED_ERROR;
    }
    return MED_OK;        
}

MED_RET_T ata_chkReg(int disk)
{
#ifdef PMA
    return MED_OK;
#else
    int i;  
    ATA_OUTB(0,IDE_SELECT);
    
    if(ATA_INB(IDE_ALTSTATUS) & IDE_STATUS_BSY)
        return -MED_ERROR;
    
    for(i=0;i<64;i++)
    {
        ATA_OUTB(CHK_WRT_NSECTOR,IDE_NSECTOR);
        ATA_OUTB(CHK_WRT_SECTOR,IDE_SECTOR);
        ATA_OUTB(CHK_WRT_LCYL,IDE_LCYL);
        ATA_OUTB(CHK_WRT_HCYL,IDE_HCYL);
        if((ATA_INB(IDE_NSECTOR)&CHK_RD_MASK_NSECTOR)==CHK_RD_NSECTOR &&
            (ATA_INB(IDE_SECTOR)&CHK_RD_MASK_SECTOR)==CHK_RD_SECTOR  &&
            (ATA_INB(IDE_LCYL)&CHK_RD_MASK_LCYL)==CHK_RD_LCYL      &&
            (ATA_INB(IDE_HCYL)&CHK_RD_MASK_HCYL)==CHK_RD_HCYL
          )
            return MED_OK;
    }
    return -MED_ERROR;
#endif
}

MED_RET_T ata_freezeLock(int disk)
{
    if(disk_info[disk]->features_support[0]&0x2)
    {
        ATA_OUTB(0,IDE_SELECT);
        if(ata_waitForReady()!=MED_OK)
        {
            printk("[ata-freezeLock] failed: not rdy\n");
            return - MED_ERROR;
        }
        ATA_OUTB(IDE_SECURITY_FREEZE_LOCK,IDE_COMMAND);
        if(ata_waitForReady()!=MED_OK)
        {
            printk("[ata-freezeLock] failed: error on CMD\n");
            return - MED_ERROR;
        }
    }
    else
        printk("[ata] SECURITY_FREEZE_LOCK not supported\n");
    return MED_OK;
}

struct {
    int id_word;
    int bit_num;
    int subcommand;
    int param;
    struct ata_features * str;
} feature_list[] = {
    { 1, 3,0x05,0x80,featTab83_86}, /* power management */
    { 1, 9,0x42,0x80,featTab83_86}, /* acoustic management */
    { 0, 6,0xaa,0x00,featTab82_85}, /* read look-ahead */
    { 1,14,0x03,0x00,featTab83_86}, /* force pio mode */
    {-1,-1,0x00,0x00,NULL} /* LAST FEATURE */
};

MED_RET_T ata_setFeatures(int disk)
{
#ifdef PMA
    return MED_OK;
#else
    int i=0;
    feature_list[3].param=8+disk_info[disk]->pio_mode;
    ATA_OUTB(0,IDE_SELECT);
    if(ata_waitForReady()!=MED_OK)
    {
        printk("[ata-setFeatures] failed: not rdy\n");
        return - MED_ERROR;
    }
        
    while(feature_list[i].id_word!=-1)
    {
        if(disk_info[disk]->features_support[feature_list[i].id_word] & (1<<feature_list[i].bit_num))
        {
            ATA_OUTB(feature_list[i].subcommand,IDE_FEATURES);
            ATA_OUTB(feature_list[i].param,IDE_NSECTOR);
            ATA_OUTB(IDE_SET_FEATURES,IDE_COMMAND);
            
            if(ata_waitForReady()!=MED_OK)
            {
                printk("[ata-setFeatures] failed: error on CMD\n");
                return - MED_ERROR;
            }
            
            if(ATA_INB(IDE_ALTSTATUS) & IDE_STATUS_ERR)
                if(ATA_INB(IDE_ERROR) & IDE_ERROR_ABRT)
                    return -MED_ERROR;
            
        }
        else
            printk("[ata-setFeatures] feature '%s' not supported\n",
                   feature_list[i].str[15-feature_list[i].bit_num].str);
        i++;
    }
    return MED_OK;
#endif
}

MED_RET_T ata_doPowerOn(int disk)
{
    ata_selectDisk(disk);
    if(!ata_powered)
    {
        arch_ata_powerUpHD();
        mdelay(10);
        if(ata_hardReset(disk)!=MED_OK)
            return -MED_ERROR;
        if(ata_setFeatures(disk)!=MED_OK)
            return -MED_ERROR;
        if(ata_setMultipleSect(disk)!=MED_OK)
            return -MED_ERROR;
        if(ata_freezeLock(disk)!=MED_OK)
            return -MED_ERROR;
        ata_powered=1;
    }
    return MED_OK;
}

MED_RET_T ata_identify(int disk,struct hd_info_s * disk_info)
{
    unsigned char * sector=(unsigned char *)kmalloc(sizeof(unsigned char)*SECTOR_SIZE);
    
    if(!sector)
    {
        printk("[ata_identify] can't malloc sector\n");
        return -MED_ENOMEM;
    }
    
    if(ata_rwData(disk,0,sector,1,ATA_DO_IDENT,ATA_WITH_DMA)<0)
    {
        kfree(sector);
        printk("[ata_identify] error reading from disk\n");
        return -MED_ERROR;
    }
    
    strncpy(disk_info->serial, &sector[20], 20);
    str_swapChar(disk_info->serial,20);
    str_findEnd(disk_info->serial,20);
    strncpy(disk_info->firmware, &sector[46], 8);
    str_swapChar(disk_info->firmware,8);
    str_findEnd(disk_info->firmware,8);
    strncpy(disk_info->model, &sector[54], 40);
    str_swapChar(disk_info->model,40);
    str_findEnd(disk_info->model,40);
    disk_info->partition_list=NULL;
    disk_info->multi_sector = ATA_GET_WORD(sector,47) & 0xFF ;
    if(disk_info->multi_sector==0)
        disk_info->multi_sector=16;
    if(ATA_GET_WORD(sector,64)&0x2)
        disk_info->pio_mode=4;
    else if(ATA_GET_WORD(sector,64)&0x1)
        disk_info->pio_mode=3;
    else
        disk_info->pio_mode=2;
    disk_info->features_support[0]=ATA_GET_WORD(sector,82);
    disk_info->features_support[1]=ATA_GET_WORD(sector,83);
    disk_info->features_support[2]=ATA_GET_WORD(sector,84);
    
    printk("[DISK] reading %s info\n     %s\n     %s|%s\n     %d sectors per ata request\n     pio mode=%d\n",
           disk_name[disk],
           disk_info->model,
           disk_info->firmware,disk_info->serial,disk_info->multi_sector,
           disk_info->pio_mode);
    return MED_OK;
}

MED_RET_T ata_initDisk(int disk)
{
    struct hd_info_s * diskData;
    MED_RET_T ret_val;
    
    ata_selectDisk(disk);
    if(!ata_powered)     
    {   
        arch_ata_powerUpHD();
        mdelay(10);
    }
       
    if(ata_hardReset(disk)!=MED_OK)
    {
        printk("[ata_initDisk] error doing ata_hardReset\n");
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }
   
    if(ata_chkReg(disk)!=MED_OK)
        printk("[ata_initDisk] WARN: error doing chkReg\n");
    
    ata_powered=1;
    
    diskData = (struct hd_info_s *)kmalloc(sizeof(struct hd_info_s));
    if(!diskData)
    {
        printk("[ata_initDisk] can't malloc diskInfo struc\n");
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }

    if((ret_val=ata_identify(disk,diskData))!=MED_OK)
    {   
        printk("[ata_initDisk] error identify returned %d\n",-ret_val);
        kfree(diskData);
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }

    disk_info[disk]=diskData;
#ifndef HAVE_DBUG    
    if((ret_val=ata_setFeatures(disk))!=MED_OK)
    {   
        printk("[ata_initDisk] error doing setFeatures (%d)\n",-ret_val);
        kfree(diskData);
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }
    
    if((ret_val=ata_setMultipleSect(disk))!=MED_OK)
    {   
        printk("[ata_initDisk] error doing setMultipleSect (%d)\n",-ret_val);
        kfree(diskData);
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }
    if((ret_val=ata_freezeLock(disk))!=MED_OK)
    {   
        printk("[ata_initDisk] error doing freezeLock (%d)\n",-ret_val);
        kfree(diskData);
        disk_info[disk]=NULL;
        return -MED_ERROR;
    }
#endif
    printk("[ata_initDisk] done\n");
    return MED_OK;
}

void ata_init(void)
{
    cur_disk = -1;
    ata_sleeping=0;
    ata_spinup=0;
    ata_powered=BOOT_ATA_POWER_STATE;
    ata_RW_thread=NULL;
    spinLock_ini(&ata_lock);
        
    arch_ata_init();
    
    irq_enable(IRQ_IDE);
    
    printk("[ATA init] done\n");
}

void ide_intAction(int irq,struct pt_regs * regs)
{
    if(ata_RW_thread && ata_RW_thread->state==THREAD_BLOCKED_BY_DMA)
    {
        ata_RW_thread->state=THREAD_STATE_ENABLE;
        /*NOTE: we could implement here some kind of test if current thread can
                be preempted */
        if(threadCurrent==idleThread)
           threadCurrent=ata_RW_thread;
    }
}

/*****************************************************
*  Extended log of identify function
*  This can be called from cmd line
******************************************************/

void printFeat(int type,struct ata_features * features,int val)
{
    int i;
    char * ok;
    char * ko;
    if(type)
    {
        ok="supported";
        ko="NOT supported";
    }
    else
    {
        ok="enable";
        ko="disable";   
    }
    
    for(i=0;i<16;i++)
    {
        printk("\t-%d:",15-i);
        if(features[i].disp)
        {
            printk(" %s ",features[i].str);
            if(features[i].negate)
            {
                if(val&(0x1<<(15-i)))
                   printk(ko);
                else
                    printk(ok);
            }
            else
            {
                if(val&(0x1<<(15-i)))
                   printk(ok);
                else
                    printk(ko);
            }
            printk("\n");
        }
        else
            printk(" NONE\n");
    }
    
}

void ata_printIdentify(int disk)
{
    unsigned char * sector=(unsigned char *)kmalloc(sizeof(unsigned char)*SECTOR_SIZE);
    char str[50];
    int val1,val2;
    
    if(!sector)
        return ;
    if(ata_rwData(disk,0,sector,1,ATA_DO_IDENT,ATA_WITH_DMA)<0)
    {
        kfree(sector);
        return;                
    }
    val1=ATA_GET_WORD(sector,0);
    printk("0: %s, %s, %s\n",
           val1&0x8000?"Not ATA device":"ATA device",
           val1&0x0080?"Removable device":"Not removable device",
           val1&0x0003?"Incomplete Identify":"Identify OK");
    printk("2: Spec config=0x%04x\n",ATA_GET_WORD(sector,2));
    printk("7-8: CF spec: 0x%04x 0x%04x\n",ATA_GET_WORD(sector,7),ATA_GET_WORD(sector,8));
    strncpy(str, &sector[20], 20);
    str_swapChar(str,20);
    str_findEnd(str,20);
    printk("10-19: Serial Num: %s\n",str);
    strncpy(str, &sector[46], 8);
    str_swapChar(str,8);
    str_findEnd(str,8);
    printk("23-26: Firmware Rev: %s\n",str);
    strncpy(str, &sector[54], 40);
    str_swapChar(str,40);
    str_findEnd(str,40);
    printk("27-46: Model Num: %s\n",str);
    printk("47: Max sector number per INT in READ/WRITE MULTIPPLE: 0x%02x\n",ATA_GET_WORD(sector,47)&0xFF);
    val1=ATA_GET_WORD(sector,49);
    val2=ATA_GET_WORD(sector,50);
    printk("49-50: Capabilities: Standby timer %s %s, IORDY %s, %s, LBA %s, DMA %s\n",
           val1&0x2000?"is standard":"is vendor specific",
           val2&0x0001?"and has a min value vendor specific":"",
           val1&0x0800?"is supported":"is not supported",
           val1&0x0400?"can be disable":"can't be disable",
           val1&0x0200?"is supported":"is not supported",
           val1&0x0100?"is supported":"is not supported");
    val1=ATA_GET_WORD(sector,53);
    printk("53: field validity: word 88 is %s, words 64->70 are %s\n",
           val1&0x0003?"valid":"not valid",
           val1&0x0002?"valid":"not valid");
    val1=ATA_GET_WORD(sector,59);
    printk("59: Multiple sector setting (%s): 0x%02x sector per read/write multiple\n",
           val1&0x0100?"valid":"not valid",
           val1&0xFF);
    printk("60-61: Max number of addressable sectors = 0x%04x%04x\n",ATA_GET_WORD(sector,60),ATA_GET_WORD(sector,61));
    val1=ATA_GET_WORD(sector,63);
    printk("63: DMA mode selection: %s%s%s Support %s%s%s\n",
           val1&0x0400?"2":"-",
           val1&0x0200?"1":"-",
           val1&0x0100?"0":"-",
           val1&0x0004?"2":"-",
           val1&0x0002?"1":"-",
           val1&0x0001?"0":"-");
    val1=ATA_GET_WORD(sector,64);
    printk("64: PIO mode: (val=0x%02x) mode %s\n",
           val1&0xFF,
           val1&0x1&&val1&0x2?"Serial":(val1&0x1?"PIO 3":"PIO 4"));
    printk("65: Min Multiword DMA xfer cycle time: 0x%04x ns\n",ATA_GET_WORD(sector,65));
    printk("66: Manuf recom. Multiword DMA xfer cycle time: 0x%04x ns\n",ATA_GET_WORD(sector,66));
    printk("67: Min PIO xfer cycle time without flow ctrl: 0x%04x ns\n",ATA_GET_WORD(sector,67));
    printk("68: Min PIO xfer cycle time with IORDY flow ctrl: 0x%04x ns\n",ATA_GET_WORD(sector,68));
    printk("75: Max queue depth: 0x%01x\n",(ATA_GET_WORD(sector,68)&0xF)+1);
    val1=ATA_GET_WORD(sector,80);
    if(val1!=0x0000 && val1!=0xFFFF)
        printk("80: Version Major=0x%04x\n",val1);
    else
        printk("80: Not complient with ATA major version\n");
    val1=ATA_GET_WORD(sector,81);
    if(val1!=0x0000 && val1!=0xFFFF)
        printk("81: Version Major=0x%04x\n",val1);
    else
        printk("81: Not complient with ATA major version\n");
    val1=ATA_GET_WORD(sector,82);
    printk("82: Command set support\n");
    printFeat(1,featTab82_85,val1);
    val1=ATA_GET_WORD(sector,83);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("83: Command set support\n");
        printFeat(1,featTab83_86,val1);
    }
    else
    {
        printk("83: Command set support => not valid\n");
    }
    val1=ATA_GET_WORD(sector,84);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("84: Command set support\n");
        printFeat(1,featTab84_87,val1);
    }
    else
    {
        printk("84: Command set support => not valid\n");
    }
    val1=ATA_GET_WORD(sector,85);
    printk("85: Command set support\n");
    printFeat(0,featTab82_85,val1);
    val1=ATA_GET_WORD(sector,86);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("86: Command set support\n");
        printFeat(0,featTab83_86,val1);
    }
    else
    {
        printk("86: Command set support => not valid\n");
    }
    val1=ATA_GET_WORD(sector,87);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("87: Command set support\n");
        printFeat(0,featTab84_87,val1);
    }
    else
    {
        printk("87: Command set support => not valid\n");
    }
    val1=ATA_GET_WORD(sector,88);
    printk("88: Ultra DMA mode selection: %s%s%s%s%s%s%s Support %s%s%s%s%s%s%s\n",
           val1&0x4000?"6":"-",val1&0x2000?"5":"-",val1&0x1000?"4":"-",val1&0x0800?"3":"-",
           val1&0x0400?"2":"-",val1&0x0200?"1":"-",val1&0x0100?"0":"-",
           val1&0x0040?"6":"-",val1&0x0020?"5":"-",val1&0x0010?"4":"-",val1&0x0008?"3":"-",
           val1&0x0004?"2":"-",val1&0x0002?"1":"-",val1&0x0001?"0":"-");
    val1=ATA_GET_WORD(sector,89);
    printk("89: Time required for security erase unit ");
    if(val1==0)
        printk("not specified\n");
    else if(val1<255)
        printk("0x%x minutes\n",val1*2);
    else
        printk("> 508 minutes\n");
    val1=ATA_GET_WORD(sector,90);
    printk("90: Time required for Enhanced security erase ");
    if(val1==0)
        printk("not specified\n");
    else if(val1<255)
        printk("0x%x minutes\n",val1*2);
    else
        printk("> 508 minutes\n");
    val1=ATA_GET_WORD(sector,91);
    printk("91: Current power mgt level 0x%x",val1&0xFF);
    val1=ATA_GET_WORD(sector,92);
    printk("92: Master Password revision code ");
    if(val1==0x000 ||val1==0xFFFF)
        printk("not supported\n");
    else
        printk("0x%x\n",val1);
    val1=ATA_GET_WORD(sector,93);
    printk("93: Hardware reset result: 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,94);
    printk("94: Vendor's recommended acoustic mgt value: 0x%02x, Current: 0x%02x\n",(val1>>8)&0xFF,val1&0xFF);
    val1=ATA_GET_WORD(sector,95);
    printk("95: Stream min request size 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,96);
    printk("96: Stream xfer time - DMA 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,97);
    printk("97: Stream access latency - DMA and PIO 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,98);
    val2=ATA_GET_WORD(sector,99);
    printk("99-98: Stream xperf granurality 0x%04x%04x\n",val1,val2);
    val1=ATA_GET_WORD(sector,102);
    val2=ATA_GET_WORD(sector,103);
    printk("103-100: Max LBA in 48bit mode: 0x%04x%04x",val1,val2);
    val1=ATA_GET_WORD(sector,100);
    val2=ATA_GET_WORD(sector,101);
    printk("%04x%04x\n",val1,val2);
    val1=ATA_GET_WORD(sector,104);
    printk("104: Stream xfer time - PIO 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,106);
    printk("106: Pysical sector / logical sector size : ");
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("%s %s - c^%x logical sector per physical sector\n",
               val1&0x2000?"has multiple sector":"no multiple sector",
               val1&0x1000?"logical sector longer than 256 words":"",
               val1&0x7);
    }
    else
    {
        printk("no valid data\n");
    }
    val1=ATA_GET_WORD(sector,107);
    printk("107: Interseek delay: 0x%04x",val1);
    
    val1=ATA_GET_WORD(sector,108);
    val2=ATA_GET_WORD(sector,109);
    printk("108-111: NAA 0x%04x IEE OUI 0x%08x Unic ID 0x%x",(val1>>12)&0x7,(val1&0xFFF)<<12|(val2>>4),val2&0x7);
    val1=ATA_GET_WORD(sector,110);    
    val2=ATA_GET_WORD(sector,111);
    printk("%04x%04x\n",val1,val2);
    val1=ATA_GET_WORD(sector,117);    
    val2=ATA_GET_WORD(sector,118);
    printk("117-118: Words per logical sector : 0x%04x%04x\n",val1,val2);
    val1=ATA_GET_WORD(sector,127);    
    printk("127: Media Status Notification (0x%x): %s\n",val1&0x3,val1&0x2?"":(val1&0x1?"supported":"not supported"));
    val1=ATA_GET_WORD(sector,128);
    printk("128: Security stattus : 0x%04x\n",val1);
    val1=ATA_GET_WORD(sector,160);
    printk("160: CFA power mode : 0x%04x\n",val1);
    
}
