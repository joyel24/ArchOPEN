/*
* kernel/gfx/gui/widget/textmenu.c
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#include <gui/textmenu.h>

#include <sys_def/stddef.h>

#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

//*****************************************************************************
// TEXTMENU_ITEM
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
TEXTMENU_ITEM textMenuItem_create(){
    TEXTMENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));
    DEBUGWI_CD("txtMenuItem create => %x\n",mi);

    // init members
    textMenuItem_init(mi);

    return mi;
}

/***********************************************
* Destructor: only call menuItem destroy
************************************************/
void textMenuItem_destroy(TEXTMENU_ITEM mi){
    DEBUGWI_CD("txtMenuItem destroy=>call mItem destroy\n");
    menuItem_destroy((MENU_ITEM)mi);
}

/***********************************************
* Real init: method and properties
************************************************/
void textMenuItem_init(TEXTMENU_ITEM mi){
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)textMenuItem_destroy;
    mi->paint=(WIDGET_PAINTHANDLER)textMenuItem_paint;
    mi->autoSize=(WIDGET_AUTOSIZE)textMenuItem_autoSize;
    mi->setPos=(WIDGET_POSSETTER)textMenuItem_setPos;
    // properties
    mi->caption="TextMenuItem";

    /* call autoSize to have an initial size*/
    mi->autoSize(mi);
}

/***********************************************
* Paint method
* only draw a string at txtX/txtY position
* computed by autoSize
************************************************/
void textMenuItem_paint(TEXTMENU_ITEM mi){
    int color;
    int of;

    /* call widget paint to have background cleared
     * according to clear_background status */
    widget_paint((WIDGET)mi);

    color=(mi->focused)?mi->focusColor:mi->backColor;

    of=gfx_fontGet(); // save previous font
    gfx_fontSet(mi->font);

    gfx_putS(mi->foreColor,color,mi->txtX,mi->txtY,mi->caption);

    gfx_fontSet(of); // restore previous font
}

/***********************************************
* setPos method
* change pos using menu method, also call
* autoSize in order to recmpute txt position
************************************************/
void textMenuItem_setPos(TEXTMENU_ITEM mi,int x,int y)
{
    /* menuItem setPos */
    menuItem_setPos((MENU_ITEM)mi,x,y);
    /* sync caption pos*/
    mi->autoSize(mi);
}

/***********************************************
* autoSize method
* if menu is packed compute de size/position
* of item:
************************************************/
void textMenuItem_autoSize(TEXTMENU_ITEM mi)
{
    TEXTMENU m=(TEXTMENU)mi->parentMenu;

    if(!m)
    {
        DEBUGWI("[TXTMENU_ITEM|autoSize] Not in a menu ==> exit\n");
        return;
    }

    if(!m->packed)
    {
        DEBUGWI("[TXTMENU_ITEM|autoSize] menu not packed ==> exit\n");
        return;
    }

    /* using menuItem method, which centers in height and compute width and height*/
    menuItem_autoSize((MENU_ITEM)mi);
    mi->txtY=mi->y+mi->margin;
    if(m)
    {
        switch (mi->alignment)
        {
            case WA_RIGHT:
                mi->txtX=mi->x+m->width-mi->margin-mi->width;
                break;
            case WA_CENTER:
                mi->txtX=mi->x+(m->width-mi->width)/2;
                break;
            case WA_LEFT:
            default:
                mi->txtX=mi->x+mi->margin;
                break;
        }
    }
    else
        mi->txtX=mi->x+mi->margin;
}

//*****************************************************************************
// TEXTMENU
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
TEXTMENU textMenu_create(){
    TEXTMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));
    DEBUGWI_CD("txtMenu create => %x\n",m);

    // init members
    textMenu_init(m);

    return m;
}

/***********************************************
* Destructor: only call menu destroy
************************************************/
void textMenu_destroy(TEXTMENU m){
    DEBUGWI_CD("txtMenu destroy=>call menu destroy\n");
    menu_destroy((MENU)m);
}

