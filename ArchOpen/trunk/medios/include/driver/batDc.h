/*
*   include/kernel/batDc.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __BAT_DC_H
#define __BAT_DC_H

#include <target/chip/arch/batDc.h>

int DC_isConnected(void);
int batLevel(void);

extern int kpwrState;
void process_DC_change(void);

void DC_chkAction(void);

int  getPowerMode(void);

void chk_DC_connector(void);

void batDc_init(void);

#endif
