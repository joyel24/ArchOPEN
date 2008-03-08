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
#include <sys_def/evt.h>

#include <kernel/console.h>
#include <kernel/kernel.h>
#include <kernel/malloc.h>

#include <driver/cache.h>
#include <driver/hardware.h>
#include <driver/osd.h>
#include <driver/lcd.h>

#include <gfx/graphics.h>
#include <gfx/kfont.h>
#include <gfx/screens.h>

#include <fs/stdfs.h>

/*********************************************************************
* This code displays all debug string outputed by
* printk (called by medios) or printf (called by med applications)
* a ring buffer is used in order to keep a backlog
* the console can be displayed at boot time if BOOT_WITH_CONSOLE is defined
* by configure (used in init/main.c)
* from the shell, console is called by key combo: F1+ON
* key layout in console: ON: back to shell, F2: clear console, F3: flush to HDD
* UP/DOWN: move in the log
*********************************************************************/

/* Macro that returns a pointer to char in console buffer at pos*/
#define CON_RING_BUFFER(pos) (con_buffer[(pos)%CON_BUFFER_SIZE])
/* Macro that returns the color of the char at pos (8bit color=>in palette*/
#define CON_RING_COLORBUFFER(pos) (con_colorBuffer[(pos)%CON_BUFFER_SIZE])

#define CON_LAST_LINE_Y() ((con_numLines-1)*CON_FONT->height+CON_MARGIN_Y)

/**************************************
* grabbing some external data:
* - graphical operation structure for 8bit plane
* - graphical palette
***************************************/
extern struct graphics_operations g8ops;
extern int gui_pal[256][3];

/* NOTE: this is somehow deprecated */
fnt_needFont(CON_FONT);

/* BMAP buffer containing console screen */
char * con_gfxBuffer;

/* GFX data structure holding config of console BMAP plane */
struct graphicsBuffer con_gfxStruct = {
    offset             : 0,
    state              : OSD_BMAP_1_CFG,
    enable             : 1,
    width              : SCREEN_WIDTH,
    real_width         : SCREEN_REAL_WIDTH,
    height             : SCREEN_HEIGHT,
    x                  : SCREEN_ORIGIN_X,
    y                  : SCREEN_ORIGIN_Y,
    bitsPerPixel       : 8,
    gops               : &g8ops
};

/*
* array to store current palette: console used std medios palette, if
* current screen is using another palette, it will be saved in this array
* and restore when switching back to it => this is done auto by screens code
*/ 
int con_paletteSave[256][3];

/* Screen structure used by screens code */
struct screen_data console_screenData = {
    show:con_screenShow,
    palette:(int (*)[])con_paletteSave,
    hide:con_screenHide
};

/* txt buffer of the console */
char con_buffer[CON_BUFFER_SIZE];
/* color code of each char of con_buffer */
char con_colorBuffer[CON_BUFFER_SIZE];

/* start of history: when data buffer is overriden, this is the top of the displayable char*/
int con_bufferStart; 
int con_bufferEnd; /* last char of the data buffer */
int con_screenEnd; /* ????????? */
int con_lastUpdateScreenEnd; /* ????????????????? */
int con_lastUpdateEndY; /* ????????????????? */

bool con_screenVisible; /* true if console is currently displayed, false if another screen is displayed*/

int con_numLines; /* max number of lines to be displayed */
int con_numCols; /* max number of char per line that can be displayed*/

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

/**********************************************
* draw a whole line on screens
* position on screen is (CON_MARGIN_X,y)
* not drawing char before buffer_start
* draw chars between start and end (included)
*************************************************/
static void con_drawLine(int start,int end, int y)
{
  int i;
  int x=CON_MARGIN_X;

  // draw the chars
  for(i=start;i<=end;++i)
  {
    // NOTE: changed start<con_bufferStart by i<con_bufferStart
    if(start<con_bufferStart || CON_RING_BUFFER(i)=='\n') continue;

    g8ops.drawChar(CON_FONT,CON_RING_COLORBUFFER(i),CON_BGCOLOR,x,y,CON_RING_BUFFER(i),&con_gfxStruct);
    x+=CON_FONT->width;
  }

  //clear the remaining part of the line
  g8ops.fillRect(CON_BGCOLOR,x,y,LCD_WIDTH-x,CON_FONT->height,&con_gfxStruct);
}

