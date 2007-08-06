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
#define TOT_WIDTH         SCREEN_REAL_WIDTH
#define TOT_HEIGHT        SCREEN_HEIGHT
#define BG_COLOR          COLOR_TRSP
#define TXT_COLOR         COLOR_BLUE
#define SEL_COLOR         COLOR_BLACK
#define BLK_COLOR         COLOR_DARK_GRAY
#define TXT_BG_COLOR      COLOR_LIGHT_YELLOW

PALETTE pal[2] = {BG_COLOR, TXT_COLOR};

void leftCursor(void);
void rightCursor(void);
void delCursor(void);
void shiftCursor(void);
void numPage(void);
void insCursor(void);
void supCursor(void);

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
char my_str[MAX_PATH];
char * cur_str;

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

void putCursor(int color)
{
    int h=0,w=0,org=cur_str-my_str;
    gfx_getStringSize("M", &w, &h);
    gfx_drawLine(color,txt_x+(cur_index-org)*w,txt_y+h+1,txt_x+(cur_index-org)*w+w,txt_y+h+1);
}

void drawTxt(void)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    gfx_fillRect(TXT_BG_COLOR,txt_x-2*CELL_DIST,txt_y-2*CELL_DIST,txt_w+4*CELL_DIST,h+4*CELL_DIST+2);
    gfx_drawRect(BLK_COLOR,txt_x-2*CELL_DIST,txt_y-2*CELL_DIST,txt_w+4*CELL_DIST,h+4*CELL_DIST+2);
    /* +2 => space for the cursor*/
    gfx_putnS(TXT_COLOR,TXT_BG_COLOR,txt_x,txt_y,txt_max_char,cur_str);    
    putCursor(SEL_COLOR);
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

void putSelect(int color)
{
    int h=0,w=0;
    gfx_getStringSize("M", &w, &h);
    //printk("[putCursor] %s %s %d\n",color==COLOR_WHITE?"WHITE":"BLUE",mode==1?"HORIZ":"VERT",elem);
    if(mode) // horizontal
    {
        gfx_drawRect(color,coord_horiz[elem][0],coord_horiz[elem][1],
                     NB_CELL_PER_LINE*(w+CELL_SPACE)-CELL_SPACE+2*CELL_DIST,
                     h+2*CELL_DIST);
    }
    else    // vertical
    {
        gfx_drawRect(color,coord_vert[elem][0],coord_vert[elem][1],w+2*CELL_DIST,
                     NB_CELL_PER_LINE*(h+CELL_SPACE)-CELL_SPACE+2*CELL_DIST);
    }
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
    drawBtn();
    drawTxt();
    putSelect(SEL_COLOR);
}


void leftCursor(void)
{
    int org=cur_str-my_str;
    printk("[left] org=%d cur_index=%d str_len=%d\n",org,cur_index,str_len);
    if(cur_index>0)
    {
        if(cur_index==org)
        {
            cur_index--;
            cur_str--;
            drawTxt();   
        }
        else  
        {
            putCursor(TXT_BG_COLOR);    
            cur_index--;  
            putCursor(SEL_COLOR);
        }
    }
}

void rightCursor(void)
{
    int org=cur_str-my_str;
    printk("[right] org=%d cur_index=%d str_len=%d\n",org,cur_index,str_len);
    if(cur_index<str_len)
    {
        if(cur_index==(org+txt_max_char-1))
        {
            cur_index++;
            cur_str++;
            drawTxt(); 
        }
        else
        {
            putCursor(TXT_BG_COLOR);
            cur_index++;
            putCursor(SEL_COLOR);
        }
    }
}

void delCursor(void)
{
    if(cur_index==str_len)
        return;
    
    strcpy(&my_str[cur_index],&my_str[cur_index+1]);
    str_len--;
    my_str[str_len]='\0';
    drawTxt();
}

int oldPage;

void shiftCursor(void)
{
    printk("shift page=%d shift=%d\n",page,shift);
    switch(page)
    {
        case 0:
        case 2:
            printk("Norm page or numPage\n");
            page=1;
            shift=2;
            spec_horiz[1][4][1]=&up2Arrow_SP;
            spec_horiz[0][3][1]=spec_horiz[1][3][1]=spec_horiz[2][3][1]=&num_SP;
            drawPage();        
            break;   
        case 1:
            if(shift==1)
            {
                printk("Inv\n");
                shift=0;
                spec_horiz[1][4][1]=&up2ArrowInv_SP;
                drawPage();
            }
            else
            {
                printk("No Inv\n");
                shift=0;
                page=0;
                spec_horiz[1][4][1]=&up2Arrow_SP;
                drawPage();
            }
            break;
    }
}

void numPage(void)
{
    switch(page)
    {
        case 0:
        case 1:
            oldPage=page;
            page=2;
            spec_horiz[0][3][1]=spec_horiz[1][3][1]=spec_horiz[2][3][1]=&numInv_SP;
            if(oldPage==0)
                spec_horiz[2][4][1]=&upArrow_SP;
            else
                spec_horiz[2][4][1]=&up2Arrow_SP;
            drawPage();        
            break;
        case 2:
            page=oldPage;
            spec_horiz[0][3][1]=spec_horiz[1][3][1]=spec_horiz[2][3][1]=&num_SP;
            if(page==1)
                spec_horiz[1][4][1]=&up2Arrow_SP;
            drawPage();    
            break;
    }
    shift=0;
}

int ins=0;

void insCursor(void)
{
    if(ins)
    {
        ins=0;
        spec_horiz[0][5][0]=spec_horiz[1][5][0]=spec_horiz[2][5][0]=&ins_SP;
        drawPage();
    }
    else
    {
        ins=1;
        spec_horiz[0][5][0]=spec_horiz[1][5][0]=spec_horiz[2][5][0]=&insInv_SP;
        drawPage();
    }
    
}

