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

#define SDRAM2OSD(VAR)    ((VAR&(~SDRAM_START))

uint32_t HW_OSD::read(uint32_t addr,int size)
{
    uint32_t ret_val=0;
    switch(addr)
    {
        case OSD_START+0x0:
            ret_val=OSD_config_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Main config",ret_val,size);            
            break;
        case OSD_START+0x2:
            ret_val=OSD_config_regs[1];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0",ret_val,size);            
            break;
        case OSD_START+0x38:
            ret_val=OSD_config_regs[2];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 config",ret_val,size);            
            break;
        case OSD_START+0x3a:
            ret_val=OSD_config_regs[3];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 config",ret_val,size);            
            break;
        case OSD_START+0x36:
            ret_val=OSD_config_regs[4];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 config",ret_val,size);            
            break;
        
            
        case OSD_START+0x4:
            ret_val=OSD_width_regs[0];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid0 buffer width",ret_val,size);            
            break;
                   
        case OSD_START+0x6:
            ret_val=(((OSD_offset_regs[0]-SDRAM_START)>>21)&0xFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid=%x)\n",name,"Vid buffer HI offset",
                ret_val,size,OSD_offset_regs[0]);            
            break;        
        case OSD_START+0x8:
            ret_val=((OSD_offset_regs[0]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Vid=%x)\n",name,"Vid buffer LO offset",
                ret_val,size,OSD_offset_regs[0]);            
            break;
	case OSD_START+0xa:
            ret_val=(((OSD_offset_regs[1]-SDRAM_START)>>21)&0xFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Cursor=%x)\n",name,"Cursor buffer HI offset",
                ret_val,size,OSD_offset_regs[1]);            
            break;        
        case OSD_START+0xc:
            ret_val=((OSD_offset_regs[1]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Cursor=%x)\n",name,"Cursor buffer LO offset",
                ret_val,size,OSD_offset_regs[1]);            
            break;
	case OSD_START+0xe:
            ret_val=(((OSD_offset_regs[2]-SDRAM_START)>>21)&0xFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap0=%x)\n",name,"Bmap0 buffer HI offset",
                ret_val,size,OSD_offset_regs[2]);            
            break;        
        case OSD_START+0x10:
            ret_val=((OSD_offset_regs[2]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                ret_val,size,OSD_offset_regs[2]);            
            break;    
	case OSD_START+0x12:
            ret_val=(((OSD_offset_regs[3]-SDRAM_START)>>21)&0xFFFF);
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap1=%x)\n",name,"Bmap1 buffer HI offset",
                ret_val,size,OSD_offset_regs[3]);            
            break;        
        case OSD_START+0x14:
            ret_val=((OSD_offset_regs[3]-SDRAM_START)>>5)&0xFFFF;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x) (Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                ret_val,size,OSD_offset_regs[3]);            
            break;
            
        case OSD_START+0x16:
            ret_val=OSD_main_shift_horiz;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift HORIZ",ret_val,size);            
            break;
        case OSD_START+0x18:
            ret_val=OSD_main_shift_vert;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"main shift VERT",ret_val,size);            
            break;
            
        case OSD_START+0x1a:
            ret_val=OSD_info_regs[0].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid X",ret_val,size);            
            break;
        case OSD_START+0x1c:
            ret_val=OSD_info_regs[0].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Vid Y",ret_val,size);            
            break;
	    
        
        case OSD_START+0x26:
            ret_val=OSD_info_regs[2].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 X",ret_val,size);            
            break;
        case OSD_START+0x28:
            ret_val=OSD_info_regs[2].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 Y",ret_val,size);            
            break;
        case OSD_START+0x2a:
            ret_val=OSD_info_regs[2].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 width",ret_val,size);            
            break;
        case OSD_START+0x2c:
            ret_val=OSD_info_regs[2].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap0 height",ret_val,size);            
            break;
        case OSD_START+0x2e:
            ret_val=OSD_info_regs[3].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 X",ret_val,size);            
            break;
        case OSD_START+0x30:
            ret_val=OSD_info_regs[3].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 Y",ret_val,size);            
            break;
        case OSD_START+0x32:
            ret_val=OSD_info_regs[3].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 width",ret_val,size);            
            break;
        case OSD_START+0x34:
            ret_val=OSD_info_regs[3].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Bmap1 height",ret_val,size);            
            break;
        case OSD_START+0x1e:
            ret_val=OSD_info_regs[4].x;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 X",ret_val,size);            
            break;
        case OSD_START+0x20:
            ret_val=OSD_info_regs[4].y;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 Y",ret_val,size);            
            break;
        case OSD_START+0x22:
            ret_val=OSD_info_regs[4].width;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 width",ret_val,size);            
            break;
        case OSD_START+0x24:
            ret_val=OSD_info_regs[4].height;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s read => %x (size %x)\n",name,"Cursor0 height",ret_val,size);            
            break;
        
            
        case OSD_START+0x3c:
        case OSD_START+0x3e:
        case OSD_START+0x40:
        case OSD_START+0x42:
        case OSD_START+0x44:
        case OSD_START+0x46:
        case OSD_START+0x48:
        case OSD_START+0x4a:
        {
            int index=(addr-OSD_START+0x50)/2;
            ret_val=OSD_pallette_bank1[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank1",index,ret_val,size);            
            break;
        }
        case OSD_START+0x4c:
        case OSD_START+0x4e:
        case OSD_START+0x50:
        case OSD_START+0x52:
        case OSD_START+0x54:
        case OSD_START+0x56:
        case OSD_START+0x58:
        case OSD_START+0x5a:
        {
            int index=(addr-OSD_START+0x60)/2;
            ret_val=OSD_pallette_bank2[index];
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d read => %x (size %x)\n",name,"Palette Bank2",index,ret_val,size);            
            break;
        }
    }
    return ret_val;
}

