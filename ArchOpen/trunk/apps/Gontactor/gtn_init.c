//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_init.c                           		Author : Captain H@dock
//  Last change : 14/01/2007            	Created : 19/12/2006
//
//  Loading function of the Gontactor app
//  Called during the start up
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "gtn_init.h"
#include "newout.h"
#include "gtn_gui.h"
//------------------------------------------------------------------------------
//  gtn_loading function
//
//  Perfom all start up operations
//TODO
//	deal with file errors
//
//  Input args : (2)
//      *group_list 	=>  group_list structure to load
//      *database 	=>  Database structure to load
//
//  Input arg : (1)
//      int error code  (0=OK)
//------------------------------------------------------------------------------
int gtn_loading(Groups *group_list, Gtn_bdd *database, Statusbar *stb)
{
	int err=0;
	
	Progressbar pbar;
	
	//Load screen res
	getResolution(&LCD_WIDTH,&LCD_HEIGTH);

	pbar.w=50;
	pbar.h=4;
	pbar.x=(LCD_WIDTH-pbar.w)-2;
	pbar.y=(LCD_HEIGTH)-(BAR_BOT_H+pbar.h)/2;
	pbar.cur=0;
	pbar.max=3;


	#ifdef LOG_ON
	startout();			//Starting text log
	#endif


	#ifdef LOG_ON
	newout("Initialization :\n");
	#endif

	

	#ifdef LOG_ON
	newout("   Loading GUI layout.\n");
	#endif
	gui_layout(stb);

	pbar.cur=1;
	gui_progressbar(&pbar);		//Print progressbar	
	

	(*stb).txt="Chargement des groupes";
	gui_status_bar(stb);


	#ifdef LOG_ON
	newout("   Loading Groups names.\n");
	#endif
	err=group_read(group_list, GROUP_FILENAME);
	//deal with errors returned
	if(err!=0)	return err+10;
	
	
	pbar.cur=2;
	gui_progressbar(&pbar);		//Print progressbar	
	(*stb).txt="Chargement de la BDD";
	gui_status_bar(stb);


	#ifdef LOG_ON
	newout("   Loading Database.\n");
	#endif
	err=gtn_bdd_read(database, DATABASE_FILENAME);
	//deal with errors returned
	if(err!=0)	return err+20;

	pbar.cur=3;
	gui_progressbar(&pbar);		//Print progressbar	
	
	
	#ifdef LOG_ON
	newout("Initialization done.\n");
	#endif
	
	gui_layout_bot_bar();	//Erase bottom bar

	return 0;

}


//------------------------------------------------------------------------------
//  gtn_quiting function
//
//  Perfom all shuting down operations
//TODO
//	deal with file errors
//
//  Input args : (2)
//      *group_list 	=>  group_list structure to free
//      *database 	=>  Database structure to free
//
//  Input arg : (1)
//      int error code  (0=OK)
//------------------------------------------------------------------------------
int gtn_exiting(Groups *group_list, Gtn_bdd *database, Statusbar *stb)
{
	Progressbar pbar;
	pbar.w=50;
	pbar.h=4;
	pbar.x=(LCD_WIDTH-pbar.w)-2;
	pbar.y=(LCD_HEIGTH)-(BAR_BOT_H+pbar.h)/2;
	pbar.cur=0;
	pbar.max=4;

	pbar.cur=4;
	gui_progressbar(&pbar);		//Print progressbar	
	

	#ifdef LOG_ON
	(*stb).txt="En attente du disque";
	gui_status_bar(stb);
	newout("Exiting apps :\n");
	#endif


	(*stb).txt="Base de données";
	gui_status_bar(stb);

	pbar.cur=3;
	gui_progressbar(&pbar);		//Print progressbar	
	#ifdef LOG_ON
	newout("   Free Database.\n");
	#endif
	bdd_free(database);


	pbar.cur=2;
	gui_progressbar(&pbar);		//Print progressbar	

	(*stb).txt="Liste des groupes";
	gui_status_bar(stb);

	#ifdef LOG_ON
	newout("   Free Groups names.\n");
	#endif
	group_free(group_list);


	pbar.cur=1;
	gui_progressbar(&pbar);		//Print progressbar	
	#ifdef LOG_ON
	newout("   Quit GUI layout.\n");
	#endif
	(*stb).txt="GUI";
	gui_status_bar(stb);



//TODO really quit GUI and delete new GUI functions
	(*stb).txt="Fermeture du log";
	gui_status_bar(stb);
	pbar.cur=0;
	gui_progressbar(&pbar);		//Print progressbar	


	#ifdef LOG_ON
	newout("   Close log.\n");
	newout("Done.\n");
	#endif
	
	
	#ifdef LOG_ON
	endout();
	#endif
	
	pbar.cur=0;
	gui_progressbar(&pbar);		//Print progressbar	
	gfx_clearScreen(COLOR_WHITE);	//Remplace by the gui_layouts
	
	return 0;
}







