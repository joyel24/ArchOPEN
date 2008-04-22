/*
* kernel/gui/internal_plugin/gui_fct.c
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
#include <sys_def/colordef.h>
#include <sys_def/errors.h>

#include <kernel/kernel.h>
#include <kernel/evt.h>
#include <kernel/malloc.h>

#include <fs/file_type.h>

#include <gfx/graphics.h>

#include <gui/icons.h>
#include <gui/file_browser.h>
#include <gui/settings_browser.h>
#include <gui/status_line.h>

#include <driver/lcd.h>

BITMAP * gui_ls_upBitmap;
BITMAP * gui_ls_dwBitmap;
BITMAP * gui_ls_dirBitmap;
BITMAP * gui_ls_mp3Bitmap;
BITMAP * gui_ls_textBitmap;
BITMAP * gui_ls_imageBitmap;

//#include "browser_icons.h"

int evt_handler;

void iniBrowser(void)
{
    gui_ls_upBitmap=&icon_get("upBitmap")->bmap_data;
    gui_ls_dwBitmap=&icon_get("dwBitmap")->bmap_data;
    gui_ls_dirBitmap=&icon_get("dirBitmap")->bmap_data;

    if((evt_handler=evt_getHandler(ALL_CLASS))<0)
    {
        printk("Can't get evt handler (error:%d)\n",-evt_handler);
        evt_handler=-1;
    }
    gui_ls_mp3Bitmap=&icon_get("mp3Bitmap")->bmap_data;
    gui_ls_textBitmap=&icon_get("textBitmap")->bmap_data;
    gui_ls_imageBitmap=&icon_get("imageBitmap")->bmap_data;
    
    /* getting default setting*/
    browser_loadCfg();
}

/* oldname is used to go up in the dir tree */

MED_RET_T viewNewDir(struct browser_data *bdata,char *name,char * oldName)
{
    MED_RET_T ret_val;
    ret_val=browser_loadFoler(bdata,name);
    int pos=0;
    if(ret_val!=MED_OK)
        return ret_val;
    if(oldName!=NULL)
    {
        pos=findName(bdata,oldName);
        if(pos>4)
        {
            bdata->pos=pos-4;
            bdata->nselect=4;
        }
        else
        {
            bdata->pos=0;
            bdata->nselect=pos;   
        }
    }
    redrawBrowser(bdata);
    
    return MED_OK;
}

void browser_screenSize(struct browser_data *bdata)
{
    int fw,fh;

    if(browser_has_statusbar)
    {
        bdata->y_start += SHELL_STATUS_HEIGHT;
        bdata->height -= SHELL_STATUS_HEIGHT;
    }
    
    bdata->nb_disp_entry=(bdata->height-BROWSER_STATUS_HEIGHT)/bdata->entry_height;
    gfx_getStringSize("M",&fw,&fh);
    bdata->max_entry_length=(bdata->width-
            (bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0)-
            BROWSER_ICON_WIDTH)/fw;

    bdata->browser_scroll.x=bdata->x_start
            +(bdata->scroll_pos==LEFT_SCROLL?1:bdata->width-BROWSER_SCROLLBAR_WIDTH);
    bdata->browser_scroll.y=bdata->y_start;
    bdata->browser_scroll.height=bdata->nb_disp_entry*bdata->entry_height;
}

void browser_computeSize(struct browser_data *bdata)
{
    if(BROWSE_IS_DUAL)
    {
        /*dual screen mode => recompute position/width*/
        
        if(bdata->dual_mode==1)
        {        
            bdata->x_start=0;
            bdata->y_start=0;
            bdata->width=2*LCD_WIDTH/3;
            bdata->height=LCD_HEIGHT;
        
            bdata->dual->x_start=2*LCD_WIDTH/3;
            bdata->dual->y_start=0;
            bdata->dual->width=LCD_WIDTH/3;
            bdata->dual->height=LCD_HEIGHT;
        }
        else
        {
            bdata->x_start=0;
            bdata->y_start=0;
            bdata->width=LCD_WIDTH/3;
            bdata->height=LCD_HEIGHT;
        
            bdata->dual->x_start=LCD_WIDTH/3;
            bdata->dual->y_start=0;
            bdata->dual->width=2*LCD_WIDTH/3;
            bdata->dual->height=LCD_HEIGHT;
        }            
        
        browser_screenSize(bdata);
        browser_screenSize(bdata->dual);
    }
    else
    {
        /* single screen mode => recompute position/width*/
        bdata->x_start=0;
        bdata->y_start=0;
        bdata->width=LCD_WIDTH;
        bdata->height=LCD_HEIGHT;
        
        browser_screenSize(bdata);
    }
}

