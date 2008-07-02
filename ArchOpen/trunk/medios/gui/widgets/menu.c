/*
* kernel/gfx/gui/widget/menu.c
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

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gui/menu.h>


//*****************************************************************************
// MENU_ITEM
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
MENU_ITEM menuItem_create(){
    MENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));
    DEBUGWI_CD("mItem create => %x\n",mi);

    // init members
    menuItem_init(mi);

    return mi;
}

/***********************************************
* Destructor: only call widget destroy
************************************************/
void menuItem_destroy(MENU_ITEM mi){
    DEBUGWI_CD("mItem destroy=>call widget\n");
    widget_destroy((WIDGET)mi);
}

/***********************************************
* Real init: method and properties
************************************************/
void menuItem_init(MENU_ITEM mi){
    widget_init((WIDGET)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)menuItem_destroy;
    mi->setSize=(WIDGET_SIZESETTER)menuItem_setSize;
    mi->setPos=(WIDGET_POSSETTER)menuItem_setPos;
    mi->isVisible=(MENU_ITEM_VISIBILITYTESTER)menuItem_isVisible;
    mi->setCaption=(MENU_ITEM_CAPTIONSETTER)menuItem_setCaption;
    mi->autoSize=(WIDGET_AUTOSIZE)menuItem_autoSize;

    // properties
    mi->clearBackground=false; // will be done by the menu
    mi->caption="MenuItem";
    mi->subMenu=NULL;
    mi->parentMenu=NULL;
}

/***********************************************
* Returns true if menu item is in visible scope
************************************************/
bool menuItem_isVisible(MENU_ITEM mi)
{
    int i;
    MENU m;
    MENU_ITEM ptr;

    /* find parent menu */
    m=mi->parentMenu;

    /* some sanity checks */
    if(!mi)
    {
        printk("[menuItem|isVisible] widget address is NULL => return\n");
        return false;
    }

    /* find parent menu */
    m=mi->parentMenu;
    if(!m) return false; /* not in a menu => not visible*/

    ptr=m->topIndex;

    if(ptr==NULL || m->visibleCount<0)
    {
        printk("[menuItem|isVisible] sanity error! (visi=%d)\n",m->widgetCount);
        return false;
    }

    /* scan from first to last displayed item to find mi*/
    for(i=0;ptr!=NULL && i<m->visibleCount && ptr!=mi;ptr=ptr->nxt) i++;

    return ptr==mi;
}

/***********************************************
* Change position of widget
* this should only be called by menu
* we use widget version in order to manage
* oldPos system needed for paint
************************************************/
void menuItem_setPos(MENU_ITEM mi,int x,int y)
{
    MENU m;

    /* some snity checks */
    if(!mi)
    {
        printk("[menuItem|setPos] item address is NULL => return\n");
        return;
    }

    m=mi->parentMenu;

    /* if not in a menu, don't need to check pos value*/
    if(m && ( x<m->x || x>(m->x+m->width) || y<m->y || y>(m->y+m->height)))
    {
        printk("[menuItem]|setPos] sanity error! (item (%d,%d), menu pos=(%d,%d) and size=(%d,%d)",
            x,y,m->x,m->width,m->y,m->height);
        return;
    }
    /* all ok, let's call widget function */
    widget_setPos((WIDGET)mi,x,y);
}

/***********************************************
* Change size of widget
* setSize should only be called internally,
* we don't want update to be called all the time,
* we'll do it manually each time it is necessary
************************************************/
void menuItem_setSize(MENU_ITEM mi,int width,int height)
{
    MENU m;
    /* some snity checks */
    if(!mi)
    {
        printk("[menuItem|setSize] item address is NULL => return\n");
        return;
    }
    m=mi->parentMenu;
    if(m) /*is the item in a menu?*/
    {
        if(m->width<width)
        {
            printk("[MENU ITEM|setSize] WARN: item width > menu width\n");
            width=m->width;
        }
    }
    mi->updateSize(mi,width,height);
}


#warning study if we could not use a label for the caption

