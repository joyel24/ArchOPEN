/*
* kernel/gfx/gui/widget/virtKbd.c
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
#include <sys_def/colordef.h>
#include <sys_def/stdfs.h>

#include <kernel/kernel.h>
#include <kernel/evt.h>

#include <gfx/graphics.h>
#include <gfx/kfont.h>

#include <driver/lcd.h>
#include <driver/hardware.h>

#define CELL_SPACE        3
#define CELL_DIST         1
#define NB_CELL_PER_LINE  3
#define TOT_WIDTH         SCREEN_WIDTH
#define TOT_HEIGHT        SCREEN_HEIGHT
#define BG_COLOR          COLOR_WHITE
#define TXT_COLOR         COLOR_BLUE
#define SEL_COLOR         COLOR_BLACK
#define BLK_COLOR         COLOR_DARK_GRAY
#define TXT_BG_COLOR      COLOR_LIGHT_YELLOW

PALETTE pal[2] = {BG_COLOR, TXT_COLOR};

void leftCursor(void);
void rightCursor(void);
void delCursor(void);
void shiftCursor(void);
void majCursor(void);
void numPage(void);

#include "virtKbd_inc.h"

int bg_txt_colors[3] = 
{
    COLOR_LIGHT_RED,
    COLOR_LIGHT_YELLOW,
    COLOR_LIGHT_BLUE
};

int coord_horiz[6][2];
int coord_vert[6][2];

int mode=1;
int elem=2;
int cur_index=0;
int str_len=0;
int modify_str=1;
char cur_str[MAX_PATH];
int txt_x,txt_y,txt_w,txt_max_char;
int page,shift;
int paramVirtKbd=0;

void calcCoord(int txt_on_top)
{
    int h=0,w=0,x,y;
    gfx_getStringSize("M", &w, &h);
    /* horizontal lines coordinates */
    x=(TOT_WIDTH-(NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST))/2; /* coord X is the same for all Horzi */
    y=(TOT_HEIGHT-(NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST))/2; /* coord Y is the same for all Verti */
    coord_horiz[0][0]=coord_horiz[1][0]=coord_horiz[2][0]=coord_horiz[3][0]=coord_horiz[4][0]=coord_horiz[5][0]=x;
    
    if(txt_on_top)
        coord_horiz[0][1]=CELL_DIST+2+h+4*CELL_DIST+CELL_SPACE;
    else
    coord_horiz[0][1]=CELL_DIST+1;

    coord_horiz[1][1]=coord_horiz[0][1]+h+CELL_SPACE;
    coord_horiz[2][1]=coord_horiz[1][1]+h+CELL_SPACE;
    
    /* starting Y from bottom */
    coord_horiz[5][1]=TOT_HEIGHT-1-CELL_DIST-h;
    coord_horiz[4][1]=coord_horiz[5][1]-CELL_SPACE-h;
    coord_horiz[3][1]=coord_horiz[4][1]-CELL_SPACE-h;
    
    if(txt_on_top && (coord_horiz[2][1]+h+CELL_SPACE)>coord_horiz[3][1])
    {
        coord_horiz[2][1]=coord_horiz[3][1]-CELL_SPACE-h;
        coord_horiz[1][1]=coord_horiz[2][1]-CELL_SPACE-h;
        coord_horiz[0][1]=coord_horiz[1][1]-CELL_SPACE-h;
    }
      
    /* vertical lines coordinates */
    coord_vert[0][0]=CELL_DIST+1;
    coord_vert[1][0]=coord_vert[0][0]+w+CELL_SPACE;
    coord_vert[2][0]=coord_vert[1][0]+w+CELL_SPACE;
    
    coord_vert[5][0]=TOT_WIDTH-1-CELL_DIST-CELL_SPACE-w;
    coord_vert[4][0]=coord_vert[5][0]-CELL_SPACE-w;
    coord_vert[3][0]=coord_vert[4][0]-CELL_SPACE-w;
     
    coord_vert[0][1]=coord_vert[1][1]=coord_vert[2][1]=coord_vert[3][1]=coord_vert[4][1]=coord_vert[5][1]=y;
    
    if(txt_on_top)
    {
        txt_x=2*CELL_DIST+1;
        txt_y=2*CELL_DIST+1;
        txt_w=TOT_WIDTH-4*CELL_DIST-2;
    }
    else
    {    
        txt_x=coord_vert[2][0]+w+CELL_SPACE+2*CELL_DIST;
        txt_y=(TOT_HEIGHT-h)/2;
        txt_w=coord_vert[3][0]-txt_x-2*CELL_DIST;
    }
    
    txt_max_char=txt_w/w;
}

