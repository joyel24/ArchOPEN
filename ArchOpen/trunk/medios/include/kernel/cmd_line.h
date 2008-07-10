/*
*   include/kernel/cmd_line.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/


#ifndef __CMD_LINE_H
#define __CMD_LINE_H

struct cmd_line_s {
    char * cmd;
    char * help_str;
    void   (*cmd_action)(unsigned char ** params);
    int    nb_args;
};

/* cmd line internal functions */
void init_cmd_line(void);
void cmd_line_enable(void);
void cmd_line_disable(void);

void cmd_line_thread(void);
void process_cmd(void);

#endif
