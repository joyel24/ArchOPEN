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
#include <kernel/malloc.h>
#include <kernel/lang.h>
#include <kernel/evt.h>

#include <gfx/kfont.h>
#include <gfx/jpg.h>
#include <gfx/graphics.h>

#include <lib/string.h>

#include <gui/widgetlist.h>
#include <gui/widgetmenu.h>
#include <gui/icons.h>
#include <gui/msgBox.h>
#include <gui/file_browser.h>
#include <gui/settings_misc.h>
#include <gui/settings_screens.h>

#include <driver/lcd.h>
#include <driver/osd.h>

#include <fs/cfg_file.h>

#include <init/boot_error.h>

static WIDGETMENU widgetMenu;
static WIDGETMENU_CHECKBOX bgImg_disp_chk;
static WIDGETMENU_CHOOSER blendTrsp;
static WIDGETMENU_TRACKBAR trspVal;
static WIDGETMENU_BUTTON brw_button;
static WIDGETMENU_BUTTON discardFileImg_button;
static WIDGETMENU_BUTTON discardBg_button;
static WIDGETMENU_BUTTON reset_button;
static WIDGETMENU_ITEM  fName;

ICON logo;

#define BG_FILENAME "/medios/bg.img"

/* global param */
static char bg_img_path[250];
static int has_file=0;
static int needClean=0;

int bgImg_enable=0;
int blendMode=0;
int blendFactor=0;
int has_bgImg;

int oldHas_bg;

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
        printk("Has file=%d, Enable=%d, mode=%s, factor=%d, erase=%d\n",has_file,bgImg_disp_chk->checkbox->checked,
               blendTrsp->chooser->index?"trsp":"blend",trspVal->trackbar->value,needClean);
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

void restoreBG(void)
{
    chg_BG_enable(bgImg_enable,blendMode,blendFactor);
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

void bgImgSet_save(void)
{
    CFG_DATA * cfg;
    int needSave=0;
    
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
        bgImg_disp_chk->checkbox->checked=0;
        has_bgImg=0;
        rmfile(BG_FILENAME);
    }
    
    /* checking if someting has changed */
    
    TEST_VAR(oldHas_bg,has_bgImg,needSave);
    TEST_VAR(bgImg_enable,bgImg_disp_chk->checkbox->checked?1:0,needSave);
    TEST_VAR(blendMode,blendTrsp->chooser->index?1:0,needSave);
    TEST_VAR(blendFactor,trspVal->trackbar->value,needSave);
     
    
    if(!needSave)
    {
        printk("No change ==> no save\n");
        return;
    }
    
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
    cfg_writeInt(cfg,"bgImg_enable",bgImg_enable);
    cfg_writeInt(cfg,"blendMode",blendMode);
    cfg_writeInt(cfg,"blendFactor",blendFactor);
    
    cfg_writeFile(cfg,"/medios/medios.cfg");
    cfg_clear(cfg);
    
    BG_MENU_STATE("After OK btn",1);
}

void bg_updtFocus(bool reDraw)
{
    if(has_file || has_bgImg)
        bgImg_disp_chk->canFocus=1;
    else
        bgImg_disp_chk->canFocus=0;
    
    if(needClean)
       bgImg_disp_chk->canFocus=0;
    
    if(bgImg_disp_chk->checkbox->checked)
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
        discardFileImg_button->canFocus=1;
    else
        discardFileImg_button->canFocus=0;
    
    if(has_bgImg||has_file)
        discardBg_button->canFocus=1;
    else
        discardBg_button->canFocus=0;
    
    if(needClean)
    {
       bgImg_disp_chk->canFocus=0;
       discardBg_button->canFocus=0;
       discardFileImg_button->canFocus=0;
    }
    
    if(reDraw)
    {
        trspVal->rePaint(trspVal);
        blendTrsp->rePaint(blendTrsp);
        bgImg_disp_chk->rePaint(bgImg_disp_chk);
        discardFileImg_button->rePaint(discardFileImg_button);
        discardBg_button->rePaint(discardBg_button);
    }
}

void bg_progressDraw(j_common_ptr cinfo)
{
    //printk("%d\n", (cinfo->progress->pass_counter*100)/cinfo->progress->pass_limit);
}

