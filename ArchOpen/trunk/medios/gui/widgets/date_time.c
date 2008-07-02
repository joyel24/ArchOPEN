/*
* medios/gui/widget/date_time.c
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

#include <driver/time.h>

#include <gui/widgetlist.h>
#include <gui/widgetmenuline.h>
#include <gui/spinbox.h>
#include <gui/label.h>
#include <gui/date_time.h>

DATE_TIME dateTime_create(void){
    DATE_TIME dt;

    // allocate widget memory
    dt=malloc(sizeof(*dt));
    DEBUGWI_CD("dateTime create => %x\n",dt);
    
    // init members
    dateTime_init(dt);

    return dt;
}

void dateTime_destroy(DATE_TIME dt)
{
    DEBUGWI_CD("dateTime destroy => (1) call menuLine destroy (sub widget destroy too)\n");
    dt->menuline->ownWidgets=true;
    dt->menuline->destroy(dt->menuline);
    DEBUGWI_CD("dateTime destroy => (2) call widget destroy\n");
    widget_destroy((WIDGET)dt);
}

void dateTime_init(DATE_TIME dt)
{
    widget_init((WIDGET)dt);

    // methods
    dt->destroy=(WIDGET_DESTROYER)dateTime_destroy;
    dt->handleEvent=(WIDGET_EVENTHANDLER)dateTime_handleEvent;
    dt->paint=(WIDGET_PAINTHANDLER)dateTime_paint;
    dt->setValue=(DATE_TIME_SETVALUE)dateTime_setValue;
    dt->getValue=(DATE_TIME_GETVALUE)dateTime_getValue;
    dt->autoSize=(WIDGET_AUTOSIZE)dateTime_autoSize;
    dt->setType=(DATE_TIME_TYPESETTER)dateTime_setType;
    dt->setTimeFormat=(DATE_TIME_TIMEFORMATSETTER)dateTime_setTimeFormat;
    dt->setDateFormat=(DATE_TIME_DATEFORMATSETTER)dateTime_setDateFormat;
    dt->update=(WIDGET_UPDATER)dateTime_update;
    dt->setPos=(WIDGET_POSSETTER)dateTime_setPos;
    dt->setFont=(WIDGET_FONTSETTER)dateTime_setFont;
    // default value for properties
    dt->currentH=0;
    dt->isPm=true; /*H=0 => PM*/
    dt->date_format=FORMAT_DDMMYYYY;
    dt->time_format=FORMAT_24;
    
    //dt->x=-1;dt->y=-1;
    
    //creating sub widgets, real init will be done by setParam
    dt->menuline=widgetMenuLine_create();
    dt->menuline->parent=dt;
    //dt->menuline->ownWidgets=true;
    dt->menuline->clearBackground=false;

    dt->spinA=spinbox_create();
    dt->spinA->parent=dt;
    dt->spinA->setValue(dt->spinA,0);
    dt->spinB=spinbox_create();
    dt->spinB->parent=dt;
    dt->spinB->setValue(dt->spinB,0);
    dt->spinC=spinbox_create();    
    dt->spinC->parent=dt;
    dt->spinC->setValue(dt->spinC,0);
    dt->sep_txtA=label_create();
    dt->sep_txtB=label_create();
    
    /*printk("DateTime init: create spin %x, spin %x, spin %x, label %x, label %x\n",dt->spinA,dt->spinB,dt->spinC,
          dt->sep_txtA,dt->sep_txtB);*/
    
    dateTime_setType(dt,TIME_SETTING);
}

