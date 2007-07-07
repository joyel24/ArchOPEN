/*
*   kernel/core/uart.c
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
#include <kernel/irq.h>
#include <kernel/kernel.h>
#include <kernel/pipes.h>

#include <driver/uart.h>
#include <driver/hardware.h>
#include <driver/gio.h>

struct pipe UART_PIPES[2];

struct pipe * UART_0_Pipe;
struct pipe * UART_1_Pipe;

int uart_clockSpeed;

void uart_intAction(int irq,struct pt_regs * regs)
{
    char c;
    int uart = irq - IRQ_UART0;

    printk("[int] uart %d\n",uart);
    
//    if(inw(uart_addr[uart]+UART_SR)&0x0004)
    while(inw(UART_REG(uart)+UART_SR)&0x0004)
    {
        c=(unsigned char)(inw(UART_REG(uart)+UART_DTRR)&0xFF);
        pipeWrite (&UART_PIPES[uart], &c, 1);
    }
}

int uart_in(unsigned char * data,int uartNum)
{
    if(inw(UART_REG(uartNum)+UART_SR)&0x0004) /* check if something is in the reception buffer */
    {
        *data=(unsigned char)(inw(UART_REG(uartNum)+UART_DTRR)&0xFF);
        return 1;
    }
    else
        return 0;
}

void uart_out(unsigned char data,int uartNum)
{
    while(!(inw(UART_REG(uartNum)+UART_SR)&0x400)) /* Nothing */; /* using transmission buffer level */
    outw(data,UART_REG(uartNum)+UART_DTRR);
}

void uart_outString(unsigned char * data,int uartNum)
{
    while(*data)
    {
        if (*data=='\n') uart_out('\r',uartNum); //gligli: uart fix
        uart_out(*data,uartNum);
        data++;
    }
}

void uart_restoreIrqHandler(int uartNum)
{
    irq_changeHandler(UART_IRQ_NUM(uartNum),uart_intAction);
}

void uart_need(int uart_num)
{
    arch_uart_need(uart_num);
}

void uart_changeSpeed(int speed,int uart_num)
{
    int brsr = ((float)uart_clockSpeed/(16*speed))-1+0.5;
    outw(brsr,UART_REG(uart_num)+UART_BRSR);
}

void uart_init(void)
{
    UART_0_Pipe=&UART_PIPES[0];
    UART_1_Pipe=&UART_PIPES[1];

    pipeInit(UART_0_Pipe);
    pipeInit(UART_1_Pipe);
    
    arch_uart_init();
}