void discardFileImgItem_click(BUTTON b)
{
    if(has_file)
    {
        if(has_bgImg)
        {
            if(loadSaveImg(1)!=MED_OK)
            {
                has_bgImg=0;
                bgImg_enable=0;
                bgImg_disp_chk->checkbox->checked=0;
                chg_BG_enable(bgImg_enable,blendTrsp->chooser->index,trspVal->trackbar->value);
            }
                
        }
        else
        {
            bgImg_disp_chk->checkbox->checked=0;            
            chg_BG_enable(bgImg_disp_chk->checkbox->checked,blendTrsp->chooser->index,trspVal->trackbar->value);
        }
        has_file=0;     
        fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
        //fName->rePaint(fName);
        bg_updtFocus(false); 
        widgetMenu->setFocus(widgetMenu,brw_button);
        widgetMenu->paint(widgetMenu);
        BG_MENU_STATE("After rm file btn",1);
    }
}

void discardBgItem_click(BUTTON b)
{
    if(has_file)
    {
        has_file=0;     
        fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
        //fName->rePaint(fName);
    }
    needClean=1;
    bgImg_disp_chk->checkbox->checked=0;
    chg_BG_enable(0,blendTrsp->chooser->index,trspVal->trackbar->value);
    bg_updtFocus(false);
    //menuList->menuList->setFocusedWidget(menuList->menuList,brw_button);
    widgetMenu->setFocus(widgetMenu,brw_button);
    widgetMenu->paint(widgetMenu);
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
            bg_updtFocus(false);
            //menuList->menuList->setFocusedWidget(menuList->menuList,bgImg_disp_chk);
            widgetMenu->setFocus(widgetMenu,bgImg_disp_chk);
        }
        drawBGMenuBG();
        widgetMenu->paint(widgetMenu);   
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
                
    chg_BG_enable(chk->checked,blendTrsp->chooser->index,trspVal->trackbar->value);
    trspVal->trackbar->minimum=blendTrsp->chooser->index==0?1:0;
    if(trspVal->trackbar->value==0 && blendTrsp->chooser->index==0) trspVal->trackbar->value=1;
    
    bg_updtFocus(true);
    BG_MENU_STATE("After enable btn chk",1);
}

void blendTrsp_chg(CHOOSER chooseItem)
{
    if(bgImg_disp_chk->checkbox->checked)
    {
        chg_blendMode(chooseItem->index);        
        trspVal->trackbar->minimum=blendTrsp->chooser->index==0?1:0;
        if(trspVal->trackbar->value==0 && blendTrsp->chooser->index==0) 
        {
            trspVal->trackbar->value=1;
            chg_blendFactor(trspVal->trackbar->value);
        }
        trspVal->rePaint(trspVal);
        BG_MENU_STATE("After mode chg",1);
    }
    
}

void trspVal_chg(TRACKBAR trkBar)
{
    if(bgImg_disp_chk->checkbox->checked)
    {
        chg_blendFactor(trkBar->value);
        BG_MENU_STATE("After focus mod",1);
    }
}

void drawBGMenuBG(void)
{
    settings_initScreen(getLangStr(STRLNG_BG_SETTING),logo,NULL,NULL);
}

void reset_button_click(BUTTON b)
{   
    has_bgImg=oldHas_bg;
    if(oldHas_bg)
    {
        if(loadSaveImg(1)!=MED_OK)
        {
            has_bgImg=0;
            bgImg_enable=0;
        }
    }
    restoreBG();
    needClean=0;
    bgImg_disp_chk->checkbox->checked=bgImg_enable;
    blendTrsp->chooser->index=blendMode;
    trspVal->trackbar->value=(blendMode==0 && blendFactor==0)?1:blendFactor;
    trspVal->trackbar->minimum=blendMode==0?1:0;
    discardFileImg_button->canFocus=0;
    fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
    has_file=0;
    bg_updtFocus(true);
}

