//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_printgtn.c		        Author : Captain H@dock
//  Last change : 07/01/2007                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "str_fct.h"

#include "gtn_gui_printgtn.h"
#include "gtn_gui.h"
#include "bmp_load.h"


//------------------------------------------------------------------------------
//  gui_print_gtn function
//
//  Print gtn struct field on screen
//  TODO LATER :
//	Print age (when time function while be usable)
//
//  Input args : (1)
//      *gtn        		=>	Gontactor struture to print
//      *gr_name_str 	=>  	Groups struture 
//      *stl        		=>	Scrolltext_list to print large strings on screen
//      *prev_str		=>	Text to print in prev button (char *)
//      *next_str 		=>	Text to print in next button (char *)
//
//  Output arg : (1)
//      int         =>  Error code. (0=OK)
//------------------------------------------------------------------------------
int gui_print_gtn(Gontact *gtn, Groups *gr_name_str, Scrolltext_list *stl, char *prev_str, char *next_str)
{
	int x,y,max_w;
	Scrolltext stxt;
	
	char *title=NULL;
	
	//Clear gtn info area
	gfx_fillRect(COLOR_BG,0,BAR_TOP_H+2,LCD_WIDTH,LCD_HEIGTH-BAR_TOP_H-BAR_BOT_H-2-1);
	
//Draw layout borders
	
//Major rectangle + rounded corners
	//Top line
	gfx_drawLine(COLOR_LINE1,3+6,BAR_TOP_H+2+3,LCD_WIDTH-4-6,BAR_TOP_H+2+3);
	//Right line
	gfx_drawLine(COLOR_LINE1,LCD_WIDTH-4,BAR_TOP_H+2+3+6,LCD_WIDTH-4,BAR_TOP_H+2+3+DLG_HEIGTH-6-1);
	//Bottom line
	gfx_drawLine(COLOR_LINE1,3+6,BAR_TOP_H+2+3+DLG_HEIGTH-1,LCD_WIDTH-4-6,BAR_TOP_H+2+3+DLG_HEIGTH-1);
	//Left line
	gfx_drawLine(COLOR_LINE1,3,BAR_TOP_H+2+3+6,3,BAR_TOP_H+2+3+DLG_HEIGTH-6-1);

	//rounded corners

	//Top left corner
	x=3;
	y=BAR_TOP_H+2+3;
	
	y+=5;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);


	//Top right corner
	x=LCD_WIDTH-4;
	y=BAR_TOP_H+2+3;
	
	y+=5;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);


	//Bottom right corner
	x=LCD_WIDTH-4;
	y=BAR_TOP_H+2+3+DLG_HEIGTH-1;
	
	y-=5;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);


	//Bottom left corner
	x=3;
	y=BAR_TOP_H+2+3+DLG_HEIGTH-1;
	
	y-=5;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);




//Name line
	gfx_drawLine(COLOR_LINE1,3,BAR_TOP_H+2+3+12,LCD_WIDTH-4,BAR_TOP_H+2+3+12);
	
//date info
	gfx_drawLine(COLOR_LINE1,3+20+6,BAR_TOP_H+2+3+DLG_HEIGTH-1+11,LCD_WIDTH-4-20-6,BAR_TOP_H+2+3+DLG_HEIGTH-1+11);
	gfx_drawLine(COLOR_LINE1,3+20,BAR_TOP_H+2+3+DLG_HEIGTH-1,3+20,BAR_TOP_H+2+3+DLG_HEIGTH-1+11-6);
	gfx_drawLine(COLOR_LINE1,LCD_WIDTH-4-20,BAR_TOP_H+2+3+DLG_HEIGTH-1,LCD_WIDTH-4-20,BAR_TOP_H+2+3+DLG_HEIGTH-1+11-6);
	

	//left corner
	x=3+20;
	y=BAR_TOP_H+2+3+DLG_HEIGTH-1+11;
	
	y-=5;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);


	//right corner
	x=LCD_WIDTH-4-20;
	y=BAR_TOP_H+2+3+DLG_HEIGTH-1+11;
	
	y-=5;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y++;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);


	
	//initialize scrolltext_list
	set_number_Scrolltext_list(stl,6);

	//Calc max width
	
	max_w = LCD_WIDTH -10 -10;	//Width allowed for text
	gfx_getStringSize("x", &x,&y);		//get char size on screen
	max_w = max_w/x;

	
	//Initialize Scrolltext structure
	stxt.w= max_w;
	stxt.start_pos = 0;
	stxt.pos_dir = true;
	stxt.color = COLOR_LINE2;
	stxt.color_bg = COLOR_BG;



