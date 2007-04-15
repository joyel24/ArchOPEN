//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_init.h                           		Author : Captain H@dock
//  Last change : 19/12/2006            	Created : 19/12/2006
//
//  Loading function of the Gontactor app
//  Called during the start up
//------------------------------------------------------------------------------
#ifndef GTN_INIT
#define GTN_INIT

#include "gtn_struct.h"
#include "gtn_bdd.h"
#include "gtn_file.h"
#include "gtn_gui.h"

//Structures


//Prototypes
int gtn_loading(Groups *group_list, Gtn_bdd *database, Statusbar *stb);
int gtn_exiting(Groups *group_list, Gtn_bdd *database, Statusbar *stb);


#endif

