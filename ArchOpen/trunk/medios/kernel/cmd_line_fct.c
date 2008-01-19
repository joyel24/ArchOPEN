/*
*   kernel/core/cmd_line_fct.c
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
#include <kernel/cmd_line.h>

#include <fs/med.h>
#include <fs/disk.h>

#include <init/exit.h>

#include <kernel/thread.h>
#include <kernel/malloc.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/io.h>
#include <kernel/list.h>
#include <kernel/lang.h>

#include <driver/gio.h>
#include <driver/cpld.h>
#include <driver/fm_remote.h>
#include <driver/ata.h>
#include <driver/energy.h>
#include <driver/lcd.h>

#include <fs/stdfs.h>
#include <fs/vfs.h>
#include <fs/vfs_node.h>
#include <fs/fat.h>

/* cmd list */

void do_help(unsigned char ** params);
void do_mem (unsigned char ** params);
void do_run (unsigned char ** params);
void do_tasks (unsigned char ** params);
void do_halt (unsigned char ** params);
void do_sleep(unsigned char ** params);
void do_reload (unsigned char ** params);
void print_handler_info (unsigned char ** params);
void do_memory_dump (unsigned char ** params);
void do_reg_print (unsigned char ** params);
void do_in (unsigned char ** params);
void do_out (unsigned char ** params);
void do_ps (unsigned char ** params);
void do_kill (unsigned char ** params);
void do_ThreadState (unsigned char ** params);
void do_ThreadInfo (unsigned char ** params);
void do_ThreadNice (unsigned char ** params);
void do_gioDir (unsigned char ** params);
void do_gioSetState (unsigned char ** params);
void do_gioGetState (unsigned char ** params);
void do_cpldRead (unsigned char ** params);
void do_cpldWrite (unsigned char ** params);
void do_hdSleep (unsigned char ** params);
void do_mount (unsigned char ** params);
void do_umount (unsigned char ** params);
#ifdef HAVE_ST_RTC
void do_rtcGet (unsigned char ** params);
void do_rtcSet (unsigned char ** params);
#endif
void do_pwd(unsigned char ** params);
void do_cd(unsigned char ** params);
void do_ls(unsigned char ** params);
void do_cat(unsigned char ** params);
void do_diskFullInfo(unsigned char ** params);
void do_loadLand(unsigned char ** params);

void do_launchDecode(unsigned char ** params);
#ifdef HAVE_MAS_SOUND
void do_MasStat(unsigned char ** params);
#endif

void do_brightSet(unsigned char ** params);
void do_brightGet(unsigned char ** params);


