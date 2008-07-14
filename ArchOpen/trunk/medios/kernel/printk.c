/*
*   kernel/core/printk.c
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

#include <stdarg.h>
#include <sys_def/ctype.h>
#include <sys_def/colordef.h>

#include <driver/uart.h>
#include <driver/hardware.h>
#include <driver/lcd.h>

#include <kernel/console.h>
#include <kernel/thread.h>

#ifdef USE_GDB
#include <lib/gdb.h>
#endif

//int vsnprintf (char * buf, size_t size, const char * fmt, va_list args);
static char debugmembuf[255];

int printk_on_uart=0;
bool printk_printing=false;

void printk(char *fmt, ...)
{
    while(printk_printing) yield();
    printk_printing=true;

    va_list ap;
    va_start(ap, fmt);
    vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
#ifdef USE_GDB
    gdb_outString(debugmembuf);    
#else    
    if(printk_on_uart)
        uart_outString(debugmembuf,DEBUG_UART);
#endif
    con_write(debugmembuf,COLOR_LIGHT_GREEN);

    printk_printing=false;
}

int printf(__const char * fmt, ...)
{
    int res;
    va_list ap;

    while(printk_printing) yield();
    printk_printing=true;

    va_start(ap, fmt);
    res = vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, ap);
    va_end(ap);
#ifdef USE_GDB
    gdb_outString(debugmembuf);    
#else    
    if(printk_on_uart)
        uart_outString(debugmembuf,DEBUG_UART);
#endif
    con_write(debugmembuf,COLOR_WHITE);

    printk_printing=false;

    return res;
}

int vprintf(__const char * fmt, va_list args)
{
    int res;

    while(printk_printing) yield();
    printk_printing=true;

    res = vsnprintf(debugmembuf, sizeof(debugmembuf), fmt, args);
#ifdef USE_GDB
    gdb_outString(debugmembuf);    
#else    
    if(printk_on_uart)
        uart_outString(debugmembuf,DEBUG_UART);
#endif
    con_write(debugmembuf,COLOR_WHITE);

    printk_printing=false;

    return res;
}

void print_nonhexa(char * str)
{
    int i;
    for(i=0;i<16;i++)
    {
        if(isprint(str[i]))
            printk("%c",str[i]);
        else
            printk(".");
    }
}

void print_data(char * data,int length)
{    
    char str[17];
    int i;
    for(i=0;i<length;i++)
    {
        if(i%16==0) // new line
        {
            if(i!=0) /* do we need to end the previous line? */
            {
                printk(" ] ");
                print_nonhexa(str);
                printk("\n");
            }
            printk("%p: [",&data[i]);
        }
        
        if(i%4==0)
            printk(" ");

        printk("%02X",(unsigned char)data[i]);
        str[i%16]=data[i];        
    }
    printk(" ] ");
    print_nonhexa(str);
    printk("\n");
}

void printk_uartEnable(void)
{
    uart_need(DEBUG_UART);
    uart_changeSpeed(115200,DEBUG_UART);    
    printk_on_uart = 1;
}

void printk_uartDisable(void)
{
    printk_on_uart = 0;
}

void printk_init(void)
{
    printk_on_uart = 0;
    printk_printing = false;
    printk_uartEnable();
}

void str_swapChar(char * txt,int size)
{
    int i;
    for(i=0;i<size/2;i++)
    {
        char c=txt[2*i];
        txt[2*i]=txt[2*i+1];
        txt[2*i+1]=c;
    }
}

void str_findEnd(char * txt,int size)
{
    int i,j;
    /* find first real char */
    for(i=0;i<size;i++)
        if(txt[i]!=' ')
            break;
    /* remove head space */
    for(j=i;j<size;j++)
    {
        if(txt[j]==' ')
        {
            txt[j-i]=0;
            break;
        }
        txt[j-i]=txt[j];
    }
    if(txt[j-i]!=0)
        txt[j-i]=0;
}
