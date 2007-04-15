//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2007 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_gui_scrolltext.c       		Author : Captain H@dock
//  Last change : 09/01/2007            	Created : 01/01/2007
//
//  Function and structure to print scrolltext on screen
//------------------------------------------------------------------------------
#include <api.h>

#include "gtn_gui_scrolltext.h"

//------------------------------------------------------------------------------
//  free_scrolltext function
//
//  free meme allocated for the scrolltext structure and reset all values
//
//  Input args : (1)
//      stxt    =>      Scrolltext struture to free  (Scrolltext *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void free_scrolltext(Scrolltext *stxt)
{
    (*stxt).x=0;
    (*stxt).y=0;
    (*stxt).w=0;
    (*stxt).char_nb=0;
    (*stxt).start_pos=0;
    (*stxt).pos_dir=true;
    (*stxt).color=0;
    (*stxt).color_bg=0;

    if((*stxt).text!=NULL)
    {
        free((*stxt).text);
        (*stxt).text=NULL;
    }
    return;
}


//------------------------------------------------------------------------------
//  gui_move_scrolltext function
//
//  change the scrolltext structure value to scroll the text on screen
//
//  Input args : (1)
//      stxt    =>      Scrolltext struture to move  (Scrolltext *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void gui_move_scrolltext(Scrolltext *stxt)
{
    //check direction
    if((*stxt).pos_dir==true)       //positive
    {
        //left hidden chars after??
        if((*stxt).char_nb-(*stxt).start_pos-(*stxt).w>=0)    //yes
        {
            (*stxt).start_pos++;                       //Continue rotation
        }
        else
        {
            //~ (*stxt).start_pos--;               //Go back (avoid to pause)
            (*stxt).pos_dir=false;                      //switch direction
        }
    }
    else
    {
        //left hidden chars before??
        if((*stxt).start_pos>0)    //yes
        {
            (*stxt).start_pos--;                       //Continue rotation
        }
        else
        {
            //~ (*stxt).start_pos++;               //go ahead (avoid to pause)
            (*stxt).pos_dir=true;                      //switch direction
        }
    }
    return;
}


//------------------------------------------------------------------------------
//  gui_print_scrolltext function
//
//  Print text on screen
//
//  Input args : (1)
//      stxt    =>      Scrolltext struture to print  (Scrolltext *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void gui_print_scrolltext(Scrolltext *stxt)
{
	char *print_buff=NULL;
	int i;

	print_buff = (char *) malloc(((*stxt).w+1)*sizeof(char));


	for(i=0;i<(*stxt).w;i++)
	{
		*(print_buff+i)=*((*stxt).text+((*stxt).start_pos+i-1));
	}
	*(print_buff+(*stxt).w)='\0';

	//Print field
	gfx_putS((*stxt).color,(*stxt).color_bg,(*stxt).x,(*stxt).y,print_buff);

	free(print_buff);
	return;
}


//------------------------------------------------------------------------------
//  gui_settext_scrolltext function
//
//  Set text string in scrolltext structure Alloc mem and copy chars
//
//  Input args : (2)
//      stxt    =>      Scrolltext struture to print  (Scrolltext *)
//      str     =>       Text to add  (char *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void gui_settext_scrolltext(Scrolltext *stxt, char *str)
{
    char *txt_buff=NULL;
    int i,j;

    if(str==NULL)
    {
        printf("Invalid string to add in scrolltext.\n");
         return;
    }

    i=-1;                                   //Get string length
    do{
        i++;
    }while(*(str+i)!='\0');


    txt_buff = (char *) malloc((i+1)*sizeof(char));     //mem alloc

    for(j=0;j<i;j++)            //Copy
    {
        *(txt_buff+j)=*(str+j);;
    }
    *(txt_buff+i)='\0';

    (*stxt).text=txt_buff;  //save infos
    (*stxt).char_nb=i;

	return;
}


//------------------------------------------------------------------------------
//  init_scrolltext_list function
//
//  Init the Scrolltext structure
//
//  Input args : (1)
//      stl    =>      Scrolltext_list struture to initialize  (Scrolltext_list *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void init_Scrolltext_list(Scrolltext_list *stl)
{
    (*stl).nb=0;
    (*stl).list=NULL;

    return;
}

//------------------------------------------------------------------------------
//  free_scrolltext_list function
//
//  free memory allocated for the list
//
//  Input args : (1)
//      stl    =>      Scrolltext_list struture to free  (Scrolltext_list *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void free_Scrolltext_list(Scrolltext_list *stl)
{
    if((*stl).list!=NULL)           //If structure isn't empty
    {
        int i;
        for(i=0;i<(*stl).nb;i++)       //For each Scrolltext struct of the list
        {
            free_scrolltext((*stl).list+i);     //Free Scrolltext strcture
        }
        free((*stl).list);                      //Free list
	(*stl).list=NULL;
	(*stl).nb=0;
   }
    

    return;
}


