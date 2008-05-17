/*
*   kernel/gfx/osd.c
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
#include <kernel/io.h>

#include <driver/hardware.h>
#include <driver/osd.h>
#include <driver/lcd.h>


int osdLookupOffsetLO[4] = { OSD_SDRAM_OFF_VID0 ,
                             OSD_SDRAM_OFF_VID1 ,
							 OSD_SDRAM_OFF_BITMAP0 ,
							 OSD_SDRAM_OFF_BITMAP1 };
int osdLookupOffsetHI[4] = { OSD_SDRAM_OFF_HI_VID0_1 ,
                             OSD_SDRAM_OFF_HI_VID0_1 ,
							 OSD_SDRAM_OFF_HI_BITMAP0_1,
							 OSD_SDRAM_OFF_HI_BITMAP0_1 };

int OSD_OFF_HI_SHIFT[4] = {0,8,0,8};

int osd_palette[256][3];

int osd_RGB2Packed(int r, int g, int b)
{
	return  ((RGB2Y(r,g,b) << 24) | (RGB2Cr(r,g,b) << 16) | (RGB2Y(r,g,b) << 8) | RGB2Cb(r,g,b));
}

void osd_setCursor2Bitmap (int index, int data)
{
    int val;
    outw(data,OSD_CURSOR2_DATA);           /* Setup data reg */
    val=inw(OSD_CURSOR2_ADD_LATCH) & 0xFF;
    index=index<<8;
    outw(val | index | 0x80,OSD_CURSOR2_ADD_LATCH);       /* Set the data... */
}

void osd_setRectCursorColor(int color,int ram_palette)
{
    outw((inw(OSD_CURSOR0_CONF)&0xFF)|((color&0xff)<<8),OSD_CURSOR0_CONF);
    if(ram_palette)
        outw(inw(OSD_CURSOR0_CONF)|0x80,OSD_CURSOR0_CONF);
    else
        outw(inw(OSD_CURSOR0_CONF)&(0xFF7F),OSD_CURSOR0_CONF);
}

void osd_setRectCursorBorder(int horiz,int verti)
{
    outw((inw(OSD_CURSOR0_CONF)&0xFF81)|((verti&0x7)<<1)|((horiz&0x7)<<4),OSD_CURSOR0_CONF);
}

void osd_enableRectCursor(int state)
{
    if(state)
        outw(inw(OSD_CURSOR0_CONF)|0x1,OSD_CURSOR0_CONF);
    else
        outw((inw(OSD_CURSOR0_CONF)&0xFFFE),OSD_CURSOR0_CONF);
}

void osd_setBorderColor (int color)
{
	outw((inw(OSD_CONF) & 0xFF00) | color,OSD_CONF);
}

void osd_setMainConfig (int config)
{
	//outw(((inw(OSD_CONF) & 0xFF) << 8) | config,OSD_CONF);    
    outw(config&0xFFFF,OSD_CONF);    
}

void osd_setMainShift (int horizontal,int vertical)
{
	outw(horizontal,OSD_BITMAP0_SHIFT_HORIZ);
	outw(vertical,OSD_BITMAP0_SHIFT_VERT);
}

void osd_setEntirePalette(int palette[][3],int size,bool isRgb)
{
    int i;
    if(isRgb)
    {
        for(i=0;i<size;i++)
        {
            osd_setPaletteRGB(palette[i][0], palette[i][1], palette[i][2], i);
        }
    }
    else
    {
        for(i=0;i<size;i++)
        {
            osd_setPalette(palette[i][0], palette[i][1], palette[i][2], i);
        }
    }
}

void osd_setPaletteRGB(int r,int g,int b,int index)
{
    osd_setPalette(RGB2Y(r,g,b), RGB2Cr(r,g,b),RGB2Cb(r,g,b),index);
}

void osd_setPalette (int Y, int Cr, int Cb, int index)
{
	Y&=0xff;
    Cr&=0xff;
    Cb&=0xff;
    index&=0xff;

    osd_palette[index][0]=Y;
    osd_palette[index][1]=Cr;
    osd_palette[index][2]=Cb;

    //printk("OSD set palette (%x,%x,%x) at %d\n",Y,Cr,Cb,index);

	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;

    outw((Y << 8) | Cb,OSD_PAL_DATA_WRITE);

	while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;

    outw((Cr << 8) | index,OSD_PAL_INDEX_WRITE);

    while((inw(OSD_PAL_ACCESS_STATUS)&0x1) != 0) /* nothing */ ;
}

void osd_savePalette(int (*palette)[], int size)
{
    memcpy(palette,osd_palette,size*3*sizeof(int));
}

void osd_set16CPalette (int bankN, int index, int value)
{
	int val=inw(GET_BANK_ADDR(bankN,index));
	val &= 0xFF00 >> GET_BANK_SHIFT(index);
	val |= value << GET_BANK_SHIFT(index);
	outw(val,GET_BANK_ADDR(bankN,index));
}

void osd_setAltOffset (int address)
{
	int offset = address - SDRAM_START;
	offset = offset >> 5;

	outw(offset,OSD_ALT_VID_OFF_HI);
	outw(offset>>16,OSD_ALT_VID_OFF_LO);
}

void osd_setComponentOffset (int component, int address)
{
	int val;
	int offset = address - SDRAM_START;

	offset = offset >> 5;
	outw(offset,osdLookupOffsetLO[component]);

	offset = offset >> 16;
	val=inw(osdLookupOffsetHI[component]);
	val &= 0xFF00 >> OSD_OFF_HI_SHIFT[component];
	val |= offset << OSD_OFF_HI_SHIFT[component];

	outw(val,osdLookupOffsetHI[component]);
}

void osd_setComponentSize (int component, int width, int height)
{
	outw(width,OSD_COMP_W(component));
	outw(height,OSD_COMP_H(component));
}

void osd_setComponentPosition (int component, int x, int y)
{
	outw(x,OSD_COMP_X(component));
	outw(y,OSD_COMP_Y(component));
}

void osd_setComponentSourceWidth (int component, int width)
{
	outw(width,OSD_COMP_BUFF_W(component));
}

void osd_setComponentConfig (int component, int config)
{
	if(component == OSD_VIDEO1)
	{
		outw((inl(OSD_VID0_1_CONF) & 0xFF00) | config,OSD_VID0_1_CONF);
	}
	else if(component == OSD_VIDEO2)
	{
		outw((inl(OSD_VID0_1_CONF) & 0xFF) | (config<<8),OSD_VID0_1_CONF);
	}
	else
	{
		outw(config,OSD_COMP_CONF(component));
	}
}

void osd_restorePlane(int component, unsigned int address, int x, int y, int w, int h, int bw, int bpp, int state,int enable)
{
    int enbit=(enable)?OSD_COMPONENT_ENABLE_BIT(component):0;

    osd_setComponentOffset(component,address);
    osd_setComponentSize(component, 2*w, h);
    osd_setComponentPosition(component,x,y);
    osd_setComponentSourceWidth(component,(((bw*bpp)/32)/8));
    osd_setComponentConfig(component,state|enbit);
}

void osd_init()
{
    arch_osd_init();
}
