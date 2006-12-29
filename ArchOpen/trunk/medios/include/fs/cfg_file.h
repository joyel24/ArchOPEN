/*
* include/kernel/cfg_file.h
*
* MediOS project
* Copyright (c) 2005 by Christophe THOMAS
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*
*/

#ifndef __CFG_FILE_H
#define __CFG_FILE_H

#include <sys_def/types.h>

typedef struct CFG_DATA_STRUCT CFG_DATA;

void cfg_clear(CFG_DATA * data);

CFG_DATA * cfg_newFile();
CFG_DATA * cfg_readFile(char * filename);
bool cfg_writeFile(CFG_DATA * data, char * filename);

void cfg_rewindItems(CFG_DATA * data);
bool cfg_nextItem(CFG_DATA * data, char * * name,char * * value);

bool cfg_itemExists(CFG_DATA * data, char * name);

char * cfg_readString(CFG_DATA * data, char * name);
int cfg_readInt(CFG_DATA * data, char * name);
bool cfg_readBool(CFG_DATA * data, char * name);

void cfg_writeString(CFG_DATA * data, char * name,char * value);
void cfg_writeInt(CFG_DATA * data, char * name,int value);
void cfg_writeBool(CFG_DATA * data, char * name,bool value);

void cfg_addDummyLine(CFG_DATA * data, char * text);

bool cfg_deleteItem(CFG_DATA * data, char * name);

// debug
void cfg_printItems(CFG_DATA * data);

#endif