int browser_loadFoler(struct browser_data *bdata,char * path)
{
    if(path!=NULL)
    {
        if(strlen(path)>PATHLEN)
        {
           // msgBox("ERROR - Browser", "Can't load dir, path too long", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
            return -MED_ERROR;
        }
        strcpy(bdata->path,path);
    }


    cleanList(bdata);
    if(!doLs(bdata))
    {
        cleanList(bdata);
        //msgBox("ERROR - Browser", "Can't load dir", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
        return -MED_ERROR;
    }
    
    bdata->pos=0;
    bdata->nselect=0;
    
    return MED_OK;
}

void browser_doDraw(struct browser_data *bdata)
{
    if(BROWSE_IS_DUAL)
    {
        redrawBrowser(bdata);
        redrawBrowser(bdata->dual);
    }
    else
        redrawBrowser(bdata);
}

int browser_browse(struct browser_data *bdata,char * path,char * res)
{
    int ret_val;
        
    /* load data from disk*/
    if(bdata->is_dual!=0 && bdata->dual!=0)
    {
        ret_val=browser_loadFoler(bdata,path); 
        if(ret_val!=MED_OK)
            return ret_val;
        ret_val=browser_loadFoler(bdata->dual,path); 
        if(ret_val!=MED_OK)
            return ret_val;
    }
    else
    {
        ret_val=browser_loadFoler(bdata,path); 
        if(ret_val!=MED_OK)
            return ret_val;
    }
    
    /* compute screen sizes*/
    browser_computeSize(bdata);
    
    /*initial draw*/
    browser_doDraw(bdata);

    /*draw status bar if needed*/
    if(browser_has_statusbar)
    {
        statusLine_init();
#ifdef ALWAYS_DISPLAY_ST_LINE
        drawStatusLine();
#endif
    }
    
    /* start evt loop */
    
    if(browserEvt(bdata)!=MED_OK)
        return -MED_ERROR;
    
    if(bdata->mode==MODE_STRING && res)
    {
        if(bdata->path[0]=='/' && bdata->path[1]=='\0')
            sprintf(res,"/%s",bdata->list[bdata->pos+bdata->nselect].name);
        else
            sprintf(res,"%s/%s",bdata->path,bdata->list[bdata->pos+bdata->nselect].name);
    }
    return MED_OK;
}

int browser_simpleBrowse(char * path,char * res)
{
    int ret_val;
    struct browser_data * browseData=browser_NewBrowse();
    if(!browseData)
        return -MED_ENOMEM;
    browseData->mode=MODE_STRING;
    ret_val=browser_browse(browseData,path,res);
    browser_disposeBrowse(browseData);
    return ret_val;
}

void redrawBrowser(struct browser_data *bdata)
{
    gfx_fontSet(bdata->font);
    /*clear whole screen */
    gfx_fillRect(COLOR_TRSP,
                 bdata->x_start,bdata->y_start,
                 bdata->width,bdata->height);
    
    /*printk("erase %d (%d,%d) size %d/%d\n", bdata->is_dual,bdata->x_start,bdata->y_start,
        bdata->width+1,bdata->height);*/
    
    draw_scrollBar(&bdata->browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
    
    printAllName(bdata);
    
}

void clearBrowser(struct browser_data *bdata)
{
    cleanList(bdata);
    gfx_fillRect(COLOR_TRSP,bdata->x_start,bdata->y_start,bdata->width,bdata->height);
    clear_status(bdata);
}

void browser_computeNamePos(struct browser_data *bdata,int * x,int * y,int * w,int * h)
{
    if(x)
        *x=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0);
    if(y)
        *y=bdata->y_start;
    if(w)
        *w=bdata->width-BROWSER_SCROLLBAR_WIDTH;
    if(h)
        *h=bdata->entry_height;
}

