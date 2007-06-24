/*
* include/kernel/gui/virtKbd.h
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

#ifndef __VIRTKBD_H
#define __VIRTKBD_H

extern int paramVirtKbd;

void virtKbd(int evt_hanlder,char * str);
int defaultVirtKbdLY(void);

#endif
