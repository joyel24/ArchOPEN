//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : waiakey.h		                         Author : Captain H@dock
//  Last change : 24/12/2006                    Created : 24/12/2006
//
//  Wait for a pressed key
//------------------------------------------------------------------------------
#include <api.h>

#include "waitakey.h"
#include "printscreen.h"


//------------------------------------------------------------------------------
//  waitakey function
//
//  wait for a pressed key and return it
//
//  Input args : (0)
//
//  Input arg : (1)
//      int key pressed
//------------------------------------------------------------------------------
int waitakey(void)
{
	int eventHandler,event;
	int key=0;

	eventHandler = evt_getHandler(BTN_CLASS);

	event=0;
	while(!event){
        event=evt_getStatusBlocking(eventHandler);
		if (!event) continue; // no new events

		switch(event){
			case BTN_UP:
				key=BTN_UP;
				break;
			case BTN_DOWN:
				key=BTN_DOWN;
				break;
			case BTN_LEFT:
				key=BTN_LEFT;
				break;
			case BTN_RIGHT:
				key=BTN_RIGHT;
				break;
			case BTN_ON:
				key=BTN_ON;
				break;
			case BTN_1:
				key=BTN_1;
				break;
			case BTN_2:
				key=BTN_2;
				break;
			case BTN_F1:
				key=BTN_F1;
				break;
			case BTN_F2:
				key=BTN_F2;
				break;
			case BTN_F3:
				key=BTN_F3;
				break;
			case BTN_OFF:
				key=BTN_OFF;
				break;
		}
	}

	evt_freeHandler(eventHandler);	
	return key;	
}



//------------------------------------------------------------------------------
//  waitakey function
//
//  wait for a pressed key and return it
//
//  Input args : (0)
//
//  Input arg : (1)
//      int key pressed
//------------------------------------------------------------------------------
int waitakey2(Scrolltext_list *stl)
{
	int eventHandler,event;
	int key=0,waiter=0;

	//~ char *FILE=NULL;
	//~ int i=0;

	//~ FILE = (char *) malloc(10*sizeof(char));
	
	eventHandler = evt_getHandler(BTN_CLASS);

	event=0;
	while(!event){

		waiter++;
		if(waiter>130000)
		{
			gui_move_Scrolltext_list(stl);
			gui_print_Scrolltext_list(stl);
			waiter=0;
			//~ i++;
			//~ sprintf(FILE, "/FILE%03d", i);
			//~ gui_printscreen(FILE);
			//~ free(FILE);
			
		}
        event=evt_getStatusBlocking(eventHandler);
		if (!event) continue; // no new events

		switch(event){
			case BTN_UP:
				key=BTN_UP;
				break;
			case BTN_DOWN:
				key=BTN_DOWN;
				break;
			case BTN_LEFT:
				key=BTN_LEFT;
				break;
			case BTN_RIGHT:
				key=BTN_RIGHT;
				break;
			case BTN_ON:
				key=BTN_ON;
				break;
			case BTN_1:
				key=BTN_1;
				break;
			case BTN_2:
				key=BTN_2;
				break;
			case BTN_F1:
				key=BTN_F1;
				break;
			case BTN_F2:
				key=BTN_F2;
				break;
			case BTN_F3:
				key=BTN_F3;
				break;
			case BTN_OFF:
				key=BTN_OFF;
				break;
		}
	}

	evt_freeHandler(eventHandler);	
	return key;	
	
}


