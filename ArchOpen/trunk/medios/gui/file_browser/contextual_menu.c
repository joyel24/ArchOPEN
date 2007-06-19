/*
* kernel/gfx/gui/file_browser/contextual_menu.c
*
*   mediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <gfx/graphics.h>

#include <gui/file_browser.h>

#include <gui/widgetmenu.h>
#include <gui/widgetlist.h>
#include <gui/msgBox.h>
#include <gui/shell.h>

#include <kernel/evt.h>
#include <kernel/kernel.h>

#include <driver/lcd.h>

#include <sys_def/colordef.h>
#include <sys_def/errors.h>

#include <lib/string.h>

#include <fs/stdfs.h>

#define contM_W 200
#define contM_H 105

#define contM_X (LCD_WIDTH-contM_W)/2
#define contM_Y (LCD_HEIGHT-contM_H)/2

#define CONT_MENU_NB_ITEM 9

WIDGETLIST menuList;
WIDGETMENU contMenu;

extern int evt_handler;

int browser_ContM_exit;

WIDGETMENU_ITEM miTab[CONT_MENU_NB_ITEM];
struct browser_data * curBdata;

#define MENU_COLOR COLOR_BLACK

/* checks if path2 is a subfolder of path1 */
int browser_isInPath(char * path1,char * path2)
{
    int len1,len2;
    char * ptr1,*ptr2;
    char c;
    len1=strlen(path1);
    len2=strlen(path2);

    printk("[isInPath] %s %s\n",path1,path2);
    
    /*check if path are the same*/
    if(strcmp(path1,path2)==0) return 1;

    /* check str len: subfolder should have a path bigger */
    if(len2<len1) return 0;
    
    while(*path1=='/') path1++;
    while(*path2=='/') path2++;

    while(*path1)
    {
        /*upDir on subfolder*/
        ptr1=strchr(path1,'/');
        if(!ptr1) ptr1=path1+strlen(path1);
        
        ptr2=strchr(path2,'/');
        if(!ptr2) ptr2=path2+strlen(path2);
        
        if((ptr1-path1)!=(ptr2-path2)) return 0;
        
        if(strncmp(path1,path2,ptr1-path1-1)!=0) return 0;
        
        if(*ptr1=='\0') break;

        while(*ptr1=='/') ptr1++;
        while(*ptr2=='/') ptr2++;
        
        path1=ptr1;
        path2=ptr2;
    }
    return 1;
}


