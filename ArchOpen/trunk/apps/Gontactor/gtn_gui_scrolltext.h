//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2007 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_scrolltext.h       		Author : Captain H@dock
//  Last change : 05/01/2007            	Created : 01/01/2007
//
//  Function and structure to print scrolltext on screen
//------------------------------------------------------------------------------
#ifndef GTN_GUI_SCROLLTEXT
#define GTN_GUI_SCROLLTEXT

#include "constant.h"

#include "newout.h"

//structure

//------------------------------------------------------------------------------
//  Scrolltext                           size : unknow
//
//  Contain information used to print a Scrollable text on screen
//
//  Datas :
//      x	  	=>  x position (int)
//      y	  	=>  y position (int)
//      w	  	=>  Nb of char allowed on screen (int)
//      char_nb	=>  Number of char in the text (int)
//      start_pos => Current first printed char (int)
//      pos_dir	=>  Positive direction? (bool) T:Right scrolling F:Left srolling
//      text	=>  Text to print on screen (char *)
//      color	=>  Text color id in the pal (char )
//      color	=>  Text background color id in the pal (char )
//------------------------------------------------------------------------------
typedef struct Scrolltext Scrolltext;
struct Scrolltext
{
	int x,y,w,char_nb;
	int start_pos;
	bool pos_dir;
	char *text;
	char color, color_bg;
};


//------------------------------------------------------------------------------
//  Scrolltext_list                           size : unknow
//
//  List of Scrolltext structures
//
//  Datas :
//      nb	    =>  Number of struc in list (int) [0 to nb-1]
//      list	=>  Table of Scrolltext structures (Scrolltext *)
//------------------------------------------------------------------------------
typedef struct Scrolltext_list Scrolltext_list;
struct Scrolltext_list
{
	int nb;
	Scrolltext *list;
};



//Prototypes
void free_scrolltext(Scrolltext *stxt);
void gui_move_scrolltext(Scrolltext *stxt);
void gui_print_scrolltext(Scrolltext *stxt);
void gui_settext_scrolltext(Scrolltext *stxt, char *str);

void init_Scrolltext_list(Scrolltext_list *stl);
void free_Scrolltext_list(Scrolltext_list *stl);
void set_number_Scrolltext_list(Scrolltext_list *stl, int nb);
int set_element_Scrolltext_list(Scrolltext_list *stl, Scrolltext *stxt, int id);
void gui_move_Scrolltext_list(Scrolltext_list *stl);
void gui_print_Scrolltext_list(Scrolltext_list *stl);

#endif