/********************************************
* copy gfx data UP or DOWN
* start, end and delta are screen line number
********************************************/
static void con_drawScroll(int start,int end,int delta)
{
    int i;
#ifdef JBMM /* JBMM has a specific gfx code 1 pixel is expanded to 4*/    
    start*=2;
    end*=2;
    delta*=2;
#endif
    
    if (delta<=0)
    {
        memcpy(&con_gfxBuffer[(start+delta)*SCREEN_WIDTH],&con_gfxBuffer[start*SCREEN_WIDTH],(end-start)*SCREEN_WIDTH);
#ifdef DM320
        cache_invalidateRange(CACHE_DATA,&con_gfxBuffer[start*SCREEN_WIDTH],(end-start)*SCREEN_WIDTH);
#endif
    }
    else
    {
        for (i=end;i>=start;--i)
        {
            memcpy(&con_gfxBuffer[(i+delta)*SCREEN_WIDTH],&con_gfxBuffer[i*SCREEN_WIDTH],SCREEN_WIDTH);
#ifdef DM320
            cache_invalidateRange(CACHE_DATA,&con_gfxBuffer[i*SCREEN_WIDTH],SCREEN_WIDTH);
#endif
        }
    }
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

void con_screenHide(void)
{
    con_screenVisible=false;
}

void con_screenShow(void)
{
    osd_setComponentConfig(OSD_VIDEO1,  0);
    osd_setComponentConfig(OSD_VIDEO2,  0);
    osd_setComponentConfig(OSD_BITMAP1, 0);
    osd_setComponentConfig(OSD_BITMAP2, 0);
    osd_setComponentConfig(OSD_CURSOR1, 0);
    osd_setComponentConfig(OSD_CURSOR2, 0);
    gfx_restoreComponent(BMAP1,&con_gfxStruct);
    con_screenVisible=true;
    con_screenUpdate();
}

void con_screenSwitch()
{
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

void con_flushToDisk(void)
{
   int fd=open("/dbgFlush.log",O_WRONLY|O_CREAT|O_TRUNC);
   int size;
   size=con_bufferEnd<CON_BUFFER_SIZE?con_bufferEnd:CON_BUFFER_SIZE;
   if (fd<0){
       printk("[FlushLog] can't open file /dbgFlush.log !\n");
       return;
   }
   size=write(fd,con_buffer,size);
   printk("[FlushLog] wrote %d bytes\n",size);
   close(fd);
}

int con_handleBtn(int btn)
{
    switch(btn+1)
    {
        case BTN_ON:
            con_screenVisible=false;            
            screens_show(screens_main());
            break;
        case BTN_F2: /* not using OFF in ordre to HALT in console */
            con_clear();
            break;
        case BTN_UP:
            con_screenScroll(-1);
            break;
        case BTN_DOWN:
            con_screenScroll(1);
            break;
        case BTN_F3:
            con_flushToDisk();
            break;
        default:
            /* other keys : post the event */
            return 0;
    }
    return 1;
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
  // NOTE: LCD_WIDTH g8ops.fillRect(CON_BGCOLOR,0,0,SCREEN_WIDTH,SCREEN_HEIGHT,&con_gfxStruct);
  g8ops.fillRect(CON_BGCOLOR,0,0,LCD_WIDTH,LCD_HEIGHT,&con_gfxStruct);
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
  
  osd_setEntirePalette(gui_pal,256,true);
  osd_savePalette(con_paletteSave,256);
  
  //NOTE: LCD_HEIGHT con_numLines=(SCREEN_HEIGHT-CON_MARGIN_Y)/CON_FONT->height;
  con_numLines=(LCD_HEIGHT-CON_MARGIN_Y)/CON_FONT->height;
  //NOTE: LCD_WIDTH con_numCols=(SCREEN_REAL_WIDTH-CON_MARGIN_X)/CON_FONT->width;
  con_numCols=(LCD_WIDTH-CON_MARGIN_X)/CON_FONT->width;

  con_screenVisible=false;
  con_clear();
  
  screens_add(&console_screenData,SCREEN_CONSOLE);
}
