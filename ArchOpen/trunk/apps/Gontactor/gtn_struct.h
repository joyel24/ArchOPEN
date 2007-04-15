//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_struct.h                         Author : Captain H@dock
//  Last change : 05/12/2006                    Created : 29/11/2006
//
//  Define the Gontactor structure and functions to access and modify structure
//  data.
//------------------------------------------------------------------------------
#include <api.h>


#ifndef GTN_STRUCT
#define GTN_STRUCT

//------------------------------------------------------------------------------
//  Gontact structure                           size : unknown
//
//  Contain Gontact individuals data pointors.
//  Be carefull with dynamic memory allocated strings...
//
//  Datas :
//      str_nom         =>  Name (char *)
//      str_prenom      =>  Familly Name (char *)
//      str_naiss       =>  Birthday [dd/mm/yyyy] (char *)
//      str_tel_fixe    =>  Home phone number (char *)
//      str_tel_port    =>  Mobile number (char *)
//      str_adresse     =>  Home adress (char *)
//      str_mail        =>  E-mail adress (char *)
//      group           =>  Group ids of which this Gontact is member. (int *)
//                          Table end by 0 element (group all)
//      pict            =>  Path of Gontact picture (if NULL => no pict)
//      str_crea_date   =>  Creation date [dd/mm/yyyy] (char *)
//      str_mod_date    =>  Last modification date [dd/mm/yyyy] (char *)
//------------------------------------------------------------------------------
typedef struct Gontact Gontact;
struct Gontact
{
  char *str_nom;
  char *str_prenom;
  char *str_naiss;
  char *str_tel_fixe;
  char *str_tel_port;
  char *str_adresse;
  char *str_mail;
  int *group;
  char *pict;
  char *str_crea_date;
  char *str_mod_date;
};

//------------------------------------------------------------------------------
//  Groups                           size : unknow
//
//  Contain Groups name
//
//  Datas :
//	nb		=>	Number of groups (int)
//      name  	=>  table of names (char **)
//------------------------------------------------------------------------------
typedef struct Groups Groups;
struct Groups
{
	int nb;
	char **name;
};



//Prototypes
void gtn_init(Gontact *gtn);
int gtn_struct_rst(Gontact *gtn);
void gtn_print(Gontact *gtn);

void group_free(Groups *group);

int gtn_read_line(Gontact *gtn, char *line);
int gtn_get_age(char *birth);

#endif