void dateTime_setType(DATE_TIME dt,DT_TYPE type)
{
    int oldFormat;
    if(type!=dt->type) /*is it a real change ?*/
    {
        switch(type)
        {
            case DATE_SETTING:
                dt->type=type;
                dt->menuline->clearWidgets(dt->menuline);
                dt->sep_txtA->setText(dt->sep_txtA,"/");
                dt->sep_txtB->setText(dt->sep_txtB,"/");
                
                dt->spinA->setParam(dt->spinA,1,31,1,2); /*day*/
                dt->spinA->onChange=NULL;
                dt->spinB->setParam(dt->spinB,1,12,1,2); /*month*/
                dt->spinC->setParam(dt->spinC,1900,2100,1,4); /*year*/
                
                oldFormat=dt->date_format;
                dt->date_format=-1; /* forcing format change*/
                dateTime_setDateFormat(dt,oldFormat);
                
               // if(needPaint) dt->paint(dt);                
                break;
            case TIME_SETTING:
                dt->type=type;
                dt->menuline->clearWidgets(dt->menuline);
                dt->sep_txtA->setText(dt->sep_txtA,":");
                dt->sep_txtB->setText(dt->sep_txtB,":");
                
                dt->spinA->setParam(dt->spinA,dt->time_format==FORMAT_24?0:1,
                                    dt->time_format==FORMAT_24?23:12,1,2); /*hour*/
                dt->spinA->onChange=(SPINBOX_CHANGEEVENT)dateTime_hour_onChange;
                dt->spinB->setParam(dt->spinB,0,59,1,2); /*min*/
                dt->spinC->setParam(dt->spinC,0,59,1,2); /*sec*/
                dt->menuline->addWidget(dt->menuline,dt->spinA);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtA);
                dt->menuline->addWidget(dt->menuline,dt->spinB);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtB);
                dt->menuline->addWidget(dt->menuline,dt->spinC);
                
                //if(needPaint) dt->paint(dt);   
                break;
            default:
                printk("[setType], wrong type: %d\n",type); 
        }
    }
}


void dateTime_setTimeFormat(DATE_TIME dt,DT_TIME_FORMAT format)
{
    if(format!=dt->time_format) /*is it a real change ?*/
    {       
        switch(format)
        {
            case FORMAT_12:
                dt->time_format=format;
                dt->currentH=dt->spinA->getValue(dt->spinA);
                dt->isPm=0;
                if(dt->currentH==0)
                {                    
                    dt->currentH=12;
                }
                else
                {
                    if(dt->currentH>12)
                    {
                        dt->currentH=dt->currentH-12;
                        dt->isPm=1;
                    }
                }
                dt->spinA->setValue(dt->spinA,dt->currentH);
                //if(needPaint) dt->paint(dt);
                break;
            case FORMAT_24:
                dt->time_format=format;
                if(dt->isPm)
                {
                    dt->currentH=dt->currentH==12?0:dt->currentH+12;
                }
                dt->spinA->setValue(dt->spinA,dt->currentH);
                //if(needPaint) dt->paint(dt);
                break;
            default:
                printk("[setTimeFormat], wrong format: %d\n",format);
        }
    }
}


void dateTime_setDateFormat(DATE_TIME dt,DT_DATE_FORMAT format)
{
    if(format!=dt->date_format) /*is it a real change ?*/
    {       
        switch(format)
        {
            case FORMAT_MMDDYYYY:
                dt->date_format=format;
                dt->menuline->clearWidgets(dt->menuline);
                dt->menuline->addWidget(dt->menuline,dt->spinB);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtA);
                dt->menuline->addWidget(dt->menuline,dt->spinA);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtB);
                dt->menuline->addWidget(dt->menuline,dt->spinC);
                //if(needPaint) dt->paint(dt);
                break;
            case FORMAT_DDMMYYYY:
                dt->date_format=format;
                dt->menuline->clearWidgets(dt->menuline);
                dt->menuline->addWidget(dt->menuline,dt->spinA);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtA);
                dt->menuline->addWidget(dt->menuline,dt->spinB);
                dt->menuline->addWidget(dt->menuline,dt->sep_txtB);
                dt->menuline->addWidget(dt->menuline,dt->spinC);
                //if(needPaint) dt->paint(dt);
                break;
            default:
                printk("[setDateFormat], wrong format: %d\n",format);
        }
    }
}


void dateTime_setFont(DATE_TIME dt,int f)
{
    dt->font=f;
    dt->menuline->setFont(dt->menuline,f);
}

void dateTime_setPos(DATE_TIME dt, int x, int y)
{
    widget_setPos((WIDGET)dt,x,y);
    dt->menuline->setPos(dt->menuline,x,y);
}

/* compute size according to data content*/
void dateTime_autoSize(DATE_TIME dt)
{
    dt->menuline->autoSize(dt->menuline); 
    /*width/height is just the one of the menuline*/
    
    dt->internalWidth=dt->menuline->internalWidth;
    dt->internalHeight=dt->menuline->internalHeight;
    
    if(dt->internalHeight>dt->height)
        dt->height=dt->internalHeight;
    
    //dt->updateSize(dt,dt->menuline->width,dt->menuline->height);
}

