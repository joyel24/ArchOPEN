/*
* kernel/gui/gfx/shell/shellmenu.c
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
#include <sys_def/stddef.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <driver/lcd.h>

#include <fs/stdfs.h>
#include <fs/cfg_file.h>

#include <gui/shellmenu.h>
#include <gui/icons.h>
#include <gui/iconmenu.h>
#include <gui/widgetlist.h>

#include <init/boot_error.h>

ICON shellMenu_folderIcon = NULL;
ICON shellMenu_backIcon = NULL;
ICON shellMenu_appIcon = NULL;

WIDGETLIST shellMenu_menuList = NULL;
ICONMENU shellMenu_rootMenu = NULL;

typedef struct SHELLMENU_ITEM_STRUCT * SHELLMENU_ITEM;

int shellHasCaption;

struct SHELLMENU_ITEM_STRUCT {
    char * name;
    char * param;
    char * command;
    char * icon;

    int event;
    bool global;

    bool listView;

    SHELLMENU_ITEM parent;
    SHELLMENU_ITEM next;
};

SHELLMENU_ITEM shellMenu_firstWidget = NULL;
SHELLMENU_ITEM shellMenu_lastItem = NULL;

bool rootListView=false;

int icon_size[] = {40,64,80};

static void shellMenu_onClick(ICONMENU menu, ICONMENU_ITEM menuItem){
    SHELLMENU_ITEM item;

    item=menuItem->data;

    if(item==NULL){
        printk("[shell menu] critical error: onClick on a menuItem with NULL as shellMenu item\n");
        return;
    }

    if(item->command!=NULL){
        // execute corresponding command
        shell_execute(item->command,item->param);
    }else{
        printk("[shell menu] error: onClick on a menuItem with no associated command\n");
    }
}

static SHELLMENU_ITEM shellMenu_addItem(){
    SHELLMENU_ITEM item;

    // alloc memory
    item=malloc(sizeof(*item));

    // init members
    item->name=NULL;
    item->param=NULL;
    item->command=NULL;
    item->icon=NULL;
    item->event=NO_EVENT;
    item->global=false;
    item->listView=SHELL_MENU_MODE;
    item->parent=NULL;
    item->next=NULL;

    // handle linked list
    if(shellMenu_firstWidget==NULL) shellMenu_firstWidget=item;
    if(shellMenu_lastItem!=NULL) shellMenu_lastItem->next=item;
    shellMenu_lastItem=item;

    return item;
}

static bool shellMenu_parse(char * filename){
    char * name;
    char * value;
    SHELLMENU_ITEM item = NULL;
    CFG_DATA * data;

    data=cfg_readFile(filename);
    if(data==NULL)
    {
        gui_bootError(MISSING_MENU_FILE_ERROR,BOOT_ERROR);
        return false;
    }

    printk("[shell menu] parsing '%s' (tick=%d)\n",filename,tick);

    cfg_rewindItems(data);

    while(cfg_nextItem(data,&name,&value)){
        if(!strcmp(name,"name")){
            item=shellMenu_addItem();
            item->name=strdup(value);

        }else if (item==NULL){
            if(!strcmp(name,"flags")){
                if(strstr(value,"list_view")){
                    rootListView=true;
                };
            }else{
                printk("[shell menu] parse error: '%s' not allowed before 'name'\n",name);
            }
        }else if(!strcmp(name,"parent")){
            SHELLMENU_ITEM parent;

            // find parent
            parent=shellMenu_firstWidget;
            while(parent!=NULL){
                if(!strcmp(parent->name,value)){
                    item->parent=parent;
                    break;
                }

                parent=parent->next;
            }

            if(item->parent==NULL) printk("[shell menu] parse error: parent '%s' not found for '%s'\n",value,item->name);

        }else if(!strcmp(name,"param")){
            item->param=strdup(value);

        }else if(!strcmp(name,"command")){
            item->command=strdup(value);

        }else if(!strcmp(name,"icon")){
            item->icon=strdup(value);

        }else if(!strcmp(name,"event")){
            item->event=atoi(value);

        }else if(!strcmp(name,"flags")){
            if(strstr(value,"global_event")){
                item->global=true;
            };
            if(strstr(value,"list_view")){
                item->listView=true;
            };
        }else{
            printk("[shell menu] parse error: unknown parameter '%s'\n",name);
        }
    }

    cfg_clear(data);

    printk("[shell menu] parsing '%s' success (tick=%d)\n",filename,tick);

    return true;
}

static bool shellMenu_build(){
    SHELLMENU_ITEM item;
    ICONMENU menu;
    ICONMENU_ITEM menuItem;
    WIDGET wi;
    ICON icon;

    printk("[shell menu] build (tick=%d)\n",tick);

    // create menu list & root menu
    shellMenu_menuList=widgetList_create();
    shellMenu_menuList->setRect(shellMenu_menuList,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    shellMenu_menuList->repaintAllWidgets=false;

    shellMenu_rootMenu=iconMenu_create();
    shellMenu_rootMenu->setRect(shellMenu_rootMenu,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
    shellMenu_rootMenu->menuList=shellMenu_menuList;
    shellMenu_rootMenu->onClick=(MENU_CLICKEVENT)shellMenu_onClick;

    if(!shellHasCaption)
        iconMenu_setCaptionType(shellMenu_rootMenu,IM_NO_CAPTION);

    if(rootListView){
        shellMenu_rootMenu->setItemSize(shellMenu_rootMenu,SHELL_MENU_LISTVIEW_WIDTH,SHELL_MENU_LISTVIEW_HEIGHT);
    }else{
        shellMenu_rootMenu->setItemSize(shellMenu_rootMenu,icon_size[folderType],icon_size[folderType]);
    }

    shellMenu_menuList->addWidget(shellMenu_menuList,shellMenu_rootMenu);
    //shellMenu_menuList->setFocusedWidget(shellMenu_menuList,shellMenu_rootMenu);

    item=shellMenu_firstWidget;

    // create items & submenus
    while(item!=NULL){

        if(item->event==NO_EVENT){ // event items shouldn't appear in the menu

            menu=shellMenu_rootMenu;

            if(item->parent!=NULL){
                menu=NULL;

                // find the menu of the item
                for(wi=shellMenu_menuList->firstWidget;wi!=NULL;wi=wi->nxt){
                    if(wi->data==item->parent){
                        menu=(ICONMENU)wi;
                        break;
                    }
                }

                // if it does not exist, build it and link the corresponding folder item
                if(menu==NULL){
                    MENU parentMenu = NULL;

                    // find the parent menu of the menu
                    for(wi=shellMenu_menuList->firstWidget;wi!=NULL;wi=wi->nxt){
                        if(wi->data==item->parent->parent){
                            parentMenu=(MENU)wi;
                            break;
                        }
                    }

                    // find the folder item in the parent menu of the menu
                    menuItem=NULL;
                    if(parentMenu!=NULL){
                        for(wi=parentMenu->firstWidget;wi!=NULL;wi=wi->nxt){
                            if(wi->data==item->parent){
                                menuItem=(ICONMENU_ITEM)wi;
                                break;
                            }
                        }
                    }

                    if(parentMenu==NULL || menuItem==NULL){
                        printk("[shell menu] critical error: attempting to build an orphan menu\n");
                        return false;
                    }

                    // build menu
                    menu=iconMenu_create();
                    menu->setRect(menu,0,SHELL_STATUS_HEIGHT,LCD_WIDTH,LCD_HEIGHT-SHELL_STATUS_HEIGHT);
                    menu->menuList=shellMenu_menuList;
                    menu->parentMenu=parentMenu;
                    menu->data=item->parent;
                    menu->onClick=(MENU_CLICKEVENT)shellMenu_onClick;

                    if(!shellHasCaption)
                        iconMenu_setCaptionType(menu,IM_NO_CAPTION);

                    if(item->parent->listView)
                    {
                        menu->setItemSize(menu,SHELL_MENU_LISTVIEW_WIDTH,SHELL_MENU_LISTVIEW_HEIGHT);
                    }else
                    {
                        menu->setItemSize(menu,icon_size[folderType],icon_size[folderType]);
                    }

                    shellMenu_menuList->addWidget(shellMenu_menuList,menu);

                    // link the folder item to the menu
                    menuItem->subMenu=(MENU)menu;
                }
            }

            // build menu item
            menuItem=iconMenuItem_create();
            menuItem->data=item;
            menuItem->setCaption(menuItem,item->name);

            if((item->parent!=NULL && item->parent->listView) || (item->parent==NULL && rootListView)){
                menuItem->setIPosition(menuItem,IMIP_LEFT);
            }else{
                menuItem->setIPosition(menuItem,IMIP_TOP);
            }

            icon=NULL;
            if(item->icon==NULL){
                if(item->command==NULL){ // no command -> folder
                    icon=shellMenu_folderIcon;
                }else{
                    icon=shellMenu_appIcon;
                }
            }else{
                icon=icon_loadForce(item->icon);
            }

            if(icon!=NULL){
                menuItem->icon=icon->bmap_data;
            }

            menu->addItem(menu,menuItem);
        }

        item=item->next;
    }


    shellMenu_menuList->setFocusedWidget(shellMenu_menuList,shellMenu_rootMenu);

    printk("[shell menu] build done (tick=%d)\n",tick);

    return true;
}

void shellMenu_handleEvent(int event){
    SHELLMENU_ITEM item;

    shellMenu_menuList->handleEvent(shellMenu_menuList,event);

    // handle event based items
    item=shellMenu_firstWidget;
    while(item!=NULL){
        if(item->event==event){

            if(item->global || item->parent==shellMenu_menuList->focusedWidget->data){
                // execute corresponding command
                shell_execute(item->command,item->param);

            }
        }

        item=item->next;
    }
}

void shellMenu_printItems(){
    SHELLMENU_ITEM i;

    i=shellMenu_firstWidget;
    while(i){
        printk("* name='%s'",i->name);
        if (i->event) printk(" event=%d%s",i->event,(i->global)?" global":"");
        if (i->parent) printk(" parent='%s'",i->parent->name);
        if (i->param) printk(" param='%s'",i->param);
        if (i->command) printk(" command='%s'",i->command);
        if (i->icon) printk(" icon='%s'",i->icon);

        printk("\n");

        i=i->next;
    }
}

bool shellMenu_init(){

    rootListView=SHELL_MENU_MODE;
    shellMenu_firstWidget = NULL;
    shellMenu_lastItem = NULL;
    // load icons
    shellMenu_folderIcon = icon_loadForce(SHELL_FOLDER_ICON);
    shellMenu_backIcon = icon_loadForce(SHELL_BACK_ICON);
    shellMenu_appIcon = icon_loadForce(SHELL_APP_ICON);

    // parse menu file
    if(!shellMenu_parse(SHELL_MENU_FILE)){
        printk("[shell menu] error reading menu file, stopping\n");
        return false;
    }

    // build menu
    if(!shellMenu_build()){
        return false;
    };

    return true;
}

void shellMenu_close(){
    SHELLMENU_ITEM item;
    SHELLMENU_ITEM prev;

    // free widgets
    if(shellMenu_menuList!=NULL){
        shellMenu_menuList->destroy(shellMenu_menuList);
    }

    // free item list
    item=shellMenu_firstWidget;
    while(item!=NULL){
        // free strings
        if (item->name!=NULL) kfree(item->name);
        if (item->param!=NULL) kfree(item->param);
        if (item->command!=NULL) kfree(item->command);
        if (item->icon!=NULL) kfree(item->icon);

        // get next item pointer before freeing item
        prev=item;

        item=item->next;

        kfree(prev);
    }
}
