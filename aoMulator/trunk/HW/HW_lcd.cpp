/*
*   HW_lcd.cpp
*
*   AV3XX emulator
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/
#include "X11/keysym.h"

#include <stdlib.h>
#include <stdio.h>

#include <HW/HW_lcd.h>

#include <HW/mem_space.h>
#include <HW/HW_btn.h>

#include "pallette.h"

int lcd_update_cnt[2] = {0,0};

HW_lcd::HW_lcd(mem_space * mem,HW_OSD * osd)
{
#ifdef HAS_LCD
    int x,y;

    skip=0;
#ifdef USE_CACHE
    in_cache = 0;
    cache_size = 0;
#endif

    this->mem = mem;
    this->osd = osd;
    
    display = XOpenDisplay(0);  
    if(!display) 
    {
            printf("Error while connecting to X server");
            exit(1);
    }
                
    screen = DefaultScreen(display);
    gc = DefaultGC(display, screen);
    
    window1 = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            0,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );
    if(!window1 ) 
    {
            printf("Can't create BMAP0 windows");
            exit(1);
    }
    XStoreName(display, window1, "AV Emu LCD - BMAP1");
    
#ifdef HAS_VID0            
    window2 = XCreateSimpleWindow(
            display,                               /* Display */
            DefaultRootWindow(display),            /* Main Window */
            0, 0, SCREEN_WIDTH, SCREEN_HEIGHT,     /* Geometry */
            0,                                    /* Width border */
            BlackPixel(display, screen),	
            WhitePixel(display, screen)
            );            
    if(!window2) 
    {
            printf("Can't create VID0 windows");
            exit(1);
    }
   
    
    XStoreName(display, window2, "AV Emu LCD - VID1");
#endif    

    pal = DefaultColormap(display,screen);
    
    setFullPalette(ini_pal,256);
    
    /* initializing the tmp buffer and the screen */
    XSetForeground(display, gc, colorTab[0]);
    for(y=0; y<SCREEN_HEIGHT; y++)
        for(x=0; x<SCREEN_WIDTH; x++)
        {
            XDrawPoint(display, window1, gc, x, y);
        }            
    
    XSelectInput(display, window1, ExposureMask | KeyPressMask | KeyReleaseMask);
    XMapWindow(display, window1);
    
#ifdef HAS_VID0     
    XSelectInput(display, window2, ExposureMask);
    XMapWindow(display, window2);
#endif     

    printf("LCD init done\n");
#endif
}

void HW_lcd::setFullPalette(int palette[256][3],int size)
{
    int i,Y,Cb,Cr;

    for(i=0; i<size; i++)
    {
        Y = palette[i][0];
        Cb = palette[i][1];
        Cr = palette[i][2];

        setPaletteYCbCr(Y,Cb,Cr,i);
    }
}

int HW_lcd::setPaletteYCbCr(int y,int cb, int cr, int index)
{
    int r,g,b;

    r=(int)(y+1.402*(cr-128));
    g=(int)(y-0.34414*(cb-128)-0.71414*(cr-128));
    b=(int)(y+1.772*(cb-128));

    r=(r>255)?255:r;
    r=(r<0)?0:r;
    g=(g>255)?255:g;
    g=(g<0)?0:g;
    b=(b>255)?255:b;
    b=(b<0)?0:b;

    setPaletteRGB(r,g,b,index);
}

int HW_lcd::setPaletteRGB(int r,int g, int b, int index)
{
#ifdef HAS_LCD
    XColor c;

    c.red = r*0x100+r;
    c.green = g*0x100+g;
    c.blue = b*0x100+b;
    XAllocColor(display, pal, &c);
    colorTab[index] = c.pixel;
    return c.pixel;
#else
    return 0;
#endif
}

extern HW_btn * btn;

void HW_lcd::nxtStep(int * config,uint32_t * addr)
{   
#ifdef HAS_LCD
    ++skip;
    if(skip<10000) return ;
    skip=0;

    int pending;
    pending = XPending(display);
    KeySym keysym;
    char c = 0;
    int val=BTN_INIT_VAL;
    
  if(pending != 0)
  {
    
    /* next event */
    XNextEvent(display, &event);    
    int val=BTN_INIT_VAL;
    switch (event.type) 
    {
        case Expose :
            if(config[DISP_BMAP]&0x1 && addr[DISP_BMAP]>SDRAM_START)
                updte_lcd(addr[DISP_BMAP],LCD_BMAP);
#ifdef HAS_VID0
            if(config[1]&0x1 && addr[0]>SDRAM_START)
                updte_lcd(addr[0],LCD_VID);
#endif
            break;
       
        case KeyPress : 
            printf("keypress ");         
            XLookupString (&event.xkey, &c, 1, &keysym, 0);
            btn->processBtn(keysym);
            break;
    }
   }  
#endif
}

