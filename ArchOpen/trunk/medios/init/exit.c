/*
*   kernel/init/exit.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/

#include <kernel/kernel.h>
#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/delay.h>
#include <kernel/malloc.h>

#include <driver/ata.h>
#include <driver/hardware.h>
#include <driver/usb_fw.h>

#include <fs/stdfs.h>
#include <fs/disk.h>

#include <init/exit.h>

#include <lib/string.h>

#include <gui/shell.h>

__IRAM_DATA int powering_off = 0;

extern int kusb_fw_status;

static void medios_close(){

#ifndef BUILD_LIB
    shell_close();
#endif

#ifdef CHK_USB_FW
    if(kusb_fw_status)
        disableUsbFw();
#endif
}


void halt_device(void)
{
    powering_off=1;

    printk("[exit] device halt\n");
    arch_HaltMsg();
   
    medios_close();

    printk("[exit] medios cleanup done\n");    
    
    if(disk_rmAll()!=MED_OK)
        printk("WARN: was not able to umount\n");
    if(ata_StopHD(HD_DISK)!=MED_OK)
       printk("WARN: was not able to stop HDD\n");

    udelay(100);
    printk("[exit] ready to halt\n");
    clf();
    cli();
    //outw(0,0x30a1a);
    while(1) /* nothing */;
}

void reload_firmware(void)
{
    printk("[exit] firmware reload\n");

    medios_close();

    arch_reload_firmware();
}
#ifndef NO_MEDIOS_RELOAD
#ifdef RELOAD_CP_BASE
__attribute__((section(".fwuncomp_code"))) void reload_mediosStart(char * buffer, int size)
{
    void (*firmware_start)(void)=(void (*)(void))buffer;
    int i;
    char * dest=(char*)SDRAM_START;
    for(i=0;i<size;i++)
        *dest++=*buffer++;
    firmware_start();    
}
#endif
#endif

MED_RET_T reload_medios(char * fname)
{
#ifndef NO_MEDIOS_RELOAD
#ifndef MEDIOS_POS 
#error no MEDIOS_POS define for arch
#endif
    int fd;
    int size,i,binSize;
    char * filedata;
    char header[18];
    
#ifndef RELOAD_CP_BASE    
    void (*firmware_start)(void);
#endif
    
    printk("[exit] reload Medios from disk\n");
    
    /* opening medios.avi */
    fd=open(fname,O_RDONLY);
    if (fd<0){
        printk("[Reload Medios] can't open file %s !\n",fname);
        return -MED_ERROR;
    }
    
    size=filesize(fd);
    lseek(fd,MEDIOS_POS,SEEK_SET);
    
    i=read(fd,header,18);
    if(i!=18)
    {
        printk("Error reading header\n");
        close(fd);
        return -MED_ERROR;
    }
    
    /*check magik*/    
    if(strncmp(&header[4],"MEDIOS",6))
    {
        printk("Bad Magik\n");
        close(fd);
        return -MED_ERROR;
    }
    
    /* get size */
    
    binSize=0;
    for(i=0;i<4;i++)
        binSize|=header[i+12]<<(i*8);
    printk("Bin size = %d-%x\n",binSize,binSize);
    
    if(binSize>(size-MEDIOS_POS))
    {
        printk("Bad size\n");
        close(fd);
        return -MED_ERROR;
    }
    
    filedata=(char*)malloc(binSize);
    
    if(!filedata)
    {
        printk("Can't malloc\n");
        close(fd);
        return -MED_ERROR;
    }
    
    lseek(fd,MEDIOS_POS,SEEK_SET);
    read(fd,filedata,binSize);
    close(fd);
    
    medios_close();

#ifdef RELOAD_CP_BASE
    reload_mediosStart(filedata,size-MEDIOS_POS);
#else
    firmware_start=(void (*)(void)) filedata;    
    firmware_start();
#endif
#endif
    return MED_OK;
}

