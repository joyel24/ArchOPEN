/*
* kernel/gfx/gui/widget/checkbox.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#include <sys_def/stddef.h>

#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

#include <gfx/kfont.h>

#include <gui/checkbox.h>

CHECKBOX checkbox_create(){
    CHECKBOX c;

    // allocate widget memory
    c=malloc(sizeof(*c));
    DEBUGWI_CD("chkbox create => %x\n",c);
    // init members
    checkbox_init(c);

    return c;
}

void checkbox_destroy(CHECKBOX c){
    DEBUGWI_CD("chkbox bar destroy=>call widget\n");
    widget_destroy((WIDGET)c);
}

void checkbox_init(CHECKBOX c){
    widget_init((WIDGET)c);

    // methods
    c->destroy=(WIDGET_DESTROYER)checkbox_destroy;
    c->handleEvent=(WIDGET_EVENTHANDLER)checkbox_handleEvent;
    c->paint=(WIDGET_PAINTHANDLER)checkbox_paint;
    c->autoSize=(WIDGET_AUTOSIZE)checkbox_autoSize;
    c->setState=(CHECKBOX_STATESETTER)checkbox_setState;
    c->getState=(CHECKBOX_STATEGETTER)checkbox_getState;
    c->onChange=NULL;

    // properties
    c->checked=false;
    checkbox_autoSize(c);
}

void checkbox_setState(CHECKBOX c,int val){
    c->checked=(val?1:0);
}

int checkbox_getState(CHECKBOX c){
    return (c->checked?1:0);
}

bool checkbox_handleEvent(CHECKBOX c,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)c,evt)) return true;

    switch(evt){
        case BTN_LEFT:
        case BTN_RIGHT:
            if(!c->clickOnRightLeft)
                break;
        case WIDGET_ACTION_BTN:
            c->checked=!c->checked;
            c->paint(c);
            if (c->onChange!=NULL) c->onChange(c);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void checkbox_autoSize(CHECKBOX c)
{
    int w,h,of;
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(c->font);
    gfx_getStringSize("H",&w,&h); /*getting txt size*/
    gfx_fontSet(of);
    
    c->internalWidth=h;
    c->internalHeight=h+2*c->margin;
    
    if(c->internalHeight>c->height)
        c->height=c->internalHeight;
    
    //c->updateSize(c,h,h+2*c->margin);
}

void checkbox_paint(CHECKBOX c){
    int x,y,bs;
    int color;

    widget_paint((WIDGET)c);

    color=(c->focused)?c->focusColor:c->fillColor;

    // box
    bs=c->internalHeight-2*c->margin;
    x=c->x+c->margin;
    y=c->y+c->margin;

    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);

    if (c->checked){
        gfx_drawLine(c->foreColor,x,y,x+bs-1,y+bs-1);
        gfx_drawLine(c->foreColor,x+bs-1,y,x,y+bs-1);
    }    
}
