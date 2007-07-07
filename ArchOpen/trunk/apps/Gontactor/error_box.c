//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2007 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : error_box.c		      			Author : Captain H@dock
//  Last change : 07/01/2007                    Created : 07/01/2007
//
//  Function to deal with error and return if needed a term signal
//------------------------------------------------------------------------------
#include <api.h>


//Constants define
#include "constant.h"

#include "error_box.h"

#ifdef LOG_ON
#include "newout.h"
#endif


//------------------------------------------------------------------------------
//  error_box function
//
//  Print a dialog and return 1 if need to stop the apps
//
//  Input args : (1)
//      error		=>  error code (int)
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
int error_box(int error)
{
	int eventHandler;
	unsigned char tmp[64];
	int ret_val=1;
	
	sprintf(tmp,"Code d'erreur inconnu. (%d)\n", error);
	
	switch(error)
	{
		//group_read errors
		case 8:
		{
			sprintf(tmp,"Erreur d'ouverture du fichier de groupe.\n");
			ret_val=1;
			break;
		}
		case 7:
		{
			sprintf(tmp,"Structure invalide du fichier de groupe.\n");
			ret_val=1;
			break;
		}
		case 6:
		{
			sprintf(tmp,"Impossible d'atteindre la fin du fichier de groupe.\n");
			ret_val=1;
			break;
		}
		
		//bdd_read errors
		case 18:
		{
			sprintf(tmp,"Erreur d'ouverture du fichier de la base de donn�es.\n");
			ret_val=1;
			break;
		}
		case 17:
		{
			sprintf(tmp,"Structure invalide du fichier de la base de donn�es.\n");
			ret_val=1;
			break;
		}
		case 16:
		{
			sprintf(tmp,"Plus de memoire libre.\n");
			ret_val=1;
			break;
		}
		
	}

	#ifdef LOG_ON
	newout(tmp);
	if(ret_val) newout("Gontactor va s'arreter.\n");
	#endif

	
	//~ draw_box(tmp);
	
	eventHandler = evt_getHandler(BTN_CLASS);
	msgBox_show(TITLE_GTN, tmp, MSGBOX_TYPE_OK, MSGBOX_ICON_ERROR, eventHandler);
	
	evt_freeHandler(eventHandler);	
	
	
	return ret_val;
}


void draw_box(char * txt)
{
	int eventHandler,event;

	
	int h=40, w=180;
	int h2,w2;
	int h3,w3;
	char g;
	
	gfx_fillRect(COLOR_BG,(LCD_WIDTH-w)/2,(LCD_HEIGTH-h)/2, w, h);
	gfx_drawRect(70,(LCD_WIDTH-w)/2,(LCD_HEIGTH-h)/2, w, h);
	
	gfx_getStringSize("A", &w3,&h3);		//get string size on screen

	if(strlen(txt)>w/w3)
	{
		
		g = *(txt+w/w3-1);
		*(txt+w/w3-1) = '\0';

		gfx_getStringSize(txt, &w2,&h2);		//get string size on screen
		gfx_putS(70,COLOR_BG,(LCD_WIDTH-w2)/2,(LCD_HEIGTH-h)/2+4,txt);	//title
		
		//~ *(txt+w/w3) = g;

		//~ gfx_getStringSize((txt+w/w3), &w2,&h2);		//get string size on screen
		//~ gfx_putS(70,COLOR_BG,(LCD_WIDTH-w2)/2,(LCD_HEIGTH-h)/2+4+h3,(txt+w/w3));	//title
		
	}
	else
	{
		gfx_getStringSize(txt, &w2,&h2);		//get string size on screen
		gfx_putS(70,COLOR_BG,(LCD_WIDTH-w2)/2,(LCD_HEIGTH-h)/2+4,txt);	//title
	}
		
	gfx_getStringSize("Press a key to quit", &w2,&h2);		//get string size on screen
	gfx_putS(112,COLOR_BG,(LCD_WIDTH-w2)/2,(LCD_HEIGTH-h)/2+4+h2*2+2,"Press a key to quit");	//title

	eventHandler = evt_getHandler(BTN_CLASS);
	event=0;
	
	while(!event){
        event=evt_getStatusBlocking(eventHandler);
	}

	evt_freeHandler(eventHandler);	
	return;
}

