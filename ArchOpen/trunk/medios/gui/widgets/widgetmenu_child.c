/*
* kernel/gfx/gui/widget/widgetmenu_child.c
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
// WIDGETMENU_BUTTON
//*****************************************************************************

WIDGETMENU_BUTTON widgetMenuButton_create(){
    WIDGETMENU_BUTTON mc;
    
    // allocate WIDGETMENUCHECKBOX memory
    mc=malloc(sizeof(*mc));
    
    // init members
    widgetMenuButton_init(mc);
    
    return mc;
}

void widgetMenuButton_destroy(WIDGETMENU_BUTTON mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuButton_init(WIDGETMENU_BUTTON mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // create the widget
    mc->button=button_create();
    mc->widget=(WIDGET)mc->button;
    mc->button->parent=(WIDGET)mc;
    
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
// WIDGETMENU_SPINBOX
//*****************************************************************************

WIDGETMENU_SPINBOX widgetMenuSpinbox_create(){
    WIDGETMENU_SPINBOX mc;

    // allocate WIDGETSPINBOX memory
    mc=malloc(sizeof(*mc));
    
    // init members
    widgetMenuSpinbox_init(mc);

    return mc;
}

void widgetMenuSpinbox_destroy(WIDGETMENU_SPINBOX mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuSpinbox_init(WIDGETMENU_SPINBOX mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // create the widget
    mc->spinbox=spinbox_create();
    mc->widget=(WIDGET)mc->spinbox;
    mc->spinbox->parent=(WIDGET)mc;
        
    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuSpinbox_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuSpinbox_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuSpinbox_cfgFromString;
      
    // properties
    mc->caption="WidgetMenuSpinbox";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuSpinbox_cfgToString(WIDGETMENU_SPINBOX mc,char * s){
    *s=0;
}

void widgetMenuSpinbox_cfgFromString(WIDGETMENU_SPINBOX mc,char * s){
    *s=0;
}

//*****************************************************************************
// WIDGETMENU_CHECKBOX
//*****************************************************************************

WIDGETMENU_CHECKBOX widgetMenuCheckbox_create(){
    WIDGETMENU_CHECKBOX mc;

    // allocate WIDGETMENUCHECKBOX memory
    mc=malloc(sizeof(*mc));
    
    // init members
    widgetMenuCheckbox_init(mc);

    return mc;
}

void widgetMenuCheckbox_destroy(WIDGETMENU_CHECKBOX mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuCheckbox_init(WIDGETMENU_CHECKBOX mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // create the widget
    mc->checkbox=checkbox_create();
    mc->widget=(WIDGET)mc->checkbox;
    mc->checkbox->parent=(WIDGET)mc;
        
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
    
    // init members
    widgetMenuTrackbar_init(mt);

    return mt;
}

void widgetMenuTrackbar_destroy(WIDGETMENU_TRACKBAR mt){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mt);
}

void widgetMenuTrackbar_init(WIDGETMENU_TRACKBAR mt){
    widgetMenuItem_init((WIDGETMENU_ITEM)mt);

    // create the widget
    mt->trackbar=trackbar_create();
    mt->widget=(WIDGET)mt->trackbar;
    mt->trackbar->parent=(WIDGET)mt;
    
    // methods
    mt->destroy=(WIDGET_DESTROYER)widgetMenuTrackbar_destroy;
    mt->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuTrackbar_cfgToString;
    mt->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuTrackbar_cfgFromString;

    // properties
    mt->caption="WidgetMenuTrackbar";
    //mt->clearBackground=true;
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

    // init members
    widgetMenuChooser_init(mc);

    return mc;
}

void widgetMenuChooser_destroy(WIDGETMENU_CHOOSER mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuChooser_init(WIDGETMENU_CHOOSER mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // create the widget
    mc->chooser=chooser_create();
    mc->widget=(WIDGET)mc->chooser;
    mc->chooser->parent=(WIDGET)mc;
    
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
// WIDGETMENU_DATE_TIME
//*****************************************************************************

WIDGETMENU_DATE_TIME widgetMenuDateTime_create(){
    WIDGETMENU_DATE_TIME mc;

    // allocate WIDGETSPINBOX memory
    mc=malloc(sizeof(*mc));

    // create the widget
    mc->dateTime=dateTime_create();
    mc->widget=(WIDGET)mc->dateTime;
    mc->dateTime->parent=(WIDGET)mc;

    // init members
    widgetMenuDateTime_init(mc);

    return mc;
}

void widgetMenuDateTime_destroy(WIDGETMENU_DATE_TIME mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuDateTime_init(WIDGETMENU_DATE_TIME mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

   
    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuDateTime_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuDateTime_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuDateTime_cfgFromString;

    // properties
    mc->caption="WidgetMenuDateTime";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuDateTime_cfgToString(WIDGETMENU_DATE_TIME mc,char * s){
    *s=0;
}

void widgetMenuDateTime_cfgFromString(WIDGETMENU_DATE_TIME mc,char * s){
    *s=0;
}

//*****************************************************************************
// WIDGETMENU_MENULINE
//*****************************************************************************

WIDGETMENU_MENULINE widgetMenuMenuLine_create(){
    WIDGETMENU_MENULINE mc;

    // allocate WIDGETSPINBOX memory
    mc=malloc(sizeof(*mc));
    
    // init members
    widgetMenuMenuLine_init(mc);

    return mc;
}

void widgetMenuMenuLine_destroy(WIDGETMENU_MENULINE mc){
    widgetMenuItem_destroy((WIDGETMENU_ITEM)mc);
}

void widgetMenuMenuLine_init(WIDGETMENU_MENULINE mc){
    widgetMenuItem_init((WIDGETMENU_ITEM)mc);

    // create the widget
    mc->menuLine=widgetMenuLine_create();
    mc->widget=(WIDGET)mc->menuLine;
    mc->menuLine->parent=(WIDGET)mc;
    
    // methods
    mc->destroy=(WIDGET_DESTROYER)widgetMenuMenuLine_destroy;
    mc->cfgToString=(WIDGETMENU_ITEM_CFGGETTER)widgetMenuMenuLine_cfgToString;
    mc->cfgFromString=(WIDGETMENU_ITEM_CFGSETTER)widgetMenuMenuLine_cfgFromString;

    // properties
    mc->caption="widgetMenuMenuLine";
    mc->widget->margin=WIDGETMENU_WIDGET_MARGIN;
}

void widgetMenuMenuLine_cfgToString(WIDGETMENU_MENULINE mc,char * s){
    *s=0;
}

void widgetMenuMenuLine_cfgFromString(WIDGETMENU_MENULINE mc,char * s){
    *s=0;
}

