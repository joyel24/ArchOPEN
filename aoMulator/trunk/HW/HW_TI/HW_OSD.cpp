/*
*   HW_OSD.cpp
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

#include <HW/HW_OSD.h>
#include <HW/mem_space.h>
#include <HW/HW_TI.h>

#include <core/cmd_line.h>

HW_OSD * hw_OSD;

void init_mem_static_fct(HW_OSD * osd);

#define SDRAM2OSD(VAR)    ((VAR&(~SDRAM_START))


HW_OSD::HW_OSD(mem_space * mem):HW_access(OSD_START,OSD_END,"OSD")
{
    init_mem_static_fct(this);
    
    this->mem=mem;
    
    for(int i=0;i<6;i++)
    {
        OSD_config_regs[i]=0;
        
        OSD_info_regs[i].x=0;
        OSD_info_regs[i].y=0;
        OSD_info_regs[i].width=0;
        OSD_info_regs[i].height=0;
    }
    
    for(int i=0;i<4;i++)
    {  
        OSD_width_regs[i]=0;
        
        OSD_offset_regs[i]=SDRAM_START;
    }
    
    for(int i=0;i<8;i++)
    {
        OSD_pallette_bank1[i] = 0;
        OSD_pallette_bank2[i] = 0;
    }
    
    OSD_main_shift_horiz=0;
    OSD_main_shift_vert=0;
    OSD_cursor_data=0;
    OSD_cursor_offset=0;
    OSD_pallette_status=0;
    OSD_pallette_data_wr=0;
    OSD_pallette_index=0;
    OSD_alt_vid_offset=0;
    
    lcd = new HW_lcd(mem,this);
    vsync_timer=0;

}

#include "osd_cmd_line_fct.h"

void HW_OSD::nxtStep(void)
{

    vsync_timer++;
    if(vsync_timer>1000000){
        mem->hw_TI->HW_irq->write(0x30528,mem->hw_TI->HW_irq->read(0x30528,2)|0x80,2); // HACK: force irq enabled

        mem->hw_TI->HW_irq->do_INT(7);

        vsync_timer=0;
    }


#ifdef HAS_LCD
    return lcd->nxtStep(OSD_config_regs,OSD_offset_regs);
#endif
}

void HW_OSD::chk_access(uint32_t addr,uint32_t val,int size)
{
#ifdef HAS_LCD
#ifndef DSC21
    if((OSD_config_regs[DISP_BMAP]&0x1) && addr>=OSD_offset_regs[DISP_BMAP]
            && addr <= (OSD_offset_regs[DISP_BMAP]+OSD_width_regs[DISP_BMAP]*32*OSD_info_regs[DISP_BMAP].height))
#else
    if((OSD_config_regs[2]&0x1) && addr>=OSD_offset_regs[2]
            && addr <= (OSD_offset_regs[2]+SCREEN_WIDTH*SCREEN_HEIGHT*2))
#endif
    {        uint32_t v=val;
        uint32_t a=addr-OSD_offset_regs[DISP_BMAP];
        switch(size){ /* no break intended */
            case 4:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
                lcd->drawPix(a,v&0xff);++a;v>>=8;
            case 2:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
            case 1:
                lcd->drawPix(a,v&0xff);++a;v>>=8;
        }
    }

#ifdef HAS_VID0
    else if ((OSD_config_regs[1]&0x1) && addr>=OSD_offset_regs[0]
            && addr <= (OSD_offset_regs[0]+OSD_width_regs[0]*8*OSD_info_regs[0].height))
            lcd->drawVidPix(addr-OSD_offset_regs[0],val);
#endif
#endif
}