void HW_OSD::write(uint32_t addr,uint32_t val,int size)
{
    switch(addr)
    {
        case OSD_START+0x0:
            OSD_config_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Main config",val,size);            
            break;
        case OSD_START+0x2:
            OSD_config_regs[1]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid",val,size);            
            break;
        case OSD_START+0x38:
            OSD_config_regs[2]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 config",val,size);            
            break;
        case OSD_START+0x3a:
            OSD_config_regs[3]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 config",val,size);            
            break;
        case OSD_START+0x36:
            OSD_config_regs[4]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 config",val,size);            
            break;
            
        case OSD_START+0x4:
            OSD_width_regs[0]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 buffer width",val,size);            
            break;
                    
        case OSD_START+0x6:
            OSD_offset_regs[0] = (((val&0x3F)<<21) | ((OSD_offset_regs[0]-SDRAM_START) & 0x1FFFFF))+SDRAM_START; 
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid=%x)\n",name,"Vid buffer HI offset",
                val,size,OSD_offset_regs[0]);            
            break;        
        case OSD_START+0x8:
            OSD_offset_regs[0] = (((OSD_offset_regs[0]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Vid=%x)\n",name,"Vid buffer LO offset",
                val,size,OSD_offset_regs[0]);            
            break;        
        
	 case OSD_START+0xa:
            OSD_offset_regs[1] = (((val&0x3F)<<21) | ((OSD_offset_regs[1]-SDRAM_START) & 0x1FFFFF))+SDRAM_START; 
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Cursor=%x)\n",name,"Cursor buffer HI offset",
                val,size,OSD_offset_regs[1]);            
            break;        
        case OSD_START+0xc:
            OSD_offset_regs[1] = (((OSD_offset_regs[1]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Cursor=%x)\n",name,"Cursor buffer LO offset",
                val,size,OSD_offset_regs[1]);            
            break;     
	 
	 case OSD_START+0xe:
            OSD_offset_regs[2] = (((val&0x3F)<<21) | ((OSD_offset_regs[2]-SDRAM_START) & 0x1FFFFF))+SDRAM_START; 
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap0=%x)\n",name,"Bmap0 buffer HI offset",
                val,size,OSD_offset_regs[2]);            
            break;        
        case OSD_START+0x10:
            OSD_offset_regs[2] = (((OSD_offset_regs[2]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap0=%x)\n",name,"Bmap0 buffer LO offset",
                val,size,OSD_offset_regs[2]);            
            break;       
        
	 case OSD_START+0x12:
            OSD_offset_regs[3] = (((val&0x3F)<<21) | ((OSD_offset_regs[3]-SDRAM_START) & 0x1FFFFF))+SDRAM_START; 
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap1=%x)\n",name,"Bmap1 buffer HI offset",
                val,size,OSD_offset_regs[3]);            
            break;        
        case OSD_START+0x14:
            OSD_offset_regs[3] = (((OSD_offset_regs[3]-SDRAM_START)&0x03E00000) | ((val<<5)&0x1FFFFF))+SDRAM_START;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x) (Bmap1=%x)\n",name,"Bmap1 buffer LO offset",
                val,size,OSD_offset_regs[3]);            
            break;
            
        case OSD_START+0x16:
            OSD_main_shift_horiz=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift HORIZ",val,size);            
            break;
        case OSD_START+0x18:
            OSD_main_shift_vert=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"main shift VERT",val,size);            
            break;
            
        case OSD_START+0x1a:
            OSD_info_regs[0].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 X",val,size);            
            break;
        case OSD_START+0x1c:
            OSD_info_regs[0].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Vid0 Y",val,size);            
            break;
        
        case OSD_START+0x26:
            OSD_info_regs[2].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 X",val,size);            
            break;
        case OSD_START+0x28:
            OSD_info_regs[2].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 Y",val,size);            
            break;
        case OSD_START+0x2a:
            OSD_info_regs[2].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 width",val,size);            
            break;
        case OSD_START+0x2c:
            OSD_info_regs[2].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap0 height",val,size);            
            break;
        case OSD_START+0x2e:
            OSD_info_regs[3].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 X",val,size);            
            break;
        case OSD_START+0x30:
            OSD_info_regs[3].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 Y",val,size);            
            break;
        case OSD_START+0x32:
            OSD_info_regs[3].width=val;
	    DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 width",val,size);            
            break;
        case OSD_START+0x34:
            OSD_info_regs[3].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Bmap1 height",val,size);
            break;
        case OSD_START+0x1e:
            OSD_info_regs[4].x=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 X",val,size);            
            break;
        case OSD_START+0x20:
            OSD_info_regs[4].y=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 Y",val,size);            
            break;
        case OSD_START+0x22:
            OSD_info_regs[4].width=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 width",val,size);            
            break;
        case OSD_START+0x24:
            OSD_info_regs[4].height=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s write %x (size %x)\n",name,"Cursor0 height",val,size);            
            break;
        
        case OSD_START+0x3c:
        case OSD_START+0x3e:
        case OSD_START+0x40:
        case OSD_START+0x42:
        case OSD_START+0x44:
        case OSD_START+0x46:
        case OSD_START+0x48:
        case OSD_START+0x4a:
        {
            int index=(addr-OSD_START+0x50)/2;
            OSD_pallette_bank1[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank1",index,val,size);            
            break;
        }
        case OSD_START+0x4c:
        case OSD_START+0x4e:
        case OSD_START+0x50:
        case OSD_START+0x52:
        case OSD_START+0x54:
        case OSD_START+0x56:
        case OSD_START+0x58:
        case OSD_START+0x5a:
        {
            int index=(addr-OSD_START+0x60)/2;
            OSD_pallette_bank2[index]=val;
            DEBUG_HW(OSD_HW_DEBUG,"%s %s %d write %x (size %x)\n",name,"Palette Bank2",index,val,size);            
            break;
        }
            
        
    }

}

