/*
*   helloworld.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

//#include <api.h>
#include <driver/fm_remote.h>


char textMenu;
char currentPrintedScr;

void show_menu(textMenu){
    if (!currentPrintedScr || currentPrintedScr != textMenu) {
        gfx_clearScreen(COLOR_BLACK);
        gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,textMenu);
        mdelay(1000);
        currentPrintedScr = textMenu;
    }
}

void app_main(int argc,char * * argv)
{

    int stop=0;
    int evt_handler=0, evt=0, offPressed=0;//ask=0;

    char path[MAX_PATH];
    
    fs_pwd(path);
    printf("current path=%s\n",path);
    textMenu="Hello";
    show_menu(textMenu);
    mdelay(1000);
    
    while (!stop)
    {
        evt=evt_getStatus(evt_handler);
        while (!evt) {
            evt=evt_getStatus(evt_handler);
            switch(evt)
            {
                case BTN_F1:
                    textMenu="Yout Entered 1";
                    show_menu(textMenu);
                    mdelay(1000);
                    break;
                    
                case BTN_OFF:
                    evt=evt_getStatus(evt_handler);
                    offPressed+=1;
                    while (1)
                    {
                        gfx_clearScreen(COLOR_BLACK);
                        gfx_putS(COLOR_BLUE,COLOR_BLACK,10,10,"Press again to exit");
                        mdelay(300);
                        evt=evt_getStatus(evt_handler);
                        if(evt!=NO_EVENT)
                        {
                            if (evt==BTN_OFF) {
                                    offPressed+=1;
                                    stop=1;
                                    break;
                            }
                            else{
                                textMenu="Press F1 To Enter 1";
                                show_menu(textMenu);
                                break;
                            }
                        }
                    }
                    break;
                default:
                    textMenu="Press F1 To Enter 1";
                    show_menu(textMenu);
                    mdelay(1000);
                    break;
            }
                    
            
        }
        
    }
}
