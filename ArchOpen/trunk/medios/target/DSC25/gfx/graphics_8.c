/*
*   kernel/gfx/graphics_8.c
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

#include <kernel/kernel.h>
#include <kernel/io.h>

#include <driver/cache.h>

#include <gfx/graphics.h>

void         graphics8_clearScreen       (unsigned int color, struct graphicsBuffer * buff);
void         graphics8_DrawPixel         (unsigned int color, int x, int y, struct graphicsBuffer * buff);
unsigned int graphics8_ReadPixel         (int x, int y, struct graphicsBuffer * buff);
void         graphics8_DrawRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics8_FillRect          (unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff);
void         graphics8_DrawChar          (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y,
                                            unsigned char c, struct graphicsBuffer * buff);
void         graphics8_DrawSprite        (unsigned int * palette, SPRITE * sprite, unsigned int trsp,int x, int y,
                                            struct graphicsBuffer * buff);
void         graphics8_DrawBITMAP        (BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff);
void         graphics8_DrawResizedBITMAP (BITMAP * bitmap, int x, int y, int xinc, int yinc , struct graphicsBuffer * buff);
void         graphics8_ScrollWindowVert  (unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP,
                                            struct graphicsBuffer * buff);
void         graphics8_ScrollWindowHoriz (unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT,
                                            struct graphicsBuffer * buff);
void         graphics8_DrawHLine         (unsigned int color, int x, int y, int width, struct graphicsBuffer * buff);
void         graphics8_DrawVLine         (unsigned int color, int x, int y, int height, struct graphicsBuffer * buff);
void         graphics8_DrawHorizLine (unsigned int color, int width,unsigned char * offset);
void         graphics8_DrawString    (struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y,
                                            unsigned char * s, int n, struct graphicsBuffer * buff);

struct graphics_operations g8ops =  {
    clearScreen       : graphics8_clearScreen,
    drawPixel         : graphics8_DrawPixel,
    readPixel         : graphics8_ReadPixel,
    drawRect          : graphics8_DrawRect,
    fillRect          : graphics8_FillRect,
    drawSprite        : graphics8_DrawSprite,
    drawChar          : graphics8_DrawChar,
    drawBITMAP        : graphics8_DrawBITMAP,
    drawResizedBITMAP : graphics8_DrawResizedBITMAP,
    drawString        : graphics8_DrawString,
    scrollWindowVert  : graphics8_ScrollWindowVert,
    scrollWindowHoriz : graphics8_ScrollWindowHoriz,
    drawHLine         : graphics8_DrawHLine,
    drawVLine         : graphics8_DrawVLine
};

void graphics8_clearScreen(unsigned int color, struct graphicsBuffer * buff)
{
    graphics8_FillRect(color,0,0,buff->width,buff->height,buff);
}


void graphics8_DrawPixel(unsigned int color, int x, int y, struct graphicsBuffer * buff)
{
    outb(color,getOffset(x,y,buff,unsigned char));
}

unsigned int graphics8_ReadPixel(int x, int y, struct graphicsBuffer * buff)
{
    unsigned int px;

    px=inb(getOffset(x,y,buff,unsigned char));

    return px;
}

void graphics8_DrawRect(unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int i;

    unsigned char * offset=getOffset(x,y,buff,unsigned char);
    
    graphics8_DrawHorizLine(color, width,offset);
            
    offset+=buff->width;
    
    for(i=1;i<height-1;i++)
    {
        outb(color,offset);
        outb(color,offset+width-1);        
        offset+=buff->width;
    }
    
    graphics8_DrawHorizLine(color, width,offset);
}

void graphics8_FillRect(unsigned int color, int x, int y, int width, int height, struct graphicsBuffer * buff)
{
    int j;

    unsigned char * offset=getOffset(x,y,buff,unsigned char);
    
    for(j=0;j<height;j++)
    {
        graphics8_DrawHorizLine(color, width, offset);    
        offset+=buff->width;
    }
    
}

void graphics8_DrawHLine(unsigned int color, int x, int y, int width, struct graphicsBuffer * buff)
{
    unsigned char * offset=getOffset(x,y,buff,unsigned char);
//    printk("draw Hline start at: %x\n",offset);
    graphics8_DrawHorizLine(color,width,offset);
}

void graphics8_DrawVLine(unsigned int color, int x, int y, int height, struct graphicsBuffer * buff)
{
    int i;
    unsigned char * offset=getOffset(x,y,buff,unsigned char);
  //  printk("draw Vline start at: %x\n",offset);
    for(i=0;i<height;i++)
    {
        outb(color,offset);
        offset+=buff->width;
    }
}

/* draw an horizontal line starting at (x,y) */ 
void graphics8_DrawHorizLine(unsigned int color, int width,unsigned char * offset)
{
    memset((void*)offset,(unsigned char)color,width);
}

