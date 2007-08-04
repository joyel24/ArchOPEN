/*
*   kernel/gfx/gui/shell/settings/clock.c
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
#include <kernel/evt.h>
#include <kernel/lang.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/chooser.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/settings_energy.h>

#include <driver/lcd.h>
#include <driver/energy.h>
#include <driver/batDc.h>

#include <fs/cfg_file.h>

#define ENERGY_GUIFONT RADONWIDE

WIDGETLIST menuList;  

#define ICON_X 5
#define ICON_Y 5

CHOOSER chooser_list[6];

int stop_enrg_set;

void okBtnEnergy_click(BUTTON b)
{
    CFG_DATA * cfg;
    int i,j;
    char tmpStr[50];
    /* saving to cfg file */
    
    msgBox_info(getLangStr(STRLNG_SAVE_SETTINGS));
    
    cfg=cfg_readFile("/medios/medios.cfg");
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }
    }
    
    for(j=0;j<2;j++)
        for(i=0;i<3;i++)
        {
            if(chooser_list[i+j*3]->index==9)
            {
                timer_status[i][j]=0;
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,0);
            }
            else
            {
                if(i==2)
                {
                    //chooser_list[i+j*3]->index=timer_status[i][j]==1?(timer_delay[i][j]/60)-1:9;
                    timer_status[i][j]=1;
                    timer_delay[i][j]=(chooser_list[i+j*3]->index+1)*60;
                    
                }
                else
                {
                    timer_status[i][j]=1;
                    timer_delay[i][j]=(chooser_list[i+j*3]->index+1)*10;
                }
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,1);
                sprintf(tmpStr,"energy_%s_%s_delay",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,timer_delay[i][j]);
            }
        }
    cfg_printItems(cfg);
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    energy_chgMode(getPowerMode());
    stop_enrg_set=1;
}

void resetBtnEnergy_click(BUTTON b)
{
    int i,j;
    for(j=0;j<2;j++)
        for(i=0;i<3;i++)
            if(i==2)
                chooser_list[i+j*3]->index=timer_status[i][j]==1?(timer_delay[i][j]/60)-1:9;
            else
                chooser_list[i+j*3]->index=timer_status[i][j]==1?(timer_delay[i][j]/10)-1:9;
}

void energy_setting(void)
{
    ICON logo;
        
    char tmpStr[30];
       
    char * valFormStr_1[10]={"10s","20s","30s","40s","50s","60s","70s","80s","90s",getLangStr(STRLNG_NRJ_NEVER)};
    char * valFormStr_2[10]={"1min","2min","3min","4min","5min","6min","7min","8min","9min",getLangStr(STRLNG_NRJ_NEVER)};
    
    int evtHandle;
    int event;
    
    int minX,maxW,lineH,x,y,label_offset,w,h,sepH,sepW;
        
    int i,j;
    
    stop_enrg_set = 0;
    
    BUTTON mib;
    
    printk("Cur power:");
    
    for(i=0;i<3;i++)
        for(j=0;j<2;j++)
    {
        printk("%s-%s (%s|%d) -",timer_type[i],power_type[j],
               timer_status[i][j]==0?"disable":"enable",
               timer_delay[i][j]);
    }
    printk("\n");
    
    gfx_clearScreen(COLOR_WHITE);
    
    
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    
    logo=icon_get("energy");
    if(!logo)
        icon_load("energy.ico");
    
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    
    minX = ICON_X + logo->bmap_data.width;
        
    
    gfx_drawLine(COLOR_ORANGE,minX+3,5,minX+3,LCD_HEIGHT-5);
        
    minX+=3; 
       
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_NRJ_SETTINGS),&w,&h);
    gfx_putS(COLOR_DARK_GREY,COLOR_WHITE,minX+(LCD_WIDTH-minX-w)/2,ICON_Y,getLangStr(STRLNG_NRJ_SETTINGS));
    
    gfx_fontSet(ENERGY_GUIFONT);    
    gfx_getStringSize(getLangStr(STRLNG_NRJ_NEVER),&maxW,&lineH);    
    lineH=(lineH+4);
    
    gfx_getStringSize("HALT",&label_offset,NULL);
    
    maxW=maxW+35+4+label_offset;
    x=minX+(LCD_WIDTH-minX-maxW)/2;    
    y=ICON_Y+h+(LCD_HEIGHT-ICON_Y-h-lineH*9-2*2)/2;
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    // standardMenu
    
    for(j=0;j<2;j++)
    {
        gfx_putS(COLOR_BLUE,COLOR_WHITE,x,y,j==0?getLangStr(STRLNG_NRJ_ON_BATTERY):getLangStr(STRLNG_NRJ_ON_DC));
        y+=lineH;
        gfx_getStringSize(j==0?getLangStr(STRLNG_NRJ_ON_BATTERY):getLangStr(STRLNG_NRJ_ON_DC),&w,&h);
        gfx_drawLine(COLOR_BLACK,x-1,y-2,x+w+1,y-2);
        y+=2;
        for(i=0;i<3;i++)
        {
            sprintf(tmpStr,"%s",timer_type[i]);
            gfx_putS(COLOR_BLUE,COLOR_WHITE,x,y,tmpStr);
            chooser_list[i+j*3]=chooser_create();
            chooser_list[i+j*3]->items=i==2?valFormStr_2:valFormStr_1;
            chooser_list[i+j*3]->itemCount=10;
            if(i==2)
            {
                    chooser_list[i+j*3]->index=timer_status[i][j]==1?(timer_delay[i][j]/60)-1:9;                
            }
            else
            {
                chooser_list[i+j*3]->index=timer_status[i][j]==1?(timer_delay[i][j]/10)-1:9;
            }
            chooser_list[i+j*3]->font=ENERGY_GUIFONT;
            gfx_getStringSize(valFormStr_1[9],&w,&h);
            chooser_list[i+j*3]->setRect(chooser_list[i+j*3],x+label_offset+4,y,w+35,h+1);
            chooser_list[i+j*3]->wrap=WIDGET_WRAP_ON;
            menuList->addWidget(menuList,chooser_list[i+j*3]);
            y+=lineH;
        }
        //y+=lineH;
    }
        
    
    gfx_getStringSize("OK",&sepW,&sepH);
    
    mib=button_create();
    mib->caption="OK"; 
    mib->font=ENERGY_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnEnergy_click;
    menuList->addWidget(menuList,mib);
        
    gfx_getStringSize("RESET",&sepW,&sepH);
    x+=mib->width+4;
    
    mib=button_create();
    mib->caption="Reset"; 
    mib->font=ENERGY_GUIFONT;
    mib->setRect(mib,x,y,sepW+2,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)resetBtnEnergy_click;
    menuList->addWidget(menuList,mib);
    
    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,chooser_list[0]);
    menuList->paint(menuList);
    
    do{
        event=evt_getStatusBlocking(evtHandle);
        if (!event) continue; // no new events
        switch(event)
        {
            case BTN_UP:
                menuList->changeFocus(menuList,WLD_PREVIOUS);
                break;
            case BTN_DOWN:
                menuList->changeFocus(menuList,WLD_NEXT);
                break;    
            default:
                menuList->handleEvent(menuList,event);
                break;
        }
    }while(event!=BTN_OFF && !stop_enrg_set); 
       
    menuList->destroy(menuList);
}