struct cmd_line_s cmd_tab[] = {
    {
        cmd        : "help",
        help_str   : "This is help",
        cmd_action : do_help,
        nb_args    : 0
    },
    {
        cmd        : "mem",
        help_str   : "shows free memory",
        cmd_action : do_mem,
        nb_args    : 0
    },
    {
        cmd        : "run",
        help_str   : "runs specified file",
        cmd_action : do_run,
        nb_args    : 1
    },
    {
        cmd        : "handlerInfo",
        help_str   : "Prints IRQ, timer, HW chk handler",
        cmd_action : print_handler_info,
        nb_args    : 0
    },
    {
        cmd        : "dump",
        help_str   : "Dumps memory from given address, usage: dump address size",
        cmd_action : do_memory_dump,
        nb_args    : 2
    },
    {
        cmd        : "halt",
        help_str   : "Halts the device",
        cmd_action : do_halt,
        nb_args    : 0
    },
    {
        cmd        : "sleep",
        help_str   : "Put device in sleep mode",
        cmd_action : do_sleep,
        nb_args    : 0
    },
    {
        cmd        : "restart",
        help_str   : "Reloads the firmware",
        cmd_action : do_reload,
        nb_args    : 0
    },
    {
        cmd        : "in",
        help_str   : "Reads a value from memory, usage: in address size",
        cmd_action : do_in,
        nb_args    : 2
    },
    {
        cmd        : "out",
        help_str   : "Writes a value to memory, usage: out address size value",
        cmd_action : do_out,
        nb_args    : 3
    },
    {
        cmd        : "ps",
        help_str   : "List threads",
        cmd_action : do_ps,
        nb_args    : 0
    },
    {
        cmd        : "kill",
        help_str   : "kill thread with given pid",
        cmd_action : do_kill,
        nb_args    : 1
    },
    {
        cmd        : "threadState",
        help_str   : "Enable/Disable a thread with given pid",
        cmd_action : do_ThreadState,
        nb_args    : 2
    },
    {
        cmd        : "threadInfo",
        help_str   : "Prints info on a thread with given pid",
        cmd_action : do_ThreadInfo,
        nb_args    : 1
    },
    {
        cmd        : "threadNice",
        help_str   : "Set thread priority for given pid",
        cmd_action : do_ThreadNice,
        nb_args    : 2
    },
    {
        cmd        : "gioDir",
        help_str   : "Set the direction DIR for a given gio GIO: gioDir GIO DIR",
        cmd_action : do_gioDir,
        nb_args    : 2
    },
    {
        cmd        : "gioSetState",
        help_str   : "set gio GIO to state STATE: gioSetState GIO STATE",
        cmd_action : do_gioSetState,
        nb_args    : 2
    },
    {
        cmd        : "gioGetState",
        help_str   : "Print state of gio GIO: gioGetState GIO STATE",
        cmd_action : do_gioGetState,
        nb_args    : 1
    },    
    {
        cmd        : "cpldRead",
        help_str   : "Read cpld port N: cpldRead N",
        cmd_action : do_cpldRead,
        nb_args    : 1
    },
    {
        cmd        : "cpldWrite",
        help_str   : "Write VAL to cpld port N: cpldWrite N VAL",
        cmd_action : do_cpldWrite,
        nb_args    : 2
    },
    {
        cmd        : "hdSleep",
        help_str   : "Put HDD in sleep mode",
        cmd_action : do_hdSleep,
        nb_args    : 0
    },    
    {
        cmd        : "mount",
        help_str   : "mount DRIVE : mount all partitions of the drive",
        cmd_action : do_mount,
        nb_args    : 1
    },    
    {
        cmd        : "umount",
        help_str   : "umount DRIVE : umount all partitions of the drive",
        cmd_action : do_umount,
        nb_args    : 1
    },
#ifdef HAVE_ST_RTC
    {
        cmd        : "rtcGet",
        help_str   : "rtcGet : print current time and date",
        cmd_action : do_rtcGet,
        nb_args    : 0
    },
    {
        cmd        : "rtcSet",
        help_str   : "rtcSet hh mm ss dd mm yy : set rtc time",
        cmd_action : do_rtcSet,
        nb_args    : 6
    },
#endif
    {
        cmd        : "pwd",
        help_str   : "pwd : show current path",
        cmd_action : do_pwd,
        nb_args    : 0
    },
    {
        cmd        : "cd",
        help_str   : "cd PATH: change path to PATH",
        cmd_action : do_cd,
        nb_args    : 1
    },
    {
        cmd        : "ls",
        help_str   : "ls: list current folder content",
        cmd_action : do_ls,
        nb_args    : 0
    },
    {
        cmd        : "cat",
        help_str   : "cat FILE_NAME: print FILE_NAME content",
        cmd_action : do_cat,
        nb_args    : 1
    },    
    {
        cmd        : "diskFullInfo",
        help_str   : "diskInfo DISK: print identify info for DISK",
        cmd_action : do_diskFullInfo,
        nb_args    : 1
    },
    {
        cmd        : "loadLng",
        help_str   : "loadLng lng_file: load a new language file",
        cmd_action : do_loadLand,
        nb_args    : 1
    },
    {
        cmd        : "lnchDecode",
        help_str   : "lnchDecode: simulate an INT on MAS",
        cmd_action : do_launchDecode,
        nb_args    : 0
    },
#ifdef HAVE_MAS_SOUND
    {
        cmd        : "masStat",
        help_str   : "masStat: gives current MAS stat",
        cmd_action : do_MasStat,
        nb_args    : 0
    },
#endif
    {
        cmd        : "brightSet",
        help_str   : "brightSet val: set lcd brightness",
        cmd_action : do_brightSet,
        nb_args    : 1
    },
    {
        cmd        : "brightGet",
        help_str   : "brightGet: gives current value of lcd brightness",
        cmd_action : do_brightGet,
        nb_args    : 0
    },
    /* this has to be the last entry */
    {
        cmd        : NULL,
        help_str   : NULL,
        cmd_action : NULL,
        nb_args    : 0
    }
};