void browser_drawInfo(struct dir_entry * dirItem)
{
    printk("Info: %s\n",dirItem->name);
    printk("Size= %d\n",dirItem->size);
    msgBox_show("Info not supported","We need some data to print",
                MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
}

void contMenu_onClick(MENU m, WIDGETMENU_ITEM mi)
{
    int i,j;
    char fullname[PATHLEN];
    char fullname2[PATHLEN];
    MED_RET_T ret_val=MED_OK;
    struct dir_entry * selected_entry=&curBdata->list[curBdata->nselect];
    
    for(i=0;i<CONT_MENU_NB_ITEM;i++)
        if(miTab[i]==mi)
            break;
    
    if(i<CONT_MENU_NB_ITEM && (miTab[i] != NULL))
    {
        switch(i)
        {
            case 0: /*Info*/
                browser_drawInfo(selected_entry);
                menuList->focusedWidget->paint(menuList->focusedWidget);
                break;
            case 1: /*Create Folder*/
                /* NOTE: need a keyboard emu*/
                msgBox_show("Create not supported","We need a virtual keybord",
                                    MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                break;
            case 2: /*Delete*/
                /* select current item */
                selected_entry->selected=1;
                /* go thru the list */
                for(i=0;i<curBdata->listused;i++)
                {
                    if(curBdata->list[i].selected)
                    {
                        snprintf(fullname,PATHLEN,"%s/%s",curBdata->path[1]==0?"":curBdata->path,curBdata->list[i].name);
                        if(curBdata->list[i].type==TYPE_FILE)
                        {
                            printk("rm file: %s\n",fullname);
                            ret_val=rmfile(fullname);
                            if(ret_val!=MED_OK)
                            {
                                sprintf(fullname,"Error (%d)",-ret_val);
                                msgBox_show(fullname,"Removing file", 
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                printk("Error: %d - Stopping rm loop\n",-ret_val);
                                break;
                            }
                        }
                        else
                        {
                            printk("rm folder: %s\n",fullname);
                            ret_val=rmdir(fullname,0); /*NOTE: not using recurse for now*/
                            if(ret_val!=MED_OK)
                            {
                                if(ret_val==-MED_ENOTEMPTY)
                                    
                                    msgBox_show("Error","Folder isn't empty", 
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                else
                                {
                                    sprintf(fullname,"Error (%d)",-ret_val);
                                    msgBox_show(fullname,"Removing Folder", 
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                }
                                printk("Error: %d - Stopping rm loop\n",-ret_val);
                                break;
                            }
                        }
                    }
                }
                /* we have probably change the list content => reload it */
                browser_loadFoler(curBdata,NULL);
                if(curBdata->is_dual && curBdata->dual_mode!=0)
                {
                    /* NOTE: we should test reloading the dual folder while it has been removed from cur screen*/
                    browser_loadFoler(curBdata->dual,NULL);
                }
                break;
            case 3: /*select*/
                chgSelect(curBdata,curBdata->nselect);
                break;
            case 4: /*select all*/
                if(nbSelected(curBdata)==curBdata->listused)
                    j=0;
                else
                    j=1;
                for(i=0;i<curBdata->listused;i++)
                    curBdata->list[i].selected=j;
                break;
            case 5: /*Rename*/
                /* NOTE: need a keyboard */
                msgBox_show("Rename not supported","We need a virtual keybord",
                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                break;
            case 6: /*open*/
                if(selected_entry->type==TYPE_FILE)
                {
                    
                    shell_execute(fullname,NULL);
                    //TODO: not very nice, browser should be able to recieve EVT_REDRAW and process it
                    evt_purgeHandler(evt_handler);
                }
                else
                {
                    if(selected_entry->type==TYPE_BACK)
                    {
                        if(upDir(curBdata))
                            browser_loadFoler(curBdata,NULL);
                    }
                    else
                    {
                        browser_loadFoler(curBdata,fullname);
                    }
                }
                break;
            case 7: /*copy*/
                /* NOTE: need a copy fction in vfs */
                msgBox_show("Copy not supported","We need a copy in VFS", 
                       MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                break;
            case 8: /*move*/
                /* select current item */
                if(!curBdata->is_dual || curBdata->dual_mode==0 || strcmp(curBdata->path,curBdata->dual->path)==0)
                {
                    printk("Error should not do move!!!\n");
                    break;
                }
                selected_entry->selected=1;
                for(i=0;i<curBdata->listused;i++)
                {
                    if(curBdata->list[i].selected)
                    {
                        snprintf(fullname,PATHLEN,"%s/%s",
                                 curBdata->path[1]==0?"":curBdata->path,
                                 curBdata->list[i].name);
                        snprintf(fullname2,PATHLEN,"%s/%s",
                                 curBdata->dual->path[1]==0?"":curBdata->dual->path,
                                 curBdata->list[i].name);
                        if(selected_entry->type==TYPE_FILE)
                        {
                            printk("move file: %s to %s\n",fullname,fullname2);
                            ret_val=mvfile(fullname,fullname2);
                            if(ret_val!=MED_OK)
                            {
                                sprintf(fullname,"Error (%d)",-ret_val);
                                msgBox_show(fullname,"Moving File", 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                printk("Error: %d - Stopping rm loop\n",-ret_val);
                                break;
                            }
                        }
                        else
                        {
                            printk("move folder: %s to %s\n",fullname,fullname2);
                            if(!browser_isInPath(fullname,fullname2))
                            {
                                ret_val=mvdir(fullname,fullname2);
                                if(ret_val!=MED_OK)
                                {
                                    sprintf(fullname,"Error (%d)",-ret_val);
                                    msgBox_show(fullname,"Moving Folder", 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);   
                                    printk("Error: %d - Stopping rm loop\n",-ret_val);
                                    break;
                                }     
                            }
                            else
                            {
                                msgBox_show("Error moving folder","Trying to move a subfolder", 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler); 
                                printk("trying to move a subfolder!!!\n");
                                break;
                            }
                        }
                    }
                }
                /* we have probably change the list content => reload both screen */
                browser_loadFoler(curBdata,NULL);
                browser_loadFoler(curBdata->dual,NULL);
                break;
        }
        browser_ContM_exit=1;
    }
    else
        printk("Item not found!! %s\n",mi->caption);
        
}

void browser_mkMenu(struct browser_data * bdata)
{
    WIDGETMENU_ITEM mi;
    int i;
    struct dir_entry * selected_entry=&bdata->list[bdata->nselect];
    
    for(i=0;i<CONT_MENU_NB_ITEM;i++)
        miTab[i]=NULL;
    i=0;
    gfx_fillRect(COLOR_WHITE,contM_X,contM_Y,contM_W,contM_H);
    gfx_drawRect(COLOR_GRAY,contM_X,contM_Y,contM_W,contM_H);
    gfx_drawRect(COLOR_DARK_GRAY,contM_X+1,contM_Y+1,contM_W-2,contM_H-2);
    
    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;
    
    contMenu=widgetMenu_create();
    contMenu->setRect(contMenu,contM_X+5,contM_Y+5,contM_W-10,contM_H-10);
    contMenu->ownItems=true; // the menu will handle items destroy
    contMenu->onClick=(MENU_CLICKEVENT)contMenu_onClick;
    contMenu->menuList=menuList;
    menuList->addWidget(menuList,contMenu);

    /*0*/
    mi=widgetMenuItem_create();
    mi->caption=selected_entry->type==TYPE_FILE?"File Info":"Folder Info";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;
            
    /*1*/
    mi=widgetMenuItem_create();
    mi->caption="Create Folder";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*2*/
    mi=widgetMenuItem_create();
    mi->caption=nbSelected(bdata)>0?"Delete selected":(selected_entry->type==TYPE_FILE?"Delete file":"Delete Folder");
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*3*/
    mi=widgetMenuItem_create();
    mi->caption=selected_entry->type==TYPE_FILE?"Select file":"Select Folder";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*4*/
    mi=widgetMenuItem_create();
    mi->caption=nbSelected(curBdata)==curBdata->listused?"Unselect All":"Select All";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*5*/
    mi=widgetMenuItem_create();
    mi->caption=selected_entry->type==TYPE_FILE?"Rename file":"Rename Folder";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*6*/
    mi=widgetMenuItem_create();
    mi->caption=selected_entry->type==TYPE_FILE?"Open file":"Open Folder";
    mi->foreColor=MENU_COLOR;
    mi->widgetWidth=0;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;
    
    printk("|%s|%s|",bdata->dual->path,bdata->path);
    
    if(bdata->is_dual && bdata->dual_mode != 0 && strcmp(bdata->path,bdata->dual->path))
    {
        /*7*/
        mi=widgetMenuItem_create();
        mi->caption=nbSelected(bdata)>0?"Copy selected":(selected_entry->type==TYPE_FILE?"Copy file":"Copy Folder");
        mi->foreColor=MENU_COLOR;
        mi->widgetWidth=0;
        contMenu->addItem(contMenu,mi);
        miTab[i++]=mi;

        /*8*/
        mi=widgetMenuItem_create();
        mi->caption=nbSelected(bdata)>0?"Move selected":(selected_entry->type==TYPE_FILE?"Move file":"Move Folder");
        mi->foreColor=MENU_COLOR;
        mi->widgetWidth=0;
        contMenu->addItem(contMenu,mi);
        miTab[i++]=mi;
    }
        
    printk("Contextual Menu Created (nb sel:%d, type:%s)\n",nbSelected(bdata),
           selected_entry->type==TYPE_FILE?"File":"Folder");
}

void browser_contMenu(struct browser_data * bdata)
{
    int event;

    browser_ContM_exit=0;
    curBdata=bdata;
    
    browser_mkMenu(bdata);
    
    menuList->setFocusedWidget(menuList,contMenu);
    menuList->focusedWidget->paint(menuList->focusedWidget);
    printk("Initial Draw\n");
    
    do{
        event=evt_getStatus(evt_handler);
        if (!event) continue; // no new events
        menuList->handleEvent(menuList,event);
    }while(event!=BTN_OFF && !browser_ContM_exit);
    
}


