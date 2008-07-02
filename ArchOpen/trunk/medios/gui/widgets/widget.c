/*
* kernel/gfx/gui/widget/widget.c
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

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gui/widget.h>

WIDGET widget_create(){
    WIDGET w;

    // allocate widget memory
    w=malloc(sizeof(*w));

    DEBUGWI_CD("widget create => %x\n",w);

    // init members
    widget_init(w);

    return w;
}

void widget_destroy(WIDGET w){
    // free widget memory
    DEBUGWI_CD("widget destroy=>free %x\n",w);
    free(w);
}

void widget_init(WIDGET w){
    // methods
    w->destroy=(WIDGET_DESTROYER)widget_destroy;
    w->handleEvent=(WIDGET_EVENTHANDLER)widget_handleEvent;
    w->paint=(WIDGET_PAINTHANDLER)widget_paint;
    w->setRect=(WIDGET_RECTSETTER)widget_setRect;
    w->setPos=(WIDGET_POSSETTER)widget_setPos;
    w->setSize=(WIDGET_SIZESETTER)widget_setSize;
    w->updateSize=(WIDGET_SIZEUPDATER)widget_updateSize;
    w->autoSize=(WIDGET_AUTOSIZE)widget_autoSize;
    w->setFont=(WIDGET_FONTSETTER)widget_setFont;
    w->update=(WIDGET_UPDATER)widget_update;
    w->setAlign=(WIDGET_ALIGNSETTER)widget_setAlign;
    w->getInternalSize=(WIDGET_INTERNALSIZEGETTER)widget_getInternalSize;
    // properties
    w->x=0;
    w->y=0;
    w->oldX=-2; /* setting to -2 to show it has never been used */
    w->oldY=-2;
    w->width=0;
    w->height=0;
    w->oldWidth=-2;
    w->oldHeight=-2;
    w->internalWidth=0;
    w->internalHeight=0;
    w->useMaxWidth=false;
    w->margin=WIDGET_DEFAULT_MARGIN;
    w->clearBackground=true;
    w->canFocus=true;
    w->focused=false;
    w->foreColor=WIDGET_DEFAULT_FORECOLOR;
    w->backColor=WIDGET_DEFAULT_BACKCOLOR;
    w->fillColor=WIDGET_DEFAULT_FILLCOLOR;
    w->focusColor=WIDGET_DEFAULT_FOCUSCOLOR;
    w->font=WIDGET_DEFAULT_FONT;
    w->needSelection=true;
    w->tag=0;
    w->data=NULL;
    w->parent=NULL;
    w->alignment=WA_LEFT;
}

bool widget_handleEvent(WIDGET w,int evt){
    bool handled=true;

    switch(evt){
        case EVT_REDRAW:
            w->paint(w);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void widget_updateSize(WIDGET w,int width,int height)
{

    /* are we reducing the size ? */

    if(w->oldWidth!=-2) /*we'll wait to have initial paint for saving oldSize*/
    {
        if(w->oldHeight==-1)
        {
            if(w->height>height)
                w->oldHeight=w->height;
        }
        else
        {
            if(w->oldHeight<=height)
                w->oldHeight=-1;
        }

        if(w->oldWidth==-1)
        {
            if(w->width>width)
                w->oldWidth=w->width;
        }
        else
        {
            if(w->oldWidth<=width)
                w->oldWidth=-1;
        }
    }
    /* changing size */
    w->width=w->internalWidth=width;
    w->height=w->internalHeight=height;
}

void widget_setSize(WIDGET w,int width,int height)
{
    widget_updateSize(w,width,height);
    /* informing parent of the change */
    if(w->parent) ((WIDGET)w->parent)->update(w->parent,w);
}

void widget_getInternalSize(WIDGET w,int * width,int * height)
{
    if(width)
       *width=w->internalWidth;
    if(height)
        *height=w->internalHeight;
}


void widget_autoSize(WIDGET w)
{
    w->internalWidth=w->width;
    w->internalHeight=w->height;
    //w->setSize(w,0,0);
}

void widget_paint(WIDGET w){

    if(w->oldX==-2) { w->oldX=w->oldY=w->oldWidth=w->oldHeight=-1;} /*this is the first paint*/

    if(w->clearBackground)
    {
        if(w->oldX!=-1 || w->oldY!=-1)
        {
            if(w->oldWidth!=-1 || w->oldHeight!=-1)
            {
                gfx_fillRect(w->backColor,
                            w->oldX==-1?w->x:w->oldX,
                            w->oldY==-1?w->y:w->oldY,
                            w->oldWidth==-1?w->width:w->oldWidth,
                            w->oldHeight==-1?w->height:w->oldHeight);
                w->oldWidth=w->oldHeight=-1;
            }
            else
            {
                gfx_fillRect(w->backColor,
                             w->oldX==-1?w->x:w->oldX,
                            w->oldY==-1?w->y:w->oldY,
                            w->width,w->height);
            }
            w->oldX=w->oldY=-1;
        }
        else
        {
            if(w->oldWidth!=-1 || w->oldHeight!=-1)
            {
                gfx_fillRect(w->backColor,w->x,w->y,
                            w->oldWidth==-1?w->width:w->oldWidth,
                            w->oldHeight==-1?w->height:w->oldHeight);
                w->oldWidth=w->oldHeight=-1;
            }
            else
            {
                gfx_fillRect(w->backColor,w->x,w->y,w->width,w->height);
            }
        }
    }
}

void widget_setRect(WIDGET w,int x,int y,int width,int height){
    w->setPos(w,x,y);
    w->setSize(w,width,height);
}

void widget_setPos(WIDGET w,int x,int y)
{
    if(w->oldX!=-2) /*we'll wait to have initial paint for saving oldPos*/
    {
        if(w->oldX==-1) /* no old position */
        {
            if(x!=w->x) /* are we changing x ?*/
                w->oldX=w->x;
        }
        else
        {
            /* no paint since last change*/
            if(w->oldX==x)
                w->oldX=-1; /* we are back to old pos */
        }
        if(w->oldY==-1) /* no old position */
        {
            if(x!=w->y) /* are we changinf x ?*/
                w->oldY=w->y;
        }
        else
        {
            /* no paint since last change*/
            if(w->oldY==y)
                w->oldY=-1; /* we are back to old pos */
        }
    }
    w->x=x;
    w->y=y;
}

void widget_setFont(WIDGET w, int f)
{
    w->font=f; /* changes font */
    w->autoSize(w); /* recompute the size */
}

void widget_setAlign(WIDGET w, W_ALIGNMENT alignment)
{
    w->alignment=alignment;
    w->autoSize(w);
    /* don't need to call parent update as size has not change*/
}

void widget_update(WIDGET parent, WIDGET child)
{
    /*nothing to be done here, should be implemented by real parent*/
}

