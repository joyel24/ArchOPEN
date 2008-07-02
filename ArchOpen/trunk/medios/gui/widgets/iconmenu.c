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

/**********************************************
* Constructtor of icon menu
***********************************************/
ICONMENU_ITEM iconMenuItem_create(){
    ICONMENU_ITEM mi;

    // allocate widget memory
    mi=malloc(sizeof(*mi));
    DEBUGWI_CD("iconMenuItem create => %x\n",mi);

    // init members
    iconMenuItem_init(mi);

    return mi;
}

/**********************************************
* Destructtor of icon menu
***********************************************/
void iconMenuItem_destroy(ICONMENU_ITEM mi){
    DEBUGWI_CD("iconMenuItem destroy=>call mItem destroy\n");
    menuItem_destroy((MENU_ITEM)mi);
}

/**********************************************
* Init of icon menu Item
* (called by creator)
***********************************************/
void iconMenuItem_init(ICONMENU_ITEM mi){
    /* ancestor init */
    menuItem_init((MENU_ITEM)mi);

    // methods
    mi->destroy=(WIDGET_DESTROYER)iconMenuItem_destroy;
    mi->paint=(WIDGET_PAINTHANDLER)iconMenuItem_paint;
    mi->setSize=(WIDGET_SIZESETTER)iconMenuItem_setSize;
    mi->autoSize=(WIDGET_AUTOSIZE)iconMenuItem_autoSize;
    mi->setPos=(WIDGET_POSSETTER)iconMenuItem_setPos;
    mi->setIPosition=(ICONMENU_ITEM_IPOSETTER)iconMenuItem_setIPosition;

    // properties
    mi->caption="IconMenuItem";
    memset(&mi->icon,0,sizeof(BITMAP));
    mi->iconPosition=IMIP_TOP;
}

/**********************************************
* Paint of icon menu Item
* position/size of icon and caption are computed
* in autoSize
* 2 drawing mode:
* - std: icon on top, caption below (if IM_NO_CAPTION => no caption)
* - list: icon on left, txt on left
***********************************************/
void iconMenuItem_paint(ICONMENU_ITEM mi){
    int color;
    int of;

    /* managing erase of item */
    widget_paint((WIDGET)mi);

    /* color set according to focus */
    color=(mi->focused)?mi->focusColor:mi->backColor;

    /* icon drawing */
    DEBUGWI("[IMenu_item|Paint] for %s\n",mi->caption);
    gfx_drawResizedBitmap(&mi->icon,mi->ix,mi->iy,mi->iw,mi->ih,RESIZE_INTEGER);

    /* forcing caption drawing when IMI_LEFT mode */
    /* drawing caption if it is enable by IM_HAS_CAPTION */
    if(mi->itemCaption==IM_HAS_CAPTION || mi->iconPosition==IMIP_LEFT)
    { /* focus shown with text color */
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(mi->font);
        gfx_putS(mi->foreColor,color,mi->tx,mi->ty,mi->caption);
        gfx_fontSet(of); // restore previous font
    }
    else /* focus shown with a rectangle around icon */
        gfx_drawRect(color,mi->x+1,mi->y+1,mi->width-2,mi->height-2);
}

/**********************************************
* SetSize
* empty function as size should not be set
***********************************************/
void iconMenuItem_setSize(ICONMENU_ITEM mi,int width, int height)
{
    /* this is forbiden for icon menu: size is fixed by menu itself (64x64 for eg)*/
}

/**********************************************
* SetPos
* after doing std setPos, recompute the icon
* and caption position inside item
***********************************************/
void iconMenuItem_setPos(ICONMENU_ITEM mi,int x,int y)
{
    /* menuItem setPos */
    menuItem_setPos((MENU_ITEM)mi,x,y);
    /* sync caption pos*/
    mi->autoSize(mi);
}

/**********************************************
* SetIPosition
* change the poistion type of icon
***********************************************/
void iconMenuItem_setIPosition(ICONMENU_ITEM mi,IM_ICONPOSITION compo_type)
{
    mi->iconPosition=compo_type;
    mi->autoSize(mi);
}


