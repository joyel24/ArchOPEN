/*
*   include/kernel/uart.h
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

/* cmd list */

void do_help(unsigned char ** params);
void do_mem (unsigned char ** params);
void do_run (unsigned char ** params);
void do_tasks (unsigned char ** params);
void do_halt (unsigned char ** params);
void do_reload (unsigned char ** params);
void print_handler_info (unsigned char ** params);
void do_memory_dump (unsigned char ** params);
void do_reg_print (unsigned char ** params);
void do_in (unsigned char ** params);
void do_out (unsigned char ** params);
void do_ps (unsigned char ** params);
void do_kill (unsigned char ** params);
void do_ThreadState (unsigned char ** params);
void do_ThreadInfo (unsigned char ** params);
void do_ThreadNice (unsigned char ** params);
void do_gioDir (unsigned char ** params);
void do_gioSetState (unsigned char ** params);
void do_gioGetState (unsigned char ** params);
void do_diskInfo (unsigned char ** params);
void do_cpldRead (unsigned char ** params);
void do_cpldWrite (unsigned char ** params);

void init_cmd_line(void);
void cmd_line_enable(void);
void cmd_line_disable(void);

void cmd_line_thread(void);
void process_cmd(void);

#endif
