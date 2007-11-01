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

#include <init/boot_error.h>

#define BGIMG_GUIFONT RADONWIDE

static WIDGETLIST menuList;
static CHECKBOX bgImg_disp_chk;
static CHOOSER blendTrsp;
static TRACKBAR trspVal;
static BUTTON ok_btn;
static BUTTON cancel_btn;
static BUTTON brw_btn;
static BUTTON discardFileImg_btn;
static BUTTON discardBg_btn;
static LABEL  fName;

static ICON logo;

static int stop_bgImg_set;
static int minX,minY,lineH,txt_x;

#define ICON_X 5
#define ICON_Y 5

#define BG_FILENAME "/medios/bg.img"

/* global param */
static char bg_img_path[250];
static int has_file=0;
static int needClean=0;

int bgImg_enable=0;
int blendMode=0;
int blendFactor=0;
int has_bgImg;

static char * trspString[2];

static int evtHandler;

void chg_blendMode(int mode);
void chg_blendFactor(int factor);
void drawBGMenuBG(void);
        
#ifdef BGMENU_DEBUG
#define BG_MENU_STATE(a,b) {bgMenu_printState((a),(b));}
#else
#define BG_MENU_STATE(a,b)
#endif
        
void bgMenu_printState(char * cause,int full)
{
    printk("%s\n",cause);
    printk("UI state:\n");
    printk("Enable=%d, Has bg img=%d, mode=%s, factor=%d\n",bgImg_enable,has_bgImg,
            blendMode?"trsp":"blend",blendFactor);
    if(full)
        printk("Has file=%d, Enable=%d, mode=%s, factor=%d, erase=%d\n",has_file,bgImg_disp_chk->checked,
               blendTrsp->index?"trsp":"blend",trspVal->value,needClean);
}
        
void chg_BG_enable(int state,int mode, int factor)
{
    if(state)
    {
        /*enable bg */
        gfx_planeSetState(VID1,OSD_VID_1_CFG);
        /* activate parameters */
        chg_blendMode(mode); // blendTrsp->index
        chg_blendFactor(factor); // trspVal->value
        gfx_planeShow(VID1);
    }
    else
    {
        /* disable bg */
        gfx_planeHide(VID1);
    }
}

void chg_blendMode(int mode)
{
    int curState=gfx_planeGetState(BMAP1);
    switch(mode)
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
}

void chg_blendFactor(int factor)
{
    int curState;
    /* mod trsp according to trkBar->value */    
    curState=gfx_planeGetState(BMAP1);
    gfx_planeSetState(BMAP1,OSD_BITMAP_SET_BLEND_FACTOR(curState,factor));
    gfx_planeShow(BMAP1);
}

MED_RET_T loadSaveImg(int mode)
{
    int fd,cnt;
    if(mode)
        fd=open(BG_FILENAME,O_RDONLY);
    else
        fd=open(BG_FILENAME,O_RDWR | O_CREAT);
    
    if(fd<0)
    { /* error => disable img + upodate cfg */
        printk("[bgImg-menu] Can't open medios Background image %s\n",BG_FILENAME);
        return -MED_ERROR;
    }
    else
    {
        cnt=0;
        if(mode)
        {
            cnt=read(fd,(void*)gfx_planeGetBufferOffset(VID1),LCD_WIDTH*2*LCD_HEIGHT);
        }
        else
        {
            cnt=write(fd,(void*)gfx_planeGetBufferOffset(VID1),LCD_WIDTH*2*LCD_HEIGHT);
        }
        close(fd);
        
        if(cnt<LCD_WIDTH*2*LCD_HEIGHT)
        { /* error => disable img + upodate cfg */
            printk("[bgImg-menu] %s only %d/%d bytes => discarding image\n",mode?"Read":"Wrote",
                   cnt,LCD_WIDTH*LCD_HEIGHT*2);
            rmfile(BG_FILENAME);
            return -MED_ERROR;
        }                    
    }
    return MED_OK;
}

