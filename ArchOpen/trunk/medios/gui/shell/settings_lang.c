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
#include <kernel/evt.h>
#include <kernel/lang.h>
#include <kernel/malloc.h>

#include <gfx/kfont.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/checkbox.h>
#include <gui/msgBox.h>
#include <gui/chooser.h>
#include <gui/file_browser.h>

#include <gui/settings_lang.h>

#include <driver/lcd.h>

#include <fs/cfg_file.h>
#include <fs/stdfs.h>

#define LANG_GUIFONT RADONWIDE

WIDGETLIST menuList;  
CHOOSER langList;

int stop_lang_set;
char lang_path[250];

#define ICON_X 5
#define ICON_Y 5

extern char * customLang;

int nbLang;
char ** langNameList;
int evtHandle;

void cleanLangList(char ** myList,int nb);
void drawLangMenuBG(void);

void cancelBtnLang_click(BUTTON b)
{
    stop_lang_set=1;
}

void okBtnLang_click(BUTTON b)
{
    CFG_DATA * cfg;
    int useDefault=0;
    /* opening config file */
    
    msgBox_info(getLangStr(STRLNG_LOAD_LANG));
    if(langList->index==0)
    {
        printk("Loading Default\n");
        lang_loadDefault();
        useDefault=1;
    }
    else
    {
        printk("Loading: %s\n",langNameList[langList->index]);
        snprintf(lang_path,MAX_PATH,"/medios/lang/%s.lng",langNameList[langList->index]);
        if(lang_loadFile(lang_path)!=MED_OK)
        {
            snprintf(lang_path,MAX_PATH,getLangStr(STRLNG_ERROR_LOADING),langNameList[langList->index]);
            msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),lang_path,
                        MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandle);
            cleanLangList(langNameList,nbLang);
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
            stop_lang_set=1;
            cleanLangList(langNameList,nbLang);
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
        cfg_writeString(cfg,"lngName",langNameList[langList->index]);
    }
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    cleanLangList(langNameList,nbLang);
    stop_lang_set=1;
}

void browserLang_click(BUTTON b)
{
    /*printk("calling browser\n");
    if(browser_simpleBrowse("/medios/lang"  ,myPath)==MED_OK)
    {
        
    }
    printk("Res=%d,|%s|\n",-ret_val,myPath);*/
    msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),"Browser Not available yet here",
                MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandle);
    drawLangMenuBG();
    menuList->paint(menuList);
}

void cleanLangList(char ** myList,int nb)
{
    int i;
    if(nb==0 || myList==NULL)
        return;
    for(i=0;i<nb;i++)
        kfree(myList[i]);
    kfree(myList);   
}

char ** lang_createList(int * nb,int * maxW)
{
    char * cur_lang;
    char ** res;
    int nbLang=1;
    DIR * lang_folder;
    char header[3];
    int fd;
    int w,h;
    int len;
    struct dirent * entry;
    res=(char**)malloc(sizeof(char**));
    res[0]=(char*)malloc(strlen(getLangStr(STRLNG_DEFAULT)));
    strcpy(res[0],getLangStr(STRLNG_DEFAULT));
    gfx_getStringSize(res[0],&w,&h);
    *maxW=w;
    
    lang_folder=opendir("/medios/lang");
    if(lang_folder)
    {
        while((entry=readdir(lang_folder))!=NULL)
        {
            if(entry->type!=VFS_TYPE_FILE)
                continue;
            snprintf(lang_path,MAX_PATH,"/medios/lang/%s",entry->d_name);
            fd=open(lang_path,O_RDONLY);
            if(fd<0)
                continue;
            if(read(fd,header,3)!=3)
            {
                close(fd);
                continue;
            }
            
            if(strncmp(header,"LNG",3))
            {
                close(fd);
                continue;   
            }
            
            nbLang++;
            len=(int)(strrchr(entry->d_name,'.')-(char*)entry->d_name);
            cur_lang=(char*)malloc(len);
            strncpy(cur_lang,entry->d_name,len);
            cur_lang[len]='\0';
            gfx_getStringSize(cur_lang,&w,&h);
            if(*maxW<w)
                *maxW=w;
            if(res==NULL)
                res=(char**)malloc(sizeof(char**));
            else
                res=(char**)realloc(res,nbLang*sizeof(char**));
            res[nbLang-1]=cur_lang;
            close(fd);
        }
        printk("Found %d lang\n",nbLang);
        closedir(lang_folder);        
    }
    else
    {
        cleanLangList(res,nbLang);
        printk("Can't open lang folder\n");
        return NULL;
    }
    *nb=nbLang;
    return res;
}

