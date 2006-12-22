/*
*   kernel/driver/cpld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/cpld.h>

#define T_OUT_CPLD_SELECT 10000

int cpld_portState[5]=
{
    CPLD_PORT0_INIT,
    CPLD_PORT1_INIT,
    CPLD_PORT2_INIT,
    CPLD_PORT3_INIT,
    CPLD_PORT4_INIT,
};

int cpld_portArray[5] = {
    CPLD_PORT0,
    CPLD_PORT1,
    CPLD_PORT2,
    CPLD_PORT3,
    CPLD_PORT4
};

int CPLD_VER;

void cpld_init(void)
{
    printk("CPLD %d = %x\n",0,cpld_read(CPLD0));
    cpld_doSelect();
    printk("CPLD %d = %x",1,cpld_read(CPLD1));    
    outw(cpld_portState[CPLD1],CPLD_PORT1);
    printk(" => %x\n",cpld_read(CPLD1));
    printk("CPLD %d = %x",2,cpld_read(CPLD2));
    outw(cpld_portState[CPLD2],CPLD_PORT2);
    printk(" => %x\n",cpld_read(CPLD2));
    printk("CPLD %d = %x",3,cpld_read(CPLD3));
    outw(cpld_portState[CPLD3],CPLD_PORT3);
    printk(" => %x\n",cpld_read(CPLD3));

    CPLD_VER=cpld_getVersion();

    /* everything is ok */
    printk("[init] CPLD Ver:0x%x\n",CPLD_VER);
}

void cpld_changeState(int cpld_port,int bit_num,int direction)
{
    int tmp;

    tmp=cpld_portState[cpld_port];
    if(direction)
        tmp |= (0x1 << bit_num);
    else
        tmp &= ~(0x1 << bit_num);

    if(tmp!=cpld_portState[cpld_port])
    {
        cpld_portState[cpld_port]=tmp;
        outw(cpld_portState[cpld_port],cpld_portArray[cpld_port]);
    }
}

int cpld_read(int cpld_port)
{
    return inw(cpld_portArray[cpld_port]);
}

void cpld_write(int cpld_port,int value)
{
    outw(value,cpld_portArray[cpld_port]);
}

void cpld_select(int bit_num,int direction)
{
    int val;
    if(direction)
        val = cpld_portState[CPLD0] | (0x1 << bit_num);
    else
        val = cpld_portState[CPLD0] & ~(0x1 << bit_num);
    if(val != cpld_portState[CPLD0])
    {
        cpld_portState[CPLD0]=val;
        cpld_doSelect();
    }
}

void cpld_doSelect(void)
{
    int res,res2;
    //int cnt;
    printk("changing cpld select : %x\n",cpld_portState[CPLD0]);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);
    outw(cpld_portState[CPLD0],CPLD_PORT0);

    //cnt=0;
    res=inw(cpld_portArray[CPLD0]);
    while((res2=inw(cpld_portArray[CPLD0]))!=res)//||cnt>T_OUT_CPLD_SELECT) /* wait for the value to become stable */
    {
        //cnt++;
        res=res2;
    }
    
    /*if(cnt>T_OUT_CPLD_SELECT)
        printk("[CPLD] Error in doSelect: timout (%x,%x)\n",res,res2);*/
}

int cpld_getVersion(void){
    return arch_cpld_getVersion();
}


