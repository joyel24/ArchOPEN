//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui.c		                         Author : Captain H@dock
//  Last change : 24/12/2006                    Created : 08/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "gtn_gui.h"


//------------------------------------------------------------------------------
//  pose function
//
//  Wait for any pressed button (PAUSE)
//
//
//  Input args : (0)
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void pose(void)
{

	int w, h;
	int eventHandler,event;

	gfx_getStringSize("Press any key...", &w,&h);		//get string size on screen
	gfx_putS(COLOR_ORANGE,20,LCD_WIDTH-w,LCD_HEIGTH-h,"Press any key...");
	
	

	eventHandler = evt_getHandler(BTN_CLASS);

	while(1){
        event=evt_getStatusBlocking(eventHandler);
		if (!event) continue; // no new events

		switch(event){
			case BTN_UP:
			case BTN_DOWN:
			case BTN_LEFT:
			case BTN_RIGHT:
			case BTN_ON:
			case BTN_1:
			case BTN_2:
			case BTN_F1:
			case BTN_F2:
			case BTN_F3:
			case BTN_OFF:
				evt_freeHandler(eventHandler);
				return;
		}
	}

	evt_freeHandler(eventHandler);
	return;
}

//------------------------------------------------------------------------------
//  showpal function
//
//  Printf all pal color on screen and print selected color id
//
//
//  Input args : (0)
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void showpal(void)
{
	int i,cur=0,quit=0;
	char *text;
	int eventHandler,event;
	
	int redraw=0;


	text = (char *) malloc(7*sizeof(char));
	//Clear screen
	gfx_clearScreen(COLOR_WHITE);
	
	for(i=0;i<256;i++)
	{
		gfx_fillRect(i,5+(9*(i%16)),5+(9*(i/16)),8,8);
	}
	
	eventHandler = evt_getHandler(BTN_CLASS);

	do{
		redraw=0;
		gfx_drawRect(COLOR_ORANGE,4+(9*(cur%16)),4+(9*(cur/16)),10,10);
		sprintf(text,"%3d",cur);
		gfx_putS(COLOR_BLACK,COLOR_WHITE,190,8,text);
		while(!redraw){
            event=evt_getStatusBlocking(eventHandler);
			if (!event) continue; // no new events

			switch(event){
				case BTN_UP:
					gfx_drawRect(COLOR_WHITE,4+(9*(cur%16)),4+(9*(cur/16)),10,10);
					cur+=256-16;
					redraw=1;
					break;
				case BTN_DOWN:
					gfx_drawRect(COLOR_WHITE,4+(9*(cur%16)),4+(9*(cur/16)),10,10);
					cur+=16;
					redraw=1;
					break;
				case BTN_LEFT:
					gfx_drawRect(COLOR_WHITE,4+(9*(cur%16)),4+(9*(cur/16)),10,10);
					cur+=256-1;
					redraw=1;
					break;
				case BTN_RIGHT:
					gfx_drawRect(COLOR_WHITE,4+(9*(cur%16)),4+(9*(cur/16)),10,10);
					cur+=1;
					redraw=1;
					break;
				
				case BTN_2:
				case BTN_OFF:
					quit=1;
					redraw=1;
					break;
			}
		}
		cur= cur%256;
		
	}while(!quit);

	evt_freeHandler(eventHandler);
	free(text);
	return;
}


//------------------------------------------------------------------------------
//  gui_layout function
//
//  Print a progressbar
//  TODO LATER :	
//	style??
//
//  Input args : (1)
//	Status bar struc
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void gui_layout(Statusbar *stb)
{
	int w,h;
		
	gfx_clearScreen(COLOR_WHITE);

	
	//Title bar
	gfx_fillRect(20,0,0,LCD_WIDTH,BAR_TOP_H);
	gfx_drawLine(21,0,BAR_TOP_H,LCD_WIDTH,BAR_TOP_H);
	gfx_drawLine(19,0,BAR_TOP_H+1,LCD_WIDTH,BAR_TOP_H+1);
	
	gfx_getStringSize(TITLE_GTN, &w,&h);		//get string size on screen
	gfx_putS(COLOR_BLACK,20,(LCD_WIDTH-w)/2,(BAR_TOP_H+2-h)/2,TITLE_GTN);	//title
	
	
	(*stb).w=160;
	(*stb).h=10;
	(*stb).x=2;
	(*stb).y=(LCD_HEIGTH)-(BAR_BOT_H+(*stb).h)/2;
	(*stb).txt="Chargement";
	
	gui_layout_bot_bar();
	gui_status_bar(stb);

	return;
}


void gui_layout_bot_bar(void)
{
	
	//Bottom bar
	gfx_fillRect(20,0,LCD_HEIGTH-BAR_BOT_H,LCD_WIDTH,BAR_BOT_H);
	gfx_drawLine(21,0,LCD_HEIGTH-BAR_BOT_H-1,LCD_WIDTH,LCD_HEIGTH-BAR_BOT_H-1);
	
}

void gui_status_bar(Statusbar *stb)
{
	//Clear progressbar area
	gfx_fillRect(20,(*stb).x,(*stb).y,(*stb).w,(*stb).h);

	if((*stb).txt!=NULL)
	{
		gfx_putS(8,20,(*stb).x,(*stb).y,(*stb).txt);	//Infos
	}
	return;
}



