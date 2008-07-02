/*
* kernel/gfx/gui/widget/msgbox.c
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

#include <sys_def/colordef.h>
#include <sys_def/font.h>

#include <kernel/lang.h>
#include <kernel/malloc.h>
#include <kernel/evt.h>
#include <kernel/kernel.h>

#include <driver/lcd.h>

#include <gfx/graphics.h>

#include <gui/msgBox.h>
#include <gui/label.h>
#include <gui/icons.h>
#include <gui/widget.h>


BITMAP * MsgBox_ExclamationBitmap;
BITMAP * MsgBox_QuestionBitmap;
BITMAP * MsgBox_WarningBitmap;
BITMAP * MsgBox_InformationBitmap;
BITMAP * MsgBox_ErrorBitmap;

//#define BUTTON_WIDTH 60
#define BUTTON_HEIGHT 15
//#define BUTTON_DISTANCE 10


int cntButtons  = 0; /* Buttons in MSgBox */
int g_type = MSGBOX_TYPE_OK;

int fontVal;

/* msg_box event handler */
int msgBox_evtHandler(int evt_hanlder)
{
    int stopBoxLoop = 0;
    int res=MSGBOX_NOTHING;
    char evt=0;
    while(!stopBoxLoop)
    {
        evt = evt_getStatusBlocking(evt_hanlder);
        if(evt==NO_EVENT)
            continue;

        switch(evt)
        {
            case WIDGET_ACTION_BTN:
            case BTN_F1:
                if(cntButtons>0)
                {
                    if(g_type == MSGBOX_TYPE_OK)
                        res = MSGBOX_OK;
                    else if(g_type == MSGBOX_TYPE_OKCANCEL)
                        res =  MSGBOX_OK;
                    else if(g_type == MSGBOX_TYPE_YESNO)
                        res =  MSGBOX_YES;
                    else if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_YES;
                    stopBoxLoop=1;
                }
                break;

            case WIDGET_BACK_BTN:
            case BTN_F2:
                if(cntButtons>1)
                {
                    if(g_type == MSGBOX_TYPE_OKCANCEL)
                        res =  MSGBOX_CANCEL;
                    else if(g_type == MSGBOX_TYPE_YESNO)
                        res =  MSGBOX_NO;
                    else if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_NO;
                    stopBoxLoop=1;
                }
                break;

            case BTN_F3:
                if(cntButtons>2)
                {
                    if(g_type == MSGBOX_TYPE_YESNOCANCEL)
                        res =  MSGBOX_CANCEL;
                    stopBoxLoop=1;
                }
                break;
/*
            case BTN_OFF:
                res = MSGBOX_NOTHING;
                stopBoxLoop=1;
                break;
*/
        }
    }

    return res;
}

void msgBox_init(void)
{
    MsgBox_ExclamationBitmap=&icon_get("MsgBoxExclamationBitmap")->bmap_data;
    MsgBox_QuestionBitmap=&icon_get("MsgBoxQuestionBitmap")->bmap_data;
    MsgBox_WarningBitmap=&icon_get("MsgBoxWarningBitmap")->bmap_data;
    MsgBox_InformationBitmap=&icon_get("MsgBoxInformationBitmap")->bmap_data;
    MsgBox_ErrorBitmap=&icon_get("MsgBoxErrorBitmap")->bmap_data;
}

#define POS_X(X) ((X)+posX)
#define POS_Y(Y) ((Y)+posY)

#define MSGBOX_CAPTION_BG_COLOR   COLOR_LIGHT_BLUE
#define MSGBOX_MAIN_BG_COLOR      COLOR_LIGHT_WHITE

