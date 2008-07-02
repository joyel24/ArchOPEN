/*
* kernel/gfx/gui/widget/widgetmenu.c
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

#include <fs/cfg_file.h>

#include <gui/widgetmenu.h>

//*****************************************************************************
// WIDGETMENU_ITEM
//*****************************************************************************

/***********************************************
* Creator => only malloc
* create item and also subItem
* all real job is done in init
************************************************/
WIDGETMENU_ITEM widgetMenuItem_create(){
    WIDGETMENU_ITEM mi;

    // allocate WIDGETMENUITEM memory
    mi=malloc(sizeof(*mi));
    DEBUGWI_CD("wMenuItem create => (1) %x\n",mi);
    // create the widget
    mi->widget=widget_create();
    DEBUGWI_CD("wMenuItem create => (2) malloc subWidget: %x\n",mi->widget);
    // init members
    widgetMenuItem_init(mi);

    return mi;
}

/***********************************************
* Destructor: destroy subWidget
* and call menuItem destroy
************************************************/
void widgetMenuItem_destroy(WIDGETMENU_ITEM mi){
    // destroy the widget
    DEBUGWI_CD("wMenuItem destroy => (1) call sub widget destroy\n");
    mi->widget->destroy(mi->widget);
    DEBUGWI_CD("wMenuItem destroy => (2) call mItem destroy\n");
    menuItem_destroy((MENU_ITEM)mi);
}

/***********************************************
* Real init: method and properties
************************************************/
void widgetMenuItem_init(WIDGETMENU_ITEM mi){
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)widgetMenuItem_destroy;
    mi->handleEvent=(WIDGET_EVENTHANDLER)widgetMenuItem_handleEvent;
    mi->paint=(WIDGET_PAINTHANDLER)widgetMenuItem_paint;
    mi->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuItem_cfgToString;
    mi->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuItem_cfgFromString;
    mi->rePaint=(WIDGETMENU_ITEM_REPAINT)widgetMenuItem_rePaint;
    mi->autoSize=(WIDGET_AUTOSIZE)widgetMenuItem_autoSize;
    mi->setFont=(WIDGET_FONTSETTER)widgetMenuItem_setFont;
    mi->update=(WIDGET_UPDATER)widgetMenuItem_update;
    mi->setPos=(WIDGET_POSSETTER)widgetMenuItem_setPos;
    mi->setAlignAt=(WIDGETMENU_ITEM_ALIGNSETTER)widgetMenuItem_setAlignAt;

    // properties
    mi->caption="WidgetMenuItem";
    mi->cfgStored=false;
    mi->cfgName="CfgName";
    mi->tx=mi->ty=0;
    mi->alignAt=NO_ALIGN_AT;
}

/***********************************************
* Update for WIMenu item
* this should be called by subItem when its
* size has changed
************************************************/
void widgetMenuItem_update(WIDGETMENU_ITEM mi,WIDGET subItem)
{
    DEBUGWI("[WIMenuItem|update] for %s\n",mi->caption);
    WIDGETMENU m = (WIDGETMENU)mi->parentMenu;
    mi->autoSize(mi);
    if(m && m->update) m->update(m,mi);
}

/***********************************************
* setFont
* sync font of caption and subItem
************************************************/
void widgetMenuItem_setFont(WIDGETMENU_ITEM mi, int font)
{
    mi->widget->font=font;
    widget_setFont((WIDGET)mi,font);
}

/***********************************************
* setAlign
* specify a different alignAt for the item
* if setAlign is called on menu itself, this
* will be overwritten
************************************************/
void widgetMenuItem_setAlignAt(WIDGETMENU_ITEM mi,int alignAt)
{
    int oldAlign = mi->alignAt;
    if(alignAt < 0) mi->alignAt = NO_ALIGN_AT;
    else mi->alignAt=alignAt;
    if(oldAlign != mi->alignAt) mi->autoSize(mi);
}

/***********************************************
* setPos
* call ancestor and recompute internal position
************************************************/
void widgetMenuItem_setPos(WIDGETMENU_ITEM mi,int x,int y)
{
    /* menuItem setPos */
    menuItem_setPos((MENU_ITEM)mi,x,y);
    /* sync caption pos*/
    mi->autoSize(mi);
}