//Print infos

	//format title string (surname + first name)

	title =	straddinnew((*gtn).str_nom, (*gtn).str_prenom);

	//Test length and make it scrollable if needed

	//Get text position on screen
	gfx_getStringSize(title, &x,&y);		//get string size on screen

	if(x>LCD_WIDTH-10-10)
	{
		stxt.x=10;
		stxt.y=BAR_TOP_H+2+3+2;
		gui_settext_scrolltext(&stxt, title);
		set_element_Scrolltext_list(stl, &stxt, 3);
	}
	else
	{		
		//Print it
		gfx_putS(COLOR_LINE2,COLOR_BG,(LCD_WIDTH-x)/2,BAR_TOP_H+2+3+2,title);
		
		free(title);//Free mem
	}	
	

	//pict or not pict, that is the question
	if((*gtn).pict==NULL)
	{
		max_w = LCD_WIDTH - (3+3) - 6;	//Width allowed for text
		stxt.x=3+4;
	}
	else
	{
		max_w = LCD_WIDTH - (3+3+64+3) - 6;	//Width allowed for text
		stxt.x=3+3+64+3;
	}

	gfx_getStringSize("x", &x,&y);		//get char size on screen
	max_w = max_w/x;
	//Initialize Scrolltext structure
	stxt.w= max_w;

	
	//Other infos
	if((*gtn).str_naiss!=NULL)
	{
		int age;
		//Get age
		age = gtn_get_age((*gtn).str_naiss);
		if (age>1) {sprintf(title, "%s (%d ans)", (*gtn).str_naiss, age);}
		else {sprintf(title, "%s (%d an)", (*gtn).str_naiss, age);}
		gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+6,title);
	}
	else
	{
		gfx_putS(COLOR_LINE1,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+6,"(Vide)");
	}
	
	if((*gtn).str_tel_fixe!=NULL)
	{
		gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+16,(*gtn).str_tel_fixe);
	}
	else
	{
		gfx_putS(COLOR_LINE1,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+16,"(Vide)");
	}
	
	if((*gtn).str_tel_port!=NULL)
	{
		gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+26,(*gtn).str_tel_port);
	}
	else
	{
		gfx_putS(COLOR_LINE1,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+26,"(Vide)");
	}
	
	if((*gtn).str_mail!=NULL)
	{
		gui_settext_scrolltext(&stxt, (*gtn).str_mail);
		if(stxt.char_nb>=max_w)
		{
			stxt.y=BAR_TOP_H+2+3+12+36;
			set_element_Scrolltext_list(stl, &stxt, 0);
		}
		else
		{
			gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+36,(*gtn).str_mail);
		}
	}
	else
	{
		gfx_putS(COLOR_LINE1,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+36,"(Vide)");
	}
	
	
	if((*gtn).str_adresse!=NULL)
	{
		gui_settext_scrolltext(&stxt, (*gtn).str_adresse);
		if(stxt.char_nb>=max_w)
		{
			stxt.y=BAR_TOP_H+2+3+12+46;
			set_element_Scrolltext_list(stl, &stxt, 1);
		}
		else
		{		
			gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+46,(*gtn).str_adresse);
		}
	}
	else
	{
		gfx_putS(COLOR_LINE1,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+46,"(Vide)");
	}

	
	//If no group
	if((*gtn).group==NULL)
	{
		gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+56,*((*gr_name_str).name+0));	//Use group 0 name
	}
	else
	{
		int tot_len, j,i;
		char *group_str_buff=NULL;
		//Else make the group string
		
		//Count how many chars have to be copied
		tot_len=0;
		i=-1;
		do{
			i++;
			j=-1;
			do{
				j++;
				tot_len++;
				
			}while(*(*((*gr_name_str).name+*((*gtn).group+i))+j)!='\0');
			
		}while(*((*gtn).group+i)!=0);


		//mem alloc and copy each char
		group_str_buff = (char *) malloc(tot_len*sizeof(char));
		
		tot_len=-1;
		i=-1;
		do{
			i++;
			j=-1;
			do{
				j++;
				tot_len++;
				*(group_str_buff+tot_len) = *(*((*gr_name_str).name+*((*gtn).group+i))+j);
				
			}while(*(*((*gr_name_str).name+*((*gtn).group+i))+j)!='\0');
			
			*(group_str_buff+tot_len) = ' ';
			
		}while(*((*gtn).group+i)!=0);

		*(group_str_buff+tot_len) = '\0';

		
		//Test length and make it scrollable if needed
		
		gui_settext_scrolltext(&stxt, group_str_buff);
		if(stxt.char_nb>=max_w)
		{
			stxt.y=BAR_TOP_H+2+3+12+56;
			set_element_Scrolltext_list(stl, &stxt, 2);
		}
		else
		{		
			gfx_putS(COLOR_LINE2,COLOR_BG,stxt.x,BAR_TOP_H+2+3+12+56,group_str_buff);
		}
		

		free(group_str_buff);
	}
	
	
	title =	straddinnew((*gtn).str_crea_date, (*gtn).str_mod_date);

	//Get text position on screen
	gfx_getStringSize(title, &x,&y);		//get string size on screen
	//Print it
	gfx_putS(COLOR_LINE2,COLOR_BG,(LCD_WIDTH-x)/2,BAR_TOP_H+2+3+DLG_HEIGTH+1,title);

	free(title);
	
		//NEXT AND PREVIOUS BUTTONS

	//layout
	
	gfx_drawLine(COLOR_LINE1,0,LCD_HEIGTH-BAR_BOT_H-13,LCD_WIDTH/2-6,LCD_HEIGTH-BAR_BOT_H-13);
	gfx_drawLine(COLOR_LINE1,LCD_WIDTH/2+6,LCD_HEIGTH-BAR_BOT_H-13,LCD_WIDTH,LCD_HEIGTH-BAR_BOT_H-13);

	gfx_drawLine(COLOR_LINE1,LCD_WIDTH/2,LCD_HEIGTH-BAR_BOT_H-7,LCD_WIDTH/2,LCD_HEIGTH-BAR_BOT_H-2);
	
	//rounded corners

	//Top left corner
	x=LCD_WIDTH/2;
	y=LCD_HEIGTH-BAR_BOT_H-13;
	
	y+=5;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x++;
	gfx_drawPixel(COLOR_LINE1,x,y);


	//Top right corner
	x=LCD_WIDTH/2;
	y=LCD_HEIGTH-BAR_BOT_H-13;
	
	y+=5;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	y--;
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);
	x--;
	gfx_drawPixel(COLOR_LINE1,x,y);

	//printf arrows
	
		//Left one
	x=3;
	y=LCD_HEIGTH-BAR_BOT_H-7;
	
	y-=3;
	x+=3;
	gfx_drawPixel(COLOR_LINE1,x,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x-1,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x-1,y);
	gfx_drawPixel(COLOR_LINE1,x-2,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x-1,y);
	gfx_drawPixel(COLOR_LINE1,x-2,y);
	gfx_drawPixel(COLOR_SHADOW,x-3,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x-1,y);
	gfx_drawPixel(COLOR_SHADOW,x-2,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_SHADOW,x-1,y);

	y++;
	gfx_drawPixel(COLOR_SHADOW,x,y);


		//Right one
	x=LCD_WIDTH-4;
	y=LCD_HEIGTH-BAR_BOT_H-7;
	
	y-=3;
	x-=3;
	gfx_drawPixel(COLOR_LINE1,x,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x+1,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x+1,y);
	gfx_drawPixel(COLOR_LINE1,x+2,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x+1,y);
	gfx_drawPixel(COLOR_LINE1,x+2,y);
	gfx_drawPixel(COLOR_SHADOW,x+3,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_LINE1,x+1,y);
	gfx_drawPixel(COLOR_SHADOW,x+2,y);

	y++;
	gfx_drawPixel(COLOR_LINE1,x,y);
	gfx_drawPixel(COLOR_SHADOW,x+1,y);

	y++;
	gfx_drawPixel(COLOR_SHADOW,x,y);


	//text

	//Calc max width
	max_w = LCD_WIDTH/2 -9 -5;	//Width allowed for text
	gfx_getStringSize("x", &x,&y);		//get char size on screen
	max_w = max_w/x;

	
	//Initialize Scrolltext structure
	stxt.x=9;
	stxt.y=LCD_HEIGTH-BAR_BOT_H-11;
	stxt.w= max_w;
	stxt.start_pos = 0;
	stxt.pos_dir = true;
	stxt.color = COLOR_LINE2;
	stxt.color_bg = COLOR_BG;

	//Test length and make it scrollable if needed
		
	//Left one
	gui_settext_scrolltext(&stxt, prev_str);
	if(stxt.char_nb>=max_w)
	{
		set_element_Scrolltext_list(stl, &stxt, 4);
	}
	else
	{
		gfx_putS(stxt.color,stxt.color_bg,stxt.x,stxt.y,prev_str);
	}


	//Right one
	gui_settext_scrolltext(&stxt, next_str);
	if(stxt.char_nb>=max_w)
	{
		stxt.x=LCD_WIDTH/2+5;
		set_element_Scrolltext_list(stl, &stxt, 5);
	}
	else
	{
		gfx_getStringSize(next_str, &x,&y);		//get string size on screen
		gfx_putS(stxt.color,stxt.color_bg,LCD_WIDTH-10-x,stxt.y,next_str);
	}




	//Print scrolltexts

	gui_move_Scrolltext_list(stl);		//Print once (because pict will wait hdd wake up to load data so there's is
	gui_print_Scrolltext_list(stl);		//sometimes 2 sec without any answer of the apps)


		//PICTURE
	
	if((*gtn).pict==NULL)
	{										//print a standart pict TODO ???
		
	}
	else
	{
		//Print once in case a delay until load the pict
		
	//Pict rectangle + each corner		
		gfx_drawRect(COLOR_LINE2,3+4,BAR_TOP_H+2+3+12+4,64,64);

	//Top left corner
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+1,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+1);
		gfx_drawPixel(COLOR_LINE2,3+4+1,BAR_TOP_H+2+3+12+4+1);
	
	//Top right corner
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+63-1,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+1);
		gfx_drawPixel(COLOR_LINE2,3+4+63-1,BAR_TOP_H+2+3+12+4+1);
	
	//Bottom right corner
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+63-1,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+63-1);
		gfx_drawPixel(COLOR_LINE2,3+4+63-1,BAR_TOP_H+2+3+12+4+63-1);
	
	//Bottom left corner
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+1,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+63-1);
		gfx_drawPixel(COLOR_LINE2,3+4+1,BAR_TOP_H+2+3+12+4+63-1);


		print_pal_bmp32((*gtn).pict, 3+4, BAR_TOP_H+2+3+12+4);			//print pict and border
	

	//Pict rectangle + each corner		
		gfx_drawRect(COLOR_LINE2,3+4,BAR_TOP_H+2+3+12+4,64,64);

	//Top left corner
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+1,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+1);
		gfx_drawPixel(COLOR_LINE2,3+4+1,BAR_TOP_H+2+3+12+4+1);
	
	//Top right corner
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+63-1,BAR_TOP_H+2+3+12+4);
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+1);
		gfx_drawPixel(COLOR_LINE2,3+4+63-1,BAR_TOP_H+2+3+12+4+1);
	
	//Bottom right corner
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+63-1,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+63,BAR_TOP_H+2+3+12+4+63-1);
		gfx_drawPixel(COLOR_LINE2,3+4+63-1,BAR_TOP_H+2+3+12+4+63-1);
	
	//Bottom left corner
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4+1,BAR_TOP_H+2+3+12+4+63);
		gfx_drawPixel(COLOR_BG,3+4,BAR_TOP_H+2+3+12+4+63-1);
		gfx_drawPixel(COLOR_LINE2,3+4+1,BAR_TOP_H+2+3+12+4+63-1);

	}
	
	free_scrolltext(&stxt);

	
	return 0;
}
