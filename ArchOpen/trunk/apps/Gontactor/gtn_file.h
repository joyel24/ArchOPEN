//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : gtn_file.h                           Author : Captain H@dock
//  Last change : 12/12/2006                    Created : 30/11/2006
//
//  Functions to save and read Gontact structure fields from and to *.gtn files
//  Function to read groups names
//------------------------------------------------------------------------------

#ifndef GTN_FILE_H
#define GTN_FILE_H

#include "gtn_struct.h"
#include "gtn_bdd.h"


//Structures


//Prototypes

int gtn_save(Gontact *gtn, char *gtn_filename);
int gtn_read(Gontact *gtn, char *gtn_filename);


int group_read(Groups *group, char *group_filename);


int gtn_bdd_read(Gtn_bdd *db, char *db_filename);

#endif

