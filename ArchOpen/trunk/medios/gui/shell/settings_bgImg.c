/*
*   kernel/gfx/gui/shell/settings_bgImg.c
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
#include <kernel/malloc.h>
#include <kernel/lang.h>

#include <gfx/kfont.h>
#include <gfx/jpg.h>
#include <gfx/graphics.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/button.h>
#include <gui/icons.h>
#include <gui/checkbox.h>
#include <gui/trackbar.h>
#include <gui/chooser.h>
#include <gui/msgBox.h>
#include <gui/label.h>
#include <gui/file_browser.h>
#include <gui/settings_misc.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <fs/cfg_file.h>

#define BGIMG_GUIFONT RADONWIDE

WIDGETLIST menuList;
CHECKBOX hasBgImg;
CHOOSER blendTrsp;
TRACKBAR trspVal;
BUTTON ok_btn;
BUTTON cancel_btn;
BUTTON brw_btn;
LABEL  fName;

ICON logo;

int stop_bgImg_set;
int minX,minY,lineH,txt_x;

#define ICON_X 5
#define ICON_Y 5

/* global param */
char bg_img_path[250];
int has_file=0;
int enableBG=0;
int blendMode=0;
int blendFactor=0;
int has_fileInt=0;


char * trspString[2];

int evtHandler;

void drawBGMenuBG(void);

void chg_BG_enable(int state)
{
    if(state)
    {
        /*enable bg */
        gfx_planeSetState(VID1,OSD_VID_1_CFG);
        /* activate parameters */
        chg_blendMode(blendTrsp->index);
        chg_blendFactor(trspVal->value);
        gfx_planeShow(VID1);
    }
    else
    {
        /* disable bg */
        gfx_planeHide(VID1);
    }
}

void chg_blendMode(int state)
{
    int curState=gfx_planeGetState(BMAP1);
    switch(state)
    {
        case 0:
            /* blending*/
            gfx_planeSetState(BMAP1,OSD_BITMAP_TRSP_ENABLE(curState,0));
            gfx_planeShow(BMAP1);
            break;
        case 1:
            /*transparent*/
            gfx_planeSetState(BMAP1,OSD_BITMAP_TRSP_ENABLE(curState,1));
            gfx_planeShow(BMAP1);
            break;
    }
    trspVal->minimum=state==0?1:0;
    if(trspVal->value==0 && state==0) trspVal->value=1;
    printk("Trsp enable: %x -> %x\n",curState,gfx_planeGetState(BMAP1));   
}

void chg_blendFactor(int factor)
{
    int curState;
    /* mod trsp according to trkBar->value */    
    curState=gfx_planeGetState(BMAP1);
    gfx_planeSetState(BMAP1,OSD_BITMAP_SET_BLEND_FACTOR(curState,factor));
    gfx_planeShow(BMAP1);
    printk("Trsp factor: %x -> %x\n",curState,gfx_planeGetState(BMAP1));   
}

void okBtnBgImg_click(BUTTON b)
{
    
    /* saving bg Img if needed */
        
    stop_bgImg_set=1;
}

void cancelBtnBgImg_click(BUTTON b)
{
    stop_bgImg_set=1;
    if(!has_file)
        bg_img_path[0]='\0';
    chg_BG_enable(enableBG);
    chg_blendMode(blendMode);
    chg_blendFactor(blendFactor);
}

void bg_progressDraw(struct jpeg_decompress_struct *cinfo)
{
    printk("%d\n", (cinfo->progress->pass_counter*100)/cinfo->progress->pass_limit);
}

