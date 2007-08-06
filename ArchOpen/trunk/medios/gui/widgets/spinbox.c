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

#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gfx/kfont.h>


SPINBOX spinbox_create(void){
    SPINBOX t;

    // allocate widget memory
    t=malloc(sizeof(*t));

    // init members
    spinbox_init(t);

    return t;
}

void spinbox_destroy(SPINBOX t){
    widget_destroy((WIDGET)t);
}

void spinbox_computeSize(SPINBOX t)
{
    FONT f;
    f=fnt_fontFromId(t->font);
    t->height = MAX(0,f->height)+4;
    t->width = MAX(0,t->nbDigits*f->width)+4;
}

void spinbox_setFont(SPINBOX t,int font)
{
    t->font=font;
    spinbox_computeSize(t);
}

void spinbox_setParam(SPINBOX t,int minimum,int maximum,int increment,int nbDigits)
{
    t->minimum=minimum;
    t->maximum=maximum;
    t->increment=increment;
    t->nbDigits=nbDigits;
    spinbox_computeSize(t);
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
    t->setFont=(SPINBOX_SETFONT)spinbox_setFont;
    t->setPos=(SPINBOX_SETPOS)spinbox_setPos;
    t->setValue=(SPINBOX_SETVALUE)spinbox_setValue;
    t->getValue=(SPINBOX_GETVALUE)spinbox_getValue;
    
    // properties
    spinbox_setParam(t,0,99,1,2);    
}

void spinbox_setPos(SPINBOX t,int x,int y)
{
    t->x=x;
    t->y=y;
}

int spinbox_getValue(SPINBOX t)
{
    return t->value;
}

void spinbox_setValue(SPINBOX t,int value)
{
    if(value!=t->value && value >= t->minimum && value <= t->maximum)
    {
        t->value=value;
        if (t->onChange!=NULL) t->onChange(t);
    }    
}

bool spinbox_handleEvent(SPINBOX t,int evt){
    bool handled=true;
    int ov=t->value;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)t,evt)) return true;

    switch(evt){        
        case BTN_DOWN:
            if(t->value-t->increment<t->minimum)
                t->value=t->maximum;
            else
                t->value-=t->increment;                
            t->paint(t);
            break;  
        case BTN_UP:
            if(t->value+t->increment>t->maximum)
                t->value=t->minimum;
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

#define STR_SPINBOX(A) ((A)==1?"%01d":(A)==2?"%02d":(A)==3?"%03d":"%04d")

void spinbox_paint(SPINBOX t){
       
    int valw;
    int valh;
    int color;
    int oldf;
    FONT f;
    char vs[10];

    widget_paint((WIDGET)t);

    
    f=fnt_fontFromId(t->font);
    char * str;
    
    valw=MAX(0,t->nbDigits*f->width)+4;
    valh=MAX(0,f->height)+4;
    
    oldf=gfx_fontGet(); // save previous font
    gfx_fontSet(t->font);
    
    /* paint all spinbox */    
    str=STR_SPINBOX(t->nbDigits);
    color=(t->focused)?t->focusColor:t->fillColor;
    gfx_drawRect(color,t->x,t->y,valw,valh);
    snprintf(vs,10,str,t->value);
    gfx_putS(t->foreColor,t->backColor,t->x+2,t->y+2,vs);
    gfx_fontSet(oldf); // restore previous font
}

