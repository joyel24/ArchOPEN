//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_list.h		                Author : Captain H@dock
//  Last change : 27/12/2006                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

#ifndef GTN_GUI_PROGRESSBAR
#define GTN_GUI_PROGRESSBAR

//------------------------------------------------------------------------------
//  Progressbar                           size : 6*int
//
//  Contain information used to print a scrollbar on screen
//
//  Datas :
//      x	  	=>  x position (int)
//      y	  	=>  y position (int)
//      w	  	=>  width (int)
//      h	  	=>  heigth (int)
//      max	  	=>  Max value (int)
//      cur	  	=>  Current value (int)
//------------------------------------------------------------------------------
typedef struct Progressbar Progressbar;
struct Progressbar
{
	int x,y,w,h,max,cur;
};



//progress bar
void gui_progressbar(Progressbar *bar);


#endif

