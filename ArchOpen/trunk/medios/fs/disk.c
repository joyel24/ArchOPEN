/*
*   kernel/fs/disk.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#include <sys_def/stddef.h>
#include <lib/string.h>

#include <kernel/malloc.h>
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

#include <driver/ata.h>
#include <driver/cf.h>

#include <fs/vfs.h>
#include <fs/fat.h>
#include <fs/disk.h>

/* Partition table entry layout:
   -----------------------
   0: 0x80 - active
   1: starting head
   2: starting sector
   3: starting cylinder
   4: partition type
   5: end head
   6: end sector
   7: end cylinder
   8-11: starting sector (LBA)
   12-15: nr of sectors in partition
*/

#define BYTES2INT32(array,pos) \
          (array[pos] | (array[pos+1] << 8 ) | \
          (array[pos+2] << 16 ) | (array[pos+3] << 24 ))

struct hd_info_s * disk_info[NB_DISK];

char * fatStr[]={"zero", "FAT12", "FAT16<32MB", "ExtMSDOS", "FAT16>32MB", "FAT32<2048GB", "FAT32-LBA",
                    "FAT16>32MB-LBA", "ExtMSDOS-LBA"};
int fatId[]={0x00, 0x01, 0x04, 0x05, 0x06, 0x0B, 0x0C, 0x0E, 0x0F};
int fatSupported[]={0, 1, 1, 1, 1, 1, 1, 1, 1};

char * disk_name[] = { "Main HD", "CF card"};

extern int hd_sleep_state;

void dd_swapChar(char * txt,int size);
void dd_findEnd(char * txt,int size);


MED_RET_T disk_add(int disk)
{
    int part_num=0;
    MED_RET_T ret_val;
    int lastPartition=4;
    char * name=(disk==HD_DISK?"/HD_X":"/CF_X");
    
    /* is the system sane */
    if(disk_info[disk])
    {
        printk("[Disk-add] Warning: info on %s already exists => doing disk_rm()\n",disk_name[disk]);
        disk_rm(disk);
    }
    
    /* getting info on disk */
    if(ata_initDisk(disk)!=MED_OK)
    {
        printk("[Disk-add] Error: can't setup disk %s (error in ata_initDisk)\n",disk_name[disk]);
        return -MED_ERROR;
    }
    
    if((ret_val=disk_readPartition(disk))!=MED_OK)
    {
        printk("[Disk-add] Error: can't setup disk %s (error in readPartition)\n",disk_name[disk]);
        return -MED_ERROR;
    }
    
    /* checking for partition less disk */
    if(!disk_info[HD_DISK]->partition_list[0].active)
    {
        printk("[Disk-add] Info: No active partition => start sector = 0\n");
        disk_info[HD_DISK]->partition_list[0].active=1;
        disk_info[HD_DISK]->partition_list[0].start=0;
        lastPartition=1;
    }
    
    /* main disk => mount PART0 on root folder */
    if(disk==HD_DISK)
    {
        ret_val=vfs_mount("/",HD_DISK,DISK_PART_0);
        if(ret_val!=MED_OK)
        {
            printk("[Disk-add] Error: can't mount root (error=%d)\n",-ret_val);
            return -MED_ERROR;
        }
        disk_info[HD_DISK]->partition_list[0].mounted=1;
        part_num=1;     
    }
    
    /* mounting all parts */
    for(;part_num<lastPartition;part_num++)
    {
        if(disk_info[disk]->partition_list[part_num].active==1)
        {
            /* changing part num in name */
            name[4]='0'+part_num;
            ret_val=vfs_mount(name,disk,part_num);
            if(ret_val!=MED_OK)
            {
                printk("[Disk-add] Error: can't mount %s part %d (error=%d)\n",disk_name[disk],part_num,-ret_val);
                return -MED_ERROR;
            }        
            disk_info[disk]->partition_list[part_num].mounted=1;
        }
        else
        {
            printk("[Disk-add] Info: won't mount %s part %d: not active\n",disk_name[disk],part_num);
        }
    }
           
    return MED_OK;   
}