void do_help(unsigned char ** params)
{
    int i;
    printk("Available cmd:\n");
    for(i=0;cmd_tab[i].cmd!=NULL;i++)
        printk("%s: %s\n",cmd_tab[i].cmd,cmd_tab[i].help_str);
}

void do_mem (unsigned char ** params)
{
    mem_printStat();
}

void do_run (unsigned char ** params)
{
#ifdef TARGET_TYPE_STD
    med_load (params[0]);
#endif
}

void do_halt (unsigned char ** params)
{
    kernel_doCmd(CMD_HALT_DEVICE);
}

void do_sleep(unsigned char ** params)
{
    energy_lowPower();
}

void do_reload (unsigned char ** params)
{
    reload_firmware();
}

void print_handler_info (unsigned char ** params)
{
    irq_print();
    tmr_print();
}

void do_memory_dump (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);

    printk("Memory dump from %i (0x%0.8x), %d bytes:\n", Address, Address, Size);

    unsigned char* Memory = (unsigned char*)Address;

    print_data(Memory,Size);
}

void do_in (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);
    int Value=0;

    switch (Size){
        case 1:
            Value=inb(Address);
            printk("0x%0.8x=%d (0x%0.2x)\n",Address,Value,Value);
            return;
        case 2:
            Value=inw(Address);
            printk("0x%0.8x=%d (0x%0.4x)\n",Address,Value,Value);
            return;
        case 4:
            Value=inl(Address);
            printk("0x%0.8x=%d (0x%0.8x)\n",Address,Value,Value);
            return;
        default:
            printk("Size should be 1, 2 or 4\n");
            return;
    }
}

void do_out (unsigned char ** params)
{
    int Address = atoi (params[0]);
    int Size = atoi (params[1]);
    int Value = atoi (params[2]);

    switch (Size){
        case 1:
            outb(Value,Address);
            Value=inb(Address);
            printk("0x%0.8x=%d (0x%0.2x)\n",Address,Value,Value);
            return;
        case 2:
            outw(Value,Address);
            Value=inw(Address);
            printk("0x%0.8x=%d (0x%0.4x)\n",Address,Value,Value);
            return;
        case 4:
            outl(Value,Address);
            Value=inl(Address);
            printk("0x%0.8x=%d (0x%0.8x)\n",Address,Value,Value);
            return;
        default:
            printk("Size should be 1, 2 or 4\n");
            return;
    }
}

void do_ps (unsigned char ** params)
{
    thread_ps();
}

void do_kill (unsigned char ** params)
{
    int pid = atoi (params[0]);
    MED_RET_T ret_val;
    printk("Killing pid %d\n",pid);
    ret_val=thread_kill(pid);
    if(ret_val!=MED_OK)
        printk("Error killing %d : %d\n",pid,-ret_val);
}

void do_ThreadState (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int state = atoi (params[1]);
    if(state)
    {
        printk("Enable thread %d\n",pid);
        thread_enable(pid);
    }
    else
    {
        printk("Disable thread %d\n",pid);
        thread_disable(pid);
    }
}

