/*
*   include/kernel/swi.h
*
*   MediOS project
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __SWI_H
#define __SWI_H

struct user_fct_t {
    char * name;
    unsigned int ptr;
};

#define swi_call_noRet(SWI_NUM)                \
    ({                                   \
        asm volatile(                    \
            "swi %0\n"                   \
        :                                \
        : "i"(SWI_NUM)                   \
        : "memory");                     \
    })

#define swi_call(SWI_NUM)                \
    ({                                   \
        register long _r0 asm("r0");     \
        asm volatile(                    \
            "swi %0\n"                   \
        :                                \
        : "i"(SWI_NUM)                   \
        : "memory");                     \
        return (long) _r0;               \
    }) 

/* SWI num managed in trap.S */
#define nYIELD          0x0
#define nTHREAD_NXT     0x2
#define nYIELD_TO       0x3

#define nGDB_CODE_BKPT  0x5
#define nGDB_STEP_BKPT  0x6

/* SWI num managed in swi.c */
#define nAPI_INIAPI     0x1



    
#endif