MED_RET_T disk_rm(int disk)
{
    int i;
    int ret_val;
    if(disk_info[disk])
    {
        for(i=3;i>=0;i--)
        {
            if(disk_info[disk]->partition_list[i].mounted)
            {
                ret_val=vfs_umount(disk,i);
                if(ret_val!=MED_OK)
                {
                    printk("[Disk-rm] Error: can't umount %s part %d (error=%d)\n",disk_name[disk],i,-ret_val);
                    return -MED_ERROR;
                }
            }
        }
        /* now we can remove disk info */
        printk("free part list\n");
        kfree(disk_info[disk]->partition_list);
        printk("free disk info\n");
        kfree(disk_info[disk]);
        printk("end\n");
        disk_info[disk]=NULL;
    }
    return MED_OK;
}

MED_RET_T disk_addAll(void)
{
    if(disk_add(HD_DISK) != MED_OK)
    {
        printk("[Disk-addAll] Error: can't add main HD\n");
        return -MED_ERROR;
    }    

    if(CF_IS_CONNECTED)
    {
        disk_add(CF_DISK);
        /* no error returned */
    }
  
    return MED_OK;
}

MED_RET_T disk_rmAll(void)
{
    if(CF_IS_CONNECTED)
    {
        if(disk_rm(CF_DISK)!=MED_OK)
            return -MED_ERROR;
    }

    return disk_rm(HD_DISK);
}

int disk_initDone(void)
{
    if(disk_info[0]==NULL)
        return 0;
    else
        return 1;
}

MED_RET_T disk_init(void)
{
    int i;
    MED_RET_T ret_val;
    for(i=0;i<NB_DISK;i++) disk_info[i]=NULL;
    
    /* we should always have a HDD, let's add it */
    ret_val=disk_addAll();
    if(ret_val==MED_OK)
        printk("[DISK-init] done\n");
    else
        printk("[DISK-init] ERROR\n");
    return ret_val;
}

MED_RET_T disk_readPartition(int disk)
{
    int i,j;
    MED_RET_T ret_val;
    struct partition_info * part_info;
    unsigned char * sector=(unsigned char *)kmalloc(sizeof(unsigned char)*SECTOR_SIZE);
    if(!sector)
    {
        printk("[disk_readPartition] can't malloc sector\n");
        return -MED_ENOMEM;
    }
    
    /* let's assume we have only 4 partitions */
    part_info = (struct partition_info *)kmalloc(4*sizeof(struct partition_info));
    if(!part_info)
    {
        printk("[disk_readPartition] can't malloc part info\n");
        kfree(sector);
        return -MED_ENOMEM;
    }
    
    /* Read MBR */
    if((ret_val=ata_rwData(disk,0,sector,1,ATA_DO_READ,ATA_WITH_DMA))<0) /* read 1 sector at LBA 0 */
    {
        printk("[disk_readPartition] error doing ata_RW (%d)\n",-ret_val);
        kfree(sector);
        kfree(part_info);
        return -MED_EIO;
    }

    /* check that the boot sector is initialized */
    if ( (sector[510] != 0x55) ||
         (sector[511] != 0xaa)) {
        printk("Bad boot sector signature\n");
        kfree(sector);
        kfree(part_info);
        return -MED_ERROR;
    }

    /* parse partitions */
    for ( i=0; i<4; i++ ) {
        unsigned char* ptr = sector + 0x1be + 16*i;
        part_info[i].type  = ptr[4];
        part_info[i].start = BYTES2INT32(ptr, 8);
        part_info[i].size  = BYTES2INT32(ptr, 12);
        part_info[i].active = 0;
        part_info[i].mounted = 0;
        j=0;
        while(j<9 && fatId[j]!=part_info[i].type) j++;

        if(j<9)
        {
            if(fatSupported[j])
            {
                part_info[i].active = 1;
            }
            strcpy(part_info[i].strType,fatStr[j]);
            printk("\tPart%d: start=%08x, size=%08x, type:%s (%02x)\n",i,
                part_info[i].start,part_info[i].size,
                part_info[i].strType,part_info[i].type);
        }
        else        
                printk("Error: partition type not supported: %x\n",part_info[i].type);

        /* extended? */
        if ( part_info[i].type == 5 ) {
            /* not handled yet */
        }
    }
    
    /* adding part_info to disk structure */
    disk_info[disk]->partition_list=part_info;
    
    kfree(sector);
    
    return MED_OK;
}

char * disk_getName(int id)
{
    return disk_name[id];
}