void do_ThreadInfo (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int i;
    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("thread with pid %d not found\n",pid);
    }
    else
    {
        thread_printInfo(ptr);
        if(pid!=0)
        {
            for(i=0;i<THREAD_NB_RES;i++)
            {
                printk("Res %d\n",i);
                thread_listPrintPtr(i,ptr);
            }
        }
    }
}
void do_ThreadNice (unsigned char ** params)
{
    int pid = atoi (params[0]);
    int prio = atoi (params[1]);

    THREAD_INFO * ptr=thread_findPid(pid);
    if(!ptr)
    {
        printk("thread with pid %d not found\n",pid);
    }
    else
    {
        thread_printInfo(ptr);
        if(thread_nice(ptr,prio)==MED_OK)
        {
            printk("Priority %d set for PID=%d\n",prio,pid);
        }
        else
        {
            printk("Bad priority %d\n",prio);
        }
    }
}

void do_gioDir (unsigned char ** params)
{
    int gio = atoi (params[0]);
    int dir = atoi (params[1]);
    
    if(dir!=0 && dir!=1)
        return;
    GIO_DIRECTION(gio,dir);
}

void do_gioSetState (unsigned char ** params)
{
    int gio = atoi (params[0]);
    int state = atoi (params[1]);
    
    switch(state)
    {
        case 0:
            GIO_CLEAR(gio);
            break;
        case 1:
            GIO_SET(gio);
            break;
    }
}

void do_gioGetState (unsigned char ** params)
{
    int gio = atoi (params[0]);
    printk("Gio 0x%x is %s (DIR=%s)\n",gio,GIO_IS_SET(gio)?"set":"not set",GIO_GET_DIR(gio)?"IN":"OUT");
}

void do_cpldRead (unsigned char ** params)
{
    int portNum=atoi (params[0]);
    printk("cpld_%d=%x\n",portNum,cpld_read(portNum));
}

void do_cpldWrite (unsigned char ** params)
{
    int portNum=atoi (params[0]);
    int val=atoi (params[1]);
    cpld_write(portNum,val);
    printk("wrote %x in cpld_%d\n",val,portNum);    
}

void do_hdSleep (unsigned char ** params)
{
    int drive=atoi (params[0]);
    if(drive==HD_DISK || drive==CF_DISK)
    {
        ata_StopHD(drive);
    }
}

void do_mount (unsigned char ** params)
{
    int drive=atoi (params[0]);
    if(drive==HD_DISK || drive==CF_DISK)
    {
          disk_add(drive); 
    }
}

void do_umount (unsigned char ** params)
{
    int drive=atoi (params[0]);
    if(drive==HD_DISK || drive==CF_DISK)
    {
        disk_rm(drive); 
    }
}

#ifdef HAVE_ST_RTC
#include <driver/rtc_ST.h>
#include <driver/time.h>

void do_rtcGet (unsigned char ** params)
{
    struct med_tm valTime;
    if(rtc_getTime(&valTime) == MED_OK)
    {
        time_print(&valTime); 
        printk("\n");
    }
    else
        printk("Can't retrieve time/date from rtc\n");
}

void do_rtcSet (unsigned char ** params)
{
    struct med_tm valTime;
    valTime.tm_hour=atoi (params[0]);
    valTime.tm_min=atoi (params[1]);
    valTime.tm_sec=atoi (params[2]);
    valTime.tm_mday=atoi (params[3]);
    valTime.tm_mon=atoi (params[4]);
    valTime.tm_year=atoi (params[5]);
    valTime.tm_wday=time_getDayOfWeek(valTime.tm_mday,valTime.tm_mon,valTime.tm_year);
    if(rtc_setTime(&valTime) == MED_OK)
    {
        printk("Time/date send to rtc\n");
    }
    else
        printk("Can't send time/date to rtc\n");
}
#endif

