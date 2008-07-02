/*
* kernel/gui/msgBox.h
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

#ifndef __MSGBOX_H
#define __MSGBOX_H

#include <gui_user/msgBox.h>

    void msgBox_init(void);
    int msgBox_show(unsigned char* caption, unsigned char* msg, int type, int icon,int evt_hanlder);
    void msgBox_info(unsigned char* msg);

#define MSG_BOX_MAX_LINE 10
    
struct msgBox_line {
    char * str;
    int width;   
};
     
#endif
