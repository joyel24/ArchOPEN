/*
* kernel/gui/gfx/shell/internal_commands.c
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
#include <lib/gdb.h>

#include <sys_def/stddef.h>
#include <sys_def/types.h>
#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/delay.h>
#include <kernel/console.h>
#include <kernel/lang.h>

#include <init/exit.h>

#include <driver/ata.h>
#include <driver/usb_fw.h>
#include <driver/buttons.h>
#include <driver/lcd.h>
#include <driver/energy.h>

#include <fs/disk.h>
#include <fs/vfs.h>
#include <fs/stdfs.h>

#include <gfx/graphics.h>
#include <gfx/jpg.h>
#include <gfx/bmp.h>

#include <gui/internal_commands.h>
#include <gui/shell.h>
#include <gui/file_browser.h>
#include <gui/msgBox.h>

#include <gui/settings_time.h>
#include <gui/settings_energy.h>
#include <gui/settings_misc.h>
#include <gui/settings_lang.h>
#include <gui/settings_bgImg.h>
#include <gui/settings_browser.h>
#include <gui/settings_shell.h>
#include <gui/time_alarm.h>
#include <gui/status_line.h>

#include <gui/player.h>
#include <gui/playlistmenu.h>
#include <snd/sound.h>
#include <snd/playlist.h>

static bool intCmd_doBrowser(char * param);
static bool intCmd_doReloadFirmware(char * param);
static bool intCmd_doUsbMode(char * param);
static bool intCmd_doTimeSetting(char * param);
static bool intCmd_doEnergySetting(char * param);
static bool intCmd_doMiscSetting(char * param);
static bool intCmd_doLangSetting(char * param);
static bool intCmd_doPlayer(char * param);
static bool intCmd_doPlaylist(char * param);
static bool intCmd_doReloadMedios(char * param);
static bool intCmd_doReloadMediosFile(char * param);
static bool intCmd_doFlashDump(char * param);
static bool intCmd_doHalt(char * param);
static bool intCmd_doLowPower(char * param);
static bool intCmd_doBgImgSetting(char * param);
static bool intCmd_doTest(char * param);
static bool intCmd_doCon_flushToDisk(char * param);
static bool intCmd_doLoadJpg(char * param);
static bool intCmd_doLoadBmp(char * param);
static bool intCmd_doBrowserSetting(char * param);
static bool intCmd_doSet_shell(char * param);
static bool intCmd_doTime_Alarm(char * param);
#ifdef USE_GDB
static bool intCmd_doBkpt(char * param);
#endif

typedef struct{
    char * command;
    bool (*function)(char * param);
} INTERNAL_COMMAND;

INTERNAL_COMMAND intCmd_commands[] = {
    {
        command:  "test",
        function: intCmd_doTest
    },
    {
        command:  "browser",
        function: intCmd_doBrowser
    },
    {
        command:  "reload_firmware",
        function: intCmd_doReloadFirmware
    },
    {
        command:  "reload_medios",
        function: intCmd_doReloadMedios
    },
    {
        command:  "reload_mediosFile",
        function: intCmd_doReloadMediosFile
    },
    {
        command:  "usb_mode",
        function: intCmd_doUsbMode
    },
    {
        command:  "set_time",
        function: intCmd_doTimeSetting
    },
    {
        command:  "set_energy",
        function: intCmd_doEnergySetting
    },
    {
        command:  "set_browser",
        function: intCmd_doBrowserSetting
    },
    {
        command:  "set_shell",
        function: intCmd_doSet_shell
    },
    {
        command:  "set_misc",
        function: intCmd_doMiscSetting
    },
    {
        command:  "chg_bgImg",
        function: intCmd_doBgImgSetting
    },
    {
        command:  "chg_lang",
        function: intCmd_doLangSetting
    },
    {
        command:  "player",
        function: intCmd_doPlayer
    },
    {
        command:  "playlist",
        function: intCmd_doPlaylist
    },
    {
        command:  "flashDump",
        function: intCmd_doFlashDump
    },
    {
        command:  "haltDevice",
        function: intCmd_doHalt
    },
    {
        command:  "sleepDevice",
        function: intCmd_doLowPower
    },
    {
        command:  "flushConsole",
        function: intCmd_doCon_flushToDisk
    },
    {
        command:  "loadJpeg",
        function: intCmd_doLoadJpg
    },
    {
        command:  "loadBmp",
        function: intCmd_doLoadBmp
    },
    {
        command: "timeAlarm",
        function: intCmd_doTime_Alarm
    },
#ifdef USE_GDB
    {
        command: "doBkpt",
        function: intCmd_doBkpt
    },
#endif
    /* should always be the last entry */
    {
        command:  NULL,
        function: NULL
    }
};

