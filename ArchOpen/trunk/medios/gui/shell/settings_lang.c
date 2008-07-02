/*
*   kernel/gfx/gui/shell/settings_misc.c
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
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/widgetmenu.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/file_browser.h>
#include <gui/settings_screens.h>

#include <gui/settings_lang.h>

#include <init/boot_error.h>

#include <driver/lcd.h>

#include <fs/cfg_file.h>
#include <fs/stdfs.h>

#define LANG_GUIFONT RADONWIDE

#define MAX_NB_LANG   5

static WIDGETMENU widgetMenu;
static WIDGETMENU_CHOOSER langList;
ICON logo;

char lang_path[250];

extern char * customLang;

int nbLang;
char * langNameList[MAX_NB_LANG];
int langNum[MAX_NB_LANG];

void cleanLangList(int nb);
void drawLangMenuBG(void);

int evtHandler;
int hasBadVersion;

void langSet_sav(void)
{
    CFG_DATA * cfg;
    int useDefault=0;
    MED_RET_T res;
    /* opening config file */
    
    if(langNum[langList->chooser->index]==lng_langNum)
    {
        printk("No change ==> no save\n");
        return;
    }
    
    msgBox_info(getLangStr(STRLNG_LOAD_LANG));
    if(langList->chooser->index==0)
    {
        printk("Loading Default\n");
        lang_loadDefault();
        useDefault=1;
    }
    else
    {
        printk("Loading: %s\n",langNameList[langList->chooser->index]);
        snprintf(lang_path,MAX_PATH,"/medios/lang/%s.lng",langNameList[langList->chooser->index]);
        if((res=lang_loadFile(lang_path))!=MED_OK)
        {
            if(res==-MED_EBADDATA)
            {
                msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),getLangStr(STRLNG_LNG_WRONG_VERSION),
                            MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
            }
            else
            {
                snprintf(lang_path,MAX_PATH,getLangStr(STRLNG_ERROR_LOADING),langNameList[langList->chooser->index]);
                msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),lang_path,
                            MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
            }
        }
        
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
            cleanLangList(nbLang);
            return;
        }
    }
    
    /* setting the config */
    
    if(useDefault)
    {
        cfg_writeInt(cfg,"lngUseDefault",1);
    }
    else
    {
        cfg_writeInt(cfg,"lngUseDefault",0);
        cfg_writeString(cfg,"lngName",langNameList[langList->chooser->index]);
    }
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    cleanLangList(nbLang);
}

void browserLang_click(BUTTON b)
{
    /*printk("calling browser\n");
    if(browser_simpleBrowse("/medios/lang"  ,myPath)==MED_OK)
    {
        
    }
    printk("Res=%d,|%s|\n",-ret_val,myPath);*/
    msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),"Browser Not available yet here",
                MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
    drawLangMenuBG();
    widgetMenu->paint(widgetMenu);
}

void cleanLangList(int nb)
{
    int i;
    if(nb<=1)
        return;
    for(i=1;i<nb;i++)
    {
        kfree(langNameList[i]);
        langNameList[i]=NULL;
    }   
}

void lang_createList(int * nb)
{
    char * cur_lang;
    int nbLang=1;
    DIR * lang_folder;
    char header[5];
    int fd;
    int i;
    int len;
    struct dirent * entry;    
    
    hasBadVersion=0;
    
    for(i=1;i<MAX_NB_LANG;i++)
        langNameList[i]=NULL;
    
    langNameList[0]=getLangStr(STRLNG_DEFAULT);    
    lang_folder=opendir("/medios/lang");
    
    if(lang_folder)
    {
        while((entry=readdir(lang_folder))!=NULL && nbLang<MAX_NB_LANG)
        {
            if(entry->type!=VFS_TYPE_FILE)
                continue;
            snprintf(lang_path,MAX_PATH,"/medios/lang/%s",entry->d_name);
            fd=open(lang_path,O_RDONLY);
            if(fd<0)
                continue;
            if(read(fd,header,5)!=5)
            {
                close(fd);
                continue;
            }
            
            close(fd);
            
            if(strncmp(header,LNG_HEADER,3))
                continue;
            
            if(header[3]!=LNG_VERSION)
            {
                hasBadVersion=1;
                continue;
            }
            
            nbLang++;
            len=(int)(strrchr(entry->d_name,'.')-(char*)entry->d_name);
            cur_lang=(char*)malloc(len+1);
            strncpy(cur_lang,entry->d_name,len);
            cur_lang[len]='\0';            
            langNameList[nbLang-1]=cur_lang;
            langNum[nbLang-1]=header[4];
        }
        printk("Found %d lang\n",nbLang);
        closedir(lang_folder);        
    }
    else
    {
        msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),getLangStr(STRLNG_MISSING_LANG_FOLDER),
                        MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
        *nb=0;
        return ;
    }
    *nb=nbLang;
}