/***********************************************
* Change caption
* call autoSize to have correct size
************************************************/
void menuItem_setCaption(MENU_ITEM mi,char * caption)
{
    MENU m;
    /* some snity checks */
    if(!mi)
    {
        printk("[menuItem|setCaption] item address is NULL => return\n");
        return;
    }
    m=mi->parentMenu;
    mi->caption=caption;
    mi->autoSize(mi);
    DEBUGWI("[menuItem|setCaption] call update of @%x for @%x (%s)\n",m,mi,caption);
    if(m && m->update) m->update(m,mi);
}

/***********************************************
* Recompute size
* size is width and height of caption using
* current font
************************************************/
void menuItem_autoSize(MENU_ITEM mi)
{
    int tw=0,th=0;
    int of;

    /* some snity checks */
    if(!mi)
    {
        printk("[menuItem|autoSize] item address is NULL => return\n");
        return;
    }
    /* recompute size using caption */
    if(mi->caption && *mi->caption!='\0')
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(mi->font);
        gfx_getStringSize(mi->caption,&tw,&th);
        gfx_fontSet(of);
    }

    mi->setSize(mi,tw+2*mi->margin,th+2*mi->margin);
}

//*****************************************************************************
// MENU
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
MENU menu_create(){
    MENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));
    DEBUGWI_CD("menu create => %x\n",m);

    // init members
    menu_init(m);

    return m;
}

/***********************************************
* Destructor: only call widget destroy
************************************************/
void menu_destroy(MENU m){
    // destroy items
    DEBUGWI_CD("menu destroy=>call widgetList destroy\n");
    widgetList_destroy((WIDGETLIST)m);
}

/***********************************************
* Real init: method and properties
************************************************/
void menu_init(MENU m){
    widgetList_init((WIDGETLIST)m);

    // methods
    m->destroy=(WIDGET_DESTROYER)menu_destroy;
    m->onClick=NULL;
    m->handleEvent=(WIDGET_EVENTHANDLER)menu_handleEvent;
    m->paint=(WIDGET_PAINTHANDLER)menu_paint;
    m->addItem=(MENU_ITEMADDER)menu_addItem;
    /*just to be sure noone will call this*/
    m->addWidget=(MENU_ITEMADDER)menu_addItem;
    m->delItem=(MENU_ITEMDEL)menu_delItem;
    /*just to be sure noone will call this*/
    m->delWidget=(MENU_ITEMDEL)menu_delItem;
    m->clearItems=(MENU_ITEMSCLEARER)menu_clearItems;
    m->indexFromCaption=(MENU_INDEXFROMCAPTIONGETTER)menu_indexFromCaption;
    m->itemFromCaption=(MENU_ITEMFROMCAPTIONGETTER)menu_itemFromCaption;
    m->update=(WIDGET_UPDATER)menu_update;
    m->setSize=(WIDGET_SIZESETTER)menu_setSize;
    m->setPos=(WIDGET_POSSETTER)menu_setPos;
    m->updatePos=(MENU_POSUPDATER)menu_updatePos;
    m->setFont=(WIDGET_FONTSETTER)menu_setFont;
    m->autoSize=(WIDGET_AUTOSIZE)menu_autoSize;
    m->setAlign=(WIDGET_ALIGNSETTER)menu_setAlign;

    // properties
    m->index=NULL;
    m->previousIndex=NULL;
    m->topIndex=NULL;
    m->visibleCount=0;
    m->menuList=NULL;
    m->parentMenu=NULL;
    m->hasScrollBar=0;

    m->packed = false;

    m->scrollBar.border_color=COLOR_BLACK;
    m->scrollBar.bg_color=COLOR_TRSP;
    m->scrollBar.fg_color=COLOR_BLUE;
    m->scrollBar.orientation=VERTICAL;
    m->scrollBar.width=8;
}

/***********************************************
* Change position of menu
* item position will be updated by updatePos
************************************************/
void menu_setPos(MENU m,int x,int y)
{
    /* calling widget method */
    widget_setPos((WIDGET)m,x,y);
    /* recompute all pos for visible item*/
    DEBUGWI("[menu setPos] => updatePos\n");
    m->updatePos(m,false);
}