//------------------------------------------------------------------------------
//  set_number_scrolltext_list function
//
//  Allocate enought memory for the list
//
//  Input args : (2)
//      stl     =>      Scrolltext_list struture to modify  (Scrolltext_list *)
//      nb      =>      Number of Scrolltext (int)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void set_number_Scrolltext_list(Scrolltext_list *stl, int nb)
{
	int i;
	
	if((*stl).nb!=0 && (*stl).list!= NULL)      //Test struct fields
	{
		#ifdef LOG_ON
		newout("Scrolltext list isn't empty or initialised, unable to set the number.\n");
		#endif
		return;
	}

	(*stl).nb=nb;               //Save number of Scrolltext in the list

	(*stl).list= (Scrolltext *) malloc(nb*sizeof(Scrolltext));  //Alloc memory for the list

	if((*stl).list==NULL)       //if malloc failled
	{
		#ifdef LOG_ON
		newout("Unable to alloc memory for Scrolltext_list.\n");
		#endif
	}
    
	for(i=0;i<(*stl).nb;i++)		//Use this to mark as unused....
	{
		(*((*stl).list+i)).x=-1;
	}
    
	return;
}



//------------------------------------------------------------------------------
//  set_element_scrolltext_list function
//
//  Save a scrolltext structure in the list
//
//  Input args : (2)
//      stl     =>      Scrolltext_list struture to modify  (Scrolltext_list *)
//    	id      =>      Number of Scrolltext (int) [0;(*stl).nb-1]
//
//  Input arg : (1)
//      int error code
//          0   =   OK
//          -1  =   id is out of range
//          -2  =   NULL Scrolltext structure to add
//          -3  =   NULL Scrolltext_list structure to modify
//          -4  =   Out of memory
//------------------------------------------------------------------------------
int set_element_Scrolltext_list(Scrolltext_list *stl, Scrolltext *stxt, int id)
{
	int i;
	
		//Check inputs validity

	if(id<0 || id>=(*stl).nb)
	{
		#ifdef LOG_ON
		newout("Scrolltext id to add to list is out of range.\n");
		#endif
		return -1;
	}

	if(stxt==NULL)
	{
		#ifdef LOG_ON
		newout("Scrolltext struct to add to the list is NULL.\n");
		#endif
		return -2;
	}

	if(stl==NULL)
	{
		#ifdef LOG_ON
		newout("Scrolltext_list struct is NULL.\n");
		#endif
		return -3;
	}

	//Copy each value
	(*(((*stl).list)+id)).x=(*stxt).x;
	(*(((*stl).list)+id)).y=(*stxt).y;
	(*(((*stl).list)+id)).w=(*stxt).w;
	(*(((*stl).list)+id)).char_nb=(*stxt).char_nb;
	(*(((*stl).list)+id)).pos_dir=(*stxt).pos_dir;
	(*(((*stl).list)+id)).color=(*stxt).color;
	(*(((*stl).list)+id)).color_bg=(*stxt).color_bg;
	
	//Copy text
	(*(((*stl).list)+id)).text = (char *) malloc(((*stxt).char_nb+1)*sizeof(char)); 	//mem alloc

	if((*(((*stl).list)+id)).text==NULL)
	{
		#ifdef LOG_ON
		newout("Out of memory.\n");
		#endif
		return -4;
	}

	//Copy each char
	for(i=0;i<(*stxt).char_nb;i++)
	{
		*((*(((*stl).list)+id)).text+i) = *((*stxt).text+i);
	}
	*((*(((*stl).list)+id)).text+(*stxt).char_nb) ='\0';

	return 0;
}



//------------------------------------------------------------------------------
//  gui_move_scrolltext_list function
//
//  Move each none NULL Scrolltext of the list
//
//  Input args : (1)
//      stl     =>      Scrolltext_list struture to modify  (Scrolltext_list *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void gui_move_Scrolltext_list(Scrolltext_list *stl)
{
	int i;
	
	for(i=0;i<(*stl).nb;i++)
	{
		if((*((*stl).list+i)).x!=-1)
		{
			gui_move_scrolltext((*stl).list+i);
		}
	}

	return;
}


//------------------------------------------------------------------------------
//  gui_print_scrolltext_list function
//
//  Print each none NULL Scrolltext of the list
//
//  Input args : (1)
//      stl     =>      Scrolltext_list struture to Print  (Scrolltext_list *)
//
//  Input arg : (0)
//------------------------------------------------------------------------------
void gui_print_Scrolltext_list(Scrolltext_list *stl)
{
	int i;
	
	for(i=0;i<(*stl).nb;i++)
	{
		if((*((*stl).list+i)).x!=-1)
		{
			gui_print_scrolltext((*stl).list+i);
		}
	}

	return;
}


