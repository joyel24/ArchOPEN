/*
* kernel/gfx/gui/widget/iconmenu.c
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

#include <gui/iconmenu.h>

#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

//*****************************************************************************
// ICONMENU_ITEM
//*****************************************************************************

ICONMENU_ITEM iconMenuItem_create(){
    ICONMENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));

    // init members
    iconMenuItem_init(mi);

    return mi;
}

void iconMenuItem_destroy(ICONMENU_ITEM mi){
    menuItem_destroy((MENU_ITEM)mi);
}

void iconMenuItem_init(ICONMENU_ITEM mi){
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)iconMenuItem_destroy;
    mi->paint=(WIDGET_PAINTHANDLER)iconMenuItem_paint;

    // properties
    mi->caption="IconMenuItem";
    memset(&mi->icon,0,sizeof(BITMAP));
    mi->iconPosition=IMIP_TOP;
}

void iconMenuItem_paint(ICONMENU_ITEM mi){
    int tx,ty;
    int ix,iw,ih;
    int tw,th;
    int color;
    int of;

    widget_paint((WIDGET)mi);

    color=(mi->focused)?mi->focusColor:mi->backColor;

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(mi->font);

    gfx_getStringSize(mi->caption,&tw,&th);

    switch (mi->iconPosition){
        default:
        case IMIP_TOP:
            ix=mi->x;
            iw=mi->width-(mi->itemCaption==IM_HAS_CAPTION?0:2);
            ih=mi->height-mi->margin-(mi->itemCaption==IM_HAS_CAPTION?th:2);
            tx=mi->x+(mi->width-tw)/2;
            ty=mi->y+mi->height-mi->margin-th;
            break;
        case IMIP_LEFT:
            ix=mi->x+mi->margin;
            ih=mi->height;
            iw=ih;
            tx=ix+iw+ICONMENU_ITEM_SPACING;
            ty=mi->y+(mi->height-th)/2;
            break;
    }

    // icon
    gfx_drawResizedBitmap(&mi->icon,ix,mi->y,iw,ih,RESIZE_INTEGER);

    // caption
    if(mi->itemCaption==IM_HAS_CAPTION || mi->iconPosition==IMIP_LEFT)
        gfx_putS(mi->foreColor,color,tx,ty,mi->caption);
    else
        gfx_drawRect(color,mi->x+1,mi->y+1,mi->width-2,mi->height-2);
        

    gfx_fontSet(of); // restore previous font

}

//*****************************************************************************
// ICONMENU
//*****************************************************************************

ICONMENU iconMenu_create(){
    ICONMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));

    // init members
    iconMenu_init(m);

    return m;
}

void iconMenu_destroy(ICONMENU m){
    menu_destroy((MENU)m);
}

void iconMenu_init(ICONMENU m){
    menu_init((MENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)iconMenu_handleEvent;
    m->addItem=(MENU_ITEMADDER)iconMenu_addItem;
    m->paint=(WIDGET_PAINTHANDLER)iconMenu_paint;

    // properties
    m->itemWidth=64;
    m->itemHeight=64;
    m->itemCaption=IM_HAS_CAPTION;
}

void iconMenu_setCaptionType(ICONMENU m,IM_CAPTION type)
{
    int h;
    int of;
    m->itemCaption=type;
    if(m->itemCaption==IM_NO_CAPTION)
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(m->font);
        gfx_getStringSize("M",NULL,&h);
        m->btm_line_h=h+3;
        gfx_fontSet(of);
    }
    /* NOTE: we should re-calc max number of item and rebuild everyhting
    it's working now if this fction is called before adding any item */
    
    /* we will also need to update all itemCaption field in each item */
    
}

