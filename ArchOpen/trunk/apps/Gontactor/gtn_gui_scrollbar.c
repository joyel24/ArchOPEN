//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_scrollbar.c		        Author : Captain H@dock
//  Last change : 27/12/2006                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "gtn_gui_scrollbar.h"


//------------------------------------------------------------------------------
//  gui_scrollbar function
//
//  Print a scrollbar
//  TODO LATER :	
//	ajust bottom position (division round bug)
//
//  Input args : (1)
//      scrollbar*		=>  scrollbar structure
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void gui_scrollbar(Scrollbar *bar)
{
	int cursor_size;
	//Clear scrollbar area
	gfx_fillRect(COLOR_BG,(*bar).x,(*bar).y,(*bar).w,(*bar).h);
	
	//Print ends
	gfx_drawRect(COLOR_LINE1,(*bar).x,(*bar).y,(*bar).w,4);
	gfx_drawRect(COLOR_LINE1,(*bar).x,(*bar).y+(*bar).h-4,(*bar).w,4);
	
	//Calc cursor size and print it
	cursor_size=((*bar).h-12)/(*bar).max;
	gfx_drawRect(COLOR_LINE1,(*bar).x,(*bar).y+6+((((*bar).h-12)*((*bar).cur-1))/(*bar).max),(*bar).w,cursor_size);
	
	return;	
}

