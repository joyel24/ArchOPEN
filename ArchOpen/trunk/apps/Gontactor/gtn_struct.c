//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_struct.c                         Author : Captain H@dock
//  Last change : 19/12/2006                    Created : 29/11/2006
//
//  Functions to access and modify Gontactor structure datas
//------------------------------------------------------------------------------
#include <api.h>

//Constants define
#include "constant.h"

#include "gtn_struct.h"
#include "newout.h"

//------------------------------------------------------------------------------
//  gtn_sinit function
//
//  Initiate all Gontact structure fields
//
//  Input args : (1)
//      *gtn        =>  Gontactor struture to initalise
//
//  Input arg : (0)
//
//------------------------------------------------------------------------------
void gtn_init(Gontact *gtn)
{
    (*gtn).str_nom=NULL;
    (*gtn).str_prenom=NULL;
    (*gtn).str_naiss=NULL;
    (*gtn).str_tel_fixe=NULL;
    (*gtn).str_tel_port=NULL;
    (*gtn).str_adresse=NULL;
    (*gtn).str_mail=NULL;
    (*gtn).group=NULL;
    (*gtn).pict=NULL;
    (*gtn).str_crea_date=NULL;
    (*gtn).str_mod_date=NULL;
}


//------------------------------------------------------------------------------
//  gtn_struct_rst function
//
//  Reset all data in Gontactor struture
//  Free memory if needed
//
//  Input args : (1)
//      *gtn        =>  Gontactor struture to reset
//
//  Input arg : (1)
//      int         =>  Error code. (0=OK)
//------------------------------------------------------------------------------
int gtn_struct_rst(Gontact *gtn)
{
  //Test if char* isn't allocated
  if((*gtn).str_nom!=NULL)
  {
      //if it allocated free memory
      free((*gtn).str_nom);
      //set NULL value
      (*gtn).str_nom=NULL;
  }
  //Repeat for each field

  if((*gtn).str_prenom!=NULL)
  {
      free((*gtn).str_prenom);
      (*gtn).str_prenom=NULL;
  }

  if((*gtn).str_naiss!=NULL)
  {
      free((*gtn).str_naiss);
      (*gtn).str_naiss=NULL;
  }

  if((*gtn).str_tel_fixe!=NULL)
  {
      free((*gtn).str_tel_fixe);
      (*gtn).str_tel_fixe=NULL;
  }

  if((*gtn).str_tel_port!=NULL)
  {
      free((*gtn).str_tel_port);
      (*gtn).str_tel_port=NULL;
  }

  if((*gtn).str_adresse!=NULL)
  {
      free((*gtn).str_adresse);
      (*gtn).str_adresse=NULL;
  }

  if((*gtn).str_mail!=NULL)
  {
      free((*gtn).str_mail);
      (*gtn).str_mail=NULL;
  }

  if((*gtn).group!=NULL)
  {
      free((*gtn).group);
      (*gtn).group=NULL;
  }

  if((*gtn).pict!=NULL)
  {
      free((*gtn).pict);
      (*gtn).pict=NULL;
  }

  if((*gtn).str_crea_date!=NULL)
  {
      free((*gtn).str_crea_date);
      (*gtn).str_crea_date=NULL;
  }

  if((*gtn).str_mod_date!=NULL)
  {
      free((*gtn).str_mod_date);
      (*gtn).str_mod_date=NULL;
  }


  return 0;
}


