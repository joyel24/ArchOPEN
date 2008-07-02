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
#include <gui/virtKbd.h>
#include <gui/scrollbar.h>

#include <kernel/evt.h>
#include <kernel/kernel.h>
#include <kernel/lang.h>

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
    char fullname3[PATHLEN];
    char fullname4[PATHLEN];
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
                contMenu->paint(contMenu);
                break;
            case 1: /*Create Folder*/
                fullname[0]=0;
                virtKbd(evt_handler,fullname);       
                if(fullname[0]==0)
                    break;
                snprintf(fullname2,PATHLEN,"%s/%s",curBdata->path[1]==0?"":curBdata->path,fullname);
                printk("create |%s|\n",fullname2);
                sprintf(fullname3,getLangStr(STRLNG_BROWSER_CREATE_NAME),fullname);
                if(msgBox_show(fullname,getLangStr(STRLNG_BROWSER_SURE),
                   MSGBOX_TYPE_YESNO, MSGBOX_ICON_QUESTION,evt_handler) != MSGBOX_YES)
                    break;
                ret_val=mkdir(fullname2,0);
                if(ret_val!=MED_OK)
                {
                    sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                    msgBox_show(fullname,getLangStr(STRLNG_BROWSER_CREATING_FOLDER), 
                                MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                    printk("Error: %d - while creating folder\n",-ret_val);
                    break;
                }
                browser_loadFoler(curBdata,NULL);
                break;
            case 2: /*Delete*/
                if(msgBox_show(getLangStr(STRLNG_BROWSER_DELETE_SELECTED),getLangStr(STRLNG_BROWSER_SURE),
                   MSGBOX_TYPE_YESNO, MSGBOX_ICON_QUESTION,evt_handler) != MSGBOX_YES)
                    break;
                
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
                                sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                                msgBox_show(fullname,getLangStr(STRLNG_BROWSER_DELETING_FILE), 
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                printk("Error: %d - Stopping rm loop\n",-ret_val);
                                break;
                            }
                        }
                        else
                        {
                            printk("rm folder: %s\n",fullname);
                            ret_val=rmdir(fullname,1);
                            if(ret_val!=MED_OK)
                            {
                                if(ret_val==-MED_ENOTEMPTY)
                                    
                                    msgBox_show(getLangStr(STRLNG_ERROR),getLangStr(STRLNG_BROWSER_NOT_EMPTY), 
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                else
                                {
                                    sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                                    msgBox_show(fullname,getLangStr(STRLNG_BROWSER_DELETING_FOLDER), 
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
                printk("File type= %d\n",selected_entry->type);
                if(selected_entry->type!=TYPE_FILE && selected_entry->type!=TYPE_DIR)
                    break;
                strcpy(fullname3,selected_entry->name);
                virtKbd(evt_handler,fullname3);   
                snprintf(fullname,PATHLEN,"%s/%s",curBdata->path[1]==0?"":curBdata->path,selected_entry->name);
                snprintf(fullname2,PATHLEN,"%s/%s",curBdata->path[1]==0?"":curBdata->path,fullname3);             
                printk("Rename %s to %s\n",fullname,fullname2);
                if(strcmp(fullname,fullname2)!=0)
                {
                    sprintf(fullname4,getLangStr(STRLNG_BROWSER_NEW_NAME),fullname3);
                    sprintf(fullname3,getLangStr(STRLNG_BROWSER_RENAME),selected_entry->name);
                    if(msgBox_show(fullname3,fullname4,
                       MSGBOX_TYPE_YESNO, MSGBOX_ICON_QUESTION,evt_handler) != MSGBOX_YES)
                        break;
                    if(selected_entry->type==TYPE_FILE)
                        ret_val=mvfile(fullname,fullname2);
                    else
                        ret_val=mvdir(fullname,fullname2);
                    if(ret_val!=MED_OK)
                    {
                        sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                        msgBox_show(fullname,getLangStr(STRLNG_BROWSER_RENAMING_FILE), 
                                    MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                        printk("Error: %d - while renaming\n",-ret_val);
                    }
                } 
                browser_loadFoler(curBdata,NULL);                      
                break;
            case 6: /*open*/
                snprintf(fullname,PATHLEN,"%s/%s",curBdata->path[1]==0?"":curBdata->path,selected_entry->name);
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
                if(!curBdata->is_dual || curBdata->dual_mode==0 || strcmp(curBdata->path,curBdata->dual->path)==0)
                {
                    printk("Error should not do copy!!!\n");
                    break;
                }
                
                if(msgBox_show(getLangStr(STRLNG_BROWSER_COPYING_SELECTED),getLangStr(STRLNG_BROWSER_SURE),
                   MSGBOX_TYPE_YESNO, MSGBOX_ICON_QUESTION,evt_handler) != MSGBOX_YES)
                    break;
                
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
                            printk("copy file: %s to %s\n",fullname,fullname2);
                            ret_val=cpfile(fullname,fullname2);
                            if(ret_val!=MED_OK)
                            {
                                sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                                msgBox_show(fullname,getLangStr(STRLNG_BROWSER_COPYING_FILE), 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                printk("Error: %d - Stopping copy loop\n",-ret_val);
                                break;
                            }
                        }
                        else if(selected_entry->type==TYPE_DIR)
                        {
                            msgBox_show("Copy folder not supported","We need a fction in VFS",
                                        MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                        }
                    }
                }
                selected_entry->selected=0;
                /* have probably change the dest folder => reload it*/
                browser_loadFoler(curBdata->dual,NULL);
                break;
            case 8: /*move*/
                /* select current item */
                if(!curBdata->is_dual || curBdata->dual_mode==0 || strcmp(curBdata->path,curBdata->dual->path)==0)
                {
                    printk("Error should not do move!!!\n");
                    break;
                }
                
                if(msgBox_show(getLangStr(STRLNG_BROWSER_MOVE_SELECTED),getLangStr(STRLNG_BROWSER_SURE),
                   MSGBOX_TYPE_YESNO, MSGBOX_ICON_QUESTION,evt_handler) != MSGBOX_YES)
                    break;
                
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
                                sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                                msgBox_show(fullname,getLangStr(STRLNG_BROWSER_MOVING_FILE), 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);
                                printk("Error: %d - Stopping move loop\n",-ret_val);
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
                                    sprintf(fullname,getLangStr(STRLNG_ERROR_RETVAL),-ret_val);
                                    msgBox_show(fullname,getLangStr(STRLNG_BROWSER_MOVING_FOLDER), 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler);   
                                    printk("Error: %d - Stopping rm loop\n",-ret_val);
                                    break;
                                }     
                            }
                            else
                            {
                                msgBox_show(getLangStr(STRLNG_BROWSER_ERROR_MOVING),getLangStr(STRLNG_BROWSER_IS_SUBFOLDER), 
                                            MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR,evt_handler); 
                                printk("trying to move a subfolder!!!\n");
                                break;
                            }
                        }
                    }
                }
                selected_entry->selected=0;
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
    
       
    contMenu=widgetMenu_create();
    contMenu->setRect(contMenu,contM_X+5,contM_Y+5,contM_W-10,contM_H-10);
    contMenu->ownWidgets=true; // the menu will handle items destroy
    contMenu->onClick=(MENU_CLICKEVENT)contMenu_onClick;
    contMenu->menuList=NULL;
    contMenu->hasScrollBar=1;

    /*0*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,selected_entry->type==TYPE_FILE ?
                   getLangStr(STRLNG_BROWSER_FILE_INFO) : 
                   getLangStr(STRLNG_BROWSER_FOLDER_INFO));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;
            
    
    /*1*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,getLangStr(STRLNG_BROWSER_CREATE_FOLDER));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*2*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,nbSelected(bdata)>0 ? 
            getLangStr(STRLNG_BROWSER_DELETE_SELECTED) : 
            (selected_entry->type==TYPE_FILE ? 
                    getLangStr(STRLNG_BROWSER_DELETE_FILE) : 
                    getLangStr(STRLNG_BROWSER_DELETE_FOLDER)));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*3*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,selected_entry->type==TYPE_FILE ? 
            getLangStr(STRLNG_BROWSER_SELECT_FILE) : 
            getLangStr(STRLNG_BROWSER_SELECT_FOLDER));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*4*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,nbSelected(curBdata)==curBdata->listused ? 
            getLangStr(STRLNG_BROWSER_UNSELECT_ALL) : 
            getLangStr(STRLNG_BROWSER_SELECT_ALL));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*5*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,selected_entry->type==TYPE_FILE ?
            getLangStr(STRLNG_BROWSER_RENAME_FILE) : 
            getLangStr(STRLNG_BROWSER_RENAME_FOLDER));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;

    /*6*/
    mi=widgetMenuItem_create();
    mi->setCaption(mi,selected_entry->type==TYPE_FILE ?
            getLangStr(STRLNG_BROWSER_OPEN_FILE):getLangStr(STRLNG_BROWSER_OPEN_FOLDER));
    mi->foreColor=MENU_COLOR;
    contMenu->addItem(contMenu,mi);
    miTab[i++]=mi;
        
    if(bdata->is_dual && bdata->dual_mode != 0 && strcmp(bdata->path,bdata->dual->path))
    {
        /*7*/
        mi=widgetMenuItem_create();
        mi->setCaption(mi,nbSelected(bdata)>0 ?
                getLangStr(STRLNG_BROWSER_COPY_SELECTED) :
                (selected_entry->type==TYPE_FILE ?
                        getLangStr(STRLNG_BROWSER_COPY_FILE) :
                        getLangStr(STRLNG_BROWSER_COPY_FOLDER)));
        mi->foreColor=MENU_COLOR;
        contMenu->addItem(contMenu,mi);
        miTab[i++]=mi;

        /*8*/
        mi=widgetMenuItem_create();
        mi->setCaption(mi,nbSelected(bdata)>0 ?
                getLangStr(STRLNG_BROWSER_MOVE_SELECTED) :
                (selected_entry->type==TYPE_FILE ?
                        getLangStr(STRLNG_BROWSER_MOVE_FILE) :
                        getLangStr(STRLNG_BROWSER_MOVE_FOLDER)));
        mi->foreColor=MENU_COLOR;
        contMenu->addItem(contMenu,mi);
        miTab[i++]=mi;
    }
            
    printk("Contextual Menu Created\n");
}

void browser_contMenu(struct browser_data * bdata)
{
    int event;

    browser_ContM_exit=0;
    curBdata=bdata;
    
    
    browser_mkMenu(bdata);
    
    printk("Initial Draw\n");
    contMenu->paint(contMenu);
    do{
        event=evt_getStatusBlocking(evt_handler);
        if (!event) continue; // no new events
        contMenu->handleEvent(contMenu,event);
    }while(event!=BTN_OFF && !browser_ContM_exit);
    
}


