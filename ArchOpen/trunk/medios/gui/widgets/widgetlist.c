/*
* kernel/gfx/gui/widget/widgetlist.c
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

#include <gui/widgetlist.h>

//*****************************************************************************
// WIDGETLIST
//*****************************************************************************

/***********************************************
* Creator => only malloc, all is done in init
************************************************/
WIDGETLIST widgetList_create(){
    WIDGETLIST l;

    // allocate widget memory
    l=malloc(sizeof(*l));
    DEBUGWI_CD("widgetList create => %x\n",l);

    // init members
    widgetList_init(l);

    return l;
}

/***********************************************
* Destructor: clean list and free widgetlist
* from mem
************************************************/
void widgetList_destroy(WIDGETLIST l){
    DEBUGWI_CD("[widgetList|destroy] start\n");
    // destroy items
    l->clearWidgets(l);
    DEBUGWI_CD("[widgetList|destroy] clearList done => call widget destroy\n");
    widget_destroy((WIDGET)l);
}

/***********************************************
* Real init: method and properties
************************************************/
void widgetList_init(WIDGETLIST l){
    widget_init((WIDGET)l);

    // methods
    l->destroy=(WIDGET_DESTROYER)widgetList_destroy;
    l->handleEvent=(WIDGET_EVENTHANDLER)widgetList_handleEvent;
    l->paint=(WIDGET_PAINTHANDLER)widgetList_paint;
    l->addWidget=(WIDGETLIST_WIDGETADDER)widgetList_addWidget;
    l->clearWidgets=(WIDGETLIST_WIDGETSCLEARER)widgetList_clearWidgets;
    l->indexOf=(WIDGETLIST_INDEXGETTER)widgetList_indexOf;
    l->setFocusedWidget=(WIDGETLIST_FOCUSEDWIDGETSETTER)widgetList_setFocusedWidget;
    l->changeFocus=(WIDGETLIST_FOCUSCHANGER)widgetList_changeFocus;
    l->setFont=(WIDGET_FONTSETTER)widgetList_setFont;
    l->widgetAt=(WIDGETLIST_WIDGETGETTER)widgetList_widgetAt;

    // properties
    l->firstWidget=l->lastWidget=NULL;
    l->widgetCount=0;
    l->ownWidgets=true;
    l->previousWidget=l->focusedWidget=NULL;
    l->fastRepaint=false;
    l->repaintAllWidgets=true;
}

/***********************************************
* change font of all item in the list
************************************************/
void widgetList_setFont(WIDGETLIST l,int  f)
{
    WIDGET ptr;
    for(ptr=l->firstWidget;ptr!=NULL;ptr=ptr->nxt)
        ptr->setFont(ptr,f);
    /* calling ancestor method */
    widget_setFont((WIDGET)l,f);
}

/***********************************************
* Basic event handler :
* call widget handler for bg clean
* call focused widget handler
************************************************/
bool widgetList_handleEvent(WIDGETLIST l,int evt){
    // let's see if the ancestor handles the event
    if (widget_handleEvent((WIDGET)l,evt)) return true;

    if (l->focusedWidget==NULL)
    {
        return false;
    }

    // pass events to the focused widget
    return l->focusedWidget->handleEvent(l->focusedWidget,evt);
}

/***********************************************
* Paint, 2 modes:
* - fast = only prev and focus items
* - not Fast = paint all widget if repaintAllWidgets
* is true
* - call paint on all item except focus one
************************************************/
void widgetList_paint(WIDGETLIST l){
    WIDGET ptr;

    if(l->fastRepaint){
        // repaint only previous and current widget
        if(l->previousWidget!=NULL) l->previousWidget->paint(l->previousWidget);
        if(l->focusedWidget!=NULL) l->focusedWidget->paint(l->focusedWidget);
    }else{

        if(l->repaintAllWidgets){
            widget_paint((WIDGET)l);

            // repaint all widgets except focused widget
            for(ptr=l->firstWidget;ptr!=NULL;ptr=ptr->nxt)
                if(ptr!=l->focusedWidget)
                    ptr->paint(ptr);
        }

        // paint focused widget after all others (fixes overdraw problems)
        if(l->focusedWidget!=NULL){
            l->focusedWidget->paint(l->focusedWidget);
        }
    }
}

/***********************************************
* Append a widget in the list
* everything is in widget structure
* no need to create a new one
************************************************/
void widgetList_addWidget(WIDGETLIST l,WIDGET w)
{
    if(!l->lastWidget || !l->firstWidget)
    {
        if(l->lastWidget || l->firstWidget)
        {
            printk("[WidgetList|delWidget] sanity error: first and last should be both null => force NULL\n");
        }
        if(l->widgetCount!=0)
        {
            printk("[WidgetList|delWidget] sanity error: list empty but count is not null\n");
        }
        DEBUGWI("[WLIST|Add] empty list => starting list\n");
        l->lastWidget=l->firstWidget=w;
        w->nxt=w->prev=NULL;
        l->widgetCount=1;
    }
    else
    {
        l->widgetCount++;
        l->lastWidget->nxt=w;
        w->nxt=NULL;
        w->prev=l->lastWidget;
        l->lastWidget=w;
    }
    w->parent=(WIDGET)l;
    w->focused=false;
    /* sync some param */
    w->font=l->font;
    w->foreColor=l->foreColor;
    w->backColor=l->backColor;
    w->fillColor=l->fillColor;
    w->focusColor=l->focusColor;
}