void supCursor(void)
{
    int org=cur_str-my_str;
    if(cur_index==0)
        return;
    strcpy(&my_str[cur_index-1],&my_str[cur_index]);
    str_len--;
    my_str[str_len]='\0';
    cur_index--;
    
    if(org>cur_index)
        cur_str--;
    drawTxt();
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
    int org;
    int redraw;
    
    modify_str=1;
    
    while(!stopLoop)
    {
        evt = evt_getStatusBlocking(evt_hanlder);
        if(!evt)
            continue;
        char_num=0;   
        switch (evt) {        
            case BTN_DOWN:
                if(mode)
                {
                    putSelect(BG_COLOR );
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    putSelect(SEL_COLOR);    
                }
                else
                {
                    putSelect(BG_COLOR );
                    elem=3;
                    mode=1;
                    putSelect(SEL_COLOR);
                }
                break;
            case BTN_UP:
                if(mode)
                {
                    putSelect(BG_COLOR );
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    putSelect(SEL_COLOR);    
                }
                else
                {
                    putSelect(BG_COLOR );
                    elem=2;
                    mode=1;
                    putSelect(SEL_COLOR);
                }
                break;
            case BTN_LEFT:
                if(!mode)
                {
                    putSelect(BG_COLOR );
                    if(elem==0)
                        elem=5;
                    else
                        elem--;
                    putSelect(SEL_COLOR);    
                }
                else
                {
                    putSelect(BG_COLOR );
                    elem=2;
                    mode=0;
                    putSelect(SEL_COLOR);
                }
                break;
            case BTN_RIGHT:
                if(!mode)
                {
                    putSelect(BG_COLOR );
                    if(elem==5)
                        elem=0;
                    else
                        elem++;
                    putSelect(SEL_COLOR);    
                }
                else
                {
                    putSelect(BG_COLOR );
                    elem=3;
                    mode=0;
                    putSelect(SEL_COLOR);
                }
                break;
            case BTN_OFF:
                modify_str=0;
            case BTN_ON:   
                if(cur_index+1==str_len)
                    str_len--;             
                my_str[str_len]='\0';
                /* get out of here */            
                stopLoop=1;
                break;     
            case BTN_F3:
                char_num++;
            case BTN_F2:
                char_num++;
            case BTN_F1:
                str=mode==1?char_horiz[page][elem][char_num]:char_vert[page][elem][char_num];
                if(str)
                {
                    org=cur_str-my_str;
                    putCursor(TXT_BG_COLOR );
                    if(ins && cur_index!=str_len)
                    {
                        if(str_len!=(MAX_PATH-1))
                        {
                            char * ptr=my_str+str_len;
                            while(ptr>=(my_str+cur_index))
                            {
                                *(ptr+1)=*ptr;
                                ptr--;
                            }
                            my_str[cur_index]=str;
                            str_len++;
                            my_str[str_len]='\0';
                            cur_index++;
                            if(cur_index==(org+txt_max_char-1))
                                cur_str++;
                            drawTxt();                            
                            putCursor(SEL_COLOR);
                        }
                        
                    }
                    else
                    {
                        my_str[cur_index]=str;
                        gfx_putC(TXT_COLOR,TXT_BG_COLOR,txt_x+(cur_index-org)*w,txt_y,str);
                        redraw=1;
                        if(cur_index==str_len)
                        {
                            if(str_len!=(MAX_PATH-1))
                            {
                                cur_index++;
                                str_len++;  
                                my_str[str_len]='\0';                 
                            }
                            else
                                redraw=0;
                        }
                        else
                            cur_index++;
                        
                        if(redraw)
                        {
                            if(cur_index==(org+txt_max_char-1))
                            {
                                cur_str++;
                                drawTxt();
                            }
                            else
                                putCursor(SEL_COLOR);
                        }
                        else
                            putCursor(SEL_COLOR);
                    }
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
                
                if(shift==1)
                {
                    page=0;
                    drawPage();
                }
                if(shift==2)
                    shift--;  
                break;
        }
    }    
}

void iniSprite(void)
{
    spec_horiz[0][3][1]=spec_horiz[1][3][1]=spec_horiz[2][3][1]=&num_SP;
    
    spec_horiz[0][4][0]=spec_horiz[1][4][0]=spec_horiz[2][4][0]=&leftArrow_SP;
    spec_horiz[0][4][2]=spec_horiz[1][4][2]=spec_horiz[2][4][2]=&rightArrow_SP;
    spec_horiz[0][4][1]=spec_horiz[2][4][1]=&upArrow_SP;    
    spec_horiz[1][4][1]=&up2Arrow_SP;
        
    spec_horiz[0][5][0]=spec_horiz[1][5][0]=spec_horiz[2][5][0]=&ins_SP;
    spec_horiz[0][5][1]=spec_horiz[1][5][1]=spec_horiz[2][5][1]=&del_SP;
    spec_horiz[0][5][2]=spec_horiz[1][5][2]=spec_horiz[2][5][2]=&supSP;
    
    
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
    elem=4;
    mode=modify_str=1;
    ins=shift=cur_index=page=0;
    
    gfx_fontSet(STD8X13);
    str_len=strlen(str);
    strcpy(my_str,str);
    cur_str=my_str;
    
    calcCoord(paramVirtKbd);    
    iniSprite();
    
    drawPage();
    
    virtKbdEvtHandler(evt_handler);
    if(modify_str)
        strcpy(str,cur_str);
}
