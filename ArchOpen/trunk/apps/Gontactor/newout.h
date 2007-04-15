//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2006 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : newout.h                             Author : Captain H@dock
//  Last change : 05/12/2006                    Created : 29/11/2006
//
//  Utilise la sortie active et une sauvegarde texte.
//------------------------------------------------------------------------------
//Constants define
#include "constant.h"

#include <api.h>

#ifdef LOG_ON

#ifndef NEWOUT_H
#define NEWOUT_H

void newout(char *ligne);
void startout(void);
void endout(void);

#endif
#endif