void graphics8_DrawSprite(unsigned int * palette,SPRITE * sprite, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{    
    int i,j,index;
    unsigned char * dest=getOffset(x,y,buff,unsigned char);
    unsigned char * src=(unsigned char*)sprite->data;
    if(trsp != -1)
    {
        for(j=0;j<sprite->height;j++)
        {
            for(i=0;i<sprite->width;i++)
            {
                index=inb(src+i);
                if(palette[index]!=trsp)
                	outb(palette[index],dest+i);
            }
            dest+=buff->width;
            src+=sprite->width; 
        }
    }
    else
    {
        for(j=0;j<sprite->height;j++)
        {
            for(i=0;i<sprite->width;i++)
            {
                index=inb(src+i);
                outb(palette[index],dest+i);
            }
            dest+=buff->width;
            src+=sprite->width;
        }
    }
        
}

void graphics8_DrawBITMAP(BITMAP * bitmap, unsigned int trsp, int x, int y, struct graphicsBuffer * buff)
{
    int i,j,c;
    unsigned char * dest=getOffset(x,y,buff,unsigned char);
    unsigned char * src=(unsigned char*)bitmap->data;

    if(trsp != -1)
    {
        for(j=0;j<bitmap->height;j++)
        {
            for(i=0;i<bitmap->width;i++)
            {
                c=inb(src+i);
                if(c!=trsp)
                    outb(c,dest+i);
            }
            dest+=buff->width;
            src+=bitmap->width;
        }
    }
    else
    {
        for(j=0;j<bitmap->height;j++)
        {
            memcpy(dest,src,bitmap->width);

            dest+=buff->width;
            src+=bitmap->width;
        }


    }
}

void graphics8_DrawResizedBITMAP (BITMAP * bitmap, int x, int y, int xinc, int yinc , struct graphicsBuffer * buff){
    int i,j;
    unsigned char * baseDest=getOffset(x,y,buff,unsigned char);
    unsigned char * baseSrc=(unsigned char*)bitmap->data;
    unsigned char * dest=baseDest;
    unsigned char * src=baseSrc;

//    printk("x %d y %d xinc %0.8x %d yinc %0.8x %d\n",x,y,xinc,xinc>>16,yinc,yinc>>16);

    for(j=0;j<bitmap->height<<16;j+=yinc)
    {
        dest=baseDest;
        baseDest+=buff->width;
        for(i=0;i<bitmap->width<<16;i+=xinc)
        {
            *(dest++)=*(src+(i>>16));
        }
        src=baseSrc+bitmap->width*(j>>16);
    }
}

void graphics8_ScrollWindowVert(unsigned int bgColor, int x, int y, int width, int height, int scroll, int UP, struct graphicsBuffer * buff)
{
    int j,inc;

    unsigned char *src,*dest;

    if(scroll == 0)
        return;

    src=getOffset(x,y,buff,unsigned char);

    if(UP)
    {
        inc=1;
        dest=src;
        src=src+scroll*buff->width;
    }
    else
    {
        inc=-1;
        dest=src+height*buff->width;
        src=dest-scroll*buff->width;
    }

    for(j=0;j<(height-scroll);j++)
    {
        memcpy(dest,src,width);
        dest=dest+inc*buff->width;
        src=src+inc*buff->width;
    }

    if(bgColor!=-1)
    { // clear the freed zone
        for(j=0;j<scroll;j++)
        {
            graphics8_DrawHorizLine(bgColor,width,dest);
            dest+=inc*buff->width;
        }

    }
}

void graphics8_ScrollWindowHoriz(unsigned int bgColor, int x, int y, int width, int height, int scroll, int RIGHT, struct graphicsBuffer * buff)
{
    int j;
    unsigned char tmp[320];
    unsigned char *src,*dest,*offset;
    
    if(scroll == 0)
        return;
    
    if(bgColor!=-1)
    {
        if(RIGHT)
        {
            src=getOffset(x,y,buff,unsigned char);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
                memcpy(tmp,src,width-scroll);
                memcpy(dest,tmp,width-scroll);
                graphics8_DrawHorizLine(bgColor,scroll,offset);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,unsigned char);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                memcpy(dest,src,width-scroll);
                graphics8_DrawHorizLine(bgColor,scroll,offset);
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
    }
    else
    {
        if(RIGHT)
        {
            src=getOffset(x,y,buff,unsigned char);
            dest=src+scroll;
            offset=src;
            for(j=0;j<height;j++)
            {
                memcpy(tmp,src,width-scroll);
                memcpy(dest,tmp,width-scroll);                
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
        else
        {
            x-=scroll;        
            src=getOffset(x,y,buff,unsigned char);
            dest=src;
            offset=src+width-scroll;
            src=src+scroll;
            for(j=0;j<height;j++)
            {
                memcpy(dest,src,width-scroll);                
                dest+=buff->width;
                src+=buff->width;
                offset+=buff->width;
            }
        }
    }    
}

void graphics8_DrawChar(struct graphicsFont * font,unsigned int color,unsigned int bg_color, int x, int y, unsigned char c, struct graphicsBuffer * buff)
{
    int i,j,k,shift,index;
    
    unsigned char * src=font->table[(int)c];
    unsigned char * dest=getOffset(x,y,buff,unsigned char);
    
    unsigned int palette[2]={bg_color,color};
    
    if(src!=0) 
    {
    	for(j=0;j<font->height;j++)
        {
            shift=7;
            k=0;
            for(i=0;i<font->width;i++)
            {
                index=inb(src+k);
                index=(index>>shift)&0x1;
                outb(palette[index],dest+i);
                shift--;
                if(shift<0)
                {
                    shift=7;
                    k++;
                }
            }
                
            dest+=buff->width;
            src+=font->bpline;
        }
    }   
}

void graphics8_DrawString(struct graphicsFont * font, unsigned int color,unsigned int bg_color, int x, int y, unsigned char * s, int n, struct graphicsBuffer * buff)
{
#if 1
    int i,j,k,shift,index;
    unsigned char * src;
    unsigned char * org=getOffset(x,y,buff,unsigned char);
    unsigned char * dest;
    unsigned int palette[2]={bg_color,color};
    unsigned char * line_end=(getOffset(buff->width,y,buff,unsigned char))-font->width;

    if(n==-1) n=1000;
    
    while(*s && org<line_end && n)
    {
        src=font->table[(int)*s];
        dest=org;
        for(j=0;j<font->height;j++)
        {
            shift=7;
            k=0;
            for(i=0;i<font->width;i++)
            {
                index=inb(src+k);
                index=(index>>shift)&0x1;
                outb(palette[index],dest+i);
                shift--;
                if(shift<0)
                {
                    shift=7;
                    k++;
                }
            }

            dest+=buff->width;
            src+=font->bpline;
        }
        s++;
        org+=font->width;
        n--;
    }
#else
    int i,val,j,k;
    unsigned int color_table[16];
    char * src;
    char * str;
    unsigned char * dest;
    int len;
    unsigned char tmpLine[320+4];
    
    dest=getOffset(x,y,buff,unsigned char);
    len=strlen(s);
    
    for(i=0;i<0x10;i++)
    {
        color_table[i]  = (i&0x8?color:bg_color);
        color_table[i] |= (i&0x4?color:bg_color) << 8;
        color_table[i] |= (i&0x2?color:bg_color) << 16 ;
        color_table[i] |= (i&0x1?color:bg_color) << 24;
    }

    for(j=0;j<font->height;j++)
    {
        str=s;
        k=0;        
        while(*str)
        {
            src=font->table[(int)(*str)];
            if(src!=0)
            {
                for(i=0;i<font->bpline;i++)
                {
                    val=(int)(src[i+j*font->bpline]);
                    memcpy(&tmpLine[k],&color_table[(val>>4)&0xF],4);
                    k+=4;
                    memcpy(&tmpLine[k],&color_table[val&0xF],4);
                    k+=4;
                }
                if((font->width&0x3)!=0)
                    k=k-(4-(font->width&0x3));
            }
            else
            {
                /* to be done */
                /* add bg color to do blank*/
            }
            str++;
        }
        memcpy(dest,tmpLine,font->width*len);
        dest+=buff->width;        
    }
#endif    
}