/***********************************************
* autoSize
* reposition caption and subWidget
* uses several param: alignAt, useMaxWidth
* steps: compute txt size in order to know
* remaining width for widget, start positionning
* the widget at this pos or at alignAt, call
* widget autoSize to get its size/pos
************************************************/
void widgetMenuItem_autoSize(WIDGETMENU_ITEM mi)
{
    int w,h,wh,wx;
    int of;

    /* sync useMaxWidth */
    mi->widget->useMaxWidth=mi->useMaxWidth;

    of=gfx_fontGet(); // save previous font
    gfx_fontSet(mi->font);
    /* find text size */
    w=0;
    if(mi->caption)
        gfx_getStringSize(mi->caption,&w,&h);
    else
        gfx_getStringSize("M",NULL,&h);
    gfx_fontSet(of);

    DEBUGWI("[WIMenuItem|autoSize] txt size %d,%d\n",w,h);

    /* compute widget position */
    if(mi->alignAt!=NO_ALIGN_AT)
        wx=mi->x+mi->alignAt+2+mi->margin;
    else
        wx=mi->x+w+2+mi->margin;

    /* find widget internal size */
    if(mi->widget->autoSize)
    {
        /* give remaining size to widget */
        mi->widget->updateSize(mi->widget,mi->width-wx-mi->margin,h);
        mi->widget->autoSize(mi->widget);
        mi->widget->getInternalSize(mi->widget,NULL,&wh);
    }
    else
        wh=0;

    DEBUGWI("[WIMenuItem|autoSize] widget autoSize height=%d\n",wh);

    mi->updateSize(mi,mi->width,MAX(h,wh));

    /* set text position */
    mi->ty=mi->y+(mi->height-h)/2;
    mi->tx=mi->x+mi->margin;

    mi->widget->setPos(mi->widget,wx,mi->y+(mi->height-wh)/2);

    DEBUGWI("[WIMenuItem|autoSize] (%s) pos (%d,%d): txt (%d,%d-%d,%d), widget (%d,%d-%d,%d) + size (%d,%d), alignAt %d (%d)\n",
            mi->caption,mi->x,mi->y,mi->tx,mi->ty,w,h,
            mi->widget->x,mi->widget->y,-1,wh,mi->width,mi->height,mi->alignAt,NO_ALIGN_AT);
}

/***********************************************
* evt handler:
* pass event to subWidget, except for REDRAW
************************************************/
bool widgetMenuItem_handleEvent(WIDGETMENU_ITEM mi,int evt){
    if(evt==EVT_REDRAW){
        // the menu item handles widget painting too
        return widget_handleEvent((WIDGET)mi,evt);
    }else{
        // pass all other events to the widget
        return mi->widget->handleEvent(mi->widget,evt);
    }
}

/***********************************************
* rePaint:
************************************************/
#warning is rePaint still needed ?
void widgetMenuItem_rePaint(WIDGETMENU_ITEM mi)
{
    bool clearState;
    clearState=mi->clearBackground;
    mi->clearBackground=true;
    widgetMenuItem_paint(mi);
    mi->clearBackground=clearState;
}

/***********************************************
* item Paint:
* only done if item is visible
* draw string and call subWidget Paint
************************************************/
void widgetMenuItem_paint(WIDGETMENU_ITEM mi)
{
    int color;
    int of;
    bool ocb;

    DEBUGWI("[WIMenuItem|paint] of %s\n",mi->caption);

    if(!mi->isVisible(mi))
    {
        printk("[WMItem|Paint] Trying to paint an item not visible\n");
        return;
    }

    widget_paint((WIDGET)mi); /* handles background clear*/

    color=(mi->focused)?mi->focusColor:mi->backColor;

    // caption
    if(mi->caption && (*mi->caption)!='\0')
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(mi->font);
        gfx_putS(mi->foreColor,color,mi->tx,mi->ty,mi->caption);
        gfx_fontSet(of); // restore previous font
    }

    // widget
    mi->widget->focused=mi->focused;
    ocb=mi->widget->clearBackground;
    mi->widget->clearBackground=false; // done by the menu
    mi->widget->paint(mi->widget);     //  paint it
    mi->widget->clearBackground=ocb; // restore previous state


    DEBUGWI("[WIMenuItem|paint] done\n");
}

/***********************************************
* convert item value to a config string
************************************************/
void widgetMenuItem_cfgToString(WIDGETMENU_ITEM mi,char * s){
    *s=0;
}

/***********************************************
* change item value using config string
************************************************/
void widgetMenuItem_cfgFromString(WIDGETMENU_ITEM mi,char * s){
    *s=0;
}


//*****************************************************************************
// WIDGETMENU
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
WIDGETMENU widgetMenu_create(){
    WIDGETMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));
    DEBUGWI_CD("wMenu create => %x\n",m);
    // init members
    widgetMenu_init(m);

    return m;
}

/***********************************************
* Destructor: only call txtMenu destroy
************************************************/
void widgetMenu_destroy(WIDGETMENU m){
    DEBUGWI_CD("wMenu destroy=>call txtMenu destroy\n");
    textMenu_destroy((TEXTMENU)m);
}

