/*
*   include/lib/gdb.h
*
*   MediOS project
*   Copyright (c) 2008 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __GDB_H
#define __GDB_H

#ifdef USE_GDB
    
#include <kernel/swi.h>

#define gdb_bkpt  swi_call_noRet(nGDB_CODE_BKPT)

#define MAX_BREAKPOINTS 32

//#define HAS_GDB_DEBUG
#ifdef HAS_GDB_DEBUG
#define DEBUG_GDB(s...)   printk(s)
#else
#define DEBUG_GDB(s...)
#endif

#define GDB_CODE_BKPT 1
#define GDB_STEP_BKPT 2

void gdb_processBkpt(int bkptType);

void gdb_outString(char *msg);
void gdb_doCmd(void);

int gdb_getPacket(void);
void gdb_putPacket(int check);

int gdb_hex(char ch);
void gdb_hexByte(char *s, int byte);
void gdb_hexWord(char *s, unsigned long val);
int gdb_getHexByte(char *s);
unsigned long gdb_getHexWord(char *s);
        
void gdb_replyOK(void);
void gdb_replyKO(int errNum);
        
void gdb_init(void);
#else

#define gdb_bkpt

#endif

#endif