void drawHoriz(int pos)
{
    int h=0,w=0,i;
    gfx_getStringSize("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
    {
        if(char_horiz[page][pos][i] || spec_horiz[page][pos][i])
            gfx_fillRect(bg_txt_colors[i],coord_horiz[pos][0]+i*(w+CELL_SPACE),
                         coord_horiz[pos][1],w+2*CELL_DIST,h+2*CELL_DIST);
        if(char_horiz[page][pos][i])
            gfx_putC(TXT_COLOR,bg_txt_colors[i],coord_horiz[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                     coord_horiz[pos][1]+CELL_DIST,char_horiz[page][pos][i]);
        else
            if(spec_horiz[page][pos][i])
            {
                pal[0]=bg_txt_colors[i];
                gfx_drawSprite ((PALETTE*)&pal, spec_horiz[page][pos][i], coord_horiz[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                        coord_horiz[pos][1]+CELL_DIST);
            }
    }
}

void drawVert(int pos)
{
    int h=0,w=0,i;
    gfx_getStringSize("M", &w, &h);
    for(i=0;i<NB_CELL_PER_LINE;i++)
    {
        if(char_vert[page][pos][i] || spec_vert[page][pos][i])
            gfx_fillRect(bg_txt_colors[i],coord_vert[pos][0],coord_vert[pos][1]+i*(h+CELL_SPACE),w+2*CELL_DIST,h+2*CELL_DIST);
        if(char_vert[page][pos][i])
            gfx_putC(TXT_COLOR,bg_txt_colors[i],coord_vert[pos][0]+CELL_DIST,
                     coord_vert[pos][1]+i*(h+CELL_SPACE)+CELL_DIST,char_vert[page][pos][i]);
        else
            if(spec_vert[page][pos][i])
            {
                pal[0]=bg_txt_colors[i];
                gfx_drawSprite ((PALETTE*)&pal, spec_vert[page][pos][i],coord_vert[pos][0]+i*(w+CELL_SPACE)+CELL_DIST,
                        coord_vert[pos][1]+CELL_DIST);
            }
    }
}

void drawTxtZone(void)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    gfx_fillRect(TXT_BG_COLOR,txt_x-2*CELL_DIST,txt_y-2*CELL_DIST,txt_w+4*CELL_DIST,h+4*CELL_DIST+2);
    gfx_drawRect(BLK_COLOR,txt_x-2*CELL_DIST,txt_y-2*CELL_DIST,txt_w+4*CELL_DIST,h+4*CELL_DIST+2);
    /* +2 => space for the cursor*/
}

void drawBtn(void)
{    
    int x,y;
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    x=coord_horiz[0][0]+NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST;
    y=coord_vert[0][1]+NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST;
    x=x+(TOT_WIDTH-x)/2;
    y=y+(TOT_HEIGHT-y)/2;
    gfx_getStringSize("F3", &w, &h);
    gfx_drawRect(BLK_COLOR,x-w-CELL_SPACE-2*CELL_DIST,y-h-CELL_SPACE-2*CELL_DIST,
            3*(w+CELL_SPACE)-CELL_SPACE+4*CELL_DIST,2*(h+CELL_SPACE)-CELL_SPACE+4*CELL_DIST);
    gfx_putS(TXT_COLOR,bg_txt_colors[0],x-w-CELL_SPACE,y,"F1");
    gfx_putS(TXT_COLOR,bg_txt_colors[1],x,y-h-CELL_SPACE,"F2");
    gfx_putS(TXT_COLOR,bg_txt_colors[2],x+w+CELL_SPACE,y,"F3"); 
}

void drawPage(void)
{
    int i;
    gfx_clearScreen(BG_COLOR);
    for(i=0;i<6;i++)
    {
        drawVert(i);
        drawHoriz(i);
    }
}

void gfx_putSelect(int color)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    //printk("[putCursor] %s %s %d\n",color==COLOR_WHITE?"WHITE":"BLUE",mode==1?"HORIZ":"VERT",elem);
    if(mode) // horizontal
    {
        gfx_drawRect(color,coord_horiz[elem][0],coord_horiz[elem][1],NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST,
                        h+2*CELL_DIST);
    }
    else    // vertical
    {
        gfx_drawRect(color,coord_vert[elem][0],coord_vert[elem][1],w+2*CELL_DIST,
                        NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST);
    }
}

void putCursor(int color)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    gfx_drawLine(color,txt_x+cur_index*w,txt_y+h+1,txt_x+cur_index*w+w,txt_y+h+1);
}

void leftCursor(void)
{
    if(cur_index>0)
    {
        putCursor(BG_COLOR);
        cur_index--;
        putCursor(SEL_COLOR);
    }
}

void delCursor(void)
{
    printk("cur %d len %d\n",cur_index,str_len);
    if(cur_index==str_len)
        return;
    cur_str[str_len]='\0';
    strcpy(&cur_str[cur_index],&cur_str[cur_index+1]);
    drawTxtZone();
    gfx_putS(TXT_COLOR,TXT_BG_COLOR,txt_x,txt_y,cur_str);
    putCursor(SEL_COLOR);
}

void shiftCursor(void)
{
    if(page==0)        
    {
        majCursor();
        shift=2;
    }
    else
        majCursor();
}

void majCursor(void)
{
    if(page==2)
        page=1;
    else
        page=page==0?1:0;
    shift=0;
    drawPage();
    drawBtn();
    drawTxtZone();
    cur_str[str_len]='\0';
    gfx_putS(TXT_COLOR,TXT_BG_COLOR,txt_x,txt_y,cur_str);    
    gfx_putSelect(SEL_COLOR);  
    putCursor(SEL_COLOR);
}

