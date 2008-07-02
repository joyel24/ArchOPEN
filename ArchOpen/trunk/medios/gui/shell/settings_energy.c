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
#include <kernel/lang.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/widgetmenu.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/settings_energy.h>
#include <gui/settings_screens.h>

#include <driver/lcd.h>
#include <driver/energy.h>
#include <driver/batDc.h>

#include <fs/cfg_file.h>

static WIDGETMENU widgetMenu;
static WIDGETMENU_CHOOSER chooser_list[6];


void energySet_sav(void)
{
    CFG_DATA * cfg;
    int i,j;
    char tmpStr[50];
    int needSave=0;
    /* saving to cfg file */
    
    for(j=0;j<2;j++)
        for(i=0;i<3;i++)
        {
            TEST_VAR(timer_status[i][j],chooser_list[i+j*3]->chooser->index!=9,needSave);
            if(timer_status[i][j])
            {
                if(i==2)
                {
                    TEST_VAR(timer_delay[i][j],(chooser_list[i+j*3]->chooser->index+1)*60,needSave);
                }
                else
                {
                    TEST_VAR(timer_delay[i][j],(chooser_list[i+j*3]->chooser->index+1)*10,needSave);
                }
            }
        }
                    
    if(!needSave)
    {
        printk("No change ==> no save\n");
        return;
    }   
        
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
            if(chooser_list[i+j*3]->chooser->index==9)
            {
                timer_status[i][j]=0;
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,0);
            }
            else
            {
                if(i==2)
                {
                    timer_status[i][j]=1;
                    timer_delay[i][j]=(chooser_list[i+j*3]->chooser->index+1)*60;
                    
                }
                else
                {
                    timer_status[i][j]=1;
                    timer_delay[i][j]=(chooser_list[i+j*3]->chooser->index+1)*10;
                }
                sprintf(tmpStr,"energy_%s_%s_status",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,1);
                sprintf(tmpStr,"energy_%s_%s_delay",timer_type[i],power_type[j]);
                cfg_writeInt(cfg,tmpStr,timer_delay[i][j]);
            }
        }
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    energy_chgMode(getPowerMode());
}

void resetBtnEnergy_click(BUTTON b)
{
    int i,j;
    for(j=0;j<2;j++)
        for(i=0;i<3;i++)
            if(i==2)
                chooser_list[i+j*3]->chooser->setIndex(chooser_list[i+j*3]->chooser,
                    timer_status[i][j]==1?(timer_delay[i][j]/60)-1:9);
            else
                chooser_list[i+j*3]->chooser->setIndex(chooser_list[i+j*3]->chooser,
                    timer_status[i][j]==1?(timer_delay[i][j]/10)-1:9);
    widgetMenu->paint(widgetMenu);
}

void energy_setting(void)
{
    ICON logo;
        
    char * valFormStr_1[10]={"10s","20s","30s","40s","50s","60s","70s","80s","90s",getLangStr(STRLNG_NRJ_NEVER)};
    char * valFormStr_2[10]={"1min","2min","3min","4min","5min","6min","7min","8min","9min",getLangStr(STRLNG_NRJ_NEVER)};
    
    int minX,minY;
    int maxW=0;
    int of,w;
        
    int i,j;
    
    WIDGETMENU_BUTTON mib;
    WIDGETMENU_ITEM it;
    WIDGETMENU_CHOOSER co;
    
    logo=icon_get("energy");
    if(!logo)
        logo=icon_load("energy.ico");
    
    settings_initScreen(getLangStr(STRLNG_NRJ_SETTINGS),logo,&minX,&minY);
    
    // menuList
    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;
    
    /* computing max size for alignAt */
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(widgetMenu->font);
    for(i=0;i<3;i++)
    {
        gfx_getStringSize(timer_type[i],&w,NULL);
        if(w>maxW) maxW=w;
    }    
    gfx_fontSet(of); // restore previous font
    
       
    for(j=0;j<2;j++)
    {
        it=widgetMenuItem_create();
        it->caption=j==0?getLangStr(STRLNG_NRJ_ON_BATTERY):getLangStr(STRLNG_NRJ_ON_DC);
        it->canFocus=0;
        widgetMenu->addItem(widgetMenu,it);
        
        for(i=0;i<3;i++)
        {
            chooser_list[i+j*3]=co=widgetMenuChooser_create();
            co->caption=timer_type[i];
            co->chooser->items=i==2?valFormStr_2:valFormStr_1;
            co->chooser->itemCount=10;
            if(i==2)
                co->chooser->index=timer_status[i][j]==1?(timer_delay[i][j]/60)-1:9;
            else
                co->chooser->index=timer_status[i][j]==1?(timer_delay[i][j]/10)-1:9;
            
            co->chooser->wrap=WIDGET_WRAP_ON;
            co->chooser->orientation=WIDGET_ORIENTATION_HORIZ;
            widgetMenu->addItem(widgetMenu,co);
            co->setAlignAt(co,maxW);
        }
    }
    
    mib=widgetMenuButton_create();
    
    mib->caption=NULL;
    mib->button->caption=getLangStr(STRLNG_RESET);
    mib->button->onClick=(BUTTON_CLICKEVENT)resetBtnEnergy_click;   
    mib->useMaxWidth=true; 
    widgetMenu->addItem(widgetMenu,mib);
            
    // intial paint
    // set focus
    widgetMenu->setFocus(widgetMenu,chooser_list[0]);    
    widgetMenu->paint(widgetMenu);
    settings_evtLoop(widgetMenu,energySet_sav,-1);
       
    widgetMenu->destroy(widgetMenu);
}
