/*
*   kernel/core/console.c
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
#include <sys_def/ctype.h>
#include <sys_def/colordef.h>

#include <kernel/console.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <driver/cache.h>
#include <driver/hardware.h>
#include <driver/osd.h>
#include <driver/lcd.h>

#include <gfx/graphics.h>
#include <gfx/kfont.h>

#define CON_RING_BUFFER(pos) (con_buffer[(pos)%CON_BUFFER_SIZE])
#define CON_RING_COLORBUFFER(pos) (con_colorBuffer[(pos)%CON_BUFFER_SIZE])

#define CON_LAST_LINE_Y() ((con_numLines-1)*CON_FONT->height+CON_MARGIN_Y)

fnt_needFont(CON_FONT);

extern struct graphics_operations g8ops;
extern int gui_pal[256][3];

char * con_gfxBuffer;

struct graphicsBuffer con_gfxStruct = {
    offset             : 0,
    state              : OSD_BMAP_1_CFG,
    enable             : 0,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 8,
    gops               : &g8ops
};

int con_paletteSave[256][3];

char con_buffer[CON_BUFFER_SIZE];
char con_colorBuffer[CON_BUFFER_SIZE];

int con_bufferStart;
int con_bufferEnd;
int con_screenEnd;
int con_lastUpdateScreenEnd;
int con_lastUpdateEndY;
bool con_screenVisible;

int con_numLines;
int con_numCols;

static int con_nextLineEnd(int pos,bool up){
  int i=pos;
  if(up){
    //we want the next NL, not the current
    if(i>=con_bufferStart && CON_RING_BUFFER(i)=='\n'){
      --i;
    }
    // find next line end
    while((i>=con_bufferStart) && (CON_RING_BUFFER(i)!='\n')){
      --i;
    }
    // line wrap
    while(pos-i>con_numCols){
      i+=con_numCols;
    }
  }else{
    //we want the next NL, not the current
    if(i<con_bufferEnd && CON_RING_BUFFER(i)=='\n'){
      ++i;
    }
    // find next line end
    while((i<con_bufferEnd) && (CON_RING_BUFFER(i)!='\n')){
      ++i;
    }
    // line wrap
    if(i-pos>con_numCols){
      i=pos+con_numCols;
    }
  }

  return i;
}

static void con_drawLine(int start,int end, int y){
  int i;
  int x=CON_MARGIN_X;

  // draw the chars
  for(i=start;i<=end;++i){
    if(start<con_bufferStart || CON_RING_BUFFER(i)=='\n') continue;

    g8ops.drawChar(CON_FONT,CON_RING_COLORBUFFER(i),CON_BGCOLOR,x,y,CON_RING_BUFFER(i),&con_gfxStruct);
    x+=CON_FONT->width;
  }

  //clear the remaining part of the line
  g8ops.fillRect(CON_BGCOLOR,x,y,SCREEN_WIDTH-x,CON_FONT->height,&con_gfxStruct);
}

static void con_drawScroll(int start,int end,int delta){

#ifdef DM320
  //HACK: for some reason, DM320 DCache doesn't like memcpy() scrolls
  //      so I disable it while scrolling for now. (see graphics_8_DSC25.c too)
  bool dcache=cache_enabled(CACHE_DATA);
  if (dcache) cache_enable(CACHE_DATA,false);
#endif

  // copy the block of pixels
  if (delta<=0){
    memcpy(&con_gfxBuffer[(start+delta)*SCREEN_WIDTH],&con_gfxBuffer[start*SCREEN_WIDTH],(end-start)*SCREEN_WIDTH);
  }else{
    int i;
    for (i=end;i>=start;--i){
      memcpy(&con_gfxBuffer[(i+delta)*SCREEN_WIDTH],&con_gfxBuffer[i*SCREEN_WIDTH],SCREEN_WIDTH);
    }
  }

#ifdef DM320
  // restore cache
  if (dcache) cache_enable(CACHE_DATA,true);
#endif

}

void con_screenUpdate(){
  int pos;
  int prevPos;
  int topPos;
  int prevTopPos;
  int y;

  if (!con_screenVisible) return;

  if(con_screenEnd!=con_lastUpdateScreenEnd){ // need update?
    pos=con_lastUpdateScreenEnd;

    if(con_screenEnd>con_lastUpdateScreenEnd){ // moved up or down?
      // find the buffer position corresponding to the screen top after the scroll
      // in the worst case ie: screen totally filled + CRLF on each line
      topPos=con_screenEnd-((con_numLines+2)*con_numCols);

      // if there's more than 1 screen to scroll, just scroll enough to redraw the screen
      if (con_lastUpdateEndY>=CON_LAST_LINE_Y() && pos<topPos){
        pos=topPos;
      }

      y=con_lastUpdateEndY;

      // if the prev line has not ended, redraw it
      if(con_screenEnd==con_bufferEnd && pos>=con_bufferStart && CON_RING_BUFFER(pos)!='\n'){
        pos=con_nextLineEnd(pos,true);
        y-=CON_FONT->height;
      }

      do{
        // go to next line, scroll if we're at the bottom of the screen
        if(y>=CON_LAST_LINE_Y()){
          con_drawScroll(CON_FONT->height+CON_MARGIN_Y,y+CON_FONT->height,-CON_FONT->height);
        }else{
          y+=CON_FONT->height;
        }

        prevPos=pos;
        pos=con_nextLineEnd(pos,false);

        con_drawLine(prevPos+1,pos,y);
      }while(pos<con_screenEnd);

      con_lastUpdateEndY=y;
    }else{
      // find the buffer position corresponding to the current screen top
      y=con_lastUpdateEndY;
      topPos=con_lastUpdateScreenEnd;
      do{
        prevTopPos=topPos;
        topPos=con_nextLineEnd(topPos,true);
        y-=CON_FONT->height;
      }while(y>=0 && topPos<prevTopPos);

      // scroll the screen up and draw lines
      do{
        pos=con_nextLineEnd(pos,true);
        prevTopPos=topPos;
        topPos=con_nextLineEnd(topPos,true);

        con_drawScroll(CON_MARGIN_Y,con_lastUpdateEndY-1,CON_FONT->height);
        con_drawLine(topPos+1,prevTopPos,CON_MARGIN_Y);
      }while(pos>con_screenEnd && topPos<prevTopPos);
    }
  }

  con_lastUpdateScreenEnd=con_screenEnd;
}

void con_screenSwitch(){
  con_screenVisible=!con_screenVisible;

  if (con_screenVisible){
    osd_setComponentConfig(OSD_VIDEO1,  0);
    osd_setComponentConfig(OSD_VIDEO2,  0);
    osd_setComponentConfig(OSD_BITMAP1, 0);
    osd_setComponentConfig(OSD_BITMAP2, 0);
    osd_setComponentConfig(OSD_CURSOR1, 0);
    osd_setComponentConfig(OSD_CURSOR2, 0);
    gfx_restoreComponent(BMAP1,&con_gfxStruct);
    
    // save palette & set medios palette
    osd_savePalette(con_paletteSave,256);
    osd_setEntirePalette(gui_pal,256,true);
  }else{
    // restore palette
    osd_setEntirePalette(con_paletteSave,256,false);

    gfx_restoreAllComponents();
  }

  con_screenUpdate();
}

bool con_screenIsVisible(void){
  return con_screenVisible;
}

void con_screenScroll(int lines){
 int i;

 // don't scroll if we don't have more than 1 screen of text
 if (con_lastUpdateEndY<CON_LAST_LINE_Y()) return;

 for(i=0;i<abs(lines);++i){
   con_screenEnd=con_nextLineEnd(con_screenEnd,lines<0);
 }

 con_screenUpdate();
}

void con_clear(){
  con_bufferStart=0;
  con_bufferEnd=-1;
  con_screenEnd=-1;
  con_lastUpdateScreenEnd=-1;
  con_lastUpdateEndY=-CON_FONT->height+CON_MARGIN_Y;

  //clear the whole screen
  g8ops.fillRect(CON_BGCOLOR,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,&con_gfxStruct);
}

void con_write(char * text, char color){
  int len;
  int free;
  char * p;

  //copy text to the buffer
  len=0;
  p=text;
  while(*p){
    con_bufferEnd++;
    CON_RING_BUFFER(con_bufferEnd)=*p;
    CON_RING_COLORBUFFER(con_bufferEnd)=color;
    p++;
    len++;
  };

  //we have overwritten history if we lacked space
  free=CON_BUFFER_SIZE-(con_bufferEnd+1-len)+con_bufferStart;
  if (len>free){
    con_bufferStart+=len-free;
  }

  // auto scroll if the screen shows the last line
  if (con_screenEnd==con_bufferEnd-len){
    con_screenEnd=con_bufferEnd;
  }

  con_screenUpdate();
}

void con_init(){
  // allocate framebuffer
  con_gfxBuffer=kmalloc(SCREEN_WIDTH*SCREEN_HEIGHT+64);
  con_gfxBuffer=(char*)(((unsigned int)con_gfxBuffer+32)&0xffffffe0);

  // init struct
  con_gfxStruct.offset=(unsigned int)con_gfxBuffer;
  con_gfxStruct.enable=true;

  con_numLines=(SCREEN_HEIGHT-CON_MARGIN_Y)/CON_FONT->height;
  con_numCols=(SCREEN_REAL_WIDTH-CON_MARGIN_X)/CON_FONT->width;

  con_screenVisible=false;
  con_clear();
}
