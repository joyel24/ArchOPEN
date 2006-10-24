/*
*   HW_TI.h
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __HW_TI_H
#define __HW_TI_H

#include <emu.h>

#include <core/HW_node.h>
#include <core/HW_mem.h>

#include <HW/mem_space.h>
#include <target/chip/arch/HW_cpld.h>
#include <HW/HW_IRQ.h>
#include <HW/HW_OSD.h>
#include <HW/HW_ata.h>
#include <HW/HW_dma.h>
#include <HW/HW_timer.h>
#include <HW/HW_uart.h>
#include <HW/HW_wdt.h>
#include <HW/HW_gpio.h>
#include <HW/HW_clock.h>
#include <HW/HW_dsp.h>
#include <HW/HW_ECR.h>
#include <HW/HW_TI_ver.h>

class HW_TI:public HW_node {
    public:
        HW_TI(mem_space * memSpace,HW_cpld * hw_cpld);
        ~HW_TI();

        void nxtStep(void);
        void chkMemAccess(uint32_t addr,uint32_t val,int size);

        mem_space * memSpace;
        HW_OSD * osd;
        HW_dma * hw_dma;
        HW_ata * hw_ata;
        HW_timer * timer_list[4];
        HW_uart * uart_list[2];
        HW_wdt * hw_wdt;
        HW_gpio * gpio;
        HW_IRQ * HW_irq;
        HW_cpld * hw_cpld;

    private:
        void arch_init(void);
        void chip_init(void);
};

#endif
