/*
* medios/gui/widget/spinbox.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/spinbox.h>

#include <sys_def/stddef.h>

#include <lib/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gfx/kfont.h>


SPINBOX spinbox_create(void){
    SPINBOX t;

    // allocate widget memory
    t=malloc(sizeof(*t));
    DEBUGWI_CD("spinBox create => %x\n",t);

    // init members
    spinbox_init(t);

    return t;
}

void spinbox_destroy(SPINBOX t){
    DEBUGWI_CD("spinbox destroy=>call widget\n");
    widget_destroy((WIDGET)t);
}

void spinbox_init(SPINBOX t)
{
    widget_init((WIDGET)t);

    // methods
    t->destroy=(WIDGET_DESTROYER)spinbox_destroy;
    t->handleEvent=(WIDGET_EVENTHANDLER)spinbox_handleEvent;
    t->paint=(WIDGET_PAINTHANDLER)spinbox_paint;
    t->onChange=NULL;
    t->setParam=(SPINBOX_SETPARAM)spinbox_setParam;
    t->getParam=(SPINBOX_GETPARAM)spinbox_getParam;
    t->setValue=(SPINBOX_SETVALUE)spinbox_setValue;
    t->getValue=(SPINBOX_GETVALUE)spinbox_getValue;
    t->setWrap=(SPINBOX_SETWRAP)spinbox_setWrap;
    t->getWrap=(SPINBOX_GETWRAP)spinbox_getWrap;
    t->autoSize=(WIDGET_AUTOSIZE)spinbox_autoSize;
    
    // properties
    t->setWrap(t,WIDGET_WRAP_ON);
    t->setParam(t,0,99,1,2); /* autoSize will be called here */
    t->setValue(t,0);
}

void spinbox_setParam(SPINBOX t,int minimum,int maximum,int increment,int nbDigits)
{
    int tmp;
    if(minimum>maximum)
    {
        tmp=minimum;
        minimum=maximum,
        maximum=tmp;
    }
    t->minimum=minimum;
    t->maximum=maximum;
    
    if(increment<1) increment=1;
    t->increment=increment;
    
    if(nbDigits<1) nbDigits=1;
    t->nbDigits=nbDigits;
    
    t->autoSize(t);
    
    if(t->value<minimum) t->value=minimum;
    if(t->value>maximum) t->value=maximum;
}

void spinbox_setWrap(SPINBOX t,int wrap_value)
{
    if(wrap_value==WIDGET_WRAP_ON || wrap_value==WIDGET_WRAP_OFF)
        t->wrap=wrap_value;
}

int spinbox_getWrap(SPINBOX t)
{
    return t->wrap;   
}

void spinbox_getParam(SPINBOX t,int * minimum,int * maximum,int * increment,int * nbDigits)
{
    if(minimum) *minimum=t->minimum;
    if(maximum) *maximum=t->maximum;
    if(increment) *increment=t->increment;
    if(nbDigits) *nbDigits=t->nbDigits;
}

int spinbox_getValue(SPINBOX t)
{
    return t->value;
}

void spinbox_setValue(SPINBOX t,int value)
{
    if(value!=t->value && value >= t->minimum && value <= t->maximum)
        t->value=value;
}

bool spinbox_handleEvent(SPINBOX t,int evt){
    bool handled=true;
    int ov=t->value;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)t,evt)) return true;

    switch(evt){        
        case BTN_DOWN:
            if(t->value-t->increment<t->minimum)
            {
                if(t->wrap==WIDGET_WRAP_ON)
                    t->value=t->maximum;
            }
            else
                t->value-=t->increment;             
            t->paint(t);
            break;  
        case BTN_UP:
            if(t->value+t->increment>t->maximum)
            {
                if(t->wrap==WIDGET_WRAP_ON)
                    t->value=t->minimum;
            }
            else
                t->value+=t->increment;
            t->paint(t);
            break;          
        default:
            handled=false;
            break;
    }

    // onChange event
    if (t->onChange!=NULL && ov!=t->value) t->onChange(t);

    return handled;
}

void spinbox_autoSize(SPINBOX t)
{
    int w,h;
    int of;
       
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(t->font);    
    gfx_getStringSize("H",&w,&h);
    gfx_fontSet(of);
    
    t->internalWidth=t->nbDigits*w+4;
    t->internalHeight=h+4;
    
    if(t->internalHeight>t->height)
        t->height=t->internalHeight;
    
    //t->updateSize(t,t->nbDigits*w+4,h+4);    
}

#define STR_SPINBOX(A) ((A)==1?"%01d":(A)==2?"%02d":(A)==3?"%03d":"%04d")

void spinbox_paint(SPINBOX t){
    int color;
    int oldf;
    char vs[10];
    char * str;

    widget_paint((WIDGET)t);
        
    oldf=gfx_fontGet(); // save previous font
    gfx_fontSet(t->font);
    
    /* paint all spinbox */   
    str=STR_SPINBOX(t->nbDigits);
    color=(t->focused)?t->focusColor:t->fillColor;
    gfx_drawRect(color,t->x,t->y,t->internalWidth,t->internalHeight);
    snprintf(vs,10,str,t->value);
    gfx_putS(t->foreColor,t->backColor,t->x+2,t->y+2,vs);
    gfx_fontSet(oldf); // restore previous font
}