void dateTime_update(DATE_TIME dt,WIDGET w)
{
    dt->setSize(dt,dt->menuline->internalWidth,dt->menuline->internalHeight);
    if(dt->parent) ((WIDGET)dt->parent)->update(dt->parent,dt);
}

void dateTime_month_onChange(SPINBOX t)
{
    DATE_TIME dt=(DATE_TIME)t->parent;
    int max=time_getMaxDay_month(t->getValue(t),dt->spinC->getValue(dt->spinC));
    dt->spinA->setParam(dt->spinA,1,max,1,2);
    if(dt->spinA->getValue(dt->spinA)>max) dt->spinA->setValue(dt->spinA,max);
}

void dateTime_hour_onChange(SPINBOX t)
{
    DATE_TIME dt=(DATE_TIME)t->parent;
    if(dt->time_format==FORMAT_12)
    {
        int newH=t->getValue(t);
        if((newH == 12 && dt->currentH == 1)|| (newH == 1 && dt->currentH == 12))
        {
            dt->isPm=dt->isPm?0:1;
            /* redraw AM/PM*/
        }
        dt->currentH=newH;
    }
}

void dateTime_getValue(DATE_TIME dt,struct med_tm * data)
{
    switch(dt->type)
    {
        case DATE_SETTING: /*date value*/
            data->tm_mday=dt->spinA->getValue(dt->spinA);
            data->tm_mon=dt->spinB->getValue(dt->spinB);
            data->tm_year=dt->spinC->getValue(dt->spinC);
            break;
        case TIME_SETTING:
            if(dt->time_format==FORMAT_12)
            {
                if(dt->isPm)
                {
                    if(dt->currentH==12)
                        data->tm_hour=0;
                    else
                        data->tm_hour=dt->currentH+12;
                }
                else                    
                    data->tm_hour=dt->currentH;
            }
            else
                data->tm_hour=dt->spinA->getValue(dt->spinA);
            data->tm_min=dt->spinB->getValue(dt->spinB);
            data->tm_sec=dt->spinC->getValue(dt->spinC);
            break;
        default:
            printk("[getValue], wrong type: %d (critical)\n",dt->type);
    }
}

void dateTime_setValue(DATE_TIME dt,struct med_tm * data)
{
    int max;
    switch(dt->type)
    {
        case DATE_SETTING: /*date value*/
            dt->spinB->setValue(dt->spinB,data->tm_mon);
            dt->spinC->setValue(dt->spinC,data->tm_year);
            max=time_getMaxDay_month(data->tm_mon,data->tm_year);
            dt->spinA->setParam(dt->spinA,1,max,1,2);
            dt->spinA->setValue(dt->spinA,data->tm_mday>max?max:data->tm_mday);
            //dt->paint(dt);
            break;
        case TIME_SETTING:
            if(dt->time_format==FORMAT_12)
            {
                if(data->tm_hour==0)
                {
                    dt->currentH=12;
                    dt->isPm=true;
                }
                else
                {
                    if(data->tm_hour>12)
                    {
                        dt->currentH=data->tm_hour-12;
                        dt->isPm=true;
                    }
                    else
                    {
                        dt->currentH=data->tm_hour;
                        dt->isPm=false;
                    }
                }
                dt->spinA->setValue(dt->spinA,dt->currentH);
            }
            else
            {
                dt->spinA->setValue(dt->spinA,data->tm_hour);
            }
            dt->spinB->setValue(dt->spinB,data->tm_min);
            dt->spinC->setValue(dt->spinC,data->tm_sec);
            //dt->paint(dt);
            break;
        default:
            printk("[setValue], wrong type: %d (Critical)\n",dt->type);
    }
}

bool dateTime_handleEvent(DATE_TIME dt,int evt)
{
    bool res;
    res=dt->menuline->handleEvent(dt->menuline,evt);
    return res;
}

void dateTime_paint(DATE_TIME dt)
{
    widget_paint((WIDGET)dt);    
    dt->menuline->focused=dt->focused;
    /*call menuline paint*/
    dt->menuline->paint(dt->menuline);
}

