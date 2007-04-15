//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui.h		                         Author : Captain H@dock
//  Last change : 12/12/2006                    Created : 08/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_file.h"

#ifndef GTN_GUI
#define GTN_GUI

#include "gtn_gui_list.h"
#include "gtn_gui_scrollbar.h"
#include "gtn_gui_progressbar.h"
#include "gtn_gui_printgtn.h"
#include "gtn_gui_scrolltext.h"



//Structures




//------------------------------------------------------------------------------
//  Status Bar                           size : 6*int
//
//  Contain information used to print a statusbar on screen
//  TODO : Deal with str too long
//
//  Datas :
//      x	  	=>  x position (int)
//      y	  	=>  y position (int)
//      w	  	=>  width (int)
//      h	  	=>  heigth (int)
//      txt	  	=>  text to print (char *)
//------------------------------------------------------------------------------
typedef struct Statusbar Statusbar;
struct Statusbar
{
	int x,y,w,h;
	char *txt;
};






//Prototypes

void pose(void);
void showpal(void);


//layout
void gui_layout(Statusbar *stb);
void gui_layout_bot_bar(void);
void gui_status_bar(Statusbar *stb);



#endif