/**********************************************
* autoSize
* recompute the icon and caption position inside item
***********************************************/
void iconMenuItem_autoSize(ICONMENU_ITEM mi)
{
    int tw=0,th=0;
    int of;

    if(!mi->parentMenu)
    {
        DEBUGWI("[IMENU_ITEM|autoSize] Not in a menu ==> exit\n");
        return;
    }

    if(!mi->parentMenu->packed)
    {
        DEBUGWI("[IMENU_ITEM|autoSize] menu not packed ==> exit\n");
        return;
    }

    /*not using menu autoSize as we nee to
    take into account icon size and position*/
    if(mi->caption && *mi->caption!='\0')
    {
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(mi->font);
        gfx_getStringSize(mi->caption,&tw,&th);
        gfx_fontSet(of);
    }

    switch (mi->iconPosition)
    {
        default:
        case IMIP_TOP:
            mi->ix=mi->x;
            mi->iy=mi->y;
            mi->iw=mi->width-(mi->itemCaption==IM_HAS_CAPTION?0:2);
            mi->ih=mi->height-mi->margin-(mi->itemCaption==IM_HAS_CAPTION?th:2);
            mi->tx=mi->x+(mi->width-tw)/2;
            mi->ty=mi->y+mi->height-mi->margin-th;
            break;
        case IMIP_LEFT:
            mi->ix=mi->x+mi->margin;
            mi->iy=mi->y;
            mi->ih=mi->height;
            mi->iw=mi->ih;
            mi->tx=mi->ix+mi->iw+ICONMENU_ITEM_SPACING;
            mi->ty=mi->y+(mi->height-th)/2;
            break;
    }

    DEBUGWI("[IMenu_item|autoSize] for %s, (%d,%d)(%d,%d) => icon (%d,%d)|(%d,%d) txt (%d,%d)\n",
           mi->caption,mi->x,mi->y,mi->width,mi->height,mi->ix,mi->iy,mi->iw,mi->ih,mi->tx,mi->ty);
}

//*****************************************************************************
// ICONMENU
//*****************************************************************************

/**********************************************
* Creator of icon menu
***********************************************/
ICONMENU iconMenu_create(){
    ICONMENU m;

    // allocate widget memory
    m=malloc(sizeof(*m));

    DEBUGWI_CD("iconMenu create=> %x\n",m);

    // init members
    iconMenu_init(m);

    return m;
}

/**********************************************
* Destructtor of icon menu
***********************************************/
void iconMenu_destroy(ICONMENU m){
    DEBUGWI_CD("iconMenu destroy=>call mItem destroy\n");
    menu_destroy((MENU)m);
}

/**********************************************
* Init of IconMenu
* (called by creator)
***********************************************/
void iconMenu_init(ICONMENU m){
    /* ancestor init */
    menu_init((MENU)m);

    // methods
    m->handleEvent=(WIDGET_EVENTHANDLER)iconMenu_handleEvent;
    m->addItem=(MENU_ITEMADDER)iconMenu_addItem;
    m->paint=(WIDGET_PAINTHANDLER)iconMenu_paint;
    m->updatePos=(MENU_POSUPDATER)iconMenu_updatePos;
    m->setItemSize=(ICONMENU_ITEMSIZESETTER)iconMenu_setItemSize;
    m->setStatusFont=(ICONMENU_STATUSFONTSETTER)iconMenu_setStatusFont;
    m->setCaptionType=(ICONMENU_CAPTIONTYPESETTER )iconMenu_setCaptionType;

    // properties
    m->itemWidth=64;
    m->itemHeight=64;
    m->itemCaption=IM_HAS_CAPTION;
    m->statusFont=m->font;
}

/**********************************************
* Change Font used for status bar
* Status bar will contain the caption of focused
* item when menu is in IM_NO_CAPTION mode
***********************************************/
void iconMenu_setStatusFont(ICONMENU m, int font)
{
    /* saving font */
    m->statusFont = font;
    if(m->itemCaption==IM_NO_CAPTION) /* if in IM_NO_CAPTION mode, our change has an imediate effect*/
    {
        m->itemCaption=IM_HAS_CAPTION; /*forcing update by setCaptionType*/
        m->setCaptionType(m,IM_HAS_CAPTION);
    }
}

