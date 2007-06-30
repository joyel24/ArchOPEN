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

#define ATA_NO_TASKLIST
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
    printk("[ATA] err TMOUT while wait for xfer\n");
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

    printk("[ATA] err TMOUT while wait for ready\n");
    
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
        /* if (Drive Ready || ata_stopping > 1000)*/
        if(((__status & IDE_STATUS_BSY)==0 && (__status & IDE_STATUS_RDY)!=0) || ata_stopping > 1000)
        {
            if(ata_stopping > 1000) printk("[ATA] TMOUT in ata_stopTmrFct\n");
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

#define GET_WORD(DATA,N) (DATA[(N*2)+1]<<8|DATA[N*2])

struct ata_features {
    char * str;
    int disp;
    int negate;
};
struct ata_features featTab82_85[]= {
    { NULL                   ,0,0},
    { "NOP"                  ,1,0},
    { "READ BUFFER"          ,1,0},
    { "WRITE BUFFER"         ,1,0},
    { NULL                   ,0,0},
    { "Host protected area"  ,1,0},
    { "DEVICE RESET"         ,1,0},
    { "SERVICE Int"          ,1,0},
    { "Release Int"          ,1,0},
    { "Look-ahead"           ,1,0},
    { "Write cache"          ,1,0},
    { "PACKET cmd"           ,1,1},
    { "Mandat. Power Mgt"    ,1,0},
    { "Removable media feat.",1,0},
    { "Security mode"        ,1,0},
    { "SMART"                ,1,0}
};
struct ata_features featTab83_86[]= {
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "FLUSH CACHE EXT"      ,1,0},
    { "Mandat. FLUSH CACHE"  ,1,0},
    { "Device config overlay",1,0},
    { "48-bit Address"       ,1,0},
    { "Auto accoustic mgt"   ,1,0},
    { "SET MAX security"     ,1,0},
    { NULL                   ,0,0},
    { "SET FEATURES required to spinup after power up",1,0},
    { "Power up in Stdby mode",1,0},
    { "Removable media Status notification",1,0},
    { "Adv. Power Mgt."      ,1,0},
    { "CFA feature set"      ,1,0},
    { "READ/WRITE DMA QUEUED",1,0},
    { "DOWNLOAD MICROCODE"   ,1,0}
};
struct ata_features featTab84_87[]= {
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "IDLE IMMEDIATE with UNLOAD FEATURE",1,0},
    { NULL                   ,0,0},
    { NULL                   ,0,0},
    { "URG bit for WRITE STREAM DMA EXT and WRITE STREAM EXT",1,0},
    { "URG bit for READ STREAM DMA EXT and READ STREAM EXT",1,0},
    { "64bit World Wide name",1,0},
    { "WRITE DMA QUEUED FUA EXT",1,0},
    { "WRITE DMA FUA EXT and WRITE MULTIPLE FUA EXT",1,0},
    { "General purpose logging",1,0},
    { "Streaming feature"    ,1,0},
    { "Media card pass through",1,0},
    { "Media serial number"  ,1,0},
    { "SMART self-test"      ,1,0},
    { "SMART error logging"  ,1,0},
};

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
        free(sector);
        return;                
    }
    val1=GET_WORD(sector,0);
    printk("0: %s, %s, %s\n",
           val1&0x8000?"Not ATA device":"ATA device",
           val1&0x0080?"Removable device":"Not removable device",
           val1&0x0003?"Incomplete Identify":"Identify OK");
    printk("2: Spec config=0x%04x\n",GET_WORD(sector,2));
    printk("7-8: CF spec: 0x%04x 0x%04x\n",GET_WORD(sector,7),GET_WORD(sector,8));
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
    printk("47: Max sector number per INT in READ/WRITE MULTIPPLE: 0x%02x\n",GET_WORD(sector,47)&0xFF);
    val1=GET_WORD(sector,49);
    val2=GET_WORD(sector,50);
    printk("49-50: Capabilities: Standby timer %s %s, IORDY %s, %s, LBA %s, DMA %s\n",
           val1&0x2000?"is standard":"is vendor specific",
           val2&0x0001?"and has a min value vendor specific":"",
           val1&0x0800?"is supported":"is not supported",
           val1&0x0400?"can be disable":"can't be disable",
           val1&0x0200?"is supported":"is not supported",
           val1&0x0100?"is supported":"is not supported");
    val1=GET_WORD(sector,53);
    printk("53: field validity: word 88 is %s, words 64->70 are %s\n",
           val1&0x0003?"valid":"not valid",
           val1&0x0002?"valid":"not valid");
    val1=GET_WORD(sector,59);
    printk("59: Multiple sector setting (%s): 0x%02x sector per read/write multiple\n",
           val1&0x0100?"valid":"not valid",
           val1&0xFF);
    printk("60-61: Max number of addressable sectors = 0x%04x%04x\n",GET_WORD(sector,60),GET_WORD(sector,61));
    val1=GET_WORD(sector,63);
    printk("63: DMA mode selection: %s%s%s Support %s%s%s\n",
           val1&0x0400?"2":"-",
           val1&0x0200?"1":"-",
           val1&0x0100?"0":"-",
           val1&0x0004?"2":"-",
           val1&0x0002?"1":"-",
           val1&0x0001?"0":"-");
    val1=GET_WORD(sector,64);
    printk("64: PIO mode: (val=0x%02x) mode %s\n",
           val1&0xFF,
           val1&0x1&&val1&0x2?"Serial":(val1&0x1?"PIO 3":"PIO 4"));
    printk("65: Min Multiword DMA xfer cycle time: 0x%04x ns\n",GET_WORD(sector,65));
    printk("66: Manuf recom. Multiword DMA xfer cycle time: 0x%04x ns\n",GET_WORD(sector,66));
    printk("67: Min PIO xfer cycle time without flow ctrl: 0x%04x ns\n",GET_WORD(sector,67));
    printk("68: Min PIO xfer cycle time with IORDY flow ctrl: 0x%04x ns\n",GET_WORD(sector,68));
    printk("75: Max queue depth: 0x%01x\n",(GET_WORD(sector,68)&0xF)+1);
    val1=GET_WORD(sector,80);
    if(val1!=0x0000 && val1!=0xFFFF)
        printk("80: Version Major=0x%04x\n",val1);
    else
        printk("80: Not complient with ATA major version\n");
    val1=GET_WORD(sector,81);
    if(val1!=0x0000 && val1!=0xFFFF)
        printk("81: Version Major=0x%04x\n",val1);
    else
        printk("81: Not complient with ATA major version\n");
    val1=GET_WORD(sector,82);
    printk("82: Command set support\n");
    printFeat(1,featTab82_85,val1);
    val1=GET_WORD(sector,83);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("83: Command set support\n");
        printFeat(1,featTab83_86,val1);
    }
    else
    {
        printk("83: Command set support => not valid\n");
    }
    val1=GET_WORD(sector,84);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("84: Command set support\n");
        printFeat(1,featTab84_87,val1);
    }
    else
    {
        printk("84: Command set support => not valid\n");
    }
    val1=GET_WORD(sector,85);
    printk("85: Command set support\n");
    printFeat(0,featTab82_85,val1);
    val1=GET_WORD(sector,86);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("86: Command set support\n");
        printFeat(0,featTab83_86,val1);
    }
    else
    {
        printk("86: Command set support => not valid\n");
    }
    val1=GET_WORD(sector,87);
    if(!(val1&0x8000) && val1&0x4000)
    {
        printk("87: Command set support\n");
        printFeat(0,featTab84_87,val1);
    }
    else
    {
        printk("87: Command set support => not valid\n");
    }
    val1=GET_WORD(sector,88);
    printk("88: Ultra DMA mode selection: %s%s%s%s%s%s%s Support %s%s%s%s%s%s%s\n",
           val1&0x4000?"6":"-",val1&0x2000?"5":"-",val1&0x1000?"4":"-",val1&0x0800?"3":"-",
           val1&0x0400?"2":"-",val1&0x0200?"1":"-",val1&0x0100?"0":"-",
           val1&0x0040?"6":"-",val1&0x0020?"5":"-",val1&0x0010?"4":"-",val1&0x0008?"3":"-",
           val1&0x0004?"2":"-",val1&0x0002?"1":"-",val1&0x0001?"0":"-");
    val1=GET_WORD(sector,89);
    printk("89: Time required for security erase unit ");
    if(val1==0)
        printk("not specified\n");
    else if(val1<255)
        printk("0x%x minutes\n",val1*2);
    else
        printk("> 508 minutes\n");
    val1=GET_WORD(sector,90);
    printk("90: Time required for Enhanced security erase ");
    if(val1==0)
        printk("not specified\n");
    else if(val1<255)
        printk("0x%x minutes\n",val1*2);
    else
        printk("> 508 minutes\n");
    val1=GET_WORD(sector,91);
    printk("91: Current power mgt level 0x%x",val1&0xFF);
    val1=GET_WORD(sector,92);
    printk("92: Master Password revision code ");
    if(val1==0x000 ||val1==0xFFFF)
        printk("not supported\n");
    else
        printk("0x%x\n",val1);
    val1=GET_WORD(sector,93);
    printk("93: Hardware reset result: 0x%04x\n",val1);
    val1=GET_WORD(sector,94);
    printk("94: Vendor's recommended acoustic mgt value: 0x%02x, Current: 0x%02x\n",(val1>>8)&0xFF,val1&0xFF);
    val1=GET_WORD(sector,95);
    printk("95: Stream min request size 0x%04x\n",val1);
    val1=GET_WORD(sector,96);
    printk("96: Stream xfer time - DMA 0x%04x\n",val1);
    val1=GET_WORD(sector,97);
    printk("97: Stream access latency - DMA and PIO 0x%04x\n",val1);
    val1=GET_WORD(sector,98);
    val2=GET_WORD(sector,99);
    printk("99-98: Stream xperf granurality 0x%04x%04x\n",val2,val1);
    val1=GET_WORD(sector,102);
    val2=GET_WORD(sector,103);
    printk("103-100: Max LBA in 48bit mode: 0x%04x%04x",val2,val1);
    val1=GET_WORD(sector,100);
    val2=GET_WORD(sector,101);
    printk("%04x%04x\n",val2,val1);
    val1=GET_WORD(sector,104);
    printk("104: Stream xfer time - PIO 0x%04x\n",val1);
    val1=GET_WORD(sector,106);
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
    val1=GET_WORD(sector,107);
    printk("107: Interseek delay: 0x%04x",val1);
    
    val1=GET_WORD(sector,108);
    val2=GET_WORD(sector,109);
    printk("108-111: NAA 0x%04x IEE OUI 0x%08x Unic ID 0x%x",(val1>>12)&0x7,(val1&0xFFF)<<12|(val2>>4),val2&0x7);
    val1=GET_WORD(sector,110);    
    val2=GET_WORD(sector,111);
    printk("%04x%04x\n",val1,val2);
    val1=GET_WORD(sector,117);    
    val2=GET_WORD(sector,118);
    printk("117-118: Words per logical sector : 0x%04x%04x\n",val1,val2);
    val1=GET_WORD(sector,127);    
    printk("127: Media Status Notification (0x%x): %s\n",val1&0x3,val1&0x2?"":(val1&0x1?"supported":"not supported"));
    val1=GET_WORD(sector,128);
    printk("128: Security stattus : 0x%04x\n",val1);
    val1=GET_WORD(sector,160);
    printk("160: CFA power mode : 0x%04x\n",val1);
    
}
