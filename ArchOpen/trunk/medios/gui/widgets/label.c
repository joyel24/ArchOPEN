/*
* kernel/gfx/gui/widget/label.c
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

#include <gui/label.h>

#include <kernel/malloc.h>
#include <kernel/kernel.h>

#include <driver/lcd.h>

LABEL label_create(){
    LABEL l;

    // allocate widget memory
    l=malloc(sizeof(*l));
    DEBUGWI_CD("label create => %x\n",l);
    // init members
    label_init(l);

    return l;
}

void label_destroy(LABEL l)
{
    DEBUGWI_CD("label destroy => (1) free internalString and lines\n");
    if(l->internal_str) free(l->internal_str);
    if(l->lines) free(l->lines);
    DEBUGWI_CD("label destroy => (2) call widget destroy\n");
    widget_destroy((WIDGET)l);   
}

void label_init(LABEL l){
    widget_init((WIDGET)l);

    // methods
    l->paint=(WIDGET_PAINTHANDLER)label_paint;
    l->destroy=(WIDGET_DESTROYER)label_destroy;
    l->setText=(LABEL_SETTEXT)label_setText;
    l->getMaxSize=(LABEL_GETMAXSIZE)label_getMaxSize;
    l->getNbLines=(LABEL_GETNBLINES)label_getNbLines;
    l->autoSize=(WIDGET_AUTOSIZE)label_autoSize;
    l->setSize=(WIDGET_SIZESETTER)label_setFrameSize;
    l->updateSize=(WIDGET_SIZESETTER)label_setFrameSize;
    l->setPos=(WIDGET_POSSETTER)label_setPos;
    l->setFrameSize=(LABEL_FRAMESIZESETTER)label_setFrameSize;
    l->getFrameSize=(LABEL_FRAMESIZEGETTER)label_getFrameSize;
    l->setParam=(LABEL_PARAMSETTER)label_setParam;
    l->getParam=(LABEL_PARAMGETTER)label_getParam;

    // properties
    l->canFocus=false;
    l->caption=NULL;
    l->alignment=WA_LEFT;
    l->internal_str=NULL;
    l->lines=NULL;
    l->nbLines=0;
    l->interline=1;
    l->frameWidth=LCD_WIDTH-1;
    l->frameHeight=LCD_HEIGHT-1;
    l->cutWord=LA_CUT_WORD;
}

void label_getMaxSize(LABEL l,int * w,int * h)
{
    int i;
    if(w)
    {
        *w=0;
        if(l->lines && l->nbLines)
            for(i=0;i<l->nbLines;i++)
                if(l->lines[i].width> *w) *w=l->lines[i].width;
    }

    if(h)
    {
        *h=0;
        if(l->lines && l->nbLines)
        {
            int of=gfx_fontGet();
            gfx_fontSet(l->font);
            gfx_getStringSize("M",NULL,h);
            gfx_fontSet(of);
            *h=(*h+l->interline)*l->nbLines;
        }
    }
}

int label_getNbLines(LABEL l)
{
    if(l->lines)
        return l->nbLines;
    return 0;
}

void label_setText(LABEL l,char * txt)
{
    char * ptr;
    int of;
    char * endPtr;
    int charWidth,charHeight;
    int newNbLines;

    l->caption=txt;

    l->nbLines=0;

    if(!l->lines)
    {
        l->lines=(struct label_line *)malloc(LABEL_MAX_LINE*sizeof(struct label_line));
        if(!l->lines)
        {
            printk("[label_setText] Can't malloc lines array\n");
            return;
        }
    }

    if(l->internal_str) {free(l->internal_str); l->internal_str=NULL; }

    if(l->frameWidth==0) l->frameWidth=(LCD_WIDTH-l->x-1);
    if(l->frameHeight==0) l->frameHeight=(LCD_HEIGHT-l->y-1);

    ptr=(char *)malloc(strlen(txt) + LABEL_MAX_LINE);
    endPtr=ptr+strlen(txt);

    if(!ptr)
    {
        printk("[label_setText] Can't malloc internal Str\n");
        return ;
    }

    /*skipping \n and \r at the head */
    while((*txt=='\n' || *txt=='\r') && *txt!='\0') txt++;
    if(*txt=='\0')
    {
        free(l->internal_str);
        printk("[label_setText] empty string\n");
        return ;
    }

    l->internal_str=ptr;

    /*saving initial font*/
    of=gfx_fontGet();
    gfx_fontSet(l->font);

    /* getting:
     std width of char ==> needed for label_cutBigLine
     std height for label height computation */
    gfx_getStringSize("M",&charWidth,&charHeight);
    charHeight+=l->interline;
    /* copying intial string to internal */
    strcpy(ptr,txt);
    /* first line start with first char*/
    l->lines[0].str=ptr;

    while(l->nbLines < (LABEL_MAX_LINE-1) && *ptr!='\0' && (l->nbLines*charHeight) < l->frameHeight)
    {
        if(*ptr=='\n' || *ptr=='\r')
        {
            *ptr='\0';
            gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
            newNbLines=label_cutBigLine(l,charWidth,&endPtr);
            ptr=ptr+(newNbLines-l->nbLines)+1;
            l->nbLines=newNbLines;
            while(*ptr=='\n' || *ptr=='\r') ptr++;
            if(*ptr!='\0')
            {
                l->nbLines++;
                l->lines[l->nbLines].str=ptr;
            }
        }
        else
            ptr++;
    }

    if(l->nbLines==(LABEL_MAX_LINE-1))
    {
        /* trying to cut line at first \n or \r */
        char * i;
        for(i=l->lines[l->nbLines].str;*i!='\n' && *i!='\r' && *i!='\0';i++) /*nothing*/;
        if(*i!='\0') *i='\0';
    }

    gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
    /*trying to cut last line */
    l->nbLines=label_cutBigLine(l,charWidth,&endPtr);
    gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
    gfx_fontSet(of);
    l->nbLines++;

    /* updating size */
    l->autoSize(l);
}

