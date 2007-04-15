//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_bdd.h                      		Author : Captain H@dock
//  Last change : 15/12/2006            	Created : 15/12/2006
//
//  Define the Gontactor Database structure and functions to access and modify structure
//  data.
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_struct.h"

#ifndef GTN_BDD
#define GTN_BDD


//Structures

//------------------------------------------------------------------------------
//  Gtn_bdd                           size : unknow
//
//  Contain loaded Gontact
//
//  Datas :
//	nb		=>	Number of gontact (int)
//      gtn_list	=>  table of Gontacts (Gontact *)
//------------------------------------------------------------------------------
typedef struct Gtn_bdd Gtn_bdd;
struct Gtn_bdd
{
	int nb;
	Gontact *gtn_list;
};


//Prototypes
void bdd_init(Gtn_bdd *db);
void bdd_free(Gtn_bdd *db);


#endif

