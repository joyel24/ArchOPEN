/*
*   kernel/driver/i2c.c
*
*   AMOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/thread.h>

#include <driver/hardware.h>
#include <driver/i2c.h>
#include <driver/gio.h>
/*******************************************************/
/* if 1 ==> i2c code not in iram, 0 ==> i2c code in iram (if the device has some)*/
#if 1
#undef __IRAM_CODE
#define __IRAM_CODE
#undef __IRAM_DATA
#define __IRAM_DATA
#endif
/*******************************************************/

#define DELAY_1              { int _x; for(_x=0;_x<10;_x++); };
#define DELAY_2              { int _x; for(_x=0;_x<10;_x++); };

/* we don't want to disable INT during i2c port acces*/
#define ENA_INT
//sti();
#define DIS_INT            
//cli();

/* Macro used to change the data and clock port of the i2c master*/
/* These macro are used internaly by i2c driver*/
#define SDA_HI             GIO_SET(GIO_I2C_DATA);
#define SDA_LO             GIO_CLEAR(GIO_I2C_DATA);
#define SDA_IN             GIO_DIRECTION(GIO_I2C_DATA,GIO_IN);
#define SDA_OUT            GIO_DIRECTION(GIO_I2C_DATA,GIO_OUT);
#define SDA                GIO_IS_SET(GIO_I2C_DATA)
#define SCL_HI             GIO_SET(GIO_I2C_CLOCK);
#define SCL_LO             GIO_CLEAR(GIO_I2C_CLOCK);
#define SCL_IN             GIO_DIRECTION(GIO_I2C_CLOCK,GIO_IN);
#define SCL_OUT            GIO_DIRECTION(GIO_I2C_CLOCK,GIO_OUT);
#define SCL                GIO_IS_SET(GIO_I2C_CLOCK)
#define DA_1               DIS_INT SDA_IN ENA_INT
#define CL_1               DIS_INT SCL_IN ENA_INT 
#define DA_0               DIS_INT SDA_LO SDA_OUT ENA_INT
#define CL_0               DIS_INT SCL_LO SCL_OUT ENA_INT

/*
spinlock used to lock access to i2c bus for one driver at a time
lock/unlock is done by i2c_getLock / i2c_release
these functions are directly used in i2c_read and i2c_write
if you need direct access to i2c port (like for MAS) use them in your core direclty
*/
__IRAM_DATA struct spinLock i2c_lock;

/******************************************************
i2c driver init 
******************************************************/

/* we just need to init the spinlock atm*/
void i2c_init(void)
{
    spinLock_ini(&i2c_lock);
    printk("[init] i2c\n");
}

/* Main i2c function for reading data
device is the device ID (found in i2c.h)
address is the address in slave chip to be read
buffer : a byte buffer were the dat will be put
count: number of bytes to readCPUMode*/

__IRAM_CODE int i2c_read(int device, int address, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;

    /* we assume that we can yo a yield, 
    if we are in INT mode, we will exit directly ! (see i2c_getLock code)*/
    if(!i2c_getLock(I2C_YIELD,0))
    {
        printk("[i2c_read] Can't get a lock on I2C\n");
        return -4;
    }
    
    i2c_ini_xfer();    
    i2c_start(); /* i2c bus is ready, all slave chip are waiting to be called*/

    /* sending device ID (in write mode)*/
    if(i2c_outb(((device<<24)>>25)<<1) != 0)
    {
        i2c_release();
        return -2;
    }

    /*sending the address to be read*/
    if(i2c_outb((address<<24)>>24) != 0)
    {
        i2c_release();
        return -3;
    }

    /* ready to start the read process*/
    i2c_start();

    /* calling the device (in read mode)*/
    if(i2c_outb(((device<<24)>>24)|1) != 0)
    {
        i2c_release();
        return -4;
    }
    
    /* reading count-1 data + ack after each byte*/
    for(i=0;i<count-1;i++)
    {
        buff[i]=i2c_inb();
        i2c_ack();
    }
    
    /*reading last byte*/
    buff[i]=i2c_inb();

    /* as it is the last byte, sending nAck*/
    i2c_notAck();
    i2c_stop();
    /* i2c read is finished => we can release the lock on the bus*/
    i2c_release();
    return 0;
}

/* Main i2c function for writing data
device is the device ID (found in i2c.h)
address is the address in slave chip to be read
buffer : a byte buffer were the dat will be put
count: number of bytes to readCPUMode*/

__IRAM_CODE int i2c_write(int device, int address, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;
    /* we assume that we can yo a yield, 
    if we are in INT mode, we will exit directly ! (see i2c_getLock code)*/
    if(!i2c_getLock(I2C_YIELD,0))
    {
        printk("[i2c_write] Can't get a lock on I2C\n");
        return -4;
    }
    
    i2c_ini_xfer();    
    i2c_start(); /* i2c bus is ready, all slave chip are waiting to be called*/
    /* sending device ID (in write mode)*/
    if(i2c_outb(((device<<24)>>25)<<1) != 0)
    {
        i2c_release();
        return -1;
    }
    
    /*sending the address to be read*/
    if(i2c_outb((address<<24)>>24) != 0)
    {
        i2c_release();
        return -2;
    }

    /* sending all data, i2c_outb return 0 if we get an ack from the slave chip
    doing an arror otherwise*/
    for(i=0;i<count;i++)
        if(i2c_outb(buff[i])!=0)
        {
            i2c_release();
            return -3;
        }

    i2c_stop();
    /* i2c write is finished => we can release the lock on the bus*/
    i2c_release();
    return 0;
}