void brwBtnBgImg_click(BUTTON b)
{
    char * myPath=(char*)malloc(250*sizeof(char));
    
    if(!myPath)
    {
        printk("[brwBtnBgImg_click] Can't malloc path string\n");
        return;    
    }
    
    if(!has_fileInt)
        strcpy(myPath,"/");
    else
    {
        int cp=(int)(strrchr(bg_img_path,'/')-bg_img_path);        
        strncpy(myPath,bg_img_path,cp);
        myPath[cp+1]='\0';
    }
    
    printk("starting: %s\n",myPath);
    
    if(browser_simpleBrowse(myPath,myPath)==MED_OK)
    {
        msgBox_info(getLangStr(STRLNG_LOAD_IMG));
        printk("Choosen: %s\n",myPath);
        if(gfx_loadJpg(myPath,bg_progressDraw)!=MED_OK)
        {
            msgBox_show(getLangStr(STRLNG_BG_SETTING),getLangStr(STRLNG_ERROR_LOADING_FILE),
                        MSGBOX_TYPE_OK,MSGBOX_ICON_ERROR,evtHandler);
        }
        else
        {
            fName->caption=bg_img_path;
            strncpy(bg_img_path,myPath,250);
            has_fileInt=1;
        }
        free(myPath);
        drawBGMenuBG();
        menuList->paint(menuList);
        evt_purgeHandler(evtHandler);
    }
}

void hasBgImg_chg(CHECKBOX chk)
{
    if(!has_fileInt && chk->checked)
    {
        chk->checked=0;
        chk->paint(chk);
        return;    
    }    
        
    chg_BG_enable(chk->checked);
}

void blendTrsp_chg(CHOOSER chooseItem)
{
    chg_blendMode(chooseItem->index);
}

void trspVal_chg(TRACKBAR trkBar)
{
    chg_blendFactor(trkBar->value);
}

void drawBGMenuBG(void)
{
    int w,h;
    
    gfx_clearScreen(COLOR_TRSP);   
    gfx_drawBitmap(&logo->bmap_data,ICON_X,ICON_Y);
    gfx_drawLine(COLOR_RED,minX+3,5,minX+3,LCD_HEIGHT-5);
     
    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_BG_SETTING),&w,&h);
    lineH=h+5;
    gfx_putS(COLOR_DARK_GREY,COLOR_TRSP,txt_x,ICON_Y,getLangStr(STRLNG_BG_SETTING));
    gfx_fontSet(BGIMG_GUIFONT);
}

