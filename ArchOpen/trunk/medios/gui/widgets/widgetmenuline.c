/*
* medios/gui/widget/widgetMenuLine.c
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

#include <lib/string.h>

#include <kernel/kernel.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>

#include <gui/widgetlist.h>
#include <gui/widgetmenuline.h>

WIDGETMENULINE widgetMenuLine_create(void){
    WIDGETMENULINE ml;

    // allocate widget memory
    ml=malloc(sizeof(*ml));
    DEBUGWI_CD("wMenuLine create => %x\n",ml);

    // init members
    widgetMenuLine_init(ml);

    return ml;
}

void widgetMenuLine_destroy(WIDGETMENULINE ml){
    DEBUGWI_CD("wMLine destroy => wList destroy\n");
    widgetList_destroy((WIDGETLIST)ml);
}

void widgetMenuLine_init(WIDGETMENULINE ml)
{
    widgetList_init((WIDGETLIST)ml);

    // methods
    ml->handleEvent=(WIDGET_EVENTHANDLER)widgetMenuLine_handleEvent;
    ml->paint=(WIDGET_PAINTHANDLER)widgetMenuLine_paint;
    ml->autoSize=(WIDGET_AUTOSIZE)widgetMenuLine_autoSize;
    ml->addWidget=(WIDGETLIST_WIDGETADDER)widgetMenuLine_addWidget;
    ml->delWidget=(WIDGETLIST_WIDGETDEL)widgetMenuLine_delWidget;
    ml->clearWidgets=(WIDGETLIST_WIDGETSCLEARER)widgetMenuLine_clearWidgets;
    ml->update=(WIDGET_UPDATER)widgetMenuLine_update;
    ml->setPos=(WIDGET_POSSETTER)widgetMenuLine_setPos;

    //default value for properties
    ml->hasFocus=false;
    ml->hasSelect=false;
    ml->spacing=2; /* one 1 pixel btw elements*/
}

bool widgetMenuLine_handleEvent(WIDGETMENULINE ml,int evt)
{
    bool handled=true;

    /* not calling ancestor, we need to catch UP/DOWN before it*/
    switch(evt)
    {
        case BTN_OFF:
            if(ml->focusedWidget &&  ml->focusedWidget->needSelection)
            {
                if(ml->hasSelect) /*are we trying to exit focus?*/
                {
                    ml->hasSelect=false;
                    ml->focusedWidget->focusColor=WIDGET_DEFAULT_FOCUSCOLOR;
                    ml->focusedWidget->paint(ml->focusedWidget);
                }
                else handled=false;
            }
            else handled=false;
            break;
        case BTN_ON:
            if(ml->focusedWidget &&  ml->focusedWidget->needSelection)
            {
                if(!ml->hasSelect) /*are we trying to get focus ?*/
                {
                    ml->hasSelect=true;
                    ml->focusedWidget->focusColor=WIDGET_DEFAULT_SELECTCOLOR;
                    ml->focusedWidget->paint(ml->focusedWidget);
                }
                else handled=false;
            }
            else handled=widgetList_handleEvent((WIDGETLIST)ml,evt);
            break;
        case BTN_LEFT:
            DEBUGWI("(WMLine|handleEvt] LEFT\n");
            if(ml->focusedWidget &&  ml->focusedWidget->needSelection) /*changing select color*/
            {
                DEBUGWI("(WMLine|handleEvt] a sub item has focus\n");
                if(ml->hasSelect) /*are we trying to exit focus?*/
                {
                    DEBUGWI("(WMLine|handleEvt] it is selected => unselect\n");
                    ml->hasSelect=false;
                    ml->focusedWidget->focusColor=WIDGET_DEFAULT_FOCUSCOLOR;
                }
            }
            DEBUGWI("(WMLine|handleEvt] call chgFocus\n");
            ml->changeFocus(ml,WLD_PREVIOUS); /*always handle left/right: moving inside line*/
            ml->hasSelect=false;
            break;
        case BTN_RIGHT:
            DEBUGWI("(WMLine|handleEvt] LEFT\n");
            if(ml->focusedWidget &&  ml->focusedWidget->needSelection) /*changing select color*/
            {
                DEBUGWI("(WMLine|handleEvt] a sub item has focus\n");
                if(ml->hasSelect) /*are we trying to exit focus?*/
                {
                    DEBUGWI("(WMLine|handleEvt] it is selected => unselect\n");
                    ml->hasSelect=false;
                    ml->focusedWidget->focusColor=WIDGET_DEFAULT_FOCUSCOLOR;
                }
            }
            DEBUGWI("(WMLine|handleEvt] call chgFocus\n");
            ml->changeFocus(ml,WLD_NEXT); /*always handle left/right: moving inside line*/
            ml->hasSelect=false;
            break;
        case BTN_UP:
        case BTN_DOWN:
            if(ml->hasSelect)
            {
                handled=widgetList_handleEvent((WIDGETLIST)ml,evt);
            }
            /*if we have the focus, send UP/DOWN to widget*/
            else handled=false;
            break;
        default:
            handled=false;
    }

    // let's see if the ancestor handles the event
    if (!handled && widget_handleEvent((WIDGET)ml,evt)) return true;

    return handled;
}