void numPage(void)
{
    page=page==2?0:2;
    shift=0;
    drawPage();
    drawBtn();
    drawTxtZone();
    cur_str[str_len]='\0';
    gfx_putS(TXT_COLOR,TXT_BG_COLOR,txt_x,txt_y,cur_str);    
    gfx_putSelect(SEL_COLOR);  
    putCursor(SEL_COLOR);
}

void rightCursor(void)
{
    if(cur_index<str_len)
    {
        putCursor(BG_COLOR);
        cur_index++;
        putCursor(SEL_COLOR);
    }
}

void virtKbdEvtHandler(int evt_hanlder)
{
    int h=0,w=0;
    int stopLoop=0;
    int char_num=0;
    char str;
    void (*routine)(void);
    gfx_getStringSize("M", &w, &h);
    char evt=0;
    
    modify_str=1;
    
    while(!stopLoop)
    {
        evt = evt_getStatus(evt_hanlder);
        if(!evt)
            continue;
        char_num=0;   
        switch (evt) {        
            case BTN_DOWN:
                if(mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=3;
                    mode=1;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_UP:
                if(mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=2;
                    mode=1;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_LEFT:
                if(!mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=2;
                    mode=0;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_RIGHT:
                if(!mode)
                {
                    gfx_putSelect(BG_COLOR);
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    gfx_putSelect(SEL_COLOR);    
                }
                else
                {
                    gfx_putSelect(BG_COLOR);
                    elem=3;
                    mode=0;
                    gfx_putSelect(SEL_COLOR);
                }
                break;
            case BTN_OFF:
                modify_str=0;
            case BTN_ON:
                if(str_len+1==txt_max_char)
                    str_len++;
                cur_str[str_len]='\0';
                /* get out of here */            
                stopLoop=1;
                break;     
            case BTN_F3:
                char_num++;
            case BTN_F2:
                char_num++;
            case BTN_F1:
                //printk("char num=%d mode=%d, elem=%d, page=%d\n",char_num,mode,elem,page);
                if(cur_index<txt_max_char)
                {                    
                    str=mode==1?char_horiz[page][elem][char_num]:char_vert[page][elem][char_num];
                    if(str)
                    {
                        putCursor(BG_COLOR);
                        gfx_putC(TXT_COLOR,TXT_BG_COLOR,txt_x+cur_index*w,txt_y,str);
                        cur_str[cur_index]=str;
                        if(cur_index+1!=txt_max_char)
                        {
                            if(cur_index==str_len)
                                str_len++;
                            cur_index++;
                        }
                        putCursor(SEL_COLOR);
                    }
                    else
                    {
                        if(mode==1)
                        {
                            if(action_horiz[page][elem][char_num])
                            {
                                routine=action_horiz[page][elem][char_num];
                                routine();
                            }
                        }
                        else
                        {
                            if(action_vert[page][elem][char_num])
                            {
                                routine=action_vert[page][elem][char_num];
                                routine();
                            }
                        }
                    }
                }
                
                if(shift==1)
                    majCursor();
                if(shift==2)
                    shift--;  
                break;
        }
    }    
}

void iniSprite(void)
{
    spec_horiz[0][4][0]=spec_horiz[1][4][0]=spec_horiz[2][4][0]=&leftArrow_SP;
    spec_horiz[0][4][1]=spec_horiz[1][4][1]=spec_horiz[2][4][1]=&del_SP;
    spec_horiz[0][4][2]=spec_horiz[1][4][2]=spec_horiz[2][4][2]=&rightArrow_SP;
    spec_horiz[0][5][0]=spec_horiz[1][5][0]=spec_horiz[2][5][0]=&upArrow_SP;
    spec_horiz[0][5][2]=spec_horiz[1][5][2]=spec_horiz[2][5][2]=&up2Arrow_SP;
    spec_horiz[0][5][1]=spec_horiz[1][5][1]=spec_horiz[2][5][1]=&num_SP;
}

int defaultVirtKbdLY(void)
{
    switch(CURRENT_ARCH)
    {
        case AV3XX_ARCH:
        case AV4XX_ARCH:
        case PMA_ARCH:
        case AV5XX_ARCH:
            return 0;
        break;
        case GMINI4XX_ARCH:
        case AV1XX_ARCH:
        case JBMM_ARCH:
        case GMINI402_ARCH:
            return 1;
    }   
}



void virtKbd(int evt_handler,char * str)
{
    mode=1;
    elem=4;
    cur_index=0;
    modify_str=1;
    page=0;
    shift=0;
    gfx_fontSet(STD8X13);
    str_len=strlen(str);
    strcpy(cur_str,str);
    
    calcCoord(paramVirtKbd);
    
    iniSprite();
    drawPage();
    drawBtn();
    drawTxtZone();
    gfx_putS(TXT_COLOR,TXT_BG_COLOR,txt_x,txt_y,cur_str);    
    gfx_putSelect(SEL_COLOR);  
    putCursor(SEL_COLOR);
    virtKbdEvtHandler(evt_handler);
    if(modify_str)
        strcpy(str,cur_str);
}