void label_setPos(LABEL l, int x, int y)
{
    int nw=-1;
    int nh=-1;
    /* calling widget std setPos */
    widget_setPos((WIDGET)l,x,y);

    /* as we have changed the position we might need to reduce the size of the frame */
    if(l->frameWidth>(LCD_WIDTH-x)) nw=LCD_WIDTH-x-1;
    if(l->frameHeight>(LCD_HEIGHT-y)) nh=LCD_HEIGHT-y-1;
    if(nw!=-1 || nh!=-1) l->setFrameSize(l,nw,nh);
}

void label_setFrameSize(LABEL l,int width, int height)
{
    l->frameWidth=width;
    l->frameHeight=height;
    if(l->caption)
        l->setText(l,l->caption);
}

void label_getFrameSize(LABEL l,int * width, int * height)
{
    if(width) *width=l->frameWidth;
    if(height) *height=l->frameHeight;
}

void label_setParam(LABEL l, int alignment,int cutWord,int interline)
{
    if(alignment==WA_LEFT || alignment==WA_CENTER || alignment==WA_RIGHT)
        l->alignment=alignment;
    if(cutWord==LA_CUT_WORD || cutWord==LA_SMART_CUT)
    {
        l->cutWord=cutWord;
        /* need to recompute string, this will do autoSize too */
        if(l->caption)
            l->setText(l,l->caption);
    }

    if(interline>=0)
    {
        l->interline=interline;
        /* need to recompute string, this will do autoSize too */
        if(l->caption)
            l->setText(l,l->caption);
    }
}

void label_getParam(LABEL l,int * alignment,int * cutWord,int * interline)
{
    if(alignment) *alignment=l->alignment;
    if(cutWord) *cutWord=l->cutWord;
    if(interline) *interline=l->interline;
}

