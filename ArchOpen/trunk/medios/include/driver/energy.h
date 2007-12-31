/*
*   include/kernel/energy.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __ENERGY_H
#define __ENERGY_H

#include <sys_def/int_timer.h>

extern int timer_status[3][2];
extern int timer_delay[3][2];
extern char * timer_type[3];
extern char * power_type[2];

void set_timer_status(int timer_type, int power_mode, int status);
void set_timer_delay(int timer_type, int power_mode, int delay);
int  get_timer_status(int timer_type, int power_mode);
int  get_timer_delay(int timer_type, int power_mode);

void lcd_launchTimer(void);
void halt_launchTimer(void);
void halt_disableTimer(int disable);
void hd_launchTimer(void);

void hd_stopTimer(void);

void energy_chgMode(int power_mode);

void energy_lowPower(void);

void energy_loadPref(void);
void energy_init(void);

#endif
