/*
*   include/driver/uart.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __UART_H
#define __UART_H

#include <target/chip/uart.h>

#include <kernel/irq.h>

#include <sys_def/uart.h>

#define UART_DTRR     0x00000000
#define UART_BRSR     0x00000002
#define UART_MSR      0x00000004
#define UART_RFCR     0x00000006
#define UART_TFCR     0x00000008
#define UART_LCR      0x0000000a
#define UART_SR       0x0000000c

#define UART_REG(NUM) (NUM==UART_0?UART0_BASE:UART1_BASE)
#define UART_IRQ_NUM(NUM) (NUM==UART_0?IRQ_UART0:IRQ_UART1)



/* available in API */

int  uart_in         (unsigned char * data,int uartNum);
void uart_out        (unsigned char data,int uartNum);
void uart_outString  (unsigned char * data,int uartNum);
void uart_changeSpeed(int speed,int uart_num);


/* will have to see if we need to put this in API
maybe on device where UART is shared with video */
void uart_need       (int uart_num);

/*internal function */

void uart_init       (void);
void uart_intAction(int irq,struct pt_regs * regs);
void uart_restoreIrqHandler(int uartNum);
void arch_uart_need  (int uart_num);
void arch_uart_init  ();
int  arch_uart_getBrsr(int speed);
#endif
