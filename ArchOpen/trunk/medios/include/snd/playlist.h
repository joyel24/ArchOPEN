/*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __PLAYLIST_H
#define __PLAYLIST_H

#include <sys_def/types.h>

typedef struct PLAYLIST_ITEM_STRUCT PLAYLIST_ITEM;

struct PLAYLIST_ITEM_STRUCT{
    char * name;
    int startPos;
    int fileSize;
    int bufferedSize;

    PLAYLIST_ITEM * next;
    PLAYLIST_ITEM * prev;
};

extern PLAYLIST_ITEM * playlist_first;
extern PLAYLIST_ITEM * playlist_last;
extern int playlist_count;

PLAYLIST_ITEM * playlist_new();
bool playlist_remove(PLAYLIST_ITEM * item);
void playlist_clear();

bool playlist_addFile(char * name);
bool playlist_addFolder(char * name,bool recurse);

void playlist_printItems();

void playlist_init();

#endif
