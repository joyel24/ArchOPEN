/*
*   include/gfx/screens.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SCREENS_H
#define __SCREENS_H

#define SCREEN_CONSOLE 0
#define SCREEN_SPLASH  1
#define SCREEN_GFX     2

#define NB_SCREENS 3

struct screen_data {
    void (*show)(void);
    void (*hide)(void);
    int (*palette)[];
};

typedef struct screen_data * SCREEN_DATA;

void screens_init(void);
void screens_add(struct screen_data * data,int screen_num);
void screens_show(int screen_num);
int screens_current(void);
int screens_main(void);
void screens_mainSet(int screen_num);
        
#endif

