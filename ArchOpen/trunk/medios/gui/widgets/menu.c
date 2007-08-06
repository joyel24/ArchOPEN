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

MENU_ITEM menuItem_create(){
    MENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));

    // init members
    menuItem_init(mi);

    return mi;
}

void menuItem_destroy(MENU_ITEM mi){
    widget_destroy((WIDGET)mi);
}

void menuItem_init(MENU_ITEM mi){
    widget_init((WIDGET)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)menuItem_destroy;

    // properties
    mi->clearBackground=false; // will be done by the menu
    mi->caption="MenuItem";
    mi->subMenu=NULL;
}

//*****************************************************************************
// MENU
//*****************************************************************************

MENU menu_create(){
    MENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));

    // init members
    menu_init(m);

    return m;
}

void menu_destroy(MENU m){
    // destroy items
    m->clearItems(m);

    // free items list
    kfree(m->items);

    widget_destroy((WIDGET)m);
}

void menu_init(MENU m){
    widget_init((WIDGET)m);

    // methods
    m->destroy=(WIDGET_DESTROYER)menu_destroy;
    m->onClick=NULL;
    m->handleEvent=(WIDGET_EVENTHANDLER)menu_handleEvent;
    m->paint=(WIDGET_PAINTHANDLER)menu_paint;
    m->addItem=(MENU_ITEMADDER)menu_addItem;
    m->clearItems=(MENU_ITEMSCLEARER)menu_clearItems;
    m->indexOf=(MENU_INDEXGETTER)menu_indexOf;
    m->indexFromCaption=(MENU_INDEXFROMCAPTIONGETTER)menu_indexFromCaption;

    // properties
    m->items=malloc(0); // will be realloced as items are added
    m->itemCount=0;
    m->ownItems=true;
    m->index=-1;
    m->previousIndex=-1;
    m->topIndex=-1;
    m->visibleCount=0;
    m->fastRepaint=false;
    m->menuList=NULL;
    m->parentMenu=NULL;
    m->hasScrollBar=0;
    
    m->scrollBar.border_color=COLOR_BLACK;
    m->scrollBar.bg_color=COLOR_TRSP;
    m->scrollBar.fg_color=COLOR_BLUE;
    m->scrollBar.orientation=VERTICAL;
    m->scrollBar.width=8;
}

bool menu_handleEvent(MENU m,int evt){
    bool handled=true;
    MENU_ITEM item;

    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)m,evt)) return true;

    item=m->items[m->index];

    switch(evt){
        case WIDGET_ACTION_BTN:
            if(item->subMenu==NULL){
                if (m->onClick) m->onClick(m,item);
            }else{
                if (m->menuList!=NULL){
                    m->menuList->setFocusedWidget(m->menuList,item->subMenu);
                }
            }
            break;
        case WIDGET_BACK_BTN:
            if(m->menuList!=NULL && m->parentMenu!=NULL){
                m->menuList->setFocusedWidget(m->menuList,m->parentMenu);
            }
            break;
        default:
            handled=false;
            break;
    }

    return handled;
}

void menu_paint(MENU m){
    MENU_ITEM mi;
    int i,ti,bi;

    if(m->fastRepaint){

        if(m->previousIndex<0 || m->previousIndex>=m->itemCount || m->index<0 || m->index>m->itemCount){
            printk("[widget] menu_paint sanity error!\n");
            return;
        }

        // repaint only previous and current item
        mi=m->items[m->previousIndex];
        mi->paint(mi);
        mi=m->items[m->index];
        mi->paint(mi);
    }else{
        
        widget_paint((WIDGET)m);

        ti=m->topIndex;
        bi=MIN(m->itemCount-1,m->topIndex+m->visibleCount-1);

        if(ti<0 || ti>=m->itemCount || bi<0 || bi>m->itemCount){
            printk("[widget] menu_paint sanity error! (ti=%d,cnt=%d,bi=%d)\n",ti,m->itemCount,bi);
            return;
        }

        // repaint all visible items
        for(i=ti;i<=bi;++i){
            mi=m->items[i];
            mi->paint(mi);
        }
        
        // repaint scrollbar
        if(m->hasScrollBar)
        {
            m->scrollBar.x=m->x+m->width-m->scrollBar.width;
            m->scrollBar.y=m->y;
            m->scrollBar.height=m->height;
            draw_scrollBar(&m->scrollBar,m->itemCount,m->topIndex,m->topIndex+m->visibleCount);
        }
        
    }
}

void menu_addItem(MENU m, MENU_ITEM item){
    m->itemCount++;
    m->items=realloc(m->items,m->itemCount*sizeof(item));
    m->items[m->itemCount-1]=item;

    // we have now at least 1 item -> indicies can now be valid
    if (m->index<0 && item->canFocus) m->index=m->itemCount-1; // don't set a non focusable item as the active one
    m->topIndex=MAX(0,m->topIndex);
}

void menu_clearItems(MENU m){
    // destroy items
    if (m->ownItems){
        int i;
        for(i=0;i<m->itemCount;++i) m->items[i]->destroy(m->items[i]);
    }

    m->items=realloc(m->items,0);
    m->itemCount=0;
    m->index=-1;
    m->previousIndex=-1;
    m->topIndex=-1;
    m->visibleCount=0;
    m->fastRepaint=false;
}

int menu_indexOf(MENU m, MENU_ITEM item){
    int i;
    for(i=0;i<m->itemCount;++i){
        if(m->items[i]==item) return i;
    }
    // item not found
    return -1;
}

int menu_indexFromCaption(MENU m, char * caption){
    int i;
    for(i=0;i<m->itemCount;++i){
        if(!strcmp(m->items[i]->caption,caption)) return i;
    }
    // item not found
    return -1;
}