/***********************************************
* Remove widget from list
************************************************/
int widgetList_delWidget(WIDGETLIST l,WIDGET w)
{
    if(w==l->firstWidget)
    {
        l->firstWidget=l->firstWidget->nxt;
        l->firstWidget->prev=NULL; /* first is already new first widget*/
        if(w==l->lastWidget) l->lastWidget=NULL;
    }
    else if(w==l->lastWidget)
    {
        l->lastWidget=l->lastWidget->prev;
        l->lastWidget->nxt=NULL; /* last is already new last widget*/
    }
    else
    { /* w->prev and w->nxt should not be null as
        they would then be first or last widget */
        if(!w->prev)
        {
            printk("[WidgetList|delWidget] sanity error: prev is NULL and widget is not first\n");
            return 0;
        }
        if(!w->nxt)
        {
            printk("[WidgetList|delWidget] sanity error: nxt is NULL and widget is not last\n");
            return 0;
        }
        ((WIDGET)w->prev)->nxt=w->nxt;
        ((WIDGET)w->nxt)->prev=w->prev;

    }
    l->widgetCount--;
    w->parent=NULL;

    return 1;
}

/***********************************************
* Remove all widgets from list, also destroy
* them if ownWidgets is true
************************************************/
void widgetList_clearWidgets(WIDGETLIST l)
{
    int i;
    WIDGET ptr,ptr2;

    for(ptr=l->firstWidget,i=0;ptr!=NULL;ptr=ptr2,i++)
    {
        DEBUGWI("wList rm  from list %s, item %d\n",l->ownWidgets?"with destroy":" ",i);
        ptr2=ptr->nxt;
        ptr->parent=ptr->nxt=ptr->nxt=NULL;
        if (l->ownWidgets)
            ptr->destroy(ptr);
    }

    l->previousWidget=l->focusedWidget=NULL;
    l->firstWidget=l->lastWidget=NULL;
    l->widgetCount=0;
    l->fastRepaint=false;
}

/***********************************************
* Find index of an item
************************************************/
#warning is this still usefull ?
int widgetList_indexOf(WIDGETLIST l,WIDGET w){
    WIDGET ptr;
    int i=0;

    for(ptr=l->firstWidget;ptr!=w && ptr!=NULL;ptr=ptr->nxt) i++;

    if(ptr==NULL) return -1; // widget not found
    else return i;
}

/***********************************************
* Find item i of the list
************************************************/
WIDGET widgetList_widgetAt(WIDGETLIST l,int numWidget)
{
    WIDGET ptr;
    int i;
    for(ptr=l->firstWidget,i=0;i!=numWidget && ptr!=NULL;ptr=ptr->nxt) i++;
    return ptr; /* return NULL if not found*/
}

/***********************************************
* Change focused item of the list with given widget
* update the previousWidget property
* repaint the list
*************************************************/
void widgetList_setFocusedWidget(WIDGETLIST l,WIDGET w){
    // update previousWidget
    if (l->focusedWidget!=NULL){
        DEBUGWI("oldWidget %x\n",l->focusedWidget);
        l->previousWidget=l->focusedWidget;
        l->previousWidget->focused=false;
    }

    // update focusedWidget
    DEBUGWI("[widgetList|setFocus] new focus on %x\n",w);
    l->focusedWidget=w;
    if(w)
        l->focusedWidget->focused=true;

    // repaint widgets
    l->fastRepaint=true;
    l->paint(l);
    l->fastRepaint=false;
    DEBUGWI("[widgetList|setFocus] end\n");
}

/***********************************************
* Change focused item of the list according
* to direction (dir)
* update the previousWidget property
* repaint the list
*************************************************/
void widgetList_changeFocus(WIDGETLIST l,WL_DIRECTION dir)
{
    WIDGET ptr;

    if (l->focusedWidget!=NULL)
    {
        DEBUGWI("[widgetlist|chgFocus] a widget is focused\n");
        if(dir==WLD_PREVIOUS)
        {
            for(ptr=l->focusedWidget->prev;ptr!=NULL && !ptr->canFocus;ptr=ptr->prev) /*nothing */;
            DEBUGWI("[widgetlist|chgFocus] doing PREV, found %x\n",ptr);
        }
        else
        {
            for(ptr=l->focusedWidget->nxt;ptr!=NULL && !ptr->canFocus;ptr=ptr->nxt) /*nothing */;
            DEBUGWI("[widgetlist|chgFocus] doing NEXT, found %x\n",ptr);
        }
    }
    else
    {
        DEBUGWI("[widgetlist|chgFocus] no currently focused widget\n");
        for(ptr=l->firstWidget;ptr!=NULL && !ptr->canFocus;ptr=ptr->nxt) /*nothing */;
    }
    
    if(ptr!=NULL)
        l->setFocusedWidget(l,ptr);
}
