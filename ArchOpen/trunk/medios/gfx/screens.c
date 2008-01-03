/*
*   kernel/gfx/screens.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <kernel/kernel.h>
#include <kernel/console.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <gfx/graphics.h>
#include <gfx/screens.h>

#include <gui/splash.h>

int current_screen;
int main_screen;

/* array of screen data */
SCREEN_DATA screen_table[NB_SCREENS]; 

void screens_init(void)
{
    int i;
    current_screen = -1;
    main_screen=SCREEN_SPLASH;
    for(i=0;i<NB_SCREENS;i++) screen_table[i]=NULL;
}

void screens_add(struct screen_data * data,int screen_num)
{
    if(screen_num>=0 && screen_num<NB_SCREENS)
        screen_table[screen_num]=data;
}

void screens_show(int screen_num)
{
    if(screen_num<0 && screen_num>=NB_SCREENS)
    {
        printk("Bad screen number : %d (max=%d)\n",screen_num,NB_SCREENS);
        return;
    }
    if(!screen_table[screen_num] || !screen_table[screen_num]->show)
    {
        printk("No or incompletet data for screen %d (ptr=%x)\n",screen_num,screen_table[screen_num]);
        return;
    }

    if(current_screen==-1)
        printk("switch from NO_SCREEN to %d\n",screen_num);
    else
        printk("switch from %d to %d\n",current_screen,screen_num);
    
    
    
    if(current_screen!=-1 && screen_table[current_screen]) // need to save data of previous screen 
    {
        if(screen_table[current_screen]->palette)
            osd_savePalette(screen_table[current_screen]->palette,256);
        else
            printk("no need to save palette\n");
        /* disable all OSD plane */    
        osd_setComponentConfig(OSD_VIDEO1,  0);
        osd_setComponentConfig(OSD_VIDEO2,  0);
        osd_setComponentConfig(OSD_BITMAP1, 0);
        osd_setComponentConfig(OSD_BITMAP2, 0);
        osd_setComponentConfig(OSD_CURSOR1, 0);
        osd_setComponentConfig(OSD_CURSOR2, 0);
    }
    
    if(screen_table[screen_num]->palette)
        osd_setEntirePalette((int(*)[3])screen_table[screen_num]->palette,256,false);
    else
        printk("no need to restore palette\n");
    if(current_screen!=-1 && screen_table[current_screen]->hide)
        screen_table[current_screen]->hide();
    screen_table[screen_num]->show();
    current_screen=screen_num;
}

int screens_current(void)
{
    return current_screen;
}

int screens_main(void)
{
    return main_screen;
}

void screens_mainSet(int screen_num)
{
    if(screen_num<0 && screen_num>=NB_SCREENS)
    {
        printk("Bad screen number : %d (max=%d)\n",screen_num,NB_SCREENS);
        return;
    }
    
    if(screen_num!=main_screen)
    {
        main_screen=screen_num;
    }
}