//------------------------------------------------------------------------------
//  gtn_print function
//
//  print every available Gontact struture fields
//
//  Output args : (1)
//      *gtn        =>  Gontactor struture to print
//
//  Input arg : (0)
//
//------------------------------------------------------------------------------
void gtn_print(Gontact *gtn)
{

    #ifdef LOG_ON
    newout("Nom : ");
    #endif

  //Test if char* isn't allocated
  if((*gtn).str_nom==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de Nom");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_nom);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif

  //Repeat for each field

    #ifdef LOG_ON
    newout("Pr�om : ");
    #endif
  if((*gtn).str_prenom==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de Pr�om");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_prenom);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("Date de naissance : ");
    #endif
  if((*gtn).str_naiss==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de Date de naissance");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_naiss);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("T��hone fixe : ");
    #endif
  if((*gtn).str_tel_fixe==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de T��hone fixe");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_tel_fixe);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("T��hone portable : ");
    #endif
  if((*gtn).str_tel_port==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de T��hone portable");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_tel_port);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("Adresse : ");
    #endif
  if((*gtn).str_adresse==NULL)
  {
    #ifdef LOG_ON
    newout("Pas d'adresse");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_adresse);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("E-mail : ");
    #endif
  if((*gtn).str_mail==NULL)
  {
    #ifdef LOG_ON
    newout("Pas d'e-mail ");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_mail);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif

    #ifdef LOG_ON
    newout("Groupes : ");
    #endif
    if((*gtn).group==NULL)                             //If no ID
    {
        #ifdef LOG_ON
        newout("0");                        //Group all
        #endif
    }
    else
    {
        char *group_id;
        group_id = (char *) malloc(5*sizeof(char));     //mem alloc ("|xxx\0")

        int i=-1;
        do{                                             //For each ID
            i++;
        #ifdef LOG_ON
            sprintf(group_id,"%d|",*((*gtn).group+i));  //formating text
            newout(group_id);                //Write
        #endif
        }while(*((*gtn).group+i+1)!=0);                   //End with 0 id
        i++;
        #ifdef LOG_ON
            sprintf(group_id,"%d",*((*gtn).group+i));  //formating text
            newout(group_id);                //Write
        #endif

        free(group_id);                                 //mem free
        group_id=NULL;
    }

    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("Image : ");
    #endif
  if((*gtn).pict==NULL)
  {
    #ifdef LOG_ON
    newout("Pas d'image ");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).pict);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif


    #ifdef LOG_ON
    newout("Date de cr�tion : ");
    #endif
  if((*gtn).str_crea_date==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de date de cr�tion ");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_crea_date);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif

    #ifdef LOG_ON
    newout("Date de derni�e modification : ");
    #endif
  if((*gtn).str_mod_date==NULL)
  {
    #ifdef LOG_ON
    newout("Pas de date de derni�e modification ");
    #endif
  }
  else
  {
    #ifdef LOG_ON
    newout((*gtn).str_mod_date);
    #endif

  }
    #ifdef LOG_ON
    newout("\n");
    #endif




}



//------------------------------------------------------------------------------
//  group_free function
//
//  Free structure pointers
//
//  Input args : (1)
//      *gtn        =>  Groups name structure to free
//
//  Output arg : (0)
//
//------------------------------------------------------------------------------
void group_free(Groups *group)
{
	int cur_name;
	for(cur_name=0;cur_name<(*group).nb;cur_name++)
	{
		free(*((*group).name+cur_name));
		*((*group).name+cur_name)=NULL;
	}
	free((*group).name);
	(*group).name=NULL;
	return;
}


//------------------------------------------------------------------------------
//  gtn_read_line function
//
//  read gtn info in a line send by the bdd parser
//
//  Input args : (1)
//      *gtn        =>  Groups name structure to free
//      *line        =>  The line string to parse (char *)
//
//  Output arg : (1)
//      int error code
//          0   =   OK
//          -3  =   Invalid file structure
//          -4  =   Out of memory
//------------------------------------------------------------------------------
int gtn_read_line(Gontact *gtn, char *line)
{
	int *field_length=NULL;                 //Length of each GTN struct field
	char **gtn_ptn=NULL;
	int cur_field=0,cur_char=0;
	int line_offset=0;


	//First pass : Determine string length for each field.

	field_length = (int *) malloc(11*sizeof(int));
	//field length (for group, nb of group ids)
	//initialize
	for(cur_field=0;cur_field<11;cur_field++)
	{
		*(field_length+cur_field)=0;
	}
	cur_field=0;

	line_offset=-1;				//First char
	line_offset++;					//Next char
	if (*(line+line_offset)!='#')
	{
		#ifdef LOG_ON
		newout("Premier caract�e invalide, ligne corrompue.\n");
		#endif
		return -3;
	}
	do{
		line_offset++;					//Next char
		if (*(line+line_offset)=='#')               //if field end
		{
			cur_field++;                    //Next field
		}
		else
		{
			*(field_length+cur_field)+= 1;  //incr field length
		}

	}while(cur_field<10);                   //for each field

	//Group
	do{
		line_offset++;					//Next char
		if (*(line+line_offset)=='#')               //if field end
		{
			cur_field++;                    //Next field
		}
		if (*(line+line_offset)=='|')               //if field end
		{
			*(field_length+cur_field)+= 1;  //incr field length
		}

	}while(cur_field<11);                   //for each field


//DEBUG PRINT FIELD LENGTH
	//~ for(cur_field=0;cur_field<11;cur_field++)
	//~ {
	//~ printf("%d-",*(field_length+cur_field));
	//~ }
	//~ printf("\n");

	line_offset=-1;					//First char
    
	//Second pass : load string in gtn struct for each field.

	//mem allocation
	//table of strings pointers
	gtn_ptn = (char **) malloc(10*sizeof(char *));


	for(cur_field=0;cur_field<10;cur_field++)       //string field mem alloc
	{
		*(gtn_ptn+cur_field)=(char *) malloc((*(field_length+cur_field)+1)*sizeof(char));
	}

	(*gtn).group = (int *) malloc(*(field_length+10)*sizeof(int));  //group list


	//Loading data
	for(cur_field=0;cur_field<10;cur_field++)           //For each string field
	{
		line_offset++;					//Next char (jump the '#')
		for(cur_char=0;cur_char<*(field_length+cur_field);cur_char++)   //for each char
		{
			line_offset++;					//Next char
			*(*(gtn_ptn+cur_field)+cur_char)=*(line+line_offset);
		}
		*(*(gtn_ptn+cur_field)+cur_char)='\0';          //End char for each string
	}

	//group

	line_offset++;					//Next char
	for(cur_field=0;cur_field<*(field_length+10);cur_field++)
	{
		*((*gtn).group+cur_field)=0;
		do{
			line_offset++;					//Next char
			if(*(line+line_offset)>='0' && *(line+line_offset)<='9')        //if it's a digit
			{
				*((*gtn).group+cur_field)*=10;              //update group int
				*((*gtn).group+cur_field)+=*(line+line_offset)-'0';
			}
			else
			{
				if(*(line+line_offset)!='|')                        //unexpected char
				{
				#ifdef LOG_ON
				newout("Digit de groupe invalide, ligne corrompue.\n");
				#endif
				free((*gtn).group);         //free mem
				(*gtn).group=NULL;          //set NULL value
				return -4;
				}
			}
		}while(*(line+line_offset)!='|');
	}

	if(*(field_length+10)==1)
	{
		free((*gtn).group);         //free mem
		(*gtn).group=NULL;          //set NULL value
	}
	else
	{
		if(*((*gtn).group-1+(*(field_length+10)))!=0)         //if last id isn't 0
		{
		#ifdef LOG_ON
		newout("Pas de groupe 0 terminal, ligne corrompue.\n");
		#endif
		free((*gtn).group);         //free mem
		(*gtn).group=NULL;          //set NULL value
		return -5;
		}
	}

	//set pointer of "\0" string to NULL;
	for(cur_field=0;cur_field<10;cur_field++)
	{
		if(*(field_length+cur_field)==0)
		{
		free(*(gtn_ptn+cur_field));         //free mem
		*(gtn_ptn+cur_field)=NULL;          //set NULL value
		}
	}

	//Transfert pointer to gtn struct
	(*gtn).str_nom=*(gtn_ptn+0);
	(*gtn).str_prenom=*(gtn_ptn+1);
	(*gtn).str_naiss=*(gtn_ptn+2);
	(*gtn).str_tel_fixe=*(gtn_ptn+3);
	(*gtn).str_tel_port=*(gtn_ptn+4);
	(*gtn).str_adresse=*(gtn_ptn+5);
	(*gtn).str_mail=*(gtn_ptn+6);
	(*gtn).pict=*(gtn_ptn+7);
	(*gtn).str_crea_date=*(gtn_ptn+8);
	(*gtn).str_mod_date=*(gtn_ptn+9);


	free(field_length);                     //free mem
	free(gtn_ptn);

	return 0;
}



//------------------------------------------------------------------------------
//  gtn_read_line function
//
//  parse birthday and return age by comparing to the day date
//
//  Input args : (1)
//      *birth        =>  Birthday string (char *)
//
//  Output arg : (1)
//      int error code
//          0   =   OK
//------------------------------------------------------------------------------
int gtn_get_age(char *birth)
{
	int year, month, day, age;
	struct med_tm t;
		
	//Parse birthday
	day = atoi(birth);
	month = atoi(birth+3);
	year = atoi(birth+6);

	//Get arch date
	time_get(&t);

	//Calc
	age = t.tm_year-year;
	if(month>t.tm_mon) age--;
	if(month==t.tm_mon)
	{
		if(day>t.tm_mday) age--;
	}

	return age;
}


