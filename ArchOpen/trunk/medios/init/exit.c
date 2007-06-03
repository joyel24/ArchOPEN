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

#include <driver/ata.h>
#include <driver/hardware.h>
#include <driver/usb_fw.h>

#include <fs/stdfs.h>

#include <init/exit.h>

__IRAM_DATA int powering_off = 0;

extern int kusb_fw_status;

void halt_device(void)
{
    powering_off=1;
    printk("[exit] device halt\n");
#ifdef CHK_USB_FW
    if(kusb_fw_status)
        disableUsbFw();
#endif        
    arch_HaltMsg();
    
    ata_stopHD(ATA_FORCE_STOP); /* we need to call halt_hd later to unmount all partitions */

    udelay(100);
    printk("[exit] ready to halt\n");
    clf();
    //outw(0,0x30a1a);
    while(1) /* nothing */;
}

void reload_firmware(void)
{
    printk("[exit] firmware reload\n");

    arch_reload_firmware();
}
#ifndef NO_MEDIOS_RELOAD
#ifdef RELOAD_CP_BASE
__attribute__((section(".fwuncomp_code"))) reload_mediosStart(char * buffer, int size)
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

void reload_medios(void)
{
#ifndef NO_MEDIOS_RELOAD
#ifndef MEDIOS_POS 
#error no MEDIOS_POS define for arch
#endif
#ifndef MEDIOS_FILE 
#error no MEDIOS_FILE define for arch
#endif
    int fd;
    int size;
    char * filedata;
    void (*firmware_start)(void);
    
    printk("[exit] reload Medios from disk\n");
    
    /* opening medios.avi */
    fd=open(MEDIOS_FILE,O_RDONLY);
    if (fd<0){
        printk("[Reload Medios] can't open file %s !\n",MEDIOS_FILE);
        return;
    }
    
    size=filesize(fd);
    lseek(fd,MEDIOS_POS,SEEK_SET);
    filedata=(char*)malloc(size-MEDIOS_POS);
    
    if(!filedata)
    {
        printk("Can't malloc\n");
        return;
    }
    
    read(fd,filedata,size-MEDIOS_POS);
    close(fd);
    
#ifdef RELOAD_CP_BASE
    reload_mediosStart(filedata,size-MEDIOS_POS);
#else
    firmware_start=(void (*)(void)) filedata;    
    firmware_start();
#endif
#endif
}