/* draw the msg box */
void msgBox_draw(unsigned char* caption, unsigned char* msg, int type, int icon,int useOwnPlane)
{
    int w_buttonText = 0;
    int h_buttonText = 0;
    
    int posX = 0;
    int posY = 0;
    int value = 0;
    int buttonWidth = 0;
    int allButtonsWidth = 0;
    int buttonTextOffsetX=0;
    int buttonTextOffsetY=0;
    int buttonOffset=0;
    
    char strButtonText[10];
    int x;
    
    int prevFont;
    int caption_w= 0,caption_h= 0;
    int msg_h= 0, msg_w= 0;
    int icon_h= 0,icon_w= 0;

    int width = 0,height=0,content_h=0;
    int i = 0;
    int w;
    BITMAP * curIcon;

    
    int nbLines;    
    LABEL msg_label;
    
    //save font
    prevFont=gfx_fontGet();
    
    /* find icon and compute its size */
    switch(icon)
    {
        case MSGBOX_ICON_EXCLAMATION:
            curIcon=MsgBox_ExclamationBitmap;
            break;
        case MSGBOX_ICON_WARNING:
            curIcon=MsgBox_WarningBitmap;
            break;
        case MSGBOX_ICON_INFORMATION:
            curIcon=MsgBox_InformationBitmap;
            break;
        case MSGBOX_ICON_QUESTION:
            curIcon=MsgBox_QuestionBitmap;
            break;
        case MSGBOX_ICON_ERROR:
            curIcon=MsgBox_ErrorBitmap;
            break;
        default:
            curIcon=NULL;
    }
       
    /*icon_w includes the space btw icon and txt */
    if(curIcon!=NULL)
    {
        icon_w=curIcon->width+2;
        icon_h=curIcon->height;
    }
    else
        icon_w=icon_h=0;
    
    // calculate width
    gfx_fontSet(STD7X13);
    gfx_getStringSize(caption, &caption_w, &caption_h);

    /* create label for msg */
    msg_label=label_create();
    msg_label->font=STD5X8;
    msg_label->margin=0;
    msg_label->setRect(msg_label,2+icon_w,0,LCD_WIDTH-2-icon_w-2,LCD_HEIGHT-2-caption_h-2-5-20);
    msg_label->backColor=MSGBOX_MAIN_BG_COLOR;
    msg_label->setText(msg_label,msg);
    
    nbLines=msg_label->nbLines;
    
    gfx_fontSet(STD5X8);
    gfx_getStringSize("H", NULL, &msg_h);
    
    /* find max width */
    label_getMaxSize(msg_label,&msg_w,&content_h);
       
    if(msg_w+icon_w > caption_w)
        width = msg_w+4+icon_w;
    else
        width = caption_w+4;
    
    /* compute width button box and overall size of the button bar */
    gfx_fontSet(STD5X8);
    switch(type)
    {
        case MSGBOX_TYPE_OK:
            cntButtons = 1; //only one button            
            gfx_getStringSize("Ok(F1)", &buttonWidth, NULL);
            buttonWidth+=4;   
            allButtonsWidth=buttonWidth;
            break;
        case MSGBOX_TYPE_YESNOCANCEL:
            cntButtons = 3; //three buttons
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_YES), &buttonWidth, NULL);            
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_NO), &w, NULL); /* second button width*/ 
            if(buttonWidth<w) buttonWidth=w;            
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_CANCEL_F3), &w, NULL); /* third button width*/ 
            if(buttonWidth<w) buttonWidth=w;            /* buttonWidth is the biggest width */
            buttonWidth+=4;   
            allButtonsWidth=3*buttonWidth+2;         /* allButtonsWidth == width of all button + space btw */            
            break;
        case MSGBOX_TYPE_OKCANCEL:   
            cntButtons = 2; //two buttons
            gfx_getStringSize("Ok(F1)", &buttonWidth, NULL);
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_CANCEL_F2), &w, NULL); /* second button width*/
            if(buttonWidth<w) buttonWidth=w; /* buttonWidth is the biggest width */
            buttonWidth+=4;
            allButtonsWidth=2*buttonWidth+1;         /* allButtonsWidth == width of all button + space btw */
            break;
        case MSGBOX_TYPE_YESNO:
            cntButtons = 2; //two buttons  
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_YES), &buttonWidth, NULL); 
            gfx_getStringSize(getLangStr(STRLNG_MSGBOX_NO), &w, NULL); /* second button width*/
            if(buttonWidth<w) buttonWidth=w; /* buttonWidth is the biggest width */
            buttonWidth+=4;
            allButtonsWidth=2*buttonWidth+1;         /* allButtonsWidth == width of all button + space btw */
            break;
        case MSGBOX_TYPE_INFO:
            cntButtons = 0;
            buttonWidth=0;
            allButtonsWidth=0;
            break;
    }
   
    if(width < (allButtonsWidth+4)) width = allButtonsWidth+4;
    
    if(width > LCD_WIDTH)
    {
        printk("width too big !!!! (w=%d/%d)\n",width,LCD_WIDTH);
        width = LCD_WIDTH;
    }
        
    content_h += 10;
    if(curIcon!=NULL && content_h<(curIcon->height+5))
        content_h=curIcon->height+5;
    
    height = 2 + caption_h + 2 + content_h + 5 + 20;    
    if(height<MSGBOX_HEIGHT) height=MSGBOX_HEIGHT;
    
    // the width must be a multiple of 32 !
    if(useOwnPlane)
    {
        value = width/8;
        width = value*8;
        gfx_planeSetSize(BMAP2,width,height,8,GFX_SMODE_STD);
    }
    // center box
    if(useOwnPlane)
        posX = LCD_WIDTH-width;
    else
        posX = (LCD_WIDTH-width)/2;
    
    posY = (LCD_HEIGHT/2)-(height/2);
    
    if(useOwnPlane)
        gfx_planeSetPos(BMAP2,SCREEN_ORIGIN_X+posX,SCREEN_ORIGIN_Y+posY);
    
    // show box
    if(useOwnPlane)
        gfx_setPlane(BMAP2);
    
    // main box
    gfx_drawRect(COLOR_BLACK, POS_X(0), POS_Y(0), width, height);
     
    // write caption
    gfx_fontSet(STD7X13);
    x = (width/2)-(caption_w/2);
    
    gfx_fillRect(MSGBOX_CAPTION_BG_COLOR, POS_X(1), POS_Y(1), width-2, caption_h+2+1); /* fill caption box */
    gfx_putS(COLOR_BLACK, MSGBOX_CAPTION_BG_COLOR, POS_X(x), POS_Y(2), caption);
    
    /* sep line */
    gfx_drawLine(COLOR_BLACK,POS_X(0), POS_Y(caption_h+2+2), POS_X(width-1), POS_Y(caption_h+2+2));
    /* fill main box */
    gfx_fillRect(MSGBOX_MAIN_BG_COLOR, POS_X(1), POS_Y(caption_h+2+2+1), width-2, height-(caption_h+2+2+2));
    
    // draw icon
    if(curIcon!=NULL)
        gfx_drawBitmap (curIcon, POS_X(2),POS_Y(caption_h+2+2+5));
    
    /* add main message */
    gfx_fontSet(STD5X8);    
    msg_label->setRect(msg_label,POS_X(2+icon_w),POS_Y(caption_h+2+2+10),msg_w,content_h);
    msg_label->paint(msg_label);
    
    // print buttons
    gfx_fontSet(STD5X8);
    switch(type)
    {
        case MSGBOX_TYPE_OK:
            buttonOffset = (width-allButtonsWidth)/2; /* center buttons bar */
            
            /*draw box*/
            gfx_drawRect(COLOR_BLACK, POS_X(buttonOffset), POS_Y(height-20),
                buttonWidth, BUTTON_HEIGHT);
            gfx_fillRect(COLOR_WHITE, POS_X(buttonOffset+1), POS_Y((height-20)+1),
                buttonWidth-2, BUTTON_HEIGHT-2);

            gfx_getStringSize("Ok(F1)", &w_buttonText, &h_buttonText);
            buttonTextOffsetX = (buttonWidth-w_buttonText)/2;
            buttonTextOffsetY = (BUTTON_HEIGHT-h_buttonText)/2;
            /*centering text in box*/
            gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(1+buttonOffset+buttonTextOffsetX),
                POS_Y(height-20+buttonTextOffsetY) , "Ok(F1)");
            break;
        case MSGBOX_TYPE_YESNOCANCEL:
            buttonOffset = (width-allButtonsWidth)/2; /* center buttons bar */
            for(i = 0; i < cntButtons; i++)
            {
                /*draw box*/
                gfx_drawRect(COLOR_BLACK, POS_X(buttonOffset+i*(buttonWidth+1)),
                    POS_Y(height-20), buttonWidth, BUTTON_HEIGHT);
                gfx_fillRect(COLOR_WHITE, POS_X(buttonOffset+i*(buttonWidth+1)+1), POS_Y((height-20)+1),
                    buttonWidth-2, BUTTON_HEIGHT-2);
                /*define button string*/
                if(i == 0)
                    strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_YES));
                else if (i == 1)
                    strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_NO));
                else
                    strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_CANCEL_F3));
                /*centering text in box*/
                gfx_getStringSize(strButtonText, &w_buttonText, &h_buttonText);
                buttonTextOffsetX = (buttonWidth-w_buttonText)/2;
                buttonTextOffsetY = (BUTTON_HEIGHT-h_buttonText)/2;
                gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(1+i*(buttonWidth+1)+buttonOffset+buttonTextOffsetX),
                    POS_Y(height-20+buttonTextOffsetY) , strButtonText);
            }
            break;
        case MSGBOX_TYPE_OKCANCEL:
        case MSGBOX_TYPE_YESNO:
            buttonOffset = (width-allButtonsWidth)/2; /* center buttons bar */
            for(i = 0; i < cntButtons; i++)
            {
                /*draw box*/
                gfx_drawRect(COLOR_BLACK, POS_X(buttonOffset+i*(buttonWidth+1)), POS_Y(height-20),
                    buttonWidth, BUTTON_HEIGHT);
                gfx_fillRect(COLOR_WHITE, POS_X(buttonOffset+i*(buttonWidth+1)+1), POS_Y((height-20)+1),
                    buttonWidth-2, BUTTON_HEIGHT-2);
                /*define button string*/
                if(i == 0)
                {
                    if(type == MSGBOX_TYPE_YESNO)
                        strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_YES));
                    else
                        strcpy(strButtonText,"Ok(F1)");
                }
                else
                {
                    if(type == MSGBOX_TYPE_YESNO)
                        strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_NO));
                    else
                        strcpy(strButtonText,getLangStr(STRLNG_MSGBOX_CANCEL_F2));
                }
                /*centering text in box*/
                gfx_getStringSize(strButtonText, &w_buttonText, &h_buttonText);
                buttonTextOffsetX = (buttonWidth-w_buttonText)/2;
                buttonTextOffsetY = (BUTTON_HEIGHT-h_buttonText)/2;
                gfx_putS(COLOR_BLACK, COLOR_WHITE, POS_X(1+i*(buttonWidth+1)+buttonOffset+buttonTextOffsetX),
                    POS_Y(height-20+buttonTextOffsetY) , strButtonText);
            }
            break;
        case MSGBOX_TYPE_INFO:
            /* no button */
            break;
    }
    //restore font
    gfx_fontSet(prevFont);

    if(useOwnPlane)
    {
        gfx_planeShow(BMAP2);
        gfx_setPlane(BMAP1);
    }
}



/* restore the previous state */
void msgBox_erase(void)
{
    gfx_planeHide(BMAP2);
    gfx_setPlane(BMAP1);
    gfx_fontSet(fontVal);
}

/* main function */
int msgBox_show(unsigned char* caption, unsigned char* msg, int type, int icon, int evt_hanlder)
{
    int res=MSGBOX_NOTHING;
    
    g_type = type;
    fontVal=gfx_fontGet();

    //gli: use default plane for now
    msgBox_draw(caption,msg,type,icon,0);

    //info message boxes are display only
    if(type!=MSGBOX_TYPE_INFO){
        res=msgBox_evtHandler(evt_hanlder);
    }
    
    //msgBox_erase();

    return res;
}

void msgBox_info(unsigned char* msg)
{
    fontVal=gfx_fontGet();
    msgBox_draw("Info",msg,MSGBOX_TYPE_INFO,MSGBOX_ICON_INFORMATION,0);
}