void okBtnBgImg_click(BUTTON b)
{
    CFG_DATA * cfg;
    
    msgBox_info(getLangStr(STRLNG_SAVE_SETTINGS));
    
    if(has_file)
    {
        has_bgImg=1;
        if(loadSaveImg(0)!=MED_OK)
        {
            printk("[bgImg-menu] Error writing img to disk\n");
            has_bgImg=0;
            bgImg_enable=0;
        }
    }
    else if(needClean)
    {
        bgImg_disp_chk->checked=0;
        has_bgImg=0;
        rmfile(BG_FILENAME);
    }
    
    stop_bgImg_set=1;
    
    cfg=cfg_readFile("/medios/medios.cfg");
    if(!cfg)
    {
        printk("[bgImg-menu] Can't open cfg file\n");
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("[bgImg-menu] Can't create new cfg file\n");
            return;
        }
    }
    
    /* setting the config */
    
    cfg_writeInt(cfg,"has_bgImg",has_bgImg);
    
    bgImg_enable= bgImg_disp_chk->checked?1:0;
    cfg_writeInt(cfg,"bgImg_enable",bgImg_enable);
    
    blendMode=blendTrsp->index?1:0;
    cfg_writeInt(cfg,"blendMode",blendMode);
    
    blendFactor=trspVal->value;
    cfg_writeInt(cfg,"blendFactor",blendFactor);
    
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    
    stop_bgImg_set=1;
    BG_MENU_STATE("After OK btn",1);
}

void bg_updtFocus(void)
{
    if(has_file || has_bgImg)
        bgImg_disp_chk->canFocus=1;
    else
        bgImg_disp_chk->canFocus=0;
    
    if(needClean)
       bgImg_disp_chk->canFocus=0;
    
    if(bgImg_disp_chk->checked)
    {
        trspVal->canFocus=1;
        blendTrsp->canFocus=1;
    }
    else
    {
        trspVal->canFocus=0;
        blendTrsp->canFocus=0;
    }
    
    if(has_file)
        discardFileImg_btn->canFocus=1;
    else
        discardFileImg_btn->canFocus=0;
    
    if(has_bgImg||has_file)
        discardBg_btn->canFocus=1;
    else
        discardBg_btn->canFocus=0;
    
    if(needClean)
    {
       bgImg_disp_chk->canFocus=0;
       discardBg_btn->canFocus=0;
       discardFileImg_btn->canFocus=0;
    }
    
    trspVal->paint(trspVal);
    blendTrsp->paint(blendTrsp);
    bgImg_disp_chk->paint(bgImg_disp_chk);
    discardFileImg_btn->paint(discardFileImg_btn);
    discardBg_btn->paint(discardBg_btn);
}

void exit_BgImg_Menu(void)
{   
    stop_bgImg_set=1;
    if(has_bgImg)
    {
        if(loadSaveImg(1)!=MED_OK)
        {
            has_bgImg=0;
            bgImg_enable=0;
        }
    }
    chg_BG_enable(bgImg_enable,blendMode,blendFactor);  
}

void cancelBtnBgImg_click(BUTTON b)
{
      exit_BgImg_Menu();
      BG_MENU_STATE("After cancel btn",1);
}

void bg_progressDraw(j_common_ptr cinfo)
{
    //printk("%d\n", (cinfo->progress->pass_counter*100)/cinfo->progress->pass_limit);
}

void discardFileImgBtn_click(BUTTON b)
{
    if(has_file)
    {
        if(has_bgImg)
        {
            if(loadSaveImg(1)!=MED_OK)
            {
                has_bgImg=0;
                bgImg_enable=0;
                bgImg_disp_chk->checked=0;
                chg_BG_enable(bgImg_enable,blendTrsp->index,trspVal->value);
            }
                
        }
        else
        {
            bgImg_disp_chk->checked=0;            
            chg_BG_enable(bgImg_disp_chk->checked,blendTrsp->index,trspVal->value);
        }
        has_file=0;     
        fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
        fName->paint(fName);
        bg_updtFocus(); 
        menuList->setFocusedWidget(menuList,brw_btn);  
        BG_MENU_STATE("After rm file btn",1);
    }
}

void discardBgBtn_click(BUTTON b)
{
    if(has_file)
    {
        has_file=0;     
        fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
        fName->paint(fName);
    }
    needClean=1;
    bgImg_disp_chk->checked=0;
    chg_BG_enable(0,blendTrsp->index,trspVal->value);
    bg_updtFocus();
    menuList->setFocusedWidget(menuList,brw_btn);
    BG_MENU_STATE("After rm bg btn",1);
}

