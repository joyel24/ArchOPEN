/*
*   kernel/target/arch_AV3XX/timer.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <sys_def/stddef.h>

#include <kernel/io.h>
#include <kernel/irq.h>
#include <kernel/timer.h>
#include <kernel/kernel.h>

#include <driver/hardware.h>
#include <driver/clkc.h>

void arch_tmr_init(void)
{
    /*
    * System clock formula:
    *         freq = clock / (div * scale)
    *  freq need to be 100Hz
    */

    TMR_SET_SEL(TMR_SEL_ARM,TMR0);

    /* prescale  */

    TMR_SET_SCAL(9, TMR0);

    /* div  */
    TMR_SET_DIV(((CONFIG_ARM_CLK/1000)-1), TMR0);

    /* freerun */
    TMR_SET_MODE(TMR_MODE_FREERUN, TMR0);

    /* enable timer irq */
    irq_enable(IRQ_TMR_0);
}

inline unsigned int arch_tmr_getMicroTick(void){
    return tmr_getTick()*1000+TMR_GET_CNT(TMR0)/(CONFIG_ARM_CLK/1000000);
}
