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
#include <kernel/kernel.h>

#include <gfx/kfont.h>

#include <gui/chooser.h>

CHOOSER chooser_create(){
    CHOOSER c;

    // allocate widget memory
    c=malloc(sizeof(*c));
    DEBUGWI_CD("chooser create => %x\n",c);
    
    // init members
    chooser_init(c);

    return c;
}

void chooser_destroy(CHOOSER c){
    DEBUGWI_CD("chooser destroy=>call widget\n");
    widget_destroy((WIDGET)c);
}

void chooser_init(CHOOSER c){
    widget_init((WIDGET)c);

    // methods
    c->destroy=(WIDGET_DESTROYER)chooser_destroy;
    c->handleEvent=(WIDGET_EVENTHANDLER)chooser_handleEvent;
    c->paint=(WIDGET_PAINTHANDLER)chooser_paint;
    c->autoSize=(WIDGET_AUTOSIZE)chooser_autoSize;
    
    c->setItemList=(CHOOSER_ITEMLISTSETTER)chooser_setItemList;
    c->setIndex=(CHOOSER_INDEXSETTER)chooser_setIndex;
    c->getIndex=(CHOOSER_INDEXGETTER)chooser_getIndex;
    c->setHandleEvt=(CHOOSER_EVTSETTER)chooser_setHandleEvt;
    c->getHandleEvt=(CHOOSER_EVTGETTER)chooser_getHandleEvt;
    c->setParam=(CHOOSER_PARAMSETTER)chooser_setParam;
    c->getParam=(CHOOSER_PARAMGETTER)chooser_getParam;
    
    c->onChange=NULL;

    // properties
    c->items=NULL;
    c->itemCount=0;
    c->index=-1;
    c->wrap=WIDGET_WRAP_OFF;
    c->evt_prev=BTN_LEFT;
    c->evt_nxt=BTN_RIGHT;
    c->orientation=WIDGET_ORIENTATION_HORIZ;
        
    /*first size compute with empty list*/
    c->autoSize(c);
}

void chooser_setItemList(CHOOSER c,char ** items,int itemCount)
{
    if(!items || itemCount<=0)
    {
        c->items=NULL;
        c->itemCount=0;
        c->index=-1;
    }
    else
    {
        c->items=items;
        c->itemCount=itemCount;
        c->index=0;
    }
    c->autoSize(c);
}

void chooser_setIndex(CHOOSER c,int index)
{
    if(index<0 || index>=c->itemCount)
        return;
    c->index=index;
}

int chooser_getIndex(CHOOSER c)
{
    return c->index;
}

void chooser_setHandleEvt(CHOOSER c, int evt_prev, int evt_nxt)
{
    c->evt_prev=evt_prev;       
    c->evt_nxt=evt_nxt;
}

void chooser_getHandleEvt(CHOOSER c, int * evt_prev, int * evt_nxt)
{
    if(evt_prev)
        *evt_prev=c->evt_prev;
    if(evt_nxt)
        *evt_nxt=c->evt_nxt;
}

void chooser_setParam(CHOOSER c, int orientation, int wrap)
{
    if(orientation==WIDGET_ORIENTATION_HORIZ || orientation==WIDGET_ORIENTATION_VERT)
        c->orientation=orientation;
    if(wrap==WIDGET_WRAP_ON || wrap==WIDGET_WRAP_OFF)
        c->wrap=wrap;
}

void chooser_getParam(CHOOSER c, int * orientation, int * wrap)
{
    if(orientation)
        *orientation=c->orientation;
    if(wrap)
        *wrap=c->wrap;    
}

bool chooser_handleEvent(CHOOSER c,int evt){
    bool handled=true;
    int oi=c->index;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)c,evt)) return true;

    if(evt==c->evt_prev)
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
    else if(evt==c->evt_nxt)
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

void chooser_autoSize(CHOOSER c)
{
    int w,h,w2;
    int nh,nw;
    int of,i;
       
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(c->font);
    
    w=0;
    gfx_getStringSize("H",NULL,&h);
    
    nh=h+2*c->margin;
    
    if(c->items)
    {
        w=0;
        for(i=0;i<c->itemCount;i++)
        {
            gfx_getStringSize(c->items[i],&w2,NULL);
            if(w2>w) w=w2;
        }
    }
    /*adding box+arrow*/
    nw=w+2*(nh-2*c->margin)+2*c->margin+4;
    gfx_fontSet(of);
    
    c->internalWidth=nw;
    c->internalHeight=nh;
    
    if(c->internalHeight>c->height)
        c->height=c->internalHeight;
    if(c->useMaxWidth && c->internalWidth<c->width)
        c->internalWidth=c->width;
    //c->updateSize(c,nw,nh);
}

void chooser_paint(CHOOSER c){
    int x,y,bs;
    int a1,a2,a3;
    int color;

    widget_paint((WIDGET)c);

    color=(c->focused)?c->focusColor:c->fillColor;

    // text
    if(c->index>=0 && c->index<c->itemCount && c->items && c->items[c->index]){ // make sure everything is ok
        char * txt = c->items[c->index];
        int of=gfx_fontGet(); // save previous font
        int tw,th;
        int w;
        
        w=c->internalWidth-(c->internalHeight-2*c->margin)*2-2*c->margin-4;

        gfx_fontSet(c->font);

        gfx_getStringSize(txt,&tw,&th);
        x=c->x+c->internalHeight-2*c->margin+2+(w-tw)/2;
        y=c->y+(c->internalHeight-th)/2;

        gfx_putS(c->foreColor,c->backColor,x,y,txt);

        gfx_fontSet(of); // restore previous font
    }

    // arrows
    y=c->y+c->margin;
    bs=c->internalHeight-2*c->margin;
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
    x=c->x+c->internalWidth-c->margin-bs;
        
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