bool iconMenu_handleEvent(ICONMENU m,int evt){
    int itemsperline=((m->width-2*m->margin)/m->itemWidth);
    bool handled=true;

    // let's see if the ancestor handles the event
    if (menu_handleEvent((MENU)m,evt)) return true;

    // sanity check
    if(m->index<0 || m->index>=m->itemCount) return false;

    // save prev index
    m->previousIndex=m->index;

    switch(evt){
        case BTN_UP:
            m->index-=itemsperline;
            if(m->index<0) m->index=m->itemCount-1;
            break;
        case BTN_DOWN:
            m->index+=itemsperline;
            if(m->index>=m->itemCount) m->index=0;
            break;
        case BTN_LEFT:
            m->index--;
            if(m->index<0) m->index=m->itemCount-1;
            break;
        case BTN_RIGHT:
            m->index++;
            if(m->index>=m->itemCount) m->index=0;
            break;
        default:
            handled=false;
            break;
    }

    // handle moves
    if(m->index!=m->previousIndex){
        if(m->index>=m->topIndex && m->index<m->topIndex+m->visibleCount){ // no changes in items visibility?

            // only changes in focus -> fast update & repaint
            iconMenu_updateItems(m,true);
            m->fastRepaint=true;
            m->paint(m);
            m->fastRepaint=false;            
        }else{

            if(m->index<m->topIndex){ // we moved up?
                m->topIndex=m->index;
                m->topIndex-=m->topIndex%itemsperline; // stay on the beginning of a line
            }else{
                m->topIndex=m->index-m->visibleCount;
                m->topIndex+=itemsperline-m->topIndex%itemsperline; // stay on the beginning of a line
            }

            // full update & repaint
            iconMenu_updateItems(m,false);
            m->fastRepaint=false;
            m->paint(m);            
        }
    }

    return handled;
}

void iconMenu_paint(ICONMENU m)
{
    int of;
    menu_paint((MENU)m);
    
    if(m->itemCaption==IM_NO_CAPTION)
    {
        gfx_drawLine(m->foreColor,
                        m->x+2,m->y+m->height-m->btm_line_h,
                        m->width-2-2,m->y+m->height-m->btm_line_h);
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(m->font);
        gfx_fillRect(m->backColor,m->x+1,m->y+m->height-m->btm_line_h+2,m->width-2,m->btm_line_h-2);
        gfx_putS(m->foreColor,m->backColor,m->x+1,m->y+m->height-m->btm_line_h+2,(m->items[m->index])->caption);
        gfx_fontSet(of); // restore previous font
    }
    
}

void iconMenu_addItem(ICONMENU m, ICONMENU_ITEM item){
    int maxvc=((m->width-2*m->margin)/m->itemWidth)
            *((m->height-2*m->margin-(m->itemCaption==IM_HAS_CAPTION?0:m->btm_line_h))/m->itemHeight);
    // count of max visible items in the menu
    
    menu_addItem((MENU)m,(MENU_ITEM)item);
    
    item->itemCaption=m->itemCaption;

    if(m->visibleCount<maxvc) m->visibleCount++;

    iconMenu_updateItems(m,false);
}

void iconMenu_updateItems(ICONMENU m, bool fast){
    int lastx=(((m->width-2*m->margin)/m->itemWidth)-1)*m->itemWidth; // relative x coordinate of the last item on a line
    int i,x,y;
    MENU_ITEM mi;

    if (fast){

        // only update focus
        m->items[m->previousIndex]->focused=false;
        m->items[m->index]->focused=true;

    }else{

        // full repositioning of visible items
        x=0;
        y=0;
        for(i=m->topIndex;(i<m->topIndex+m->visibleCount) && (i<m->itemCount);++i){
            mi=m->items[i];

            mi->focused=(i==m->index);

            mi->x=m->x+m->margin+x;
            mi->y=m->y+m->margin+y;
            mi->width=m->itemWidth;
            mi->height=m->itemHeight;

            x+=m->itemWidth;
            if(x>lastx){
                x=0;
                y+=m->itemHeight;
            }
        }
    }
}