bool intCmd_execute(char * command,char * param){
    bool retval;
    int i;

    printk("[shell] executing internal command '%s', param='%s'\n",command,(param)?param:"");

    i=0;
    retval=false;
    for(;;){
        if(intCmd_commands[i].command==NULL){
            printk("[shell] error: internal command '%s' not found\n",command);
            retval=false;
            break;

        }else if(!strcmp(intCmd_commands[i].command,command)){
            retval=intCmd_commands[i].function(param);
            break;
        }
        ++i;
    }

    return retval;
}

/******************************************************************************/
/* INTERNAL COMMANDS                                                          */
/******************************************************************************/

static bool intCmd_doBrowser(char * param){
    struct browser_data * browser;

    browser=browser_NewDualBrowse();
    //browser=browser_NewBrowse();

    if(!browser){
        return false;
    }

    /*browser->is_dual=1;
    browser->dual_mode=1;*/
    
    browser_browse(browser,param,NULL);

    browser_disposeBrowse(browser);

    return true;
}

static bool intCmd_doReloadFirmware(char * param){

    reload_firmware();

    return true;
}

static bool intCmd_doReloadMedios(char * param)
{
#ifndef NO_MEDIOS_RELOAD 
#ifndef MEDIOS_FILE 
#error no MEDIOS_FILE define for arch
#endif
    if(reload_medios(MEDIOS_FILE)!=MED_OK)
        return false;
#endif
    return true;
}

static bool intCmd_doReloadMediosFile(char * param)
{
#ifndef NO_MEDIOS_RELOAD
    if(reload_medios(param)!=MED_OK)
        return false;
#endif
    return true;
}

int usbMode=0;

static bool intCmd_doUsbMode(char * param){
    if(usbMode)
    {
// in usb mode => disable usb
        printk( "Warning starting USB mode, but we are already in this mode\n");
        return false;
// maybe we can do better: checking if usb cable is present, usb is enable ...
    }
    else
    {
        int eh=evt_getHandler(ALL_CLASS);
        int evt;

        // not in usb mode => enable usb if cable present
        if(usb_isConnected() || FW_isConnected())
        {

            msgBox_show(getLangStr(STRLNG_USB_MODE),getLangStr(STRLNG_USB_SWITCHING),MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,-1);
            
            // stop sound player if playing
            sound_stop();

            if(disk_rmAll()!=MED_OK)
            {
                printk("can't umount\n");

                evt_freeHandler(eh);
                return false;
            }

            enableUsbFw();

            usbMode=1;
            mdelay(10);
            msgBox_show(getLangStr(STRLNG_USB_MODE),getLangStr(STRLNG_USB_UNPLUG),MSGBOX_TYPE_INFO,
                MSGBOX_ICON_INFORMATION,-1);

            /* wait for key unpress */
            do{
                evt=evt_getStatus(eh);
            }while(evt==BTN_F3);
            
            /* wait for repress */            
            do{
                evt=evt_getStatus(eh);
                statusLine_handleEvent(evt);
            }while(evt!=BTN_F3 && evt!=EVT_USB_OUT);

            if(!lcd_enabled())
                lcd_keyPress();
            
            msgBox_show(getLangStr(STRLNG_USB_MODE),getLangStr(STRLNG_USB_LEAVING),MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,-1);
            mdelay(10);
            disableUsbFw();
            usbMode=0;
            mdelay(10);
            ata_hardReset(HD_DISK);
            vfs_init();
            disk_addAll();     
            
            /* reloading menu */
#if 0
            shellMenu_close();
            if(!shellMenu_init())
            {
                msgBox_show("MediOS - USB mode","Can't reload Menu",MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,eh);
                
            }
#endif
#warning we should reload menu.cfg or other menu related files
        }
        else
        {
            msgBox_show(getLangStr(STRLNG_USB_MODE),getLangStr(STRLNG_USB_NOT_CONNECTED),MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,eh);
        }

        evt_freeHandler(eh);
    }

    return true;
}

static bool intCmd_doTimeSetting(char * param){
    clock_setting();
    return true;
}

static bool intCmd_doEnergySetting(char * param){
    energy_setting();
    return true;
}

static bool intCmd_doMiscSetting(char * param){
    misc_setting();
    return true;
}