void drawLangMenuBG(void)
{
    settings_initScreen(getLangStr(STRLNG_NRJ_SETTINGS),logo,NULL,NULL);
}

void lang_setting(void)
{       
    int minX,minY;
    int curIndex,i;
    
    WIDGETMENU_BUTTON mib;

    nbLang=0;
    curIndex=0;
    hasBadVersion=0;
    
    evtHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandler<0)
    {
        printk("Can't get evt handler\n");  
        return; 
    }
    
    lang_createList(&nbLang);
       
    if(nbLang==0)
    {
        evt_freeHandler(evtHandler);
        return;
    }
    
    if(nbLang==1)
    {
        if(hasBadVersion)
        {
            msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),getLangStr(STRLNG_LNG_WRONG_VERSION),
                        MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
        }
        else
        {
            msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),"No lang file found",
                        MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandler);
        }
        evt_freeHandler(evtHandler);
        return;
    }
    
    logo=icon_get("lang");
    if(!logo)
        logo=icon_load("lang.ico");
    settings_initScreen(getLangStr(STRLNG_LANG_SETTINGS),logo,&minX,&minY);
        
    // menuList
    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;

    if(lng_langNum==0 || nbLang==1)
        curIndex=0;
    else
    {
        for(i=1;i<nbLang;i++)
        {
            if(langNum[i]==lng_langNum)
            {
                   curIndex=i;
                   break;
            }
            curIndex=0;                
        }   
    }
    // standardMenu
    langList=widgetMenuChooser_create();
    langList->caption=NULL;
    langList->chooser->items=langNameList;
    langList->chooser->itemCount=nbLang;
    langList->chooser->index=curIndex;
    langList->chooser->wrap=WIDGET_WRAP_ON;
    langList->chooser->orientation=WIDGET_ORIENTATION_HORIZ;
    widgetMenu->addItem(widgetMenu,langList);
        
    mib=widgetMenuButton_create();
    mib->caption=NULL;
    mib->button->caption=getLangStr(STRLNG_OTHER);
    mib->button->onClick=(BUTTON_CLICKEVENT)browserLang_click;
    widgetMenu->addItem(widgetMenu,mib);
        
    // intial paint
    // set focus
    widgetMenu->setFocus(widgetMenu,langList);    
    widgetMenu->paint(widgetMenu);
    
    settings_evtLoop(widgetMenu,langSet_sav,evtHandler);
       
    widgetMenu->destroy(widgetMenu);
    evt_freeHandler(evtHandler);
}

void lang_loadLang(void)
{

    CFG_DATA * cfg;
    int needWrite=0;
    int useDefault=0;
    
    cfg=cfg_readFile("/medios/medios.cfg");
               
    if(!cfg)
    {
        printk("Can't open cfg file\n");
        /* creating default */
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("Can't create new cfg file\n");
            return;
        }
        cfg_writeInt(cfg,"useDefault",1);        
        needWrite=1;   
    }
    else
    {
        if(cfg_itemExists(cfg,"lngUseDefault"))
        {
            useDefault=cfg_readInt(cfg,"lngUseDefault");
            if(useDefault==0)
            {
                if(cfg_itemExists(cfg,"lngName"))
                {
                    snprintf(lang_path,MAX_PATH,"/medios/lang/%s.lng",cfg_readString(cfg,"lngName"));
                    if(lang_loadFile(lang_path)!=MED_OK)
                    {
                        cfg_writeInt(cfg,"lngUseDefault",1);
                        lang_loadDefault();
                        gui_bootError(MISSING_LANG_FILE_ERROR,BOOT_WARN);
                        needWrite=1;
                    }
                }
                else
                {
                    cfg_writeInt(cfg,"lngUseDefault",1);
                    lang_loadDefault();
                    needWrite=1;
                }
            }
            else
            {
                lang_loadDefault();
            }
        }
        else
        {
            if(cfg_itemExists(cfg,"lngName"))
            {
                snprintf(lang_path,MAX_PATH,"/medios/lang/%s.lng",cfg_readString(cfg,"lngName"));
                if(lang_loadFile(lang_path)!=MED_OK)
                {
                    cfg_writeInt(cfg,"lngUseDefault",1);
                    lang_loadDefault();
                    gui_bootError(MISSING_LANG_FILE_ERROR,BOOT_WARN);
                }
                else
                {
                    cfg_writeInt(cfg,"lngUseDefault",0);
                }
            }
            else
            {
                cfg_writeInt(cfg,"lngUseDefault",1);
                lang_loadDefault();
            }
            needWrite=1;
        }
        
        
    }
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
}
