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

WIDGETMENU_ITEM widgetMenuItem_create(){
    WIDGETMENU_ITEM mi;

    // allocate WIDGETMENUITEM memory
    mi=malloc(sizeof(*mi));

    // create the widget
    mi->widget=widget_create();

    // init members
    widgetMenuItem_init(mi);

    return mi;
}

void widgetMenuItem_destroy(WIDGETMENU_ITEM mi){
    // destroy the widget
    mi->widget->destroy(mi->widget);

    menuItem_destroy((MENU_ITEM)mi);
}

void widgetMenuItem_init(WIDGETMENU_ITEM mi){
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)widgetMenuItem_destroy;
    mi->handleEvent=(WIDGET_EVENTHANDLER)widgetMenuItem_handleEvent;
    mi->paint=(WIDGET_PAINTHANDLER)widgetMenuItem_paint;
    mi->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuItem_cfgToString;
    mi->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuItem_cfgFromString;
    mi->rePaint=(WIDGETMENU_ITEM_REPAINT)widgetMenuItem_rePaint;
    
    // properties
    mi->caption="WidgetMenuItem";
    mi->cfgStored=false;
    mi->cfgName="CfgName";
    mi->widgetWidth=128;
    mi->doAutoSize=false;
}

bool widgetMenuItem_handleEvent(WIDGETMENU_ITEM mi,int evt){
    if(evt==EVT_REDRAW){
        // the menu item handles widget painting too
        return widget_handleEvent((WIDGET)mi,evt);
    }else{
        // pass all other events to the widget
        return mi->widget->handleEvent(mi->widget,evt);
    }
}

void widgetMenuItem_rePaint(WIDGETMENU_ITEM mi)
{
    bool clearState;
    clearState=mi->clearBackground;
    mi->clearBackground=true;
    widgetMenuItem_paint(mi);
    mi->clearBackground=clearState;
}

void widgetMenuItem_paint(WIDGETMENU_ITEM mi){
    int txt_x,txt_y;
    int txt_w,txt_h;
    int w2;
    int color;
    int of;
    int no_caption=0;
    bool ocb;

    if(!menuItem_isVisible((MENU_ITEM)mi))
    {
        return;   
    }
    
    widget_paint((WIDGET)mi);

    color=(mi->focused)?mi->focusColor:mi->backColor;

    txt_x=mi->x;
    txt_y=mi->y;
    
    // caption
    if(mi->caption && (*mi->caption)!='\0')
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(mi->font);
        gfx_getStringSize(mi->caption,&txt_w,&txt_h);
        txt_y=mi->y+(mi->height-txt_h)/2;
        txt_x=mi->x+mi->margin;
    
        gfx_putS(mi->foreColor,color,txt_x,txt_y,mi->caption);
    
        gfx_fontSet(of); // restore previous font
    }
    else
    {
        no_caption=1;
    }

    // widget

    //  sync widget props with the menuitem ones
    mi->widget->focused=mi->focused;
    if(!no_caption)
    {
        if(mi->doAutoSize)
        {
            mi->widget->autoSize(mi->widget);
            w2=mi->width*mi->widgetWidth/256-mi->margin;
            if(mi->widget->width==0 || mi->widget->width>w2)
            {
                mi->widget->width=mi->width*mi->widgetWidth/256-mi->margin;
                mi->widget->x=mi->x+mi->width-mi->margin-mi->widget->width;
            }
            else
            {
               mi->widget->x=txt_x+txt_w+2;
            }
            
            if(mi->widget->height==0)
            {
                mi->widget->height=mi->height-2*mi->margin;
                mi->widget->y=mi->y+mi->margin;
            }
            else if(mi->widget->height > mi->height)
            {
                mi->widget->height=mi->height;
                mi->widget->y=mi->y;
            }
            else
                mi->widget->y=mi->y+(mi->height-mi->widget->height)/2;                
        }
        else /* no autoSize */
        {            
            mi->widget->width=mi->width*mi->widgetWidth/256-mi->margin;
            mi->widget->height=mi->height-2*mi->margin;    
            mi->widget->x=mi->x+mi->width-mi->margin-mi->widget->width;
            mi->widget->y=mi->y+mi->margin;
        }
    }
    else
    {
        if(mi->doAutoSize)
        {
            mi->widget->autoSize(mi->widget);
            w2=mi->width-2*mi->margin;
            if(mi->widget->width==0 || mi->widget->width>w2)
                mi->widget->width=w2;
            if(mi->widget->height==0)
                mi->widget->height=mi->height-2*mi->margin;
            else if(mi->widget->height > mi->height)
                mi->widget->height=mi->height;
                
        }
        else /* no autosize */
        {        
            mi->widget->width=mi->width-2*mi->margin;
            mi->widget->height=mi->height-2*mi->margin;           
        }
        mi->widget->x=mi->x+mi->margin;
        mi->widget->y=mi->y+(mi->height-mi->widget->height)/2;
    }

    ocb=mi->widget->clearBackground;
    mi->widget->clearBackground=false; // done by the menu

    //  paint it
    mi->widget->paint(mi->widget);

    mi->widget->clearBackground=ocb; // restore previous state
}

