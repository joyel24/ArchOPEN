/*
*   kernel/lang.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __LANG_H
#define __LANG_H

#define LNG_HEADER        "LNG"
#define LNG_VERSION       2
#define LNG_LAST_ENTRY    0xFFFF

#ifndef MK_LNG_C

#include <sys_def/lang.h>
#include <sys_def/errors.h>

/* This macro is to be called to get a string */
extern char * langString[];
extern int lng_langNum;

/* This macro is to be called to get a string */
#define getLangStr(ID) langString[ID]

MED_RET_T lang_loadLng(char * ptr,int size,int * num);
void lang_init(void);
MED_RET_T lang_loadFile(char * fName);
void lang_loadDefault(void);

char * lang_getStr(int id);
#endif
#endif