void bgImg_setting(void)
{    
    int event;
    int w,h,x,y,sepW,sepH,w1,h1;
    
    stop_bgImg_set=0;   
    
    evtHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandler<0)
    {
        printk("Can't get evt handler\n");   
    }
    
    trspString[0]=getLangStr(STRLNG_BLENDING);
    trspString[1]=getLangStr(STRLNG_TRSPARENT);

    logo=icon_get("bgcfg");
    if(!logo)
        icon_load("bgCfg.ico");
    
    gfx_fontSet(BGIMG_GUIFONT);
    minX = ICON_X + logo->bmap_data.width;

    gfx_fontSet(STD8X13);
    gfx_getStringSize(getLangStr(STRLNG_BG_SETTING),&w,&h);
    lineH=h+5;
    txt_x=(minX+8)+(LCD_WIDTH-(minX+8)-w)/2;
    
    drawBGMenuBG();    

    x=minX+8;
    y=minY=ICON_Y+2*lineH;

    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;
    
    enableBG=enableBG>0?1:0;

    hasBgImg=checkbox_create();
    hasBgImg->caption=getLangStr(STRLNG_BG_ENABLE);
    hasBgImg->font=BGIMG_GUIFONT;
    hasBgImg->setRect(hasBgImg,x,y,8,8);
    hasBgImg->checked=enableBG;
    hasBgImg->onChange=(CHECKBOX_CHANGEEVENT)hasBgImg_chg;
    menuList->addWidget(menuList,hasBgImg);

    y+=lineH;
    
    blendMode=blendMode>0?1:0;

    blendTrsp=chooser_create();
    blendTrsp->items = trspString;
    blendTrsp->itemCount=2;
    blendTrsp->index=blendMode;
    blendTrsp->font=BGIMG_GUIFONT;
    
    gfx_getStringSize(trspString[0],&w,&h);
    gfx_getStringSize(trspString[1],&w1,&h1);    
    if(w1>w) w=w1;
    if(h1>h) h=h1;
    
    blendTrsp->setRect(blendTrsp,x,y,w+29,h+1);
    blendTrsp->wrap=WIDGET_WRAP_ON;
    blendTrsp->orientation=WIDGET_ORIENTATION_HORIZ;
    blendTrsp->onChange=(CHOOSER_CHANGEEVENT)blendTrsp_chg;
    menuList->addWidget(menuList,blendTrsp);

    y+=lineH;
    
    blendFactor=blendFactor<0 ? 0 :
            blendFactor>OSD_BITMAP_BLEND_FACTOR_MAX ? OSD_BITMAP_BLEND_FACTOR_MAX : blendFactor;
    trspVal=trackbar_create();
    trspVal->value=blendFactor;
    trspVal->minimum=blendMode==0?1:0;
    trspVal->maximum=OSD_BITMAP_BLEND_FACTOR_MAX; /* mas is probably different on DSC21 */
    trspVal->increment=1;
    trspVal->setRect(trspVal,x,y,w+29,h+1); /* using same height and width as previous widget*/
    trspVal->font=BGIMG_GUIFONT;
    trspVal->onChange=(TRACKBAR_CHANGEEVENT)trspVal_chg;
    menuList->addWidget(menuList,trspVal);
    
    y+=lineH;

    gfx_getStringSize(getLangStr(STRLNG_BROWSE),&sepW,&sepH);
    brw_btn=button_create();
    brw_btn->caption=getLangStr(STRLNG_BROWSE);
    brw_btn->font=BGIMG_GUIFONT;
    brw_btn->setRect(brw_btn,x,y,sepW+4,sepH+2);
    brw_btn->clickOnRightLeft=0;
    brw_btn->onClick=(BUTTON_CLICKEVENT)brwBtnBgImg_click;
    menuList->addWidget(menuList,brw_btn);
    
    y+=lineH;

    has_fileInt=has_file=has_file>0?1:0;
    
    fName=label_create();
    fName->caption=has_file?bg_img_path:getLangStr(STRLNG_NO_FILE_SELECTED);
    fName->font=BGIMG_GUIFONT;
    fName->alignment=LA_LEFT; 
    gfx_getStringSize(getLangStr(STRLNG_NO_FILE_SELECTED),&w,&h);   
    fName->setRect(fName,x,y,LCD_WIDTH-x,h+1);
    menuList->addWidget(menuList,fName);
    
    y+=lineH;
    
    gfx_getStringSize(getLangStr(STRLNG_OK),&sepW,&sepH);
    ok_btn=button_create();
    ok_btn->caption=getLangStr(STRLNG_OK);
    ok_btn->font=BGIMG_GUIFONT;
    ok_btn->setRect(ok_btn,x,y,sepW+4,sepH+2);
    ok_btn->clickOnRightLeft=0;
    ok_btn->onClick=(BUTTON_CLICKEVENT)okBtnBgImg_click;
    menuList->addWidget(menuList,ok_btn);

    x+=ok_btn->width+4;

    gfx_getStringSize(getLangStr(STRLNG_CANCEL),&sepW,&sepH);
    cancel_btn=button_create();
    cancel_btn->caption=getLangStr(STRLNG_CANCEL);
    cancel_btn->font=BGIMG_GUIFONT;
    cancel_btn->setRect(cancel_btn,x,y,sepW+4,sepH+2);
    cancel_btn->clickOnRightLeft=0;
    cancel_btn->onClick=(BUTTON_CLICKEVENT)cancelBtnBgImg_click;
    menuList->addWidget(menuList,cancel_btn);

    // intial paint
    // set focus
    menuList->setFocusedWidget(menuList,hasBgImg);
    menuList->paint(menuList);

    do{
        event=evt_getStatusBlocking(evtHandler);
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
    }while(event!=BTN_OFF && !stop_bgImg_set);

    menuList->destroy(menuList);
    evt_freeHandler(evtHandler);
}

void bgImg_loadPref(void)
{
    bg_img_path[0]='\0';
    has_file=0;
    enableBG=0;
    blendMode=1;
    blendFactor=OSD_BITMAP_BLEND_FACTOR_MAX/2;
}

