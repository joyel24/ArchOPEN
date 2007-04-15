//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_bdd.c		                        Author : Captain H@dock
//  Last change : 19/12/2006                    Created : 15/12/2006
//
//  Define the Gontactor Database structure and functions to access and modify structure
//  data.
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_struct.h"
#include "gtn_bdd.h"

//Constants define
#include "constant.h"

#include "newout.h"

//------------------------------------------------------------------------------
//  bdd_init function
//
//  init bdd pointers
//
//
//  Input args : (1)
//      *Gtn_bdd =>  Gontactor database structure to initialize
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void bdd_init(Gtn_bdd *db)
{
	(*db).gtn_list = NULL;
	(*db).nb = 0;
	return;
}


//------------------------------------------------------------------------------
//  bdd_free function
//
//  free bdd pointers
//
//
//  Input args : (1)
//      *Gtn_bdd =>  Gontactor database structure to free
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void bdd_free(Gtn_bdd *db)
{
	int i;

	if((*db).gtn_list!=NULL)		//If there is allocated mem for gtn struc list (assuming numer of element in the list isn't 0
	{
		if((*db).nb<=0)			//If number of element in list is negative or null there is a problem...
		{
		        #ifdef LOG_ON
			newout("Uncoherent number of gtn structures in database, can't free bdd structure.\n");
			#endif
			return;
		}
		for(i=0;i<(*db).nb;i++)			//For each element of the list
		{
			gtn_struct_rst((*db).gtn_list+i);	//Free gtn structure
		}
		free((*db).gtn_list);				//Free table alloc
		(*db).gtn_list=NULL;				//Set to NULL
	}
	(*db).nb = 0;							//No element in list

	return;								//Job done
}

