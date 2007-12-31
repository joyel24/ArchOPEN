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

void app_main(int argc,char * * argv)
{
    int i,j,fg;
    char tmp[10];
    int evt_handler,evt;
    int scr_w,scr_h;

    evt_handler=evt_getHandler(BTN_CLASS);
    
    gfx_fontSet(STD6X9);
    gfx_clearScreen(COLOR_WHITE);
    
    getResolution(&scr_w,&scr_h);
    
    if(scr_w<320 || scr_h < 240)
        msgBox_show("Size error","Screen size is too small",MSGBOX_TYPE_OK,MSGBOX_ICON_EXCLAMATION,evt_handler);
    else    
        for(i=0;i<16;i++)
        {
            for(j=0;j<16;j++)
            {
                gfx_fillRect(i*16+j, j*20 , i*15, 20, 15);
                if(i*16+j==1)
                    fg=0;
                else
                    fg=1;
                sprintf(tmp,"%03d",i*16+j);
                gfx_putS(i*16+j,fg,j*20+1, i*15+3, tmp);
            }
        }
    
    while(1)
    {
        evt=evt_getStatusBlocking(evt_handler);
        
        if(evt==BTN_OFF) break;
    }
}
