/*
*   include/kernel/irqs.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __IRQ_DSC21_H
#define __IRQ_DSC21_H

#define NR_IRQS              28

#define INTC_BASE                         0x00030580

#define INTC_FIQ0_STATUS                  (INTC_BASE + 0x0000)
#define INTC_FIQ1_STATUS                  (0)
#define INTC_IRQ0_STATUS                  (INTC_BASE + 0x0002)
#define INTC_IRQ1_STATUS                  (INTC_BASE + 0x0004)

#define INTC_FIQ0_ENTRY                   (INTC_BASE + 0x0008)
#define INTC_FIQ1_ENTRY                   (INTC_BASE + 0x000A)
#define INTC_IRQ0_ENTRY                   (INTC_BASE + 0x000C)
#define INTC_IRQ1_ENTRY                   (INTC_BASE + 0x000E)

#define INTC_FISEL0                       (INTC_BASE + 0x0020)
#define INTC_FISEL1                       (0)
#define INTC_INT0_ENABLE                  (INTC_BASE + 0x0022)
#define INTC_INT1_ENABLE                  (INTC_BASE + 0x0024)

#define INTC_INTIDR                       (INTC_BASE + 0x0040)
#define INTC_INTRAW                       (INTC_BASE + 0x0042)

#define INTC_IRQ_STATUS(val)              ((val < 16) ? INTC_IRQ1_STATUS : INTC_IRQ0_STATUS)
#define INTC_IRQ_ENABLE(val)              ((val < 16) ? INTC_INT1_ENABLE : INTC_INT0_ENABLE)
#define INTC_FIQ_STATUS(val)              ((val < 16) ? INTC_FIQ0_STATUS : INTC_FIQ1_STATUS)
#define INTC_FIQ_ENABLE(val)              ((val < 16) ? INTC_INT0_ENABLE : INTC_INT1_ENABLE)

#define INTC_SHIFT(val)                   ((val < 16) ? val : val-16)
#define INTC_FIQ_SHIFT(val)               INTC_SHIFT(val)
#define INTC_IRQ_SHIFT(val)               INTC_SHIFT(val)

#define IRQ_TMR_0            0
#define IRQ_TMR_1            1
#define IRQ_TMR_2            2
#define IRQ_TMR_3            3

#define IRQ_DSP              11

#define IRQ_OSD              7

#define IRQ_UART0            13
#define IRQ_UART1            12

// badly named and/or should not be there
#define IRQ_IDE              15  // ide
#define IRQ_BTN_ON           18  // GIO0
#define IRQ_MAS_DATA         22  // GIO4
#define IRQ_IR               24  // GIO6

#endif