/***********************************************
* Change size of menu
* size is also updated for all item
* calls updatePos to reposition all item
************************************************/
void menu_setSize(MENU m,int width,int height)
{
    MENU_ITEM mi;
    /* calling widget method */
    widget_setSize((WIDGET)m,width,height);
    /* update size info in all items */
    for(mi=(MENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        mi->setSize(mi,width,height);
    /* recompute all pos for visible item*/
    DEBUGWI("[menu setSize] => updatePos\n");
    m->updatePos(m,false);
}

/***********************************************
* Change font of menu
* font is also updated for all item (which will
* call item autoSize)
* calls updatePos to reposition all item
************************************************/
void menu_setFont(MENU m, int font)
{
    MENU_ITEM mi;
    m->font=font;
    /* sync with all item*/
    /* calling item setFont
     * as it will do an autosize of the item*/
    for(mi=(MENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        mi->setFont(mi,font);
    DEBUGWI("[menu setFont] => updatePos\n");
    m->updatePos(m,false);
}

/***********************************************
* this is a method of menu class
* but is empty here as it depends on real menu
* architecture
************************************************/
void menu_updatePos(MENU m,bool fast)
{
    /* Nothing to do */
}

/***********************************************
* A child has changed
* call updatePos is item is visible
************************************************/
void menu_update(MENU parent, MENU_ITEM child)
{
    if(child->isVisible(child))
    {
        DEBUGWI("[menu update] => updatePos\n");
        parent->updatePos(parent,false);
    }
}

/***********************************************
* autoSize is not to be used here
************************************************/
void menu_autoSize(MENU m)
{

}

/***********************************************
* Event handler for menu
* manages action for submenu
************************************************/
bool menu_handleEvent(MENU m,int evt)
{
    bool handled=true;
    MENU_ITEM item;

    // let's see if the ancestor handles the event
    if (widgetList_handleEvent((WIDGETLIST)m,evt)) return true;

    item=(MENU_ITEM)m->index;
    
    if(!item)
    {
        printk("[MENU|Hdl Evt] WARN: index is NULL\n");
        return false; /* error => return evt not handled*/
    }
    
    switch(evt)
    {
        case WIDGET_ACTION_BTN: /* trying to enter the subMenu */
            if(item->subMenu==NULL)
            {
                if (m->onClick) m->onClick(m,item);
                else handled=false;
            }
            else
            {
                if (m->menuList!=NULL) m->menuList->setFocusedWidget(m->menuList,item->subMenu);
                else handled=false;
            }
            break;
        case WIDGET_BACK_BTN: /* trying to go back to parent menu */
            if(m->menuList!=NULL && m->parentMenu!=NULL){
                m->menuList->setFocusedWidget(m->menuList,m->parentMenu);
            }
            else
                handled=false;
            break;
        default:
            handled=false;
            break;
    }
    return handled;
}

/***********************************************
* Paint of menu
* in fastPaint mode: only paint oldFocus and newFocus
* otherwise: go through all visible item and paint them
************************************************/
void menu_paint(MENU m){
    MENU_ITEM mi=NULL;
    int i;

    /* menu not packed yet => first paint
    * calling a full updatePos
    * also change packed property */
    if(!m->packed)
    {
        m->packed=true;
        m->updatePos(m,false);
    }


    if(m->fastRepaint)
    {

        if(!m->index)
        {
            printk("[MENU|Paint] WARN: index (@%x) is NULL\n",m->index);
            m->index=m->topIndex;
            m->previousIndex=m->topIndex;
            /* NOTE: this is maybe a bit buggy, other problems should be tested ? */
        }
        DEBUGWI("[MENU|Paint] fast paint: index=%x, old=%x\n",m->index,m->previousIndex);
        // repaint only previous and current item
        if(m->previousIndex) m->previousIndex->paint(m->previousIndex);
        m->index->paint(m->index);
    }
    else
    {
        widget_paint((WIDGET)m); /* manages clear background */

        if(!m->topIndex)
        {
            printk("[MENU|Paint] WARN 2: topIndex (@%x) is NULL\n");
            m->topIndex=(MENU_ITEM)m->firstWidget;
            m->updatePos(m,false); /*full repositionning*/
        }

        // repaint all visible items
        for(mi=m->topIndex,i=0;mi!=NULL && i<m->visibleCount;mi=mi->nxt,i++)
            mi->paint(mi);

        // repaint scrollbar
        if(m->hasScrollBar)
        {
            m->scrollBar.x=m->x+m->width-m->scrollBar.width;
            m->scrollBar.y=m->y;
            m->scrollBar.height=m->height;
            i=m->indexOf(m,m->topIndex);
            draw_scrollBar(&m->scrollBar,m->widgetCount,i,i+m->visibleCount);
        }

    }
}

/********************************
* Menu addItem
* menu is a widgetList
* ==> we use ancestor addItem
* we update Index
*********************************/
void menu_addItem(MENU m, MENU_ITEM item)
{
    widgetList_addWidget((WIDGETLIST)m,(WIDGET)item);

    DEBUGWI("[MENU|addItem] add @%x in @%x\n",item,m);

    item->updateSize(item,m->width,m->height);

    // we have now at least 1 item -> indicies can now be valid
    if(!m->index && item->canFocus) m->index=item;
    if(!m->topIndex) m->topIndex=item;
    item->parentMenu=m;
}

/********************************
* Menu delItem
* menu is a widgetList
* ==> we use ancestor delItem
* if item is visible, need to update
* index, topIndex
* also recompute position
*********************************/
int menu_delItem(MENU m, MENU_ITEM item)
{
    /* saving visibility, nxt and prev item in the list*/
    bool isVisi=item->isVisible(item);
    MENU_ITEM prev=item->prev;
    MENU_ITEM nxt=item->nxt;
    MENU_ITEM mi;
    bool updateTop=false;

    if(widgetList_delWidget((WIDGETLIST)m,(WIDGET)item))
    {
        /*del was successfull*/
        if(!m->firstWidget)
        { /* no more widget here */
            m->index=NULL;
            m->previousIndex=NULL;
            m->topIndex=NULL;
            m->visibleCount=0;
        }
        else
        {
            if(isVisi)
            { /* item was visible */
                if(m->topIndex==item)
                {
                    if(prev)
                        m->topIndex=prev;
                    else
                        m->topIndex=nxt;
                }

                if(m->index==item)
                {
                    /* try to go backward */
                    for(mi=prev;mi!=NULL && !mi->canFocus;mi=mi->prev)
                        if(mi==m->topIndex) updateTop=true;
                    if(!mi)
                    {
                        /* can't find a focusable widget => go forward */
                        for(mi=nxt;mi!=NULL && !mi->canFocus;mi=mi->nxt) /*nothing*/;
                        if(!mi)
                        {
                            /* still no focusable item */
                            m->index=m->topIndex;
                        }
                        else
                        {
                            m->index=mi;
                        }
                    }
                    else
                    {
                        if(updateTop) m->topIndex=mi;
                        m->index=mi;
                        m->previousIndex=NULL;;
                    }
                }
                m->updatePos(m,false);
            }
        }
        return 1;
    }
    else
        return 0;
}

/********************************
* Menu clearItems
* menu is a widgetList
* ==> we use ancestor clearItems
* also update all index
*********************************/
void menu_clearItems(MENU m){
    // destroy items
    DEBUGWI("[menu|clearItems] start\n");
    widgetList_clearWidgets((WIDGETLIST)m);

    m->index=NULL;
    m->previousIndex=NULL;
    m->topIndex=NULL;
    m->visibleCount=0;
    m->fastRepaint=false;
    DEBUGWI("[menu|clearItems] done\n");
}

/********************************
* Menu setAlign
* Change menu property and call
* item setAlign on all items of
* the menu
*********************************/
void menu_setAlign(MENU m, W_ALIGNMENT alignment)
{
    MENU_ITEM mi;
    /* not calling widget methods as it would launch an autosize */
    m->alignment = alignment;
    /* sync proprety for all items */
    for(mi=(MENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        mi->setAlign(mi,alignment);
}

/********************************
* Menu indexFromCaption
* find item index in menu from
* its caption
*********************************/
int menu_indexFromCaption(MENU m, char * caption){
    int i;
    MENU_ITEM mi;
    for(mi=(MENU_ITEM)m->firstWidget,i=0;mi!=NULL;mi=mi->nxt,i++)
        if(!strcmp(mi->caption,caption)) return i;
    // item not found
    return -1;
}

/********************************
* Menu itemFromCaption
* find item in menu from its
* caption txt
*********************************/
MENU_ITEM menu_itemFromCaption(MENU m,char * caption)
{
    MENU_ITEM mi;
    for(mi=(MENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        if(!strcmp(mi->caption,caption)) return mi;
    // item not found
    return NULL;
}