void brwBtnBgImg_click(BUTTON b)
{
    char * myPath=(char*)malloc(250*sizeof(char));
    int evt;
    
    if(!myPath)
    {
        printk("[bgImg-menu] Can't malloc path string\n");
        return;    
    }
    
    if(!has_file)
        strcpy(myPath,"/");
    else
    {
        int cp=(int)(strrchr(bg_img_path,'/')-bg_img_path);        
        strncpy(myPath,bg_img_path,cp);
        myPath[cp+1]='\0';
    }
    
    printk("[bgImg-menu] starting: %s\n",myPath);
    
    if(browser_simpleBrowse(myPath,myPath)==MED_OK)
    {
        msgBox_info(getLangStr(STRLNG_LOAD_IMG));
        printk("[bgImg-menu] Choosen: %s\n",myPath);
        if(gfx_loadJpg(myPath,bg_progressDraw)!=MED_OK)
        {
            msgBox_show(getLangStr(STRLNG_BG_SETTING),getLangStr(STRLNG_ERROR_LOADING_FILE),
                        MSGBOX_TYPE_OK,MSGBOX_ICON_ERROR,evtHandler);
        }
        else
        {
            fName->caption=bg_img_path;
            strncpy(bg_img_path,myPath,250);
            has_file=1;
            needClean=0;
            /* updating focus enable/disable*/
            BG_MENU_STATE("before update",1);
            bg_updtFocus();
            menuList->setFocusedWidget(menuList,bgImg_disp_chk);
        }
        drawBGMenuBG();
        menuList->paint(menuList);   
        evt_purgeHandler(evtHandler);
        do {evt=evt_getStatus(evtHandler);} while(evt==BTN_OFF || evt==BTN_ON);
        evt_purgeHandler(evtHandler);
    }
    free(myPath);
    BG_MENU_STATE("After brw btn",1);
}

void bgImg_disp_chk_chg(CHECKBOX chk)
{
    if(!has_file && !has_bgImg && chk->checked)
    {
        chk->checked=0;
        return;    
    }    
                
    chg_BG_enable(chk->checked,blendTrsp->index,trspVal->value);
    trspVal->minimum=blendTrsp->index==0?1:0;
    if(trspVal->value==0 && blendTrsp->index==0) trspVal->value=1;
    
    bg_updtFocus();
    BG_MENU_STATE("After enable btn chk",1);
}

void blendTrsp_chg(CHOOSER chooseItem)
{
    if(bgImg_disp_chk->checked)
    {
        chg_blendMode(chooseItem->index);        
        trspVal->minimum=blendTrsp->index==0?1:0;
        if(trspVal->value==0 && blendTrsp->index==0) 
        {
            trspVal->value=1;
            chg_blendFactor(trspVal->value);
        }
        trspVal->paint(trspVal);
        BG_MENU_STATE("After mode chg",1);
    }
    
}

