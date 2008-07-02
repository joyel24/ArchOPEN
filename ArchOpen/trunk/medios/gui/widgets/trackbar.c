/*
* kernel/gfx/gui/widget/trackbar.c
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

#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

#include <gfx/kfont.h>

#include <gui/trackbar.h>

TRACKBAR trackbar_create(){
    TRACKBAR t;

    // allocate widget memory
    t=malloc(sizeof(*t));
    DEBUGWI_CD("trck create => %x\n",t);
    
    // init members
    trackbar_init(t);

    return t;
}

void trackbar_destroy(TRACKBAR t){
    DEBUGWI_CD("trck bar destroy=>call widget\n");
    widget_destroy((WIDGET)t);
}

void trackbar_init(TRACKBAR t){
    widget_init((WIDGET)t);

    // methods
    t->destroy=(WIDGET_DESTROYER)trackbar_destroy;
    t->handleEvent=(WIDGET_EVENTHANDLER)trackbar_handleEvent;
    t->paint=(WIDGET_PAINTHANDLER)trackbar_paint;    
    t->setValPosition=(TRACKBAR_SETVALPOSITION)trackbar_setValPosition;
    t->getValPosition=(TRACKBAR_GETVALPOSITION)trackbar_getValPosition;
    t->setValue=(TRACKBAR_SETVALUE)trackbar_setValue;
    t->getValue=(TRACKBAR_GETVALUE)trackbar_getValue;
    t->setParam=(TRACKBAR_SETPARAM)trackbar_setParam;
    t->getParam=(TRACKBAR_GETPARAM)trackbar_getParam;
    t->onChange=NULL;
    t->autoSize=(WIDGET_AUTOSIZE)trackbar_autoSize;

    // properties
    t->value=0;
    t->minSpacing=TRACKBAR_MIN_SPACING;
    t->numValueDigits=3;
    t->val_position=TR_VAL_RIGHT;
    
    t->setParam(t,0,50,5); /* this will call autoSize */
}