void bgImg_setting(void)
{    
    int minX,minY;
    
    bg_img_path[0]='\0';    
    has_file=0; 
    needClean=0;
    oldHas_bg=has_bgImg;
    
    BG_MENU_STATE("Init menu",0);
    
    evtHandler = evt_getHandler(BTN_CLASS|GUI_CLASS);
    if(evtHandler<0)
    {
        printk("Can't get evt handler\n");
        return;
    }
    
    trspString[0]=getLangStr(STRLNG_BLENDING);
    trspString[1]=getLangStr(STRLNG_TRSPARENT);

    logo=icon_get("bgCfg");
    if(!logo)
        logo=icon_load("bgCfg.ico");
    
    settings_initScreen(getLangStr(STRLNG_BG_SETTING),logo,&minX,&minY);

    widgetMenu=widgetMenu_create();
    widgetMenu->setRect(widgetMenu,minX,minY,LCD_WIDTH-minX,LCD_HEIGHT-minY);
    widgetMenu->ownWidgets=true; // the menu will handle items destroy
    widgetMenu->font=WIDGET_CONFIG_FONT;
    
    bgImg_disp_chk=widgetMenuCheckbox_create();
    bgImg_disp_chk->caption=getLangStr(STRLNG_BG_ENABLE);
    bgImg_disp_chk->checkbox->checked=bgImg_enable;
    bgImg_disp_chk->checkbox->onChange=(CHECKBOX_CHANGEEVENT)bgImg_disp_chk_chg;
    widgetMenu->addItem(widgetMenu,bgImg_disp_chk);    
    if(!has_bgImg) bgImg_disp_chk->canFocus=0;
        
    blendTrsp=widgetMenuChooser_create();
    blendTrsp->caption=NULL;
    blendTrsp->chooser->items=trspString;
    blendTrsp->chooser->itemCount=2;
    blendTrsp->chooser->index=blendMode;
    blendTrsp->chooser->wrap=WIDGET_WRAP_ON;
    blendTrsp->chooser->orientation=WIDGET_ORIENTATION_HORIZ;
    blendTrsp->chooser->onChange=(CHOOSER_CHANGEEVENT)blendTrsp_chg;
    widgetMenu->addItem(widgetMenu,blendTrsp);
    if(!bgImg_enable) blendTrsp->canFocus=0;
    
    trspVal=widgetMenuTrackbar_create();
    trspVal->caption=NULL;
    trspVal->trackbar->value=(blendMode==0 && blendFactor==0)?1:blendFactor;
    trspVal->trackbar->minimum=blendMode==0?1:0;
    trspVal->trackbar->maximum=OSD_BITMAP_BLEND_FACTOR_MAX; /* mas is probably different on DSC21 */
    trspVal->trackbar->increment=1;
    trspVal->trackbar->onChange=(TRACKBAR_CHANGEEVENT)trspVal_chg;
    widgetMenu->addItem(widgetMenu,trspVal);
    if(!bgImg_enable) trspVal->canFocus=0;
    
    discardBg_button=widgetMenuButton_create();
    discardBg_button->caption=NULL;
    discardBg_button->button->caption=getLangStr(STRLNG_DISCARD_BG_IMAGE);
    discardBg_button->button->onClick=(BUTTON_CLICKEVENT)discardBgItem_click;
    widgetMenu->addItem(widgetMenu,discardBg_button);
    if(!has_bgImg) discardBg_button->canFocus=0;
    
    discardFileImg_button=widgetMenuButton_create();
    discardFileImg_button->caption=NULL;
    discardFileImg_button->button->caption=getLangStr(STRLNG_DISCARD_FILE_IMAGE);
    discardFileImg_button->button->onClick=(BUTTON_CLICKEVENT)discardFileImgItem_click;
    widgetMenu->addItem(widgetMenu,discardFileImg_button);
    discardFileImg_button->canFocus=0;
    
    fName=widgetMenuItem_create();
    fName->caption=getLangStr(STRLNG_NO_FILE_SELECTED);
    widgetMenu->addItem(widgetMenu,fName);
    fName->canFocus=0;
    
    brw_button=widgetMenuButton_create();
    brw_button->caption=NULL;
    brw_button->button->caption=getLangStr(STRLNG_BROWSE);
    brw_button->button->onClick=(BUTTON_CLICKEVENT)brwBtnBgImg_click;
    widgetMenu->addItem(widgetMenu,brw_button);
    
    reset_button=widgetMenuButton_create();
    reset_button->caption=NULL;
    reset_button->button->caption=getLangStr(STRLNG_RESET);
    reset_button->button->onClick=(BUTTON_CLICKEVENT)reset_button_click;
    widgetMenu->addItem(widgetMenu,reset_button);
    
    // intial paint
    // set focus
    if(has_bgImg)
        widgetMenu->setFocus(widgetMenu,bgImg_disp_chk);
    else
        widgetMenu->setFocus(widgetMenu,brw_button);
    
    widgetMenu->paint(widgetMenu);

    BG_MENU_STATE("Init done",1);
    
    settings_evtLoop(widgetMenu,bgImgSet_save,evtHandler);
    
    widgetMenu->destroy(widgetMenu);
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
                    gui_bootError(MISSING_IMG_FILE_ERROR,BOOT_WARN);
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
    
    restoreBG();
    
    BG_MENU_STATE("Loading cfg",0);
    
}