void trspVal_chg(TRACKBAR trkBar)
{
    if(bgImg_disp_chk->checked)
    {
        chg_blendFactor(trkBar->value);
        BG_MENU_STATE("After focus mod",1);
    }
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
    bg_img_path[0]='\0';    
    has_file=0; 
    needClean=0;
    
    BG_MENU_STATE("Init menu",0);
    
    evtHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandler<0)
    {
        printk("[bgImg-menu] Can't get evt handler\n");   
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
    if(LCD_HEIGHT<240)        
        y=minY=ICON_Y+lineH;
    else
        y=minY=ICON_Y+2*lineH;
    

    // menuList
    menuList=widgetList_create();
    menuList->ownWidgets=true;
    
    bgImg_disp_chk=checkbox_create();
    bgImg_disp_chk->caption=getLangStr(STRLNG_BG_ENABLE);
    bgImg_disp_chk->font=BGIMG_GUIFONT;
    bgImg_disp_chk->setRect(bgImg_disp_chk,x,y,8,8);
    bgImg_disp_chk->checked=bgImg_enable;
    bgImg_disp_chk->onChange=(CHECKBOX_CHANGEEVENT)bgImg_disp_chk_chg;
    menuList->addWidget(menuList,bgImg_disp_chk);
    if(!has_bgImg) bgImg_disp_chk->canFocus=0;
    
    y+=lineH;
    
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
    if(!bgImg_enable) blendTrsp->canFocus=0;
        
    y+=lineH;
    
    trspVal=trackbar_create();
    trspVal->value=(blendMode==0 && blendFactor==0)?1:blendFactor;
    trspVal->minimum=blendMode==0?1:0;
    trspVal->maximum=OSD_BITMAP_BLEND_FACTOR_MAX; /* mas is probably different on DSC21 */
    trspVal->increment=1;
    trspVal->setRect(trspVal,x,y,w+29,h+1); /* using same height and width as previous widget*/
    trspVal->font=BGIMG_GUIFONT;
    trspVal->onChange=(TRACKBAR_CHANGEEVENT)trspVal_chg;
    menuList->addWidget(menuList,trspVal);
    if(!bgImg_enable) trspVal->canFocus=0;
    
    y+=lineH;
    
    gfx_getStringSize(getLangStr(STRLNG_DISCARD_BG_IMAGE),&sepW,&sepH);
    
    discardBg_btn=button_create();
    discardBg_btn->caption=getLangStr(STRLNG_DISCARD_BG_IMAGE);
    discardBg_btn->font=BGIMG_GUIFONT;
    discardBg_btn->setRect(discardBg_btn,x,y,sepW+4,sepH+2);
    discardBg_btn->clickOnRightLeft=0;
    discardBg_btn->onClick=(BUTTON_CLICKEVENT)discardBgBtn_click;
    menuList->addWidget(menuList,discardBg_btn);
    if(!has_bgImg) discardBg_btn->canFocus=0;
    
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
    
    gfx_getStringSize(getLangStr(STRLNG_DISCARD_FILE_IMAGE),&sepW,&sepH);
    discardFileImg_btn=button_create();
    discardFileImg_btn->caption=getLangStr(STRLNG_DISCARD_FILE_IMAGE);
    discardFileImg_btn->font=BGIMG_GUIFONT;
    discardFileImg_btn->setRect(discardFileImg_btn,x,y,sepW+4,sepH+2);
    discardFileImg_btn->clickOnRightLeft=0;
    discardFileImg_btn->onClick=(BUTTON_CLICKEVENT)discardFileImgBtn_click;
    menuList->addWidget(menuList,discardFileImg_btn);
    discardFileImg_btn->canFocus=0;

    y+=lineH;

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
    if(has_bgImg)
        menuList->setFocusedWidget(menuList,bgImg_disp_chk);
    else
        menuList->setFocusedWidget(menuList,brw_btn);
    menuList->paint(menuList);

    BG_MENU_STATE("Init done",1);
    
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
            case BTN_OFF:
                exit_BgImg_Menu();
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
    CFG_DATA * cfg;
    int needWrite=0;
    
    /* init the global var, if something goes wrong, 
    at least this has been done*/
           
    has_bgImg=0;
    bgImg_enable=0;
    blendMode=1;
    blendFactor=OSD_BITMAP_BLEND_FACTOR_MAX/2;
    
    /* reading param from HDD */
    cfg=cfg_readFile("/medios/medios.cfg");
               
    if(!cfg)
    {
        printk("[bgImg-init] Can't open cfg file\n");
        /* creating default */
        cfg=cfg_newFile();
        if(!cfg)
        {
            printk("[bgImg-init] Can't create new cfg file\n");
            return;
        }
        cfg_writeInt(cfg,"has_bgImg",0);
        cfg_writeInt(cfg,"bgImg_enable",0);
        cfg_writeInt(cfg,"blendMode",1);
        cfg_writeInt(cfg,"blendFactor",OSD_BITMAP_BLEND_FACTOR_MAX/2);
        needWrite=1;   
    }
    else
    {
        if(cfg_itemExists(cfg,"has_bgImg"))
        {
            has_bgImg=cfg_readInt(cfg,"has_bgImg");
            if(has_bgImg)
            {
                if(loadSaveImg(1)!=MED_OK)
                {
                    printk("BG Load failed\n");
                    cfg_writeInt(cfg,"has_bgImg",0);
                    gui_bootError(MISSING_MIMG_FILE_ERROR,BOOT_WARN);
                    has_bgImg=0;
                    needWrite=1;  
                }
                else
                {                    
                    printk("BG Load done\n");
                }
            }
        }
        else
        {
            cfg_writeInt(cfg,"has_bgImg",0);
            needWrite=1;
        }
        
        if(cfg_itemExists(cfg,"bgImg_enable"))
        {
            bgImg_enable=cfg_readInt(cfg,"bgImg_enable");
            if(bgImg_enable & !has_bgImg)
            {
                cfg_writeInt(cfg,"bgImg_enable",0);
                bgImg_enable=0;
                needWrite=1;
            }
        }
        else
        {
            cfg_writeInt(cfg,"bgImg_enable",0);
            needWrite=1;
        }
        
        if(cfg_itemExists(cfg,"blendMode"))
        {
            blendMode=cfg_readInt(cfg,"blendMode");
        }
        else
        {
            cfg_writeInt(cfg,"blendMode",1);
            needWrite=1;
        }
        
        if(cfg_itemExists(cfg,"blendFactor"))
        {
            blendFactor=cfg_readInt(cfg,"blendFactor");
        }
        else
        {
            cfg_writeInt(cfg,"blendFactor",OSD_BITMAP_BLEND_FACTOR_MAX/2);
            needWrite=1;
        }
        
        
    }
    
    if(needWrite) cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
        
    /* chging img state */
    
    chg_BG_enable(bgImg_enable,blendMode,blendFactor);
    
    BG_MENU_STATE("Loading cfg",0);
    
}