/***********************************************
* Real init: method and properties
************************************************/
void widgetMenu_init(WIDGETMENU m){
    textMenu_init((TEXTMENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)widgetMenu_handleEvent;
    m->getButton=(WIDGETMENU_BUTTONGETTER)widgetMenu_getButton;
    m->getCheckbox=(WIDGETMENU_CHECKBOXGETTER)widgetMenu_getCheckbox;
    m->getTrackbar=(WIDGETMENU_TRACKBARGETTER)widgetMenu_getTrackbar;
    m->getChooser=(WIDGETMENU_CHOOSERGETTER)widgetMenu_getChooser;
    m->addItem=(MENU_ITEMADDER)widgetMenu_addItem;
    m->cfgLoad=(WIDGETMENU_CFGLOADER)widgetMenu_cfgLoad;
    m->cfgSave=(WIDGETMENU_CFGSAVER)widgetMenu_cfgSave;
    m->setAlignAt=(WIDGETMENU_ALIGNSETTER)widgetMenu_setAlignAt;

    // properties
    m->alignAt=NO_ALIGN_AT;
}

/***********************************************
* setAlignAt
* change alignAt property and sync it for all
* item in the menu (call also item autoSize
************************************************/
void widgetMenu_setAlignAt(WIDGETMENU m,int alignAt)
{
    WIDGETMENU_ITEM mi;
    if(alignAt < 0) m->alignAt = NO_ALIGN_AT;
    else m->alignAt=alignAt;

    for(mi=(WIDGETMENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
    {
        mi->alignAt=alignAt;
        mi->autoSize(mi);
    }
    m->updatePos(m,false);
}

/***********************************************
* Adding an item to txtMenu
* using textmenuAddItem
************************************************/
void widgetMenu_addItem(WIDGETMENU m, WIDGETMENU_ITEM item)
{
    item->widget->font=m->font; /*NOTE:choosing not to use setFont, maybe a bad idea*/
    item->widget->foreColor=m->foreColor;
    item->widget->backColor=m->backColor;
    item->widget->fillColor=m->fillColor;
    item->widget->focusColor=m->focusColor;
    item->widget->updateSize(item->widget,m->width,m->height);
    /* this is a conservative value, it will be updated during autoSize*/
    textMenu_addItem((TEXTMENU)m,(TEXTMENU_ITEM)item);
    item->alignAt=m->alignAt;
}

/***********************************************
* Event handler
* give priority to focused item for handling
* all event except REDRAW
************************************************/
bool widgetMenu_handleEvent(WIDGETMENU m,int evt){
    bool handled=false;

    // the widget has priority on events, except EVT_REDRAW,BTN_UP and BTN_DOWN
    // Oxy: remove priority over UP/DOWN event for menuline management
    if (m->index && evt!=EVT_REDRAW ){
        handled=m->index->handleEvent(m->index,evt);
    }

    if (!handled) handled=textMenu_handleEvent((TEXTMENU)m,evt);

    return handled;
}

/***********************************************
* Get button subWidget
************************************************/
BUTTON widgetMenu_getButton(WIDGETMENU m,int index){
    WIDGETMENU_BUTTON wi = m->widgetAt(m,index);
    if(!wi) return NULL;
    return wi->button;
}

/***********************************************
* Get chkBox subWidget
************************************************/
CHECKBOX widgetMenu_getCheckbox(WIDGETMENU m,int index){
    WIDGETMENU_CHECKBOX wi = m->widgetAt(m,index);
    if(!wi) return NULL;
    return wi->checkbox;
}

/***********************************************
* Get trkBar subWidget
************************************************/
TRACKBAR widgetMenu_getTrackbar(WIDGETMENU m,int index){
    WIDGETMENU_TRACKBAR wi = m->widgetAt(m,index);
    if(!wi) return NULL;
    return wi->trackbar;
}

/***********************************************
* Get chooser subWidget
************************************************/
CHOOSER widgetMenu_getChooser(WIDGETMENU m,int index){
    WIDGETMENU_CHOOSER wi = m->widgetAt(m,index);
    if(!wi) return NULL;
    return wi->chooser;
}

/***********************************************
* Read cfg file and store value in menu
************************************************/
bool widgetMenu_cfgLoad(WIDGETMENU m,char * filename){
    WIDGETMENU_ITEM mi;
    CFG_DATA * data;

    // read cfg file
    data=cfg_readFile(filename);
    if (data==NULL){
        return false;
    }

    // read stored items value
    for(mi=(WIDGETMENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt){

        if(mi->cfgStored && cfg_itemExists(data,mi->cfgName)){
            mi->cfgFromString(mi,cfg_readString(data,mi->cfgName));
        }
    }

    cfg_clear(data);

    return true;
}

/***********************************************
* Save menu status into config file
************************************************/
bool widgetMenu_cfgSave(WIDGETMENU m,char * filename){
    WIDGETMENU_ITEM mi;
    char * s = malloc(256);
    CFG_DATA * data;

    // try to read cfg file (we will update it), else create a new file
    data=cfg_readFile(filename);
    if (data==NULL){
        data=cfg_newFile();
    }

    // write stored items value
    for(mi=(WIDGETMENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt){
        if(mi->cfgStored){
            mi->cfgToString(mi,s);
            cfg_writeString(data,mi->cfgName,s);
        }
    }

    free(s);

    // update or write cfg file
    if (!cfg_writeFile(data,filename)){
        return false;
    }

    cfg_clear(data);

    return true;
}
