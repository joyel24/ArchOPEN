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

#ifndef GTN_GUI_LIST
#define GTN_GUI_LIST

#include "constant.h"
#include "gtn_gui_scrollbar.h"

//------------------------------------------------------------------------------
//  List_bend structure                           size : unknown
//
//  Contain information used to print a list_bend on screen
//
//  Datas :
//	element_nb	=>	Number of element in the list (int)
//      elements_str	=>  	Pointers to each list element title (char **)
//------------------------------------------------------------------------------
typedef struct List_bend List_bend;
struct List_bend
{
	int element_nb;
	char **elements_str;
};


//gui_list_bend
int gui_list_bend(List_bend *list, int sel);
void list_bend_draw_element(char *title, int y, char style);

#endif