int minX;
ICON logo;
int txt_x;

void drawLangMenuBG(void)
{

    gfx_clearScreen(COLOR_WHITE);
    gfx_fontSet(LANG_GUIFONT);
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    minX = ICON_X + logo->bmap_data.width;
    gfx_drawLine(COLOR_DARK_BLUE,minX+3,5,minX+3,LCD_HEIGHT-5);
    gfx_drawLine(COLOR_BLUE,minX+4,5,minX+4,LCD_HEIGHT-5);
    gfx_drawLine(COLOR_LIGHT_BLUE,minX+5,5,minX+5,LCD_HEIGHT-5);
    minX+=7;
    gfx_fontSet(STD8X13);
    gfx_putS(COLOR_DARK_GREY,COLOR_WHITE,txt_x,ICON_Y,getLangStr(STRLNG_LANG_SETTINGS));
}

void lang_setting(void)
{       
    int event;
    int w,h,x,y,sepW,sepH,lineH,maxW;
    BUTTON mib;

    nbLang=0;
    evtHandle = evt_getHandler(BTN_CLASS|GUI_CLASS);
    gfx_fontSet(LANG_GUIFONT);
    langNameList=lang_createList(&nbLang,&maxW);
    if(!langNameList)
    {
        msgBox_show(getLangStr(STRLNG_MISSING_FOLDER),"Create /medios/lang and put lng file",
                    MSGBOX_TYPE_OK,MSGBOX_ICON_ERROR,evtHandle);
        return;
    }
    
    if(nbLang==1)
    {
        msgBox_show(getLangStr(STRLNG_LANG_SETTINGS),"No lang file found",
                    MSGBOX_TYPE_OK,MSGBOX_ICON_INFORMATION,evtHandle);
    }
    
    stop_lang_set=0;
    logo=icon_get("lang");
    if(!logo)
        icon_load("lang.ico");
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_LANG_SETTINGS),&w,&h);
    lineH=h+5;
    txt_x=minX+(LCD_WIDTH-minX-w)/2;

    drawLangMenuBG();
    
    x=minX;    
    y=ICON_Y+2*lineH;
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;

    gfx_fontSet(LANG_GUIFONT);
    
    // standardMenu
    
    langList=chooser_create();
    langList->items=langNameList;
    langList->itemCount=nbLang;
    /* NOTE:need something to find which lang is currently selected */
    langList->index=0;
    langList->font=LANG_GUIFONT;
    gfx_getStringSize(langNameList[0],&w,&h);
    langList->setRect(langList,x,y,maxW+29,h+1);
    langList->wrap=WIDGET_WRAP_ON;
    langList->orientation=WIDGET_ORIENTATION_HORIZ;
    menuList->addWidget(menuList,langList);
            
    y+=lineH;

    gfx_getStringSize(getLangStr(STRLNG_OK),&sepW,&sepH);
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OK); 
    mib->font=LANG_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)okBtnLang_click;
    mib->clickOnRightLeft=0;
    menuList->addWidget(menuList,mib);
    
    x=x+sepW+4+4;

    gfx_getStringSize(getLangStr(STRLNG_CANCEL),&sepW,&sepH);
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_CANCEL); 
    mib->font=LANG_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)cancelBtnLang_click;
    mib->clickOnRightLeft=0;
    menuList->addWidget(menuList,mib);

    x=minX;
    y+=lineH; 


    gfx_getStringSize(getLangStr(STRLNG_OTHER),&sepW,&sepH);
    
    mib=button_create();
    mib->caption=getLangStr(STRLNG_OTHER);
    mib->font=LANG_GUIFONT;
    mib->setRect(mib,x,y,sepW+4,sepH+2);
    mib->onClick=(BUTTON_CLICKEVENT)browserLang_click;
    mib->clickOnRightLeft=0;
    menuList->addWidget(menuList,mib);
    
    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,langList);
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
    }while(event!=BTN_OFF && !stop_lang_set); 
       
    menuList->destroy(menuList);
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
