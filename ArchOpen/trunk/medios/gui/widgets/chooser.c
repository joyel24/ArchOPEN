/*
* kernel/gfx/gui/widget/chooser.c
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

#include <gfx/kfont.h>

#include <gui/chooser.h>

CHOOSER chooser_create(){
    CHOOSER c;

    // allocate widget memory
    c=malloc(sizeof(*c));

    // init members
    chooser_init(c);

    return c;
}

void chooser_destroy(CHOOSER c){
    widget_destroy((WIDGET)c);
}

void chooser_init(CHOOSER c){
    widget_init((WIDGET)c);

    // methods
    c->destroy=(WIDGET_DESTROYER)chooser_destroy;
    c->handleEvent=(WIDGET_EVENTHANDLER)chooser_handleEvent;
    c->paint=(WIDGET_PAINTHANDLER)chooser_paint;
    c->onChange=NULL;

    // properties
    c->items=NULL;
    c->itemCount=0;
    c->index=-1;
    c->wrap=WIDGET_WRAP_OFF;
    c->orientation=WIDGET_ORIENTATION_HORIZ;
}

bool chooser_handleEvent(CHOOSER c,int evt){
    bool handled=true;
    int oi=c->index;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)c,evt)) return true;

    if(evt==BTN_LEFT)
    {
        if (c->itemCount){
            if(c->index == 0)
            {
                if(c->wrap)
                    c->index=c->itemCount-1;
                else
                    c->index=0;
            }
            else
                c->index--;
        }else{
            c->index=-1;
        }
        c->paint(c);
    }
    else if(evt==BTN_RIGHT)
    {
        if (c->itemCount){
            if(c->index == (c->itemCount-1))
            {
                if(c->wrap)
                    c->index=0;
                else
                    c->index=c->itemCount-1;
            }
            else
                c->index+=1;
        }else{
            c->index=-1;
        }
        c->paint(c);
    }
    else
    {
        handled=false;
    }

    // onChange event
    if (c->onChange!=NULL && oi!=c->index) c->onChange(c);

    return handled;
}

void chooser_paint(CHOOSER c){
    int x,y,bs;
    int a1,a2,a3;
    int color;

    widget_paint((WIDGET)c);

    color=(c->focused)?c->focusColor:c->backColor;

    // text
    if(c->index>=0 && c->index<c->itemCount && c->items && c->items[c->index]){ // make sure everything is ok
        char * txt = c->items[c->index];
        int of=gfx_fontGet(); // save previous font
        int tw,th;

        gfx_fontSet(c->font);

        gfx_getStringSize(txt,&tw,&th);
        x=c->x+(c->width-tw)/2;
        y=c->y+(c->height-th)/2;

        gfx_putS(c->foreColor,c->backColor,x,y,txt);

        gfx_fontSet(of); // restore previous font
    }

    // arrows
    y=c->y+c->margin;
    bs=c->height-2*c->margin;
    a1=bs*1/4;
    a2=bs*1/2;
    a3=bs*3/4;

    // left arrow
    x=c->x+c->margin;
        
    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);
    
    if(c->orientation==WIDGET_ORIENTATION_HORIZ)
    {
        gfx_drawLine(c->foreColor,x+a1,y+a2,x+a3,y+a1);
        gfx_drawLine(c->foreColor,x+a1,y+a2,x+a3,y+a3);
        gfx_drawLine(c->foreColor,x+a3,y+a1,x+a3,y+a3);
    }
    else
    {
        gfx_drawLine(c->foreColor,x+a1,y+a3,x+a2,y+a1);
        gfx_drawLine(c->foreColor,x+a3,y+a3,x+a2,y+a1);
        gfx_drawLine(c->foreColor,x+a1,y+a3,x+a3,y+a3);
    }    
    
    // right arrow
    x=c->x+c->width-c->margin-bs;
        
    gfx_drawRect(c->foreColor,x,y,bs,bs);
    gfx_fillRect(color,x+1,y+1,bs-2,bs-2);
    
    if(c->orientation==WIDGET_ORIENTATION_HORIZ)
    {
        gfx_drawLine(c->foreColor,x+a3,y+a2,x+a1,y+a1);
        gfx_drawLine(c->foreColor,x+a3,y+a2,x+a1,y+a3);
        gfx_drawLine(c->foreColor,x+a1,y+a1,x+a1,y+a3);
    }
    else
    {        
        gfx_drawLine(c->foreColor,x+a1,y+a1,x+a2,y+a3);
        gfx_drawLine(c->foreColor,x+a3,y+a1,x+a2,y+a3);
        gfx_drawLine(c->foreColor,x+a1,y+a1,x+a3,y+a1);        
    }

}