void HW_lcd::drawPix(uint32_t addr,uint32_t val)
{
    int i,j;
#ifndef DSC21
    int w=osd->OSD_width_regs[DISP_BMAP]*32;
    int z=(osd->OSD_config_regs[DISP_BMAP] & 0xc00)==0?1:0;

    addr >>= z;
    w >>= z;

    i=addr%w;
    j=addr/w;
#else    
    addr >>= (osd->OSD_config_regs[DISP_BMAP] & 0xc00)==0?1:0;
    i=(addr%SCREEN_WIDTH);
    j=addr/SCREEN_WIDTH;
#endif    
    XSetForeground(display, gc, colorTab[val&0xFF]);
    XDrawPoint(display, window1, gc, i, j);
}

void HW_lcd::drawVidPix(uint32_t addr,uint32_t val)
{
#ifdef HAS_VID0
    int i,j;
    int w=osd->OSD_width_regs[0]*8;

    addr >>= 2;
    i=addr%w;
    j=addr/w;
    XSetForeground(display, gc, getColor(val));
    XDrawPoint(display, window2, gc, i, j);
#endif
}



void HW_lcd::updte_lcd(uint32_t base_addr,int type)
{
#ifdef HAS_LCD
    int a=0;
    int b=0;
    char data[4];
    uint32_t color;
#ifndef DSC21
    int bz=(osd->OSD_config_regs[DISP_BMAP] & 0xc00)==0?1:0;
    int bw=osd->OSD_width_regs[DISP_BMAP]*32;
    int bh=osd->OSD_info_regs[DISP_BMAP].height;
    int vw=osd->OSD_width_regs[0]*8;
    int vh=osd->OSD_info_regs[0].height;

    bw >>= bz;
#endif
    lcd_update_cnt[type]++;
    if(type == LCD_BMAP)
    {
#ifndef DSC21
        for(int j = 0 ; j < bh ; j++)
            for(int i = 0 ; i < bw ; i++)
            {
                color = colorTab[mem->read(base_addr+(j*bw+i)*(bz+1),1)&0xFF];
                XSetForeground(display, gc, color);
                XDrawPoint(display, window1, gc, i, j);
            }
#else	    
	for(int j = 0 ; j < SCREEN_HEIGHT+1 ; j++)
            for(int i = 0 ; i < SCREEN_WIDTH+1 ; i++)        
            {
                color = colorTab[mem->read(base_addr+(j*(SCREEN_WIDTH)+i)*((osd->OSD_config_regs[DISP_BMAP] & 0xc00)==0?2:1),1)&0xFF];
                XSetForeground(display, gc, color);
                XDrawPoint(display, window1, gc, i, j);
            }
#endif
    }
#ifdef HAS_VID0
    else if(type == LCD_VID)
    {
        for(int j = 0 ; j < vh ; j++)
            for(int i = 0 ; i < vw ; i++)
            {
                color = getColor(mem->read(base_addr+(j*(vw*4)+i*4),4));
                XSetForeground(display, gc, color);
                XDrawPoint(display, window2, gc, i, j);
            }
            
    }
#endif
#endif
}

uint32_t HW_lcd::getColor(uint32_t color)
{
    XColor c;
    int r,g,b,Y,Cr,Cb;
    int i,j;
    
    
    
#ifdef USE_CACHE
    //printf("getColor: asked: %x (cache size=%x cache pos=%x) ",color,cache_size,in_cache);
    /*if(cache_size>PX_CACHE_SIZE)
    {
        printf("error in cache_size\n");
        exit(0);
    }*/
    // trying to find already computed value
    for(i=0;i<cache_size;i++)
        if(pixel_cache[i][0]==color)
        {
            //printf("-- find %x (cache size=%x cache pos=%x)\n",pixel_cache[i][1],cache_size,in_cache);
            return pixel_cache[i][1];            
        }
#endif    
    Cb=color&0xFF;
    Y=(color>>8)&0xFF;
    Cr=(color>>16)&0xFF;
    r=(int)(Y+1.402*(Cr-128));
    g=(int)(Y-0.34414*(Cb-128)-0.71414*(Cr-128));
    b=(int)(Y+1.772*(Cb-128));

    r=(r>255)?255:r;
    r=(r<0)?0:r;
    g=(g>255)?255:g;
    g=(g<0)?0:g;
    b=(b>255)?255:b;
    b=(b<0)?0:b;
    
    c.red = r*0x100+r;
    c.green = g*0x100+g;
    c.blue = b*0x100+b;
    XAllocColor(display, pal, &c);

#ifdef USE_CACHE
    pixel_cache[in_cache][0]=color;
    pixel_cache[in_cache][1]=c.pixel;
    in_cache++;
    if(cache_size<PX_CACHE_SIZE)
        cache_size++;
    if(in_cache>=PX_CACHE_SIZE)
        in_cache=0;
    //printf("-- find %x (cache size=%x cache pos=%x)\n",c.pixel,cache_size,in_cache);
#endif
    return c.pixel;
}