/***********************************************
* Real init: method and properties
************************************************/
void textMenu_init(TEXTMENU m){
    menu_init((MENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)textMenu_handleEvent;
    m->addItem=(MENU_ITEMADDER)textMenu_addItem;
    m->setFocus=(TEXTMENU_FOCUSSETTER)textMenu_setFocus;
    m->updatePos=(MENU_POSUPDATER)textMenu_updatePos;

    // properties

}

/***********************************************
* setFocus:
* if menu is packed, find widget and give it
* focus: change menu topIndex and index property
************************************************/
void textMenu_setFocus(TEXTMENU m, MENU_ITEM item)
{
    int index;
    int bi,top;
    int cnt=m->widgetCount;

/*not packed ? need to updatepos in order to have correct pos*/
    if(!m->packed)
    {
        DEBUGWI("[TXTMENU|setFocus] not packed => doing updatePos and set packed\n");
        m->packed=true;
        m->updatePos(m,false);
    }

    /* try to find widget in list */
    index=m->indexOf(m,item);
    if(index==-1)
    {
        printk("[TXTMENU|setFocus] ERR: item (%s) not found\n",item->caption?item->caption:"NO CAPTION");
        return;
    }

    bi=MIN(cnt,m->visibleCount);
    top=index-(bi/2);

    if(top<0)
        top=0;
    if(top+bi>(cnt))
        top=cnt-bi;
    /* update menu topIndex and index */
    m->topIndex=m->widgetAt(m,top);
    m->index=item;

    /* update pos will correct a wrong visibleCount*/
    DEBUGWI("[TXTMENU|setFocus] ==> updatePos (index is @%x, top=%d, index=%d)\n",m->index,top,index);
    m->updatePos(m,false);
}

/***********************************************
* evt handler:
* handle UP/DOWN event
* changes right event in ACTION_BTN (handled by
* menu as an "enter in submenu")
************************************************/
bool textMenu_handleEvent(TEXTMENU m,int evt)
{
    bool handled=true;

    // let's see if the ancestor handles the event
    if (menu_handleEvent((MENU)m,evt)) return true;

    // sanity check
    if(!m->index)
    {
        printk("[TXT MENU|Hdl Evt] WARN: index is NULL\n");
        return false; /* error => return evt not handled*/
    }

    // save prev index
    m->previousIndex=m->index;
    DEBUGWI("[TXTMENU|handleEvt] index is %x\n",m->index);
    switch(evt){
        case BTN_UP:
            do{ // find previous focusable item
                m->index=m->index->prev;
                if(!m->index) m->index=(MENU_ITEM)m->lastWidget;
            }while(!m->index->canFocus && m->index != m->previousIndex);
            break;
        case BTN_DOWN:
            do{ // find next focusable item
                m->index=m->index->nxt;
                if(!m->index) m->index=(MENU_ITEM)m->firstWidget;
            }while(!m->index->canFocus && m->index != m->previousIndex);
            break;
        case BTN_RIGHT:
            menu_handleEvent((MENU)m,WIDGET_ACTION_BTN);
            break;
        default:
            handled=false;
            break;
    }

    // handle moves
    if(m->index!=m->previousIndex){
        /* find topIndex and index from focused item*/
        int index=m->indexOf(m,m->index);
        int topIndex=m->indexOf(m,m->topIndex);
        DEBUGWI("[TXTMENU|handleEvt] top=%d index=%d, cnt=%d\n",topIndex,index,m->widgetCount);
        // no changes in items visibility?
        if(index>=topIndex && index<topIndex+m->visibleCount){
            // only changes in focus -> fast update & repaint
            DEBUGWI("[TXTMENU|handleEvt] fast update\n");
            m->updatePos(m,true);
            m->fastRepaint=true;
            m->paint(m);
            m->fastRepaint=false;

        }else{
            DEBUGWI("[TXTMENU|handleEvt] full update\n");
            if(index<topIndex){
                // we moved up
                m->topIndex=m->index;
                DEBUGWI("[TXTMENU|handleEvt] moved UP (new top=%d)\n",index);
                if(!m->topIndex) m->topIndex=m->index=(MENU_ITEM)m->firstWidget;
            }else{
                m->topIndex=m->widgetAt(m,index-m->visibleCount+1);
                DEBUGWI("[TXTMENU|handleEvt] moved DOWN (new top=%d)\n",index-m->visibleCount+1);
                if(!m->topIndex) m->topIndex=m->index=(MENU_ITEM)m->firstWidget;
            }

            // full update & repaint
            m->updatePos(m,false);
            m->fastRepaint=false;
            m->paint(m);
        }
    }

    return handled;
}

/***********************************************
* Adding an item to txtMenu
* using menuAddItem
************************************************/
void textMenu_addItem(TEXTMENU m, TEXTMENU_ITEM item){
    menu_addItem((MENU)m,(MENU_ITEM)item);
    DEBUGWI("[TXT MENU|addItem] after menu addItem\n");
    /* synch some properties */
    /* not using setter in order to do an autoSize manually */
    
    item->alignment=m->alignment;
    item->autoSize(item);
    DEBUGWI("[TXT MENU|addItem] after autoSize\n");
    m->updatePos(m,false); /* visible count will be updated here */
    DEBUGWI("[TXT MENU|addItem] after udpatePos\n");
}

/***********************************************
* UpdatePos:
* compute each visible item position
* also recompute visibleCount
* if index is out of visibility, recompute
* topIndex
************************************************/
void textMenu_updatePos(TEXTMENU m, bool fast)
{
    int i;
    MENU_ITEM mi;
    int x,y,maxY;
    int loop;
    bool hasFocusable;
    int index;
    int topIndex;

    hasFocusable=true;

    if(!m->packed)
    {
        DEBUGWI("[TXT MENU|updatePos] Not packed ==> exit\n");
        return;
    }

    /* nothing to do if no items */
    if(m->widgetCount<=0)
    {
        DEBUGWI("[TXT MENU|updatePos] No item => exit\n");
        return;
    }

    /* sanity check */
    if(!m->index)
    {
        printk("[TXT MENU|Updte pos] WARN: index is NULL\n");
        /*try to find a focusable item*/
        for(mi=(MENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        {
            if(mi->canFocus)
            {
                m->topIndex=m->index;
                break;
            }
        }

        if(!mi) /* no focusable item found*/
        {
            m->index=NULL;
            m->topIndex=(MENU_ITEM)m->firstWidget;
            hasFocusable=false;
        }

    }

    if(!m->topIndex)
    {
        printk("[TXT MENU|Updte pos] WARN: topIndex is NULL => setting it to firstWidget\n");
        m->topIndex=(MENU_ITEM)m->firstWidget;   
    }
    
    if(hasFocusable && !m->previousIndex)
    { /* no previous item defined => set it to current index*/
        m->previousIndex=m->index;
    }

    /* starting real update */
    if (fast)
    {
        if(!hasFocusable)
        {
            printk("[TXT MENU|Updte pos] WARN: no item can be focused ==> can't do fast paint\n");
            return;
        }
        // only update focus
        m->previousIndex->focused=false;
        m->index->focused=true;

    }
    else
    {
        DEBUGWI("[TXT MENU|Updte pos] full update\n");
        maxY=m->y+m->height-2*m->margin; /* height includes the margin */
        loop=0;
reCompute: /* loop point used when index is not visible */

        x=m->x+m->margin;
        y=m->y+m->margin;

        DEBUGWI("loop %d, pos %d,%d - maxY=%d - cnt=%d,visi=%d\n",
               loop,x,y,maxY,m->widgetCount,m->visibleCount);

        // full repositioning of visible items
        for(mi=m->topIndex,i=0;mi!=NULL && y<maxY;mi=mi->nxt,i++)
        {
            mi->focused=(mi==m->index); /* update focus */

            /* uset item Position */
            DEBUGWI("SetPos for %s %d,%d\n",mi->caption,x,y);
            mi->setPos(mi,x,y);

            /* update position using item height*/
            y+=mi->height+1;
        }

        if(y>maxY) /* last item is outside the menu height ==> removing one item */
            i--;
        m->visibleCount=i;
        loop++;

        index=m->indexOf(m,m->index);
        topIndex=m->indexOf(m,m->topIndex);
        
        DEBUGWI("New visi=%d (top=%d, index=%d)\n",m->visibleCount,topIndex,index);

        if(hasFocusable && (index < topIndex || index >i))
        {
            DEBUGWI("Bad index => loop\n");
            /* focused item is out of the view
             * ==> update topIndex */
            if(loop == 1)
            {
                /*first time we try to change topIndex
                 * ==> try to put focused item in
                 * the middle */
                 m->topIndex = m->widgetAt(m,MAX(0,index-(m->visibleCount/2)));
            }
            else
            {
                /* we failed in positionning focused item in view last time
                 * ==> put focus item at the top of the screen */
                 m->topIndex = m->index;
            }
            goto reCompute;
        }
        DEBUGWI("[TXT MENU|Updte pos] full update ............ done\n");
    }
}
