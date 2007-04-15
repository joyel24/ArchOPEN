//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_printgtn.h	                Author : Captain H@dock
//  Last change : 07/01/2007                    Created : 27/12/2006
//
//  Gontactor Graphic User Interfarce functions
//------------------------------------------------------------------------------
#include <api.h>


#ifndef GTN_GUI_PRINTGTN
#define GTN_GUI_PRINTGTN

#include "gtn_struct.h"
#include "gtn_gui_scrolltext.h"

int gui_print_gtn(Gontact *gtn, Groups *gr_name_str, Scrolltext_list *stl, char *prev_str, char *next_str);

#endif

