//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.


#include <api.h>


//Constants define
#include "constant.h"

#ifdef LOG_ON
#include "newout.h"
#endif

#include "str_fct.h"
#include "waitakey.h"


#include "gtn_init.h"

#include "printscreen.h"

#include "gtn_struct.h"
#include "gtn_file.h"

#include "gtn_gui.h"
#include "error_box.h"




void app_main(int argc,char * * argv)
{
	
	Gtn_bdd database;
	Groups group_list;
	Statusbar stb;
	Scrolltext_list stl;
	
	List_bend testlist;
	char **strs=NULL;
	
	int a=0,b=0,c=0,olda=-2;
	int error;

	//Loader
	error = gtn_loading(&group_list, &database, &stb);
	//deal with errors
	if (error!=0)
	{
		if(error_box(error)==1)
		{
			return;
		}
	}
	
	
	
	stb.txt=NULL;
	gui_status_bar(&stb);
	

	strs = (char **) malloc(database.nb*sizeof(char *));
	
	for(a=0;a<database.nb;a++)
	{
		*(strs+a) = straddinnew((*(database.gtn_list+a)).str_nom, (*(database.gtn_list+a)).str_prenom);

	}
	testlist.element_nb=database.nb;
	testlist.elements_str=strs;
	
	

	
	init_Scrolltext_list(&stl);			//Init structure
	set_number_Scrolltext_list(&stl,6);
	free_Scrolltext_list(&stl);			//free structure
	//I need to do that first because scrolltext aren't printed firt times they are called
	
	
	a=0;
	b=0;
	do{
		
		a = gui_list_bend(&testlist, a);
		if(a==-1)
		{
			b=1;
		}
		else
		{
			olda=-2;				//BUGFIXED : Allow to rewrite the same one
			do{
				
				a %= database.nb;
				if(olda!=a)
				{
					free_Scrolltext_list(&stl);
					gui_print_gtn((database.gtn_list)+a, &group_list, &stl, *(testlist.elements_str+((a+database.nb-1)%database.nb)), *(testlist.elements_str+((a+1)%database.nb)));
				}
				olda=a;
				c = waitakey2(&stl);
				if (c==BTN_OFF) b=1;
				if (c==BTN_2) b=1;
				if (c==BTN_DOWN) a++;
				if (c==BTN_RIGHT) a++;
				if (c==BTN_UP) a+= database.nb-1;
				if (c==BTN_LEFT) a+= database.nb-1;
				if (c==BTN_F2)
				{
					stb.txt="Copie d'écran en cours";
					gui_status_bar(&stb);
					gui_printscreen("/Gontactor.bmp");
					stb.txt=NULL;
					gui_status_bar(&stb);
				}
			}while(!b);
			b=0;
		}
		mdelay(150);
	}
	while(!b);

	free(strs);
	

	//exiting function
	gtn_exiting(&group_list, &database, &stb);
	//TODO deal with errors

	return;
}