void widgetMenuItem_cfgToString(WIDGETMENU_ITEM mi,char * s){
    *s=0;
}

void widgetMenuItem_cfgFromString(WIDGETMENU_ITEM mi,char * s){
    *s=0;
}

//*****************************************************************************
// WIDGETMENU_BUTTON
//*****************************************************************************

WIDGETMENU_BUTTON widgetMenuButton_create(){
    WIDGETMENU_BUTTON mc;

    // allocate WIDGETMENUCHECKBOX memory
    mc=malloc(sizeof(*mc));

    // create the widget
    mc->button=button_create();
    mc->widget=(WIDGET)mc->button;

    // init members
    widgetMenuButton_init(mc);

    return mc;
}

void widgetMenuButton_destroy(WIDGETMENU_BUTTON mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuButton_init(WIDGETMENU_BUTTON mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuButton_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuButton_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuButton_cfgFromString;

    // properties
    mc->caption="WidgetMenuButton";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuButton_cfgToString(WIDGETMENU_BUTTON mc,char * s){
    *s=0;
}

void widgetMenuButton_cfgFromString(WIDGETMENU_BUTTON mc,char * s){
    *s=0;
}

//*****************************************************************************
// WIDGETMENU_CHECKBOX
//*****************************************************************************

WIDGETMENU_CHECKBOX widgetMenuCheckbox_create(){
    WIDGETMENU_CHECKBOX mc;

    // allocate WIDGETMENUCHECKBOX memory
    mc=malloc(sizeof(*mc));

    // create the widget
    mc->checkbox=checkbox_create();
    mc->widget=(WIDGET)mc->checkbox;

    // init members
    widgetMenuCheckbox_init(mc);

    return mc;
}

void widgetMenuCheckbox_destroy(WIDGETMENU_CHECKBOX mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuCheckbox_init(WIDGETMENU_CHECKBOX mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuCheckbox_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuCheckbox_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuCheckbox_cfgFromString;

    // properties
    mc->caption="WidgetMenuCheckbox";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuCheckbox_cfgToString(WIDGETMENU_CHECKBOX mc,char * s){
    strcpy(s,(mc->checkbox->checked)?"1":"0");
}

void widgetMenuCheckbox_cfgFromString(WIDGETMENU_CHECKBOX mc,char * s){
    mc->checkbox->checked=(atoi(s))?true:false;
}

//*****************************************************************************
// WIDGETMENU_TRACKBAR
//*****************************************************************************

WIDGETMENU_TRACKBAR widgetMenuTrackbar_create(){
    WIDGETMENU_TRACKBAR mt;

    // allocate WIDGETMENUCHECKBOX memory
    mt=malloc(sizeof(*mt));

    // create the widget
    mt->trackbar=trackbar_create();
    mt->widget=(WIDGET)mt->trackbar;

    // init members
    widgetMenuTrackbar_init(mt);

    return mt;
}

void widgetMenuTrackbar_destroy(WIDGETMENU_TRACKBAR mt){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mt);
}

void widgetMenuTrackbar_init(WIDGETMENU_TRACKBAR mt){
    widgetMenuItem_init((WIDGETMENU_ITEM)mt);

    // methods
    mt->destroy=(WIDGET_DESTROYER)widgetMenuTrackbar_destroy;
    mt->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuTrackbar_cfgToString;
    mt->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuTrackbar_cfgFromString;

    // properties
    mt->caption="WidgetMenuTrackbar";
    mt->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuTrackbar_cfgToString(WIDGETMENU_TRACKBAR mt,char * s){
    sprintf(s,"%d",mt->trackbar->value);
}

void widgetMenuTrackbar_cfgFromString(WIDGETMENU_TRACKBAR mt,char * s){
    mt->trackbar->value=atoi(s);
}

//*****************************************************************************
// WIDGETMENU_CHOOSER
//*****************************************************************************

WIDGETMENU_CHOOSER widgetMenuChooser_create(){
    WIDGETMENU_CHOOSER mc;

    // allocate WIDGETMENUCHECKBOX memory
    mc=malloc(sizeof(*mc));

    // create the widget
    mc->chooser=chooser_create();
    mc->widget=(WIDGET)mc->chooser;

    // init members
    widgetMenuChooser_init(mc);

    return mc;
}

void widgetMenuChooser_destroy(WIDGETMENU_CHOOSER mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuChooser_init(WIDGETMENU_CHOOSER mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuChooser_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuChooser_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuChooser_cfgFromString;

    // properties
    mc->caption="WidgetMenuChooser";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuChooser_cfgToString(WIDGETMENU_CHOOSER mc,char * s){
    sprintf(s,"%d",mc->chooser->index);
}

void widgetMenuChooser_cfgFromString(WIDGETMENU_CHOOSER mc,char * s){
    mc->chooser->index=atoi(s);
}

//*****************************************************************************
// WIDGETMENU
//*****************************************************************************

WIDGETMENU widgetMenu_create(){
    WIDGETMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));

    // init members
    widgetMenu_init(m);

    return m;
}

void widgetMenu_destroy(WIDGETMENU m){
    textMenu_destroy((TEXTMENU)m);
}

void widgetMenu_init(WIDGETMENU m){
    textMenu_init((TEXTMENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)widgetMenu_handleEvent;
    m->getButton=(WIDGETMENU_BUTTONGETTER)widgetMenu_getButton;
    m->getCheckbox=(WIDGETMENU_CHECKBOXGETTER)widgetMenu_getCheckbox;
    m->getTrackbar=(WIDGETMENU_TRACKBARGETTER)widgetMenu_getTrackbar;
    m->getChooser=(WIDGETMENU_CHOOSERGETTER)widgetMenu_getChooser;
    m->cfgLoad=(WIDGETMENU_CFGLOADER)widgetMenu_cfgLoad;
    m->cfgSave=(WIDGETMENU_CFGSAVER)widgetMenu_cfgSave;

    // properties
    m->itemHeight=13;
}

bool widgetMenu_handleEvent(WIDGETMENU m,int evt){
    bool handled=false;

    // the widget has priority on events, except EVT_REDRAW,BTN_UP and BTN_DOWN
    if (m->index>=0 && m->index<m->itemCount && evt!=EVT_REDRAW && evt!=BTN_UP && evt!=BTN_DOWN){
        handled=m->items[m->index]->handleEvent(m->items[m->index],evt);
    }

    if (!handled) handled=textMenu_handleEvent((TEXTMENU)m,evt);

    return handled;
}

BUTTON widgetMenu_getButton(WIDGETMENU m,int index){
    if(index<0 ||index>=m->itemCount) return NULL;
    return ((WIDGETMENU_BUTTON)m->items[index])->button;
}

CHECKBOX widgetMenu_getCheckbox(WIDGETMENU m,int index){
    if(index<0 ||index>=m->itemCount) return NULL;
    return ((WIDGETMENU_CHECKBOX)m->items[index])->checkbox;
}

TRACKBAR widgetMenu_getTrackbar(WIDGETMENU m,int index){
    if(index<0 ||index>=m->itemCount) return NULL;
    return ((WIDGETMENU_TRACKBAR)m->items[index])->trackbar;
}

CHOOSER widgetMenu_getChooser(WIDGETMENU m,int index){
    if(index<0 ||index>=m->itemCount) return NULL;
    return ((WIDGETMENU_CHOOSER)m->items[index])->chooser;
}

bool widgetMenu_cfgLoad(WIDGETMENU m,char * filename){
    int i;
    WIDGETMENU_ITEM mi;
    CFG_DATA * data;

    // read cfg file
    data=cfg_readFile(filename);
    if (data==NULL){
        return false;
    }

    // read stored items value
    for(i=0;i<m->itemCount;++i){

        mi=(WIDGETMENU_ITEM)m->items[i];

        if(mi->cfgStored && cfg_itemExists(data,mi->cfgName)){
            mi->cfgFromString(mi,cfg_readString(data,mi->cfgName));
        }
    }

    cfg_clear(data);

    return true;
}

bool widgetMenu_cfgSave(WIDGETMENU m,char * filename){
    int i;
    WIDGETMENU_ITEM mi;
    char * s = malloc(256);
    CFG_DATA * data;

    // try to read cfg file (we will update it), else create a new file
    data=cfg_readFile(filename);
    if (data==NULL){
        data=cfg_newFile();
    }

    // write stored items value
    for(i=0;i<m->itemCount;++i){

        mi=(WIDGETMENU_ITEM)m->items[i];

        if(mi->cfgStored){
            mi->cfgToString(mi,s);
            cfg_writeString(data,mi->cfgName,s);
        }
    }

    kfree(s);

    // update or write cfg file
    if (!cfg_writeFile(data,filename)){
        return false;
    }

    cfg_clear(data);

    return true;
}