static bool intCmd_doLangSetting(char * param){
    lang_setting();
    return true;
}

static bool intCmd_doPlayer(char * param){
    char * ext;
    char * p;
    PLAYLIST_ITEM * item;

    if(param!=NULL && strlen(param)>0){

        p=strrchr(param,'.');

        ext="";
        if(p!=NULL){
            ext=p+1;
        }

        if(strcmp(ext,"m3u")){ //is it not a playlist?

            p=strrchr(param,'/');

            if(p!=NULL){

                sound_stop();

                //we want only the path part of the filename
                *p='\0';

                playlist_clear();
                playlist_addFolder(param,false);

                *p='/';

                //find the item corresponding to the filename
                item=playlist_first;
                while(item!=NULL && strcasecmp(param,item->name)){
                    item=item->next;
                }

                //play it
                sound_start();
                sound_activeItem=item;
                sound_play(false);
            }

        }else{

            sound_stop();

            playlist_clear();

            playlist_addM3UPlaylist(param);

            //play the playlist
            sound_start();
            sound_activeItem=playlist_first;
            sound_play(false);
        }
    }

    //show player
    player_eventLoop();
    return true;
}

static bool intCmd_doPlaylist(char * param){
    //show playlist
    playlistMenu_eventLoop();
    return true;
}

static bool intCmd_doFlashDump(char * param){
    int fd,size;
    
    if(!disk_initDone())
    {
        printk("Error: disk is not init\n");
        return true;
    }
    
    fd=open("/flash.bin",O_WRONLY|O_CREAT);
    if (fd<0){
        printk("[Dump] can't open file /flash.bin !\n");
        return true;
    }
    size=write(fd,(char*)0x100000,0x100000);
    printk("Wrote :%x\n",size);
    close(fd);
    return true;
}

static bool intCmd_doHalt(char * param){
    //kernel_doCmd(CMD_HALT_DEVICE);
    halt_device();
    return true;
}

static bool intCmd_doLowPower(char * param)
{
    energy_lowPower();
    /* let's wait all key released */
    __cli();
    while(arch_btn_readHardware()!=0x0) /*nothing*/;
    __sti();
    return true;
}

#include <fs/vfs_node.h>
static bool intCmd_doTest(char * param)
{
    int fd;
    struct vfs_node * node;
    char tmp[20];
    fd=open(param,O_RDONLY);
    if(fd<0)
        return true;
    node=(struct vfs_node *)fd;
    printk("start: %x\n",node->storage_location);
    read(fd,tmp,10);
    close(fd);
    return true;
}

bool intCmd_doBgImgSetting(char * param)
{
    bgImg_setting();
    return true;   
}

bool intCmd_doBrowserSetting(char * param)
{
    browser_setting();
    return true;   
}

bool intCmd_doCon_flushToDisk(char * param)
{
    con_flushToDisk();
    return true;   
}

bool intCmd_doLoadJpg(char * param)
{
    int evt,evt_handler;
    gfx_planeHide(BMAP1);
    gfx_planeHide(BMAP2);
    gfx_planeShow(VID1);  
      
    gfx_loadJpg(param,NULL);
    
    evt_handler=evt_getHandler(BTN_CLASS);
    while(1)
    {
        evt=evt_getStatus(evt_handler);
        if(evt==NO_EVENT)
            continue;
        if(evt==BTN_OFF)
            break;
    }
    gfx_planeShow(BMAP1);
    gfx_planeHide(VID1);
    return true;
}

bool intCmd_doLoadBmp(char * param)
{
    int evt,evt_handler;
    gfx_planeHide(BMAP1);
    gfx_planeHide(BMAP2);
    gfx_planeShow(VID1);
      
    gfx_loadBmp_toPlane_center(param,VID1);
    printk("Back from loader\n");
    evt_handler=evt_getHandler(BTN_CLASS);
    while(1)
    {
        evt=evt_getStatus(evt_handler);
        if(evt==NO_EVENT)
            continue;
        if(evt==BTN_OFF)
            break;
    }
    printk("Exit\n");
    gfx_planeShow(BMAP1);
    gfx_planeHide(VID1);
    return true;
}

bool intCmd_doSet_shell(char * param)
{
    shell_setting();
    return true;   
}

bool intCmd_doTime_Alarm(char * param)
{
    time_alarm();
    return true;   
}

#ifdef USE_GDB
bool intCmd_doBkpt(char * param)
{
    gdb_bkpt;
    return true;   
}
#endif
