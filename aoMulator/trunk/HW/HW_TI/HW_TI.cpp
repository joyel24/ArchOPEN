/*
*   HW_TI.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include <stdlib.h>
#include <stdio.h>

#include <emu.h>

#include <HW/HW_TI.h>
#include <target/chip/arch/HW_cpld.h>

#include <core/HW_mem.h>
#include <core/HW_access.h>

/* HW in TI chip */

#include <HW/HW_null.h>
#include <HW/HW_ata.h>

HW_TI::HW_TI(mem_space * mem,HW_cpld * hw_cpld):HW_node(TI_REG_START,TI_REG_END,4,CHIP_NAME)
{
    //exit_on_not_match = false;

    this->memSpace=mem;
    this->hw_cpld=hw_cpld;
    this->hw_ata=mem->hw_ata;

    add_item(new HW_clock());
    add_item(new HW_dsp());
    add_item(new HW_ECR());
    add_item(new HW_TI_ver());

    HW_irq = new HW_IRQ();
    add_item(HW_irq);

    for(int i=0;i<4;i++)
    {
        timer_list[i]=new HW_timer(i,this);
        add_item(timer_list[i]);
    }

    uart_list[0]=new HW_uart(0,"UART0");
    add_item(uart_list[0]);
    uart_list[1]=new HW_uart(1,"UART1");
    add_item(uart_list[1]);

    osd = new HW_OSD(mem);
    add_item(osd);

    gpio = new HW_gpio();
    add_item(gpio);

    hw_wdt=new HW_wdt(this);
    add_item(hw_wdt);

    hw_dma = new HW_dma(mem);
    add_item(hw_dma);
    
#ifdef CCD_START
    add_item(new HW_null(CCD_START,CCD_END,"CCD"));
#endif    
#ifdef PREVIEW_START
    add_item(new HW_null(PREVIEW_START,PREVIEW_END,"Preview"));
#endif    
#ifdef PAL_NTSC_ENC_START
    add_item(new HW_null(PAL_NTSC_ENC_START,PAL_NTSC_ENC_END,"NTSC_PAL Encoder"));
#endif
    chip_init();
    arch_init();


}

HW_TI::~HW_TI()
{

}

void HW_TI::chkMemAccess(uint32_t addr,uint32_t val,int size)
{
    osd->chk_access(addr,val,size);
}

void HW_TI::nxtStep(void)
{
    for(int i=0;i<4;i++)
        timer_list[i]->nxtStep();

#ifdef HAS_UART_XWIN
    uart_list[0]->nxtStep();
    uart_list[1]->nxtStep();
#endif

    osd->nxtStep();
}


