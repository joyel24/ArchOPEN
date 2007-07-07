//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_list.c		                 Author : Captain H@dock
//  Last change : 08/02/2007                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_gui_list.h"
#include "printscreen.h"

//------------------------------------------------------------------------------
//  gui_list_bend function
//
//  Print a list and allow user to select one of the list item
//
//  Input args : (1)
//      List_bend *  	=>  List_bend structure
//
//  Output arg : (1)
//      int	  	=>  Seleted element [0;list.element_nb-1]
//
//------------------------------------------------------------------------------
int gui_list_bend(List_bend *list, int sel)
{
	int waitforakey=0;
	int eventHandler,event;
	Scrollbar sbar;
	
	char show_nb;
	int free_h;
	int el_y,i;

	sbar.x=LCD_WIDTH-6;
	sbar.y=BAR_TOP_H+2+6;
	sbar.w=4;
	sbar.h=LCD_HEIGTH-(BAR_TOP_H+2+BAR_BOT_H+1)-10;
	sbar.max=(*list).element_nb;
	
	eventHandler = evt_getHandler(BTN_CLASS);
	
	//Clear list area
	gfx_fillRect(COLOR_BG,0,BAR_TOP_H+2,LCD_WIDTH,LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1);
	
	sel%=(*list).element_nb;

	//Calc how many element can be showed onscreen
	show_nb=1;
	free_h=LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1;
	free_h-=21;								//First element size
	while(free_h>13 && (*list).element_nb-show_nb>0)
	{
		show_nb++;
		free_h-=13;		
	}
	printf("Can print %d list elements.\n", show_nb);
	
	//Calc middle of the list area (depend if there is un more element behind the center element (show_nb even) or not)
	if(show_nb%2==0)			//If show_nb is even --> 13/2 upper
	{
		el_y=(LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1-13-13)/2+BAR_TOP_H+2;
	}
	else						//If show_nb is odd --> Normal middle
	{
		el_y=(LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1-13)/2+BAR_TOP_H+2;
	}
	
	
	
	while(1)				//selection loop
	{
		sbar.cur=sel+1;		//Set scollbar cursor value
		gui_scrollbar(&sbar);	//Print scrollbar

		//Print selected element
		list_bend_draw_element(*((*list).elements_str+sel),el_y,'O');
		
		//Print list elements around current selected if they exist and if they can be printed
		for(i=2;i<=show_nb;i++)
		{
			if(i%2==0)			//If i is even --> print after
			{
				list_bend_draw_element(*((*list).elements_str+((sel+(i/2))%(*list).element_nb)),(el_y+17)+((i/2)-1)*13,'N');
			}
			else				//If i is odd --> Print above
			{
				list_bend_draw_element(*((*list).elements_str+((sel+(*list).element_nb-((i-1)/2))%(*list).element_nb)),(el_y-17-((i-3)/2*13)),'N');
			}
		}
		
		waitforakey=1;
		while(waitforakey)
		{
            event=evt_getStatusBlocking(eventHandler);
			if (!event) continue; // no new events

			switch(event){
				case BTN_LEFT:
				case BTN_UP:
					sel+=(*list).element_nb-1;
					waitforakey=0;
					break;
				case BTN_RIGHT:
				case BTN_DOWN:
					sel++;
					waitforakey=0;
					break;
				case BTN_OFF:
				case BTN_2:
					//Clear scrollbar area
					gfx_fillRect(COLOR_BG,sbar.x,sbar.y,sbar.w,sbar.h);
					//Clear list area
					gfx_fillRect(COLOR_BG,0,BAR_TOP_H+2,LCD_WIDTH,LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1);
					evt_freeHandler(eventHandler);
					return -1;
				
				case BTN_F2:
					gui_printscreen("/Gontactor.bmp");
					break;
				case BTN_ON:
				case BTN_1:

					//Clear scrollbar area
					gfx_fillRect(COLOR_BG,sbar.x,sbar.y,sbar.w,sbar.h);
					//Clear list area
					gfx_fillRect(COLOR_BG,0,BAR_TOP_H+2,LCD_WIDTH,LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1);
					//~ list_bend_draw_element(*((*list).elements_str+sel),81,'S');
					evt_freeHandler(eventHandler);
					//~ mdelay(100);
					//~ //Clear list area
					//~ gfx_fillRect(COLOR_BG,0,BAR_TOP_H+2,LCD_WIDTH,LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1);
					return sel;
			}				
		}
		sel%=(*list).element_nb;
		
	}


	evt_freeHandler(eventHandler);
	return -1;
}



//------------------------------------------------------------------------------
//  list_bend_draw_element function
//
//  Print a button for gui_list_bend function
//
//  Input args : (3)
//      title  	=>  Button title (char *)
//      y	  	=>  Button vertical position (int)
//      style	  	=>  Button style (char) (Normal,Over,Selected)
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void list_bend_draw_element(char *title, int y, char style)
{
	int w,h,charw,charh;
	
	char temp='\0';
	
	int btn_w=LCD_WIDTH-LIST_LPAD-LIST_RPAD;

	//Clear button area
	gfx_fillRect(COLOR_BG,LIST_LPAD,y,btn_w,13);

	gfx_getStringSize(title, &w,&h);		//get string size on screen

	//If text is too long for screen
	if(w>btn_w-CAP_LPAD-6)
	{
		gfx_getStringSize("A", &charw, &charh);
		charw = (btn_w-CAP_LPAD)/charw;		//calc max char nb
		charw--;
		if(style=='O' || style=='S') charw++;	//There is more width available if not normal style
		temp = *(title+charw-1);	//Save char
		*(title+charw-1)= '\0';		//truncate
	}

	switch(style)
	{
		case 'N':
			gfx_putS(COLOR_LINE2,COLOR_BG,LIST_LPAD+CAP_LPAD,y+((16-h)/2),title);	//title
			gfx_drawRect(COLOR_LINE1,LIST_LPAD,y+2,btn_w-CAP_LPAD,11);		//border
			break;
		
		case 'O':
			gfx_putS(200,COLOR_BG,LIST_LPAD+CAP_LPAD-5,y+((16-h)/2),title);	//title
			gfx_drawRect(241,LIST_LPAD-5,y+1,btn_w-CAP_LPAD+5,13);		//border
			break;

		case 'S':
			gfx_putS(70,COLOR_BG,LIST_LPAD+CAP_LPAD-5,y+((16-h)/2),title);	//title
			gfx_drawRect(70,LIST_LPAD-5,y+1,btn_w-CAP_LPAD+5,13);		//border
			break;
	}

	if (temp!='\0')
	{
		*(title+charw-1) = temp;	//reset right char value if needed
	}	
		
	return;
}
