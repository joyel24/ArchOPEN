//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_progressbar.c		                Author : Captain H@dock
//  Last change : 27/12/2006                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_gui_progressbar.h"



//------------------------------------------------------------------------------
//  gui_progressbar function
//
//  Print a progressbar
//  TODO LATER :	
//	PRINT PERCENT VALUE (controled by a style int)
//
//  Input args : (1)
//      scrollbar*		=>  progress structure
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void gui_progressbar(Progressbar *bar)
{
	int bar_len;
	
	//Clear progressbar area
	gfx_fillRect(COLOR_WHITE,(*bar).x,(*bar).y,(*bar).w,(*bar).h);
	
	//Print edges
	gfx_drawRect(7,(*bar).x,(*bar).y,(*bar).w,(*bar).h);
	
	//Calc bar length and print it
	bar_len=(((*bar).w-2)*(*bar).cur)/(*bar).max;
	
	gfx_fillRect(218,(*bar).x+1,(*bar).y+1,bar_len,(*bar).h-2);
	
	return;	
}


