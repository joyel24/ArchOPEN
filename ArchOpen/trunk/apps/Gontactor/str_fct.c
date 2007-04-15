//  Gontactor
//   apps for MediOS project
//   Copyright (c) 2007 by Captain H@dock (tanguycano@hotmail.com)
//
// All files in this archive are subject to the GNU General Public License.
// See the file COPYING in the source tree root for full license agreement.
// This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
// KIND, either express of implied.

//------------------------------------------------------------------------------
//  File : str_fct.c                             Author : Captain H@dock
//  Last change : 02/02/2007                     Created : 02/02/2007
//
//  Contain functions to modify strings.
//------------------------------------------------------------------------------

#include "str_fct.h"

char * straddinnew( char * str_begin , char * str_end)
{
	char *tmp;

	if(str_begin==NULL && str_end==NULL)
	{
		tmp = (char *) malloc(sizeof(char));
		tmp = '\0';
	}

	tmp = (char *) malloc((strlen(str_begin)+strlen(str_end)+1)*sizeof(char));
	tmp = strcpy(tmp, str_begin);
	tmp = strcat(tmp, " ");
	tmp = strcat(tmp, str_end);

	return tmp;
}