void printName(struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata)
{
    int             color=COLOR_BLACK;
    int             select_color=COLOR_BLUE;
    char *          cp;
    int             type;
    int             H,X,Y,W;

    browser_computeNamePos(bdata,&X,&Y,&W,&H);
    Y+=pos*H;
    
    //printk("[printName] pos=%d,selected=%d,clear=%d (%d,%d)-dim %d/%d (%d)\n",pos,selected,clear,X,Y,W,H,bdata->entry_height);
    
    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        gfx_fillRect(COLOR_TRSP, X, Y , W, H);
    
    switch(dEntry->type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            gfx_drawBitmap(gui_ls_dirBitmap, X+2, Y);
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;

            type=get_file_type(dEntry->name);
            switch(type)
            {
                case IMG_TYPE:
                    gfx_drawBitmap(gui_ls_imageBitmap, X+2, Y);
                    break;
                case MP3_TYPE:
                    gfx_drawBitmap(gui_ls_mp3Bitmap, X+2, Y);
                    break;
                case TXT_TYPE:
                    gfx_drawBitmap(gui_ls_textBitmap, X+2, Y);
                    break;
                case BIN_TYPE:
                case UKN_TYPE:
                default:
                    gfx_fillRect(COLOR_TRSP, X+2, Y, 8, 8); /* no icon */
                    break;
            }
            break;
    }
    
    if(BROWSER_IS_FOCUSED)
    {
        if(selected)
        {
            if(dEntry->selected)
                select_color=COLOR_ORANGE2;
            draw_file_size(bdata,dEntry);
        }
        else
        {
            if(dEntry->selected)
                select_color=COLOR_ORANGE;
            else
                select_color= COLOR_TRSP;
        }
    }
    else
        select_color= COLOR_TRSP;            
    
//    printk("s=%d c=%d %s\n",bdata->is_dual,select_color,dEntry->name);

    dEntry->cur_name=dEntry->name;
    dEntry->inc=1;
    if(dEntry->name_size<bdata->max_entry_length)
    {
        gfx_putS(color, select_color,X+BROWSER_ICON_WIDTH, Y, dEntry->name);
    }
    else
    {
        char c=dEntry->name[bdata->max_entry_length];
        dEntry->name[bdata->max_entry_length]=0;
        gfx_putS(color, select_color,X+BROWSER_ICON_WIDTH, Y, dEntry->name);
        dEntry->name[bdata->max_entry_length]=c;
        //printk("long: %s-%d\n",dEntry->name, dEntry->name_size);
    }
}

void printLongName(int pos,int relPos,int selected,struct browser_data *bdata)
{
    int             H,X,Y;
    int             color=COLOR_BLACK;
    int             select_color=COLOR_BLUE;

    browser_computeNamePos(bdata,&X,&Y,NULL,&H);
    Y+=relPos*H;
    
    switch(bdata->list[pos].type)
    {
        case TYPE_BACK:
            color=COLOR_BLUE;
            select_color=COLOR_RED;
            break;
        case TYPE_DIR:
            color=COLOR_RED;
            select_color=COLOR_BLUE;
            break;
        case TYPE_FILE:
            color=COLOR_BLACK;
            select_color=COLOR_BLUE;
            break;
    }
    
    if(BROWSER_IS_FOCUSED)
    {
        if(selected)
        {
            if(bdata->list[pos].selected)
                select_color=COLOR_ORANGE2;
        }
        else
        {
            if(bdata->list[pos].selected)
                select_color=COLOR_ORANGE;
            else
                select_color= COLOR_TRSP;
        }
    }
    else
        select_color= COLOR_TRSP;
    bdata->list[pos].cur_name+=bdata->list[pos].inc;
    if((bdata->list[pos].cur_name+bdata->max_entry_length) >=
        (bdata->list[pos].name+bdata->list[pos].name_size))
        bdata->list[pos].inc=-1;
    if(bdata->list[pos].cur_name==bdata->list[pos].name)
        bdata->list[pos].inc=1;

    gfx_putnS(color, select_color,X+BROWSER_ICON_WIDTH, Y,bdata->max_entry_length,bdata->list[pos].cur_name);
}

void printAllName(struct browser_data *bdata)
{
    int i;
    int pos=bdata->pos;
    int nselect=bdata->nselect;
    int H,X,Y,W;
    
    browser_computeNamePos(bdata,&X,&Y,&W,&H);
    Y+=pos*H;

    if(BROWSER_IS_FOCUSED)
    {
        draw_bottom_status(bdata);
    }
    
    for (i = pos; i < bdata->listused && i < pos+bdata->nb_disp_entry; i++)
        printName(&bdata->list[i], i-pos,1,(i-pos)==nselect,bdata);

    /*gfx_fillRect(COLOR_TRSP,X, Y+(i-pos)*H, W,H);
    We should replace this with one call to gfx_fillRect !! */
    for(;i<pos+bdata->nb_disp_entry;i++)
        gfx_fillRect(COLOR_TRSP,X, Y+(i-pos)*H, W,H);
    
}

void printAName(struct browser_data *bdata,int pos, int nselect, int clear, int selected)
{
//    if(pos>bdata->listused-1) return;

    printName(&bdata->list[pos],nselect,clear,selected,bdata);
}