/***********************************************
* Changing IM_CAPTION property of the menu
* this property is also in each item of the menu
************************************************/
void iconMenu_setCaptionType(ICONMENU m,IM_CAPTION type)
{
    int h;
    int of;
    ICONMENU_ITEM mi;

    if(type!=m->itemCaption) /* are we really changing ?*/
    {
        /* update menu property*/
        m->itemCaption=type;

        if(m->itemCaption==IM_NO_CAPTION)
        {
            /* recompute btm_line_h as we have to display the status line
            ==> we have less space for the menu itself */
            of=gfx_fontGet(); // save previous font
            gfx_fontSet(m->font);
            gfx_getStringSize("M",NULL,&h);
            m->btm_line_h=h+3;
            gfx_fontSet(of);
        }

        /* we need to sync propertiy & autoSize all items */
        for(mi=(ICONMENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
        {
            mi->itemCaption=type;
            mi->autoSize(mi);
        }

        /* update pos of visible items */
        DEBUGWI("[setCaptionType] => updatePos\n");
        m->updatePos(m,false);
    }
}

/***********************************************
* Event handler
* Managing left/right up/down move
************************************************/
bool iconMenu_handleEvent(ICONMENU m,int evt)
{
    int itemsperline=((m->width-2*m->margin)/m->itemWidth);
    bool handled=true;
    int topIndex,index;
    int a;
    
    // let's see if the ancestor handles the event
    if (menu_handleEvent((MENU)m,evt)) return true;

    // sanity check
    if(!m->index) return false;

    // save prev index
    m->previousIndex=m->index;

    /* changing index according to event*/
    switch(evt){
        case BTN_UP:
            index=m->indexOf(m,m->index);
            printk("old index=%d\n",index);
            a=index%itemsperline;
            index-=itemsperline;
            if(index<0) index=(m->widgetCount/itemsperline)*itemsperline+a;
            m->index=m->widgetAt(m,index);  
            printk("new index=%d\n",index);
            /*just to be sure*/
            if(!m->index) { printk("[Imenu|handle evt]UP sanity WARN\n"); m->index=(MENU_ITEM)m->firstWidget;} 
            break;
        case BTN_DOWN:
            index=m->indexOf(m,m->index);
            printk("old index=%d\n",index);
            a=index%itemsperline;
            index+=itemsperline;
            if(index>=m->widgetCount)
            {
                index=a;
                if(index>=m->widgetCount)
                    index=m->widgetCount-1;
            }
            m->index=m->widgetAt(m,index);
            printk("new index=%d\n",index);
            /*just to be sure*/
            if(!m->index) { printk("[Imenu|handle evt]DOWN sanity WARN\n"); m->index=(MENU_ITEM)m->lastWidget;}            
            break;
        case BTN_LEFT:
            m->index=m->index->prev;
            if(!m->index) m->index=(MENU_ITEM)m->lastWidget;
            break;
        case BTN_RIGHT:
            m->index=m->index->nxt;
            if(!m->index) m->index=(MENU_ITEM)m->firstWidget;
            break;
        default:
            handled=false;
            break;
    }

    // handle moves
    if(m->index!=m->previousIndex){
        index=m->indexOf(m,m->index);
        topIndex=m->indexOf(m,m->topIndex);
        if(index>=topIndex && index<topIndex+m->visibleCount){ // no changes in items visibility?

            // only changes in focus -> fast update & repaint
            m->updatePos(m,true);
            m->fastRepaint=true;
            m->paint(m);
            m->fastRepaint=false;
        }else{

            if(index<topIndex){ // we moved up?
                m->topIndex=m->widgetAt(m,index+index%itemsperline);
                if(!m->topIndex) m->topIndex=m->index;
            }else{
                m->topIndex=m->widgetAt(m,(index-m->visibleCount)+itemsperline
                    -(index-m->visibleCount)%itemsperline);
                if(!m->topIndex) m->topIndex=m->index;
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
* Paint Icon Menu
* all job is done by menu.c except the caption in
* status bar for IM_NO_CAPTION mode
************************************************/
void iconMenu_paint(ICONMENU m)
{
    int of;
    DEBUGWI("[Icon menu|paint] start ==> call menu paint\n");
    menu_paint((MENU)m);
    
    /* item's caption are not displayed => we have it in status bar*/
    if(m->itemCaption==IM_NO_CAPTION)
    {
        /* draw status bar separation line */
        gfx_drawLine(m->foreColor,
                        m->x+2,m->y+m->height-m->btm_line_h,
                        m->width-2-2,m->y+m->height-m->btm_line_h);
        /* erase previous caption */
        gfx_fillRect(m->backColor,m->x+1,m->y+m->height-m->btm_line_h+2,m->width-2,m->btm_line_h-2);
        /* draw caption string with right font*/
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(m->statusFont);
        gfx_putS(m->foreColor,m->backColor,m->x+1,m->y+m->height-m->btm_line_h+2,m->index->caption);
        gfx_fontSet(of); // restore previous font
    }
    DEBUGWI("[Icon menu|paint] end\n");
}

/***********************************************
* Adding an item in icon menu
************************************************/
void iconMenu_addItem(ICONMENU m, ICONMENU_ITEM item){
    /* parent addItem */
    menu_addItem((MENU)m,(MENU_ITEM)item);

    /*sync some properties */
    item->itemCaption=m->itemCaption;
    item->width=m->itemWidth;
    item->height=m->itemHeight;
    /* compute caption position*/
    item->autoSize(item);

    //if(m->visibleCount<maxvc) m->visibleCount++;

    /* update visible item position*/
    DEBUGWI("[addItem] => updatePos\n");
    m->updatePos(m,false);
}

/***********************************************
* Changing item width and height
************************************************/
void iconMenu_setItemSize(ICONMENU m, int width, int height)
{
    ICONMENU_ITEM mi;
    /* save new size*/
    m->itemWidth=width;
    m->itemHeight=height;
    DEBUGWI("[setItemSize] (@%x) %d,%d cnt=%d\n",m,width,height,m->widgetCount);
    /*sync item properties*/
    for(mi=(ICONMENU_ITEM)m->firstWidget;mi!=NULL;mi=mi->nxt)
    {
        mi->width=width;
        mi->height=height;
        /*call autosize to sync position*/
        mi->autoSize(mi);
    }
    /* full repositionning */
    DEBUGWI("[setItemSize] => updatePos\n");
    m->updatePos(m,false);
}


/***********************************************
* Update the position of visible items on the screen
* if fast is true, only changes the focused
* else recompute everything and reconsider topIndex
* (ie current focus) if it is out of the screen
************************************************/
void iconMenu_updatePos(ICONMENU m, bool fast)
{
    int lastx; /* relative x coordinate of the last item on a line (ie menu width)*/
    int iniX; /* relative x coordinate of menu after centering (using lastx as menu width*/
    int itemsperline;
    int i,x,y;
    MENU_ITEM mi;
    int index,topIndex;

    if(!m->packed)
    {
        DEBUGWI("[IMenu|updatePos] Not packed ==> exit\n");
        return;
    }

    /* nothing to do if no items */
    if(m->widgetCount<=0)
    {
        DEBUGWI("[IMenu|updatePos] No item => exit\n");
        return;
    }

    DEBUGWI("[IMenu|updatePos]\n");

    if (fast){       // only update focus
        DEBUGWI("fast update of pos\n");
        m->previousIndex->focused=false;
        m->index->focused=true;
    }
    else{    // full repositioning of visible items
        /* init some variable (coordinate, widt, ...) */
        lastx=(((m->width-2*m->margin)/m->itemWidth)-1)*m->itemWidth;

        iniX=(m->width-(lastx+m->itemWidth+2*m->margin))/2;
        lastx+=iniX;
        m->visibleCount=MAX(m->widgetCount,((m->width-2*m->margin)/m->itemWidth)
            *((m->height-2*m->margin-(m->itemCaption==IM_HAS_CAPTION?0:m->btm_line_h))/m->itemHeight));
        itemsperline=((m->width-2*m->margin)/m->itemWidth);
        DEBUGWI("[IMenu|updatePos] full update lastx=%d iniX=%d visiCount=%d nbItem=%d items per line=%d pos=(%d,%d) size=(%d,%d)\n",
               lastx,iniX,m->visibleCount,m->widgetCount,itemsperline,m->x,m->y,m->width,m->height);
        /* init of variables needed in loop*/
        x=iniX;
        y=0;
        /* is index visible ?*/
        index=m->indexOf(m,m->index);
        topIndex=m->indexOf(m,m->topIndex);
        if(index < topIndex || index >= (topIndex+m->visibleCount))
        {
            /* NO => put it in the middle of first line */
            m->topIndex = m->widgetAt(m,MAX(0,index - itemsperline/2));
            if(!m->topIndex) m->topIndex=m->index;
        }

        /* trying to reposition items starting at topIndex*/
        for(mi=m->topIndex,i=0;mi!=NULL && i<m->visibleCount;++i,mi=mi->nxt)
        {
            /* sync focus property */
            mi->focused=(mi==m->index);
            /* item position */
            mi->setPos(mi,m->x+m->margin+x,m->y+m->margin+y);
            DEBUGWI("[IMenu|updatePos] setPos for %s: %d,%d\n",mi->caption,mi->x,mi->y);

            /* moving to next item in the line */
            x+=m->itemWidth;

            /* NOTE: why not use itemsperline or check y with max height */
            /* was it the last item of the line ? */
            if(x>lastx){ /* yes => go to next line */
                x=iniX;
                y+=m->itemHeight;
            }
        }
    }
}
