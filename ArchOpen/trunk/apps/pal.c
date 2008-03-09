/*
*   pal.c
*
*   MediOS project
*   Copyright (c) 2007 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <api.h>
#include <sys_def/evt.h>
#include <sys_def/font.h>

void draw(int nb_col,int nb_line,int w,int h,int first_color)
{
    int i,j;
    char tmp[10];
    int fg;
     printf("drawing %dx%d colors (%d,%d) start at %d\n",nb_col,nb_line,w,h,first_color);
    gfx_clearScreen(COLOR_WHITE);
    for(i=0;i<nb_line;i++)
    {
        for(j=0;j<nb_col;j++)
        {
            printf("color %d at (%d,%d)\n",i*nb_col+j+first_color,j*w , i*h);
            gfx_fillRect(i*nb_col+j+first_color, j*w , i*h, w, h);
            if((i*nb_col+j+first_color)==COLOR_BLACK)
                fg=COLOR_WHITE;
            else
                fg=COLOR_BLACK;
            sprintf(tmp,"%03d",i*nb_col+j+first_color);
            printf("add str %s at (%d,%d) colors: %d,%d\n",tmp,j*w+1, i*h+1,i*nb_col+j+first_color,fg);
            gfx_putS(fg,(i*nb_col+j+first_color),j*w+1, i*h+1, tmp);
            printf("done, nxt line\n");
        }
    }
    printf("drawing done\n");
}

void app_main(int argc,char * * argv)
{
    int evt_handler,evt;
    int scr_w,scr_h;
    
    int nb_col,nb_line;
    int w,h;
    int nbPage,curPage;

    evt_handler=evt_getHandler(BTN_CLASS);
    
    gfx_fontSet(STD6X9);
    
    
    getResolution(&scr_w,&scr_h);
    
    if(scr_w<320 || scr_h < 240)
    {
        nb_col=nb_line=8;
        w=20;
        h=15;
        nbPage=3;
    }
    else
    {
        nb_col=nb_line=16;  
        w=20;
        h=15;
        nbPage=0;
    }

    curPage=0;
    
    draw(nb_col,nb_line,w,h,0);
       
    while(1)
    {
        evt=evt_getStatusBlocking(evt_handler);
        
        if(evt==BTN_OFF) break;
        if(nbPage>1)
        {
            if(evt==BTN_LEFT)
            {
                gfx_clearScreen(COLOR_WHITE);
                curPage--;
                if(curPage<0) curPage=nbPage;
                draw(nb_col,nb_line,w,h,curPage*nb_col*nb_line);
            }
            
            if(evt==BTN_RIGHT)
            {
                gfx_clearScreen(COLOR_WHITE);
                curPage++;
                if(curPage>nbPage) curPage=0;
                draw(nb_col,nb_line,w,h,curPage*nb_col*nb_line);
            }
        }
    }
}
