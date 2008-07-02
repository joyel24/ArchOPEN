/*
* kernel/gfx/gui/widget/button.c
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

#include <gui/button.h>

BUTTON button_create(){
    BUTTON b;

    // allocate widget memory
    b=malloc(sizeof(*b));
    DEBUGWI_CD("btn create => %x\n",b);
    // init members
    button_init(b);

    return b;
}

void button_destroy(BUTTON b){
    DEBUGWI_CD("btn bar destroy=>call widget\n");
    widget_destroy((WIDGET)b);
}

void button_init(BUTTON b){
    widget_init((WIDGET)b);

    // methods
    b->destroy=(WIDGET_DESTROYER)button_destroy;
    b->handleEvent=(WIDGET_EVENTHANDLER)button_handleEvent;
    b->paint=(WIDGET_PAINTHANDLER)button_paint;
    b->autoSize=(WIDGET_AUTOSIZE)button_autoSize;
    b->setText=(WIDGET_TEXTSETTER)button_setText;
    b->onClick=NULL;
    b->clickOnRightLeft=1;
    b->needSelection=false;

    // properties
    b->caption=NULL; /*starting with an empty caption*/
    
    // init size
    button_autoSize(b);
}

bool button_handleEvent(BUTTON b,int evt){
    bool handled=true;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)b,evt)) return true;

    switch(evt){
        case BTN_LEFT:
            if(!b->clickOnRightLeft)
                break;
        case BTN_RIGHT:
            if(!b->clickOnRightLeft)
                break;
        case WIDGET_ACTION_BTN:
            if (b->onClick!=NULL) b->onClick(b);
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void button_autoSize(BUTTON b)
{
    int w,h,of;
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(b->font);
    if(b->caption && (*b->caption)!='\0')  /*do we have the string and is it empty ?*/
        gfx_getStringSize(b->caption,&w,&h); /* none empty string */
    else
        gfx_getStringSize("H",&w,&h); /*empty string => draw as if there was one char*/
    gfx_fontSet(of); /* restoring previous font */
    
    b->internalWidth=2*b->margin+2*BUTTON_INTERNAL_WSPACE+w;
    b->internalHeight=2*b->margin+2*BUTTON_INTERNAL_HSPACE+h;    
    
    if(b->internalHeight>b->height)
        b->height=b->internalHeight;
    
    if(b->useMaxWidth && b->internalWidth<b->width)
        b->internalWidth=b->width;
    
    //b->updateSize(b,2*b->margin+2*BUTTON_INTERNAL_WSPACE+w,2*b->margin+2*BUTTON_INTERNAL_HSPACE+h);
}

void button_paint(BUTTON b){
    int x,y;
    int w,h;
    int color;
    int of;

    /* widget paint*/
    widget_paint((WIDGET)b);

    /* are we focused ?*/
    color=(b->focused)?b->focusColor:b->fillColor;

    // frame
    x=b->x;
    y=b->y;
    w=b->internalWidth;
    h=b->internalHeight;

    /*
    gfx_drawRect(b->foreColor,x,y,w,h);    
    if (b->focused) gfx_fillRect(color,x+1,y+1,w-2,h-2);
    */
    
    gfx_drawLine(b->foreColor,x+2,y,x+(w-1)-2,y);
    gfx_drawLine(b->foreColor,x+2,y+(h-1),x+(w-1)-2,y+(h-1));
    gfx_drawLine(b->foreColor,x,y+2,x,y+(h-1)-2);
    gfx_drawLine(b->foreColor,x+(w-1),y+2,x+(w-1),y+(h-1)-2);
    
    gfx_drawPixel(b->foreColor,x+1,y+1);
    gfx_drawPixel(b->foreColor,x+w-1-1,y+1);
    gfx_drawPixel(b->foreColor,x+1,y+h-1-1);
    gfx_drawPixel(b->foreColor,x+w-1-1,y+h-1-1);
    
    /*always fill */
    gfx_fillRect(color,x+2,y+1,w-4,h-2);
    gfx_drawLine(color,x+1,y+2,x+1,y+h-2-1);
    gfx_drawLine(color,x+w-1-1,y+2,x+w-1-1,y+h-2-1);
        
    /* caption */
    if(b->caption && (*b->caption)!='\0')  /*do we have the string and is it empty ?*/
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(b->font);
        gfx_getStringSize(b->caption,&w,&h);    
        x=b->x+(b->internalWidth-w)/2;
        y=b->y+(b->internalHeight-h)/2;
        gfx_putS(b->foreColor,color,x,y,b->caption);
        gfx_fontSet(of); // restore previous font
    }    
}

void button_setText(BUTTON b,char * txt)
{
    b->caption=txt;
    b->autoSize(b);
}
