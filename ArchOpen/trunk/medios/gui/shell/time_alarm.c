/*
*   kernel/gfx/gui/shell/time_alarm.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/malloc.h>
#include <kernel/kernel.h>
#include <kernel/lang.h>
#include <kernel/evt.h>

#include <gui/settings_bgImg.h>

#include <driver/lcd.h>
#include <driver/time.h>

#include <sys_def/colordef.h>

#include <lib/string.h>

#include <gfx/kfont.h>
#include <gfx/graphics.h>

/******************************************
Arch dep config: 
according to LCD type, invert video 
with no backlight is not visible: 
on av4 it's OK not on Av5
******************************************/


/*segment 0: upper/left one, 1: upper one, 2: upper right, ... 6: center
    1
  *****
 *     *
0*     *2
 *  6  *
  *****
 *     *
5*     *3
 *     *
  *****
    4
*/

int seg_table[10][7] = {
    /*0 1 2 3 4 5 6*/
    { 1,1,1,1,1,1,0}, /*0*/
    { 0,0,1,1,0,0,0}, /*1*/
    { 0,1,1,0,1,1,1}, /*2*/
    { 0,1,1,1,1,0,1}, /*3*/
    { 1,0,1,1,0,0,1}, /*4*/
    { 1,1,0,1,1,0,1}, /*5*/
    { 1,1,0,1,1,1,1}, /*6*/
    { 0,1,1,1,0,0,0}, /*7*/
    { 1,1,1,1,1,1,1}, /*8*/
    { 1,1,1,1,1,0,1}, /*9*/
};
                        
int tria_table[4][4] = {
    { 1, 1,-1, 0 },
    { 0, 0, 0, 1 },
    { 0, 1, 0,-1 },
    { 0, 1, 1, 0 }
};

int seg_data[7][5];

int h,l_x,l_y,s;
int seg_w,seg_h;
int color_mode;

int colors[2][2] = {
    {COLOR_WHITE,COLOR_BLACK},
    {COLOR_BLACK,COLOR_WHITE}
};


int day_str[] = {
    STRLNG_SUNDAY,STRLNG_MONDAY,STRLNG_TUESDAY,STRLNG_WEDNESDAY,STRLNG_THURSDAY,STRLNG_FRIDAY,STRLNG_SATURDAY
};

void init_sizePos(int txt_height)
{
    int h1;
/* using hardcoded value */
    s=3;
    
    h=(LCD_WIDTH-8-8*s)/27;
    
    h1=(LCD_HEIGHT-s-2*txt_height)/18;
    
    printk("h=%d, h1=%d\n",h,h1);
            
    if(h>h1) h=h1;
    
    l_x=3*h;
    l_y=6*h;
    
    
    seg_w=s+h+l_x+h+s+2; /*2 is for the 2 vertical segments on the edge*/
    seg_h=(h+l_y+h)*2+s;
    
    seg_data[0][0]=0;
    seg_data[0][1]=0;
    seg_data[0][2]=1;
    seg_data[0][3]=2;
    seg_data[0][4]=0;
    
    seg_data[1][0]=s;
    seg_data[1][1]=0;
    seg_data[1][2]=3;
    seg_data[1][3]=2;
    seg_data[1][4]=1;
    
    seg_data[2][0]=s+h+l_x+h+s-h;
    seg_data[2][1]=0;
    seg_data[2][2]=0;
    seg_data[2][3]=3;
    seg_data[2][4]=0;
    
    seg_data[3][0]=s+h+l_x+h+s-h;
    seg_data[3][1]=(h+l_y+h)+s;
    seg_data[3][2]=0;
    seg_data[3][3]=3;
    seg_data[3][4]=0;
    
    seg_data[4][0]=s;
    seg_data[4][1]=(h+l_y+h)+s+(h+l_y+h)-h;
    seg_data[4][2]=0;
    seg_data[4][3]=1;
    seg_data[4][4]=1;
    
    seg_data[5][0]=0;
    seg_data[5][1]=(h+l_y+h)+s;
    seg_data[5][2]=1;
    seg_data[5][3]=2;
    seg_data[5][4]=0;
    
    seg_data[6][0]=s;
    seg_data[6][1]=h+l_y+h+s/2-h/2;
    seg_data[6][2]=4;
    seg_data[6][3]=5;
    seg_data[6][4]=1;
}