void label_autoSize(LABEL l)
{
    int of;
    int h,nw,nh;

    if(!l->caption)
        nw=nh=0;
    else
    {
        if(!l->internal_str)
        {
            label_setText(l,l->caption);
            return ; /* autoSize will be called again by setTect */
        }
        of=gfx_fontGet(); // save previous font
        gfx_fontSet(l->font);
        nh=l->margin;
        gfx_getStringSize("H",NULL,&h);
        h+=l->interline;
        if(l->alignment==WA_LEFT)
            label_getMaxSize(l,&nw,NULL);
        else
            nw=l->frameWidth;
        nh+=l->nbLines*h;
        gfx_fontSet(of);        
    }
    l->internalWidth=nw;
    l->internalHeight=nh;
    
    /*if(l->internalHeight>l->height)
        l->updateSize(l,l->width,l->internalHeight);*/
    
    //l->updateSize(l,nw,nh);
}

void label_paint(LABEL l){
    int x,y;
    int w,h;
    int of,i;

    x=y=0;

    widget_paint((WIDGET)l);

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(l->font);

    y=l->y+l->margin;

    if(!l->caption)
        return;

    if(!l->internal_str)
        label_setText(l,l->caption);

    gfx_getStringSize("H",NULL,&h);
    h+=l->interline;
    label_getMaxSize(l,&w,NULL);
    for(i=0;i<l->nbLines;i++)
    {
        switch (l->alignment)
        {
            case WA_LEFT:
                x=l->x+l->margin;
                break;
            case WA_RIGHT:
                x=l->x+l->frameWidth-l->lines[i].width-l->margin;
                break;
            case WA_CENTER:
                x=l->x+(l->frameWidth-l->lines[i].width)/2;
                break;
        }
        gfx_putS(l->foreColor,l->backColor,x,y,l->lines[i].str);
        y+=h;
    }

    gfx_fontSet(of); // restore previous font
}

int label_cutBigLine(LABEL l,int charWidth,char ** endPtr)
{
    int nbChar,w,nbChar_sav;
    char * tmp;
    int nbLines=l->nbLines;

    while(nbLines < LABEL_MAX_LINE && l->lines[nbLines].width > l->frameWidth) /*line is too long*/
    {
        /* removing head space char */
        while(*(l->lines[nbLines].str)==' ') l->lines[nbLines].str++;
        gfx_getStringSize(l->lines[nbLines].str,&l->lines[nbLines].width,NULL);
        nbChar=l->frameWidth/charWidth; /* average char position using std char size*/
        gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,NULL);
        if(w!=l->frameWidth)
        {
            if(w<l->frameWidth)
            {
                //gfx_getStringNSize(l->lines[nbLines].str,nbChar+1,&w,NULL);
                do {nbChar++; gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,NULL);} while(w<l->frameWidth);
                if(w!=l->frameWidth) {nbChar--;gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,NULL);}
            }
            else
            {
                for(;w>l->frameWidth;nbChar--) gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,NULL);
            }
        }

        /* we found the place where we should cut*/
        if(l->cutWord==LA_SMART_CUT) /* do we need to do a smart cut*/
        {
            /*yes, let's find the beg of current word*/
            nbChar_sav=nbChar;
            while(nbChar>0 && l->lines[nbLines].str[nbChar-1]!=' ') nbChar--;
            if(nbChar==0) nbChar=nbChar_sav; /* word is taking the whole line => doing LA_CUT_WORD*/
        }

        for(tmp=*endPtr;tmp>=(l->lines[nbLines].str+nbChar);tmp--)
            *(tmp+1)=*tmp;
        (*(endPtr))++;

        *(l->lines[nbLines].str+nbChar)='\0';
        gfx_getStringSize(l->lines[nbLines].str,&l->lines[nbLines].width,NULL);
        nbLines++; /* passing to next line */

        l->lines[nbLines].str=l->lines[nbLines-1].str+nbChar+1;
        gfx_getStringSize(l->lines[nbLines].str,&l->lines[nbLines].width,NULL);
    }
    return nbLines;
}
