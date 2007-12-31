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

    // init members
    label_init(l);

    return l;
}

void label_init(LABEL l){
    widget_init((WIDGET)l);

    // methods
    l->paint=(WIDGET_PAINTHANDLER)label_paint;

    // properties
    l->canFocus=false;
    l->caption=NULL;
    l->alignment=LA_LEFT;
    l->internal_str=NULL;
    l->lines=NULL;
    l->nbLines=0;
    l->interline=1;    
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

void label_setTxt(LABEL l,char * txt)
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
            printk("[label_setTxt] Can't malloc lines array\n");
            return;
        }
    }
    
    if(l->internal_str) {free(l->internal_str); l->internal_str=NULL; }
    
    if(l->width==0) l->width=LCD_WIDTH;
    if(l->height==0) l->height=LCD_HEIGHT;
        
    ptr=(char *)malloc(strlen(txt) + LABEL_MAX_LINE);
    endPtr=ptr+strlen(txt);
    
    /*saving initial font*/
    of=gfx_fontGet();
    
    if(!ptr)
    {
        printk("[label_setTxt] Can't malloc internal Str\n");
        return ;
    }
    
    /*skipping \n and \r at the head */
    while((*txt=='\n' || *txt=='\r') && *txt!='\0') txt++;    
    if(*txt=='\0')
    {
        free(l->internal_str);
        printk("[label_setTxt] empty string\n");
        return ;
    }
    
    l->internal_str=ptr;
    
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
    
    while(l->nbLines < LABEL_MAX_LINE && *ptr!='\0' && (l->nbLines*charHeight) < l->height)
    {
        if(*ptr=='\n' || *ptr=='\r')
        {
            
            *ptr='\0';
            gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
            newNbLines=label_cutBigLine(l,charWidth,endPtr);            
            ptr=ptr+(l->nbLines-newNbLines)+1;
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
    
    if(l->nbLines==LABEL_MAX_LINE)
    {
        /* removing any tailling \n or \r */
        int len=strlen(ptr);
        int i=1;
        if(*(ptr+len-1) == '\n' || *(ptr+len-1) == '\r')
        {
            while(*(ptr+len-i)== '\n' || *(ptr+len-i) == '\r') i++;
            *(ptr+len-i+1)='\0';
        }
    }
    
    gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
    
    /*trying to cut last line */
    l->nbLines=label_cutBigLine(l,charWidth,endPtr);
    
    gfx_getStringSize(l->lines[l->nbLines].str,&l->lines[l->nbLines].width,NULL);
    
    gfx_fontSet(of);
    l->nbLines++;
}

void label_paint(LABEL l){
    int x,y;
    int w,h;
    int of;
    char * s;

    x=y=0;

    widget_paint((WIDGET)l);

    of=gfx_fontGet(); // save previous font

    gfx_fontSet(l->font);

    y=l->y+l->margin;
    
    if(!l->caption)
        return;
    
    if(!l->internal_str)
    {    
        char * c=l->caption;
        char save='\0';

        while(c!=NULL)
        {
            if(c!=l->caption){
                *c=save;
                ++c;
            }
    
            s=c;
    
            c=strpbrk(c,"\n");
    
            if(c!=NULL){
                save=*c;
                *c='\0';
            }
    
            gfx_getStringSize(s,&w,&h);
               
            switch (l->alignment){
                case LA_LEFT:
                    x=l->x+l->margin;
                    break;
                case LA_RIGHT:
                    x=l->x+l->width-l->margin-w;
                    break;
                case LA_CENTER:
                    x=l->x+(l->width-w)/2;
                    break;
            }
            
            gfx_putS(l->foreColor,l->backColor,x,y,s);
    
            y+=h;
        }
    }
    else
    {
        int i;
        gfx_getStringSize("H",NULL,&h);
        h+=l->interline;
        label_getMaxSize(l,&w,NULL);
        for(i=0;i<l->nbLines;i++)
        {
            switch (l->alignment)
            {
                case LA_LEFT:
                    x=l->x+l->margin;
                    break;
                case LA_RIGHT:
                    x=l->x+l->lines[i].width-l->margin-w;
                    break;
                case LA_CENTER:
                    x=l->x+(l->lines[i].width-w)/2;
                    break;
            }
            gfx_putS(l->foreColor,l->backColor,x,y,l->lines[i].str);    
            y+=h;
        }
    }

    gfx_fontSet(of); // restore previous font
}

int label_cutBigLine(LABEL l,int charWidth,char * endPtr)
{
    int nbChar,w;
    char * tmp;    
    int nbLines=l->nbLines;
        
    while(nbLines < LABEL_MAX_LINE && l->lines[nbLines].width > l->width) /*line is too long*/
    {
        nbChar=(l->lines[nbLines].width)/charWidth; /* average char position using std char size*/
        gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,0);
        if(w!=l->width)
        {
            if(w<l->width)
            {
                gfx_getStringNSize(l->lines[nbLines].str,nbChar+1,&w,0);
                do {nbChar++; gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,0);} while(w<l->width);
                if(w!=l->width) {nbChar--;gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,0);}
            }
            else
            {
                for(;w>l->width;nbChar--) gfx_getStringNSize(l->lines[nbLines].str,nbChar,&w,0);
            }
        }
        
        for(tmp=endPtr;tmp>=(l->lines[nbLines].str+nbChar);tmp--)
            *(tmp+1)=*tmp;
        endPtr++;
        
        *(l->lines[nbLines].str+nbChar)='\0';
        gfx_getStringSize(l->lines[nbLines].str,&l->lines[nbLines].width,NULL);
        
        nbLines++; /* passing to next line */        
        
        l->lines[nbLines].str=l->lines[nbLines-1].str+nbChar+1;
        gfx_getStringSize(l->lines[nbLines].str,&l->lines[nbLines].width,NULL);
    } 
    return nbLines;  
}