/* specific write, were no adress is specify, usefull to fully write on a devie*/
__IRAM_CODE int i2c_writeRaw(int device, void * buffer, int count)
{
    int i;
    char * buff=(char*) buffer;
    
    if(!i2c_getLock(I2C_YIELD,0))
    {
        printk("[i2c_writeRaw] Can't get a lock on I2C\n");
        return -2;
    }
    
    i2c_ini_xfer();    
    i2c_start();/* i2c bus is ready, all slave chip are waiting to be called*/
    /* sending device ID (in write mode)*/
    if(i2c_outb(((device<<24)>>25)<<1) != 0)
    {
        i2c_release();
        return -1;
    }

    /* direct write, i2c_outb return 0 if we get an ack from the slave chip
    doing an arror otherwise*/
    for(i=0;i<count;i++)
        if(i2c_outb(buff[i])==0)
            break;

    i2c_stop();
    /* i2c write is finished => we can release the lock on the bus*/
    i2c_release();
    return 0;
}

/******************************************************
i2c bus locking functions
******************************************************/

/* spinlock lock function
it returns 1 if the i2c bus has been reserved, 0 otherwise
doYield can be: I2C_NO_YIELD, I2C_YIELD, I2C_LOOP
delay is the number of loop (nothing to do with real seconds)
*/

int i2c_getLock(int doYield,int delay)
{
    while(1)
    {
        if(!spinLock_testAndLock(&i2c_lock))
        {
            if(doYield) /* is the mode different from I2C_NO_YIELD */
            {
                /* we are about to yield or loop until lock is available*/
                if(readCPUMode()==0x13) /* are we in INT mode ? yes ==> not a good idea to yield or loop*/
                {
                    printk("[I2C lock] in not INT mode => exit\n");
                    return 0;
                }
                if(doYield==I2C_YIELD) /* yield mode => let's yield*/
                {
                    yield();
                }
                else if(doYield!=I2C_LOOP) /* bad param in doYield, if we are here, we should have I2C_LOOP*/
                {
                    printk("[I2C lock] bad param => exit\n");
                    return 0;
                }
            }
            else if(delay>0) /* we are in I2C_NO_YIELD mode, using delay param to see if we need to loop*/
            {
                delay--;   
            }
            else /* I2C_NO_YIELD mode and delay ==0 => exit, the port is not available for us*/
                return 0;
        }
        else /* lock has been sucessfully taken */
            return 1;
    }
    return 0; /* we should neer come here, just for safe */
}

void i2c_release(void)
{
    /* release s simple, just unlock the spinlock*/
    spinLock_unlock(&i2c_lock);
}


/*********************************************
Internal functions
Low level i2c access, needed by i2c_read, i2c_write and i2c_writeRaw
Mas code also directly access to these functions
*********************************************/

__IRAM_CODE void i2c_ini_xfer(void)
{
    int _val; 
    DELAY_1 
    CL_1 
    DELAY_1 
    DA_1 
    DELAY_1
    _val=inw(GIO_BITSET1);
    /*if(!(_val & SCL_MASK) || !(_val & SDA_MASK))
        return -1; */
}

#define WAIT_I2C           while(!SCL) /*nothing*/;
__IRAM_CODE void wait_i2c(void)
{
    while(!SCL) /*nothing*/;
}

__IRAM_CODE void i2c_start(void)
{
    DELAY_1
    CL_1  
    DELAY_1
    DA_0    
    DELAY_1    
    CL_0
    DELAY_1
}

__IRAM_CODE void i2c_stop(void)
{
    CL_0
    DELAY_1
    DA_0
    DELAY_1
    CL_1
    DELAY_1
    DA_1
    DELAY_1
}

__IRAM_CODE void i2c_ack(void)
{
    CL_0
    DELAY_1
    DA_0
    DELAY_1
    CL_1
    DELAY_1
    while(!SCL) /*nothing*/;
    DELAY_1
    CL_0
    DELAY_1
    DA_1
    DELAY_1
}

__IRAM_CODE void i2c_notAck(void)
{
    CL_0
    DELAY_1
    DA_1
    DELAY_1
    CL_1
    DELAY_1
    WAIT_I2C
    DELAY_1
    CL_0
    DELAY_1
}

__IRAM_CODE int i2c_getAck(void)
{
    int ret=0;
    
    CL_0
    DELAY_1
    DA_1
    DELAY_1
    CL_1
    DELAY_1
    WAIT_I2C
    DELAY_1
    if(SDA)
        ret=1;
    DELAY_1
    CL_0
    DELAY_1
    /*if(!ret)
        printk("not ack\n");
    else
        printk("ack\n");*/
    return ret;
}

__IRAM_CODE char i2c_inb(void)
{
    char i;
    char ret=0;
    for(i=0x80;i;i=i>>1)
    {
        CL_0
        DELAY_1
        CL_1
        DELAY_1
        WAIT_I2C
        if(SDA)
            ret |= i; 
         DELAY_1     
    }
    CL_0
    DELAY_1    
    return ret;
}

__IRAM_CODE int i2c_outb(char data)
{
    char i;    
    for(i=0x80;i;i=i>>1)
    {
        CL_0
        DELAY_1
        if(i&data)
        {
            DA_1
        }
        else
        {
            DA_0
        }
        DELAY_1
        CL_1
        DELAY_1        
    }
    CL_0
    DELAY_1
    //while(!i2c_getAck()) /*nothing*/;
    
    return i2c_getAck();
}

__IRAM_CODE void mas_i2c_outb(char data)
{
    char i;    
    for(i=0x80;i;i=i>>1)
    {
        CL_0
        DELAY_1
        if(i&data)
        {
            DA_1
        }
        else
        {
            DA_0
        }
        DELAY_1
        CL_1
        DELAY_1        
    }
    CL_0
    DELAY_1    
}