void widgetMenuLine_clearWidgets(WIDGETMENULINE ml)
{
    ml->ownWidgets=false; /* to be sure not to destroy items*/
    widgetList_clearWidgets((WIDGETLIST)ml); /* call list function*/
    ml->ownWidgets=true; /* to be sure destroy will destroy items too*/
}

void widgetMenuLine_addWidget(WIDGETMENULINE ml,WIDGET w)
{
    widgetList_addWidget((WIDGETLIST)ml,w);
    w->clearBackground=false; /*menuline will handle the clear BG for all subwidgets */
    ml->autoSize(ml);
}

int widgetMenuLine_delWidget(WIDGETMENULINE ml,WIDGET w)
{
    if(widgetList_delWidget((WIDGETLIST)ml,w))
    {
        ml->autoSize(ml);
        return 1;
    }
    return 0;
}

/* compute size according to data content*/
void widgetMenuLine_autoSize(WIDGETMENULINE ml)
{
    WIDGET wi;
    int w=0,h=0;
    int ww,wh;
    /* scan widget list, call each autosize and spacing*/
    for(wi=ml->firstWidget;wi!=NULL;wi=wi->nxt)
    {
        wi->autoSize(wi);
        wi->getInternalSize(wi,&ww,&wh);
        w+=ww;
        if(h<wh)
            h=wh;
    }

    ml->internalWidth=w+(ml->widgetCount-1)*ml->spacing;
    ml->internalHeight=h;

    if(ml->internalHeight>ml->height)
        ml->height=ml->internalHeight;

    //ml->updateSize(ml,w+(ml->widgetCount-1)*ml->spacing,h);
}

void widgetMenuLine_setPos(WIDGETMENULINE ml,int x,int y)
{
    int h;
    WIDGET wi;
    int ww,wh;
    widget_setPos((WIDGET)ml,x,y);
    h=ml->internalHeight;
    for(wi=ml->firstWidget;wi!=NULL;wi=wi->nxt) /* loop over all widget from widgetlist*/
    {
        /*not using setPos as background will be cleared by menuline*/
        wi->x=x;
        wi->getInternalSize(wi,&ww,&wh);
        x=x+ww+ml->spacing; /*moving x to next widget*/
        wi->y=y+(h-wh)/2; /*centering widget in height*/
    }
}

void widgetMenuLine_update(WIDGETMENULINE ml,WIDGET wi)
{
    int w=0,h=0;
    WIDGET wid;
    int ww,wh;
    /* widget wi has changed */
    /* recompute menuLine size */
    for(wid=ml->firstWidget;wid!=NULL;wid=wid->nxt) /* loop over all widget from widgetlist*/
    {
        wid->getInternalSize(wid,&ww,&wh);
        w+=ww;
        if(h<wh)
            h=wh;
    }
    ml->internalWidth=w+(ml->widgetCount-1)*ml->spacing;
    ml->internalHeight=h;
    //ml->setSize(ml,w+(ml->widgetCount-1)*ml->spacing,h);
    /* recompute position */
    ml->setPos(ml,ml->x,ml->y);
    if(ml->parent) ((WIDGET)ml->parent)->update(ml->parent,ml);
}

void widgetMenuLine_paint(WIDGETMENULINE ml)
{
    int x,y,h;
    int ww,wh;
    WIDGET wi;

    widget_paint((WIDGET)ml);

    /*init position and height*/
    x=ml->x;y=ml->y;h=ml->internalHeight;

    for(wi=ml->firstWidget;wi!=NULL;wi=wi->nxt) /* loop over all widget from widgetlist*/
    {
        wi->x=x;
        wi->getInternalSize(wi,&ww,&wh);
        x=x+ww+ml->spacing; /*moving x to next widget*/
        wi->y=y+(h-wh)/2; /*centering widget in height*/
    }

    /*checking if we get or loose focus*/
    if(ml->focused) /* we have focus*/
    {
        if(!ml->hasFocus) /*we've just gain it*/
        {
            ml->hasFocus=true;
            ml->setFocusedWidget(ml,ml->firstWidget); /*giving focus to first widget*/
        } /*else nothing to do*/
    }
    else /* we don't have focus*/
    {
        if(ml->hasFocus) /*we've loose it*/
        {
            ml->hasFocus=false;
            ml->setFocusedWidget(ml,NULL);
        } /*else nothing to do*/
    }

    for(wi=ml->firstWidget;wi!=NULL;wi=wi->nxt) /* loop over all widget from widgetlist*/
    {
        wi->paint(wi); /* calling widget paint*/
    }
}