bool trackbar_handleEvent(TRACKBAR t,int evt){
    bool handled=true;
    int ov=t->value;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)t,evt)) return true;

    switch(evt){
        case BTN_LEFT:
            t->value=MAX(t->value-t->increment,t->minimum);
            t->paint(t);
            break;
        case BTN_RIGHT:
            t->value=MIN(t->value+t->increment,t->maximum);
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

void trackbar_setValue(TRACKBAR t,int value)
{
    if(value<=t->maximum && value>=t->minimum)
        t->value=value;
}

int trackbar_getValue(TRACKBAR t)
{
    return t->value;
}

void trackbar_setParam(TRACKBAR t,int min,int max,int increment)
{
    int a,b;
    
    DEBUGWI("[trkbar|setParam]\n");
    
    if(min>max) {a=min; min=max; max=a;}
    
    t->minimum=min;
    t->maximum=max;
    if(increment<=0) increment=1;
    t->increment=increment;
    
    if(t->value<min) t->value=min;
    if(t->value>max) t->value=max;
    
    if(min<0) a=1;
    else a=0;
    do { min/=10; a++; } while(min!=0);
    
    if(max<0) b=1;
    else b=0;
    do { max/=10; b++; } while(max!=0);
    
    t->numValueDigits=MAX(a,b);
    
    t->autoSize(t);
    DEBUGWI("[trkbar|setParam] done\n");
}

void trackbar_getParam(TRACKBAR t,int *min,int *max,int *increment)
{
    if(min) *min=t->minimum;
    if(max) *max=t->maximum;
    if(increment) *increment=t->increment;
}

void trackbar_setValPosition(TRACKBAR t,int val_position)
{
    if(val_position==TR_VAL_LEFT || val_position==TR_VAL_RIGHT)
        t->val_position=val_position;
}

int trackbar_getValPosition(TRACKBAR t)
{
    return t->val_position;
}

void trackbar_autoSize(TRACKBAR t)
{
    int w,h;
    int of;
    int cursor_w,txt_w;
    int nw,nh;
       
    of=gfx_fontGet(); // save previous font
    gfx_fontSet(t->font);    
    gfx_getStringSize("H",&w,&h); /* std hight and width for widget font*/
            
    nh=h+t->margin*2;
    DEBUGWI("[trkbar|autoSize] font height = %d ==> bar height=%d\n",h,nh);
    
    cursor_w=nh*TRACKBAR_CURSOR_RATIO;
    
    DEBUGWI("[trkbar|autoSize] cursor width=%d\n",cursor_w);
    
    txt_w=t->numValueDigits*w;
    DEBUGWI("[trkbar|autoSize] txt width=%d\n",txt_w);
    
    t->numTicks=(t->maximum-t->minimum)/t->increment-1;
    nw=txt_w+TRACKBAR_SPACING+2*t->margin+cursor_w+t->numTicks*(t->minSpacing+cursor_w);
    
    DEBUGWI("[trkbar|autoSize] nbTick=%d tot width=%d (max w=%d)\n",t->numTicks,nw,t->width);
    
    if(nw>t->width)
    {
        t->numTicks=t->width-(txt_w+TRACKBAR_SPACING+2*t->margin+cursor_w);
        t->numTicks=t->numTicks/(t->minSpacing+cursor_w);
        nw=txt_w+TRACKBAR_SPACING+2*t->margin+cursor_w+t->numTicks*(t->minSpacing+cursor_w);
        DEBUGWI("[trkbar|autoSize] (recomputed) nbTick=%d tot width=%d\n",t->numTicks,nw);                
    }
    
    t->internalWidth=nw;
    t->internalHeight=nh;
    if(t->internalHeight>t->height)
        t->height=t->internalHeight;
#warning useMaxWidth not implemented
    
    DEBUGWI("[trkbar|autoSize] size/internal (%d,%d)/(%d;%d)\n",t->width,t->height,nw,nh);    
    //t->updateSize(t,nw,nh);
    gfx_fontSet(of);
}

void trackbar_paint(TRACKBAR t){
    int i;
    int x,y,w,h;
    int valw;
    int curw;
    int color;
    FONT f;

    widget_paint((WIDGET)t);

    f=fnt_fontFromId(t->font);
    valw=MAX(0,t->numValueDigits*f->width);

    curw=t->internalHeight*TRACKBAR_CURSOR_RATIO;

    color=(t->focused)?t->focusColor:t->fillColor;

    x=t->x+t->margin;
    
    DEBUGWI("[trkbar|paint] starting drawing at %d\n",x);
    
    // value
    if(valw && t->val_position==TR_VAL_LEFT)
    {
        int of=gfx_fontGet(); // save previous font
        char vs[10];
        y=t->y+(t->internalHeight-f->height)/2;
        sprintf(vs,"%d",t->value);
        gfx_fontSet(t->font);
        gfx_putS(t->foreColor,t->backColor,x,y,vs);
        gfx_fontSet(of); // restore previous font
        x+=(valw+1);
        DEBUGWI("[trkbar|paint] Txt on left => new x=%d\n",x);
    }
    
        
    // bar
    x+=curw/2;
    y=t->y+t->internalHeight/3;
    w=t->internalWidth-valw-TRACKBAR_SPACING-2*t->margin-curw;
    h=t->internalHeight/3;

    DEBUGWI("[trkbar|paint] drawing bar at (%d,%d) size (%d,%d)\n",x,y,w,h);
    
    gfx_drawRect(t->foreColor,x,y,w,h);

    // ticks
    for(i=1;i<=t->numTicks;i++){
        int tx=x+i*w/(t->numTicks+1);

        gfx_drawLine(t->foreColor,tx,y,tx,y+h-1);
        DEBUGWI("[trkbar|paint] tick at (%d,%d)\n",tx,y);
    };

    // cursor
    if(t->value>=t->minimum && t->value<=t->maximum && (t->maximum-t->minimum)!=0){
      x=t->x+(t->value-t->minimum)*w/(t->maximum-t->minimum)+t->margin
              +((valw && t->val_position==TR_VAL_LEFT)?valw+t->margin+1:0);
      y=t->y+t->margin;
      h=t->internalHeight-2*t->margin;
  
      gfx_drawRect(t->foreColor,x,y,curw,h);
      gfx_fillRect(color,x+1,y+1,curw-2,h-2);
      
      DEBUGWI("[trkbar|paint] cursor at (%d,%d)\n",x,y);
    }

    // value
    if (valw && t->val_position==TR_VAL_RIGHT)
    {
        int of=gfx_fontGet(); // save previous font
        char vs[10];

        x=t->x+t->internalWidth-valw-t->margin;
        y=t->y+(t->internalHeight-f->height)/2;

        sprintf(vs,"%d",t->value);

        gfx_fontSet(t->font);

        gfx_putS(t->foreColor,t->backColor,x,y,vs);

        gfx_fontSet(of); // restore previous font
        DEBUGWI("[trkbar|paint] Txt on left at x=%d\n",x);
    }
}

