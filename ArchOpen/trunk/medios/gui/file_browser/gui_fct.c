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
}

int viewNewDir(struct browser_data *bdata,char *name)
{
    if(name!=NULL)
    {
        if(strlen(name)>PATHLEN)
        {
           // msgBox("ERROR - Browser", "Can't load dir, path too long", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
            return 0;
        }
        strcpy(bdata->path,name);
    }


    cleanList(bdata);
    if(!doLs(bdata))
    {
        cleanList(bdata);
        //msgBox("ERROR - Browser", "Can't load dir", MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR);
        return 0;
    }

    // compute nb_disp_entry if none is given
    if (bdata->nb_disp_entry<0)
    {
        bdata->nb_disp_entry=(bdata->height-BROWSER_STATUS_HEIGHT)/bdata->entry_height;
    }

    // compute max_entry_length if none is given
    if (bdata->max_entry_length<0)
    {
        int fw,fh;

        gfx_getStringSize("M",&fw,&fh);

        bdata->max_entry_length=(bdata->width-
                                  (bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0)-
                                  BROWSER_ICON_WIDTH)
                                /fw;
    }

    bdata->browser_scroll.x=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?1:bdata->width-BROWSER_SCROLLBAR_WIDTH);
    bdata->browser_scroll.y=bdata->y_start;
    bdata->browser_scroll.height=bdata->nb_disp_entry*bdata->entry_height;

    bdata->pos=0;
    bdata->nselect=0;
    redrawBrowser(bdata);

    return 1;
}

int browser_browse(struct browser_data *bdata,char * path,char * res)
{
    if(viewNewDir(bdata,path))
    {
        if(browserEvt(bdata)==MED_OK)
        {
            if(bdata->mode==MODE_STRING && res)
            {
                if(bdata->path[0]=='/' && bdata->path[1]=='\0')
                    sprintf(res,"/%s",bdata->list[bdata->pos+bdata->nselect].name);
                else
                    sprintf(res,"%s/%s",bdata->path,bdata->list[bdata->pos+bdata->nselect].name);
            }
            return MED_OK;
        }
        else
        {
            return -MED_ERROR;
        }
    }
    return -MED_ERROR;
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
    gfx_fillRect(COLOR_WHITE,bdata->x_start,bdata->y_start,bdata->width,bdata->height);

    printAllName(bdata);
    draw_scrollBar(&bdata->browser_scroll, bdata->listused, bdata->pos,bdata->nb_disp_entry+bdata->pos);
}

void clearBrowser(struct browser_data *bdata)
{
    cleanList(bdata);
    gfx_fillRect(COLOR_WHITE,bdata->x_start,bdata->y_start,bdata->width,bdata->height);
    if(bdata->clear_status)
        bdata->clear_status(bdata);
}

void printName(struct dir_entry * dEntry,int pos,int clear,int selected,struct browser_data *bdata)
{
    int             color=COLOR_BLACK;
    int             select_color=COLOR_BLUE;
    char *          cp;
    int             type;
    int             H=bdata->entry_height;
    int             X=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0);
    int             Y=bdata->y_start+pos*H;
    int             W=bdata->width-BROWSER_SCROLLBAR_WIDTH;

    cp = strrchr(dEntry->name,(int) '/');
    if (cp)
        cp++;
    else
        cp = dEntry->name;

    if(clear)
        gfx_fillRect(COLOR_WHITE, X, Y , W, H);

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
                    gfx_fillRect(COLOR_WHITE, X+2, Y, 8, 8); /* no icon */
                    break;
            }
            break;
    }

    if(selected)
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE2;
        if(bdata->draw_file_size)
            bdata->draw_file_size(bdata,dEntry);
    }
    else
    {
        if(dEntry->selected)
            select_color=COLOR_ORANGE;
        else
            select_color= COLOR_WHITE;
    }

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
        printk("long: %s-%d\n",dEntry->name, dEntry->name_size);
    }
}

void printLongName(int pos,int selected,struct browser_data *bdata)
{
    int             H=bdata->entry_height;
    int             X=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0);
    int             Y=bdata->y_start+pos*H;
    char            c;
    int             color=COLOR_BLACK;
    int             select_color=COLOR_BLUE;

    //printk("Long\n");

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
            select_color= COLOR_WHITE;
    }
    bdata->list[pos].cur_name+=bdata->list[pos].inc;
    if((bdata->list[pos].cur_name+bdata->max_entry_length) >=
        (bdata->list[pos].name+bdata->list[pos].name_size))
        bdata->list[pos].inc=-1;
    if(bdata->list[pos].cur_name==bdata->list[pos].name)
        bdata->list[pos].inc=1;

    c=bdata->list[pos].cur_name[bdata->max_entry_length];
    bdata->list[pos].cur_name[bdata->max_entry_length]=0;
    gfx_putS(color, select_color,X+BROWSER_ICON_WIDTH, Y,bdata->list[pos].cur_name);
    bdata->list[pos].cur_name[bdata->max_entry_length]=c;
}

void printAllName(struct browser_data *bdata)
{
    int i;
    int pos=bdata->pos;
    int nselect=bdata->nselect;
    int H=bdata->entry_height;
    int X=bdata->x_start+(bdata->scroll_pos==LEFT_SCROLL?BROWSER_SCROLLBAR_WIDTH:0);
    int Y=bdata->y_start+pos*H;
    int W=bdata->width-BROWSER_SCROLLBAR_WIDTH;

    for (i = pos; i < bdata->listused && i < pos+bdata->nb_disp_entry; i++)
        printName(&bdata->list[i], i-pos,1,(i-pos)==nselect,bdata);

    /*gfx_fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    We should replace this with one call to gfx_fillRect !! */
    for(;i<pos+bdata->nb_disp_entry;i++)
        gfx_fillRect(COLOR_WHITE,X, Y+(i-pos)*H, W,H);
    if(bdata->draw_bottom_status)
        bdata->draw_bottom_status(bdata);
}

void printAName(struct browser_data *bdata,int pos, int nselect, int clear, int selected)
{
//    if(pos>bdata->listused-1) return;

    printName(&bdata->list[pos],nselect,clear,selected,bdata);
}