void fill_triangle(int color,int x, int y, int type)
{
    int a,b,i;
    
    switch(type)
    {
        case 4:
            a=x+(h-1)/2;
            b=x+(h-1)/2;
            for(i=0;i<(h/2);i++)
            {
                gfx_drawLine(color,a,y+i,b,i+y);
                a--;
            }
            a++;
            for(;i<h;i++)
            {
                gfx_drawLine(color,a,y+i,b,i+y);
                a++;
            }
            break;
        case 5:
            a=x;
            b=x;
            for(i=0;i<(h/2);i++)
            {
                gfx_drawLine(color,a,y+i,b,i+y);
                b++;
            }
            b--;
            for(;i<h;i++)
            {
                gfx_drawLine(color,a,y+i,b,i+y);
                b--;
            }
            break;
        default:
            a=x+tria_table[type][0]*(h-1);
            b=x+tria_table[type][1]*(h-1);
            for(i=0;i<h;i++)
            {
                gfx_drawLine(color,a,y+i,b,i+y);
                a+=tria_table[type][2];
                b+=tria_table[type][3];
            }
            break;      
    }
}

void paint_number(int x,int y,int num)
{
    int i;
    gfx_fillRect(colors[color_mode][0],x,y,seg_w,seg_h);
    
    for(i=0;i<7;i++)
    {
        if(seg_table[num][i])
        {
            if(i==6)
            {
               fill_triangle(colors[color_mode][1],x+seg_data[i][0],y+seg_data[i][1],4);
               gfx_fillRect(colors[color_mode][1],x+seg_data[i][0]+h/2,y+seg_data[i][1],l_x+h,h);
               fill_triangle(colors[color_mode][1],x+seg_data[i][0]+h+l_x+h/2,y+seg_data[i][1],5);
               continue;
            }
            
            if(seg_data[i][4]) /*horizontal segment*/
            {
                fill_triangle(colors[color_mode][1],x+seg_data[i][0],y+seg_data[i][1],seg_data[i][2]);
                gfx_fillRect(colors[color_mode][1],x+seg_data[i][0]+h,y+seg_data[i][1],l_x,h);
                fill_triangle(colors[color_mode][1],x+seg_data[i][0]+h+l_x,y+seg_data[i][1],seg_data[i][3]);
            }
            else /*vertical segment*/
            {
                fill_triangle(colors[color_mode][1],x+seg_data[i][0],y+seg_data[i][1],seg_data[i][2]);
                gfx_fillRect(colors[color_mode][1],x+seg_data[i][0],y+seg_data[i][1]+h,h,l_y);
                fill_triangle(colors[color_mode][1],x+seg_data[i][0],y+seg_data[i][1]+h+l_y,seg_data[i][3]);
            }
        }
    }
}

