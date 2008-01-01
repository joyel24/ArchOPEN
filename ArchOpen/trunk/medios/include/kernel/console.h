/*
*   include/kernel/console.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __CONSOLE_H_
#define __CONSOLE_H_

#include <lib/string.h>
#include <sys_def/ctype.h>
#include <kernel/kernel.h>
#include <driver/hardware.h>

#define CON_BUFFER_SIZE 65536

#define CON_FONT std4x6

#define CON_BGCOLOR COLOR_BLACK

#define CON_MARGIN_X 2
#define CON_MARGIN_Y 2

void con_clear();
void con_screenUpdate();

void con_flushToDisk(void);

int con_handleBtn(int btn);

void con_screenShow(void);
void con_screenHide(void);

bool con_screenIsVisible(void);
void con_screenScroll(int lines);
void con_write(char * text, char color);
void con_init(void);

#endif /*__CONSOLE_H_*/