void do_pwd(unsigned char ** params)
{
    char path[MAX_PATH];
    
    fs_pwd(path);
    printk("%s\n",path);
}

void do_cd(unsigned char ** params)
{
    int ret_val=fs_cd(params[0]);
    if(ret_val==-MED_ENOENT)
    {
        printk("%s not found\n",params[0]);
    }
    else if(ret_val==-MED_ENOTDIR)
    {
        printk("%s not a folder\n",params[0]);
    }
}

void do_ls(unsigned char ** params)
{
    struct vfs_node * ptr,*rootPtr;
    int nb_children;
    MED_RET_T ret_val;
    
    if(threadCurrent->path)
        rootPtr=threadCurrent->path;
    else
        rootPtr=root_mountPoint->root_node;
            
    if(!rootPtr->dir_loaded)
    {
        ret_val=fat_loadDir(rootPtr);
        if(ret_val!=MED_OK)
        {
            printk("Error loading dir %s: %d\n",rootPtr->name.str,-ret_val);
            return;
        }
    }
    
    LIST_FOREACH_NAMED(rootPtr->children,ptr,nb_children,
                       siblings_prev,siblings_next)
    {
        printk("%s",ptr->name.str);
        if(ptr->type==VFS_TYPE_DIR)
        {
            printk("/");
        }
        printk("\n");
    }
}

void do_cat(unsigned char ** params)
{
    int fd;
    char tmp[513];
    int size;
    fd=open(params[0],O_RDONLY);
    if(fd>=0)
    {
        while(1)
        {
            size=read(fd,tmp,512);
            if(size<0)
                break;
            tmp[size]='\0';
            uart_outString(tmp,CMD_LINE_UART);
            if(size!=512)
                break;
        }
        printk("\n");
    }
}

void do_diskFullInfo(unsigned char ** params)
{
    int disk=atoi (params[0]);
    if(disk!=0 && disk!=1)
        return;
    ata_printIdentify(disk);
}

void do_loadLand(unsigned char ** params)
{
#ifdef TARGET_TYPE_STD
    MED_RET_T res;
    res=lang_loadFile(params[0]);
    if(res==-MED_EBADDATA)
        printk("Error bad lang version\n");
    else if(res<0)
        printk("Error loading lang\n");
#endif
}

#ifdef HAVE_MAS_SOUND
#include <driver/mas.h>
#endif

void do_launchDecode(unsigned char ** params)
{
#ifdef HAVE_MAS_SOUND
    printk("Cur state: %d\n",mas_mp3DecodeState());
    mas_mp3LaunchDecode();
#endif
}

#ifdef HAVE_MAS_SOUND
extern __IRAM_DATA int cnt_send;
extern __IRAM_DATA int tot_send;
extern struct mas_sound_buffer * soundBuffer;
void do_MasStat(unsigned char ** params)
{
    printk("Status: %x / %x(%x)\n",soundBuffer->read,soundBuffer->size,soundBuffer);
    printk("status %x-%x-(%x,%x,%x,%x)\n",mas_getD0(0x7f7),mas_getD0(0x7f9),
           mas_getD0(0x7fc),mas_getD0(0x7fd),mas_getD0(0x7fe),mas_getD0(0x7ff));
    printk("frame count %x, head1: %x, head2: %x, crc err: %x, nb anci: %x\n",mas_getD0(0xFD0),mas_getD0(0xFD1),
           mas_getD0(0xFD2),mas_getD0(0xFD3),mas_getD0(0xFD4));
           
}
#endif

void do_brightSet(unsigned char ** params)
{
    int val=atoi (params[0]);
    int old=lcd_getBrightness();
    lcd_setBrightness(val);
    printk("Set %d->%d\n",old,val);
}

void do_brightGet(unsigned char ** params)
{
    printk("Brightness %d\n",lcd_getBrightness());
}