void time_alarm(void)
{
    int evtHandler,evt;
    
    int num_a=-1,num_b=-1,num_c=-1,num_d=-1;
    int stop=0;
    int x,y;
    int i;
    int br=lcd_getBrightness();
    char tmp_str[5];
    int txt_h,txt_w;
    
    char date_str[100];
    
    color_mode=0;
    
    struct med_tm now;
        
    evtHandler = evt_getHandler(ALL_CLASS);
    
    gfx_clearScreen(colors[color_mode][0]);
        
    chg_BG_enable(0,0,0);
       
    
    time_get(&now);    
    sprintf(date_str,"%s %02d/%02d/%04d",getLangStr(day_str[now.tm_wday]),now.tm_mday,now.tm_mon,now.tm_year);
    gfx_getStringSize(date_str,&txt_w,&txt_h);
    
    init_sizePos(txt_h);
    
    x=(LCD_WIDTH-4*seg_w-2*h-3*h)/2;
    y=(LCD_HEIGHT-seg_h-2*txt_h)/2;
    
    gfx_fillRect(COLOR_BLACK,x+2*seg_w+2*h,y+(seg_h-h)/2-h,h,h);
    gfx_fillRect(COLOR_BLACK,x+2*seg_w+2*h,y+(seg_h+h)/2,h,h);
    
    
    
    gfx_putS(COLOR_BLACK,COLOR_WHITE,(LCD_WIDTH-txt_w)/2,y+seg_h+txt_h,date_str);
    
    while(!stop)
    {
        evt=evt_getStatus(evtHandler);
        
        switch(evt)
        {
            case BTN_OFF:
                stop=1;
                
                break;
            case BTN_ON:
                
                color_mode=color_mode==1?0:1;
                num_a=num_b=num_c=num_d=-1;
#ifdef LCDTYPE_0
                if(color_mode)
                {
                    lcd_setBrightness(0);
                }
                else
                {
                    lcd_setBrightness(br);
                }
#endif
                gfx_clearScreen(colors[color_mode][0]);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h-h)/2-h,h,h);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h+h)/2,h,h);
                gfx_putS(colors[color_mode][1],colors[color_mode][0],(LCD_WIDTH-txt_w)/2,y+seg_h+txt_h,date_str);
                break; 
            case BTN_UP:
                colors[0][1]++;
                if(colors[0][1]>0xFF) colors[0][1]=1;      
                colors[1][1]=colors[0][1];          
                
                num_a=num_b=num_c=num_d=-1;
                gfx_clearScreen(colors[color_mode][0]);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h-h)/2-h,h,h);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h+h)/2,h,h);
                gfx_putS(colors[color_mode][1],colors[color_mode][0],(LCD_WIDTH-txt_w)/2,y+seg_h+txt_h,date_str);
                sprintf(tmp_str,"%02x",colors[0][1]);
                gfx_putS(COLOR_BLACK,COLOR_TRSP,1,1,tmp_str);
                break;
            case BTN_DOWN:
                colors[0][1]--;
                if(colors[0][1]<1) colors[0][1]=0xff;
                colors[1][1]=colors[0][1];
                
                num_a=num_b=num_c=num_d=-1;
                gfx_clearScreen(colors[color_mode][0]);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h-h)/2-h,h,h);
                gfx_fillRect(colors[color_mode][1],x+2*seg_w+2*h,y+(seg_h+h)/2,h,h);
                gfx_putS(colors[color_mode][1],colors[color_mode][0],(LCD_WIDTH-txt_w)/2,y+seg_h+txt_h,date_str);
                sprintf(tmp_str,"%02x",colors[0][1]);
                gfx_putS(COLOR_BLACK,COLOR_TRSP,1,1,tmp_str);
                break;
        }
            
       
        time_get(&now);

        if(num_a!=(now.tm_sec%10)) { num_a=now.tm_sec%10; paint_number(x+seg_w+h+seg_w+3*h+seg_w+h,y,num_a); }
        if(num_b!=((now.tm_sec/10)%10)) { num_b=(now.tm_sec/10)%10; paint_number(x+seg_w+h+seg_w+3*h,y,num_b); }
        if(num_c!=(now.tm_min%10)) { num_c=now.tm_min%10; paint_number(x+seg_w+h,y,num_c); }
        if(num_d!=((now.tm_min/10)%10)) { num_d=(now.tm_min/10)%10; paint_number(x,y,num_d); }
        
                
        for(i=0;i<50000;i++);        
    }
    
    lcd_setBrightness(br);
    
    restoreBG();
    evt_freeHandler(evtHandler);
}
