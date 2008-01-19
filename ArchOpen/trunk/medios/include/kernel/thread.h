/*
*   include/kernel/thread.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
* All files in this archive are subject to the GNU General Public License.
* See the file COPYING in the source tree root for full license agreement.
* This software is distributed on an "AS IS" basis, WITHOUT WARRANTY OF ANY
* KIND, either express of implied.
*/

#ifndef __THREAD_H
#define __THREAD_H

#include <sys_def/errors.h>

#define THREAD_NAME_SIZE  15
#define STACK_SIZE        0x4000
#define THREAD_INIT_CPSR  0x1F

#define THREAD_NO_FORCE  0
#define THREAD_FORCE     1

#define THREAD_STATE_ENABLE      0
#define THREAD_STATE_DISABLE     1
#define THREAD_BLOCKED_BY_ATA      2
#define THREAD_BLOCKED_BY_DMA      3
#define THREAD_BLOCKED_BY_PIPE     4

/*#define THREAD_DISABLE_STATE  0
#define THREAD_ENABLE_STATE   1*/

#define THREAD_USE_SYS_STACK   1
#define THREAD_USE_OTHER_STACK 0


#define FILE_RESSOURCE   0
#define DIR_RESSOURCE    1
#define MEM_RESSOURCE    2 /* need to be the last one !!! */
/* Number of managed ressource */
#define THREAD_NB_RES    3

/* prio level is measure in number of allowed idle Tick */
#define PRIO_HIGH  1
#define PRIO_MED   3
#define PRIO_LOW   5
/* to be set to max val (ie lowest prio) */
#define THREAD_MAX_PRIO 5

#define COMPUTE_SCORE(THREAD_PTR) (THREAD_PTR->idleCnt-THREAD_PTR->priority)

/* basic function accessed from thread */
#define THREAD_DISABLE() {if(threadCurrent) { __cli(); threadCurrent->state=THREAD_STATE_DISABLE; __sti(); }}
#define THREAD_ENABLE()  {if(threadCurrent) { __cli(); threadCurrent->state=THREAD_STATE_ENABLE; __sti(); }}
#define THREAD_PAUSE() {\
    if(threadCurrent) {\
        __cli(); \
        threadCurrent->state=THREAD_STATE_DISABLE; \
        __sti(); \
        yield(); \
    }\
}
#define THREAD_SELF() (threadCurrent)

typedef struct thread_list {
    struct thread_list * nxt;
    struct thread_list * prev;
    int pid;
} THREAD_LIST;

typedef struct thread_ressource {
    void(*clean_fct)(void*ptr);
    THREAD_LIST * head_list;
    int offset;
    void(*print_fct)(void*ptr);
} THREAD_RES;

extern THREAD_RES ini_ressources[THREAD_NB_RES];

typedef struct thread_info {
    unsigned long regs[17];
    unsigned int * stackBottom; /* NULL if stack is not malloc and not SYS*/
    unsigned int * codeMalloc; /* NULL if code is part of medios, !NULL for med*/
    unsigned int * saveStack;
    unsigned int stackSize;
    /* thread info */
    char name[THREAD_NAME_SIZE];
    int pid;
    int tickCnt;
    int idleCnt;
    int priority;

    /* flags */
    int state;
    int useSysStack;

    /* linkage */
    struct thread_info * nxt;
    struct thread_info * prev;

    struct vfs_node * path;
    
    /* ressource */
    THREAD_RES ressources[THREAD_NB_RES];

} THREAD_INFO;

extern THREAD_INFO * threadCurrent;
extern THREAD_INFO * idleThread;

#define THREAD_LINKS THREAD_LIST __thread_list;
#define THREAD_PTR_2_LIST(PTR)     (&PTR->__thread_list)

void thread_listIni(THREAD_LIST * ptr);
MED_RET_T thread_listRm(THREAD_LIST * ptr,int res_id,int force);
void thread_listAdd(THREAD_LIST * ptr,int res_id,int force);

#define THREAD_INIT_RES(TYPE,FCT,ID,PRINT) { \
    ini_ressources[ID].clean_fct=(void(*)(void*))FCT;  \
    ini_ressources[ID].head_list=NULL; \
    ini_ressources[ID].offset = (unsigned long)(&(((TYPE*)0)->__thread_list)); \
    ini_ressources[ID].print_fct=(void(*)(void*))PRINT; \
}

void thread_listFree(THREAD_INFO * thread,int res_id);

MED_RET_T thread_init(void(*)(void));
void thread_loadContext(void);

void thread_nxt(void);

MED_RET_T thread_enable(int pid);
MED_RET_T thread_disable(int pid);
MED_RET_T thread_nice(THREAD_INFO * ptr,int prio);

unsigned long yield(void);
unsigned long yieldTo(THREAD_INFO * nxtThread);

void thread_startMed(void * entry_fct,void * code_malloc,void * iram_top,int stack_type,char * name,
                       char * path,int argc,char ** argv);
int thread_startFct(THREAD_INFO ** ret_thread,void * entry_fct,char * name,
                    int enable,int prio,int stack_type);

int thread_create(THREAD_INFO ** ret_thread,void * entry_fct,void * exit_fct,
    void * code_malloc,void * stack_top,unsigned stack_size,int useSysStack,
    void * stack_bottom,int prio,char * name,struct vfs_node * path,
    unsigned long arg1,unsigned long arg2);
    
MED_RET_T thread_insert(THREAD_INFO * thread);
MED_RET_T thread_remove(THREAD_INFO * thread);
MED_RET_T thread_kill(int pid);
int thread_doKill(THREAD_INFO * thread);
THREAD_INFO * thread_findPid(int pid);
void thread_medExit(void);

void thread_listPrintAll(int res_id);
void thread_listPrintPtr(int res_id,THREAD_INFO * thread);
void thread_ps(void);
void thread_printInfo(THREAD_INFO * thread);

#define THREAD_SWAPB(PTR,VAL)    \
    ({                           \
        long tmp;                \
        asm volatile(            \
            "swpb %0, %1, [%2]\n"\
        : "=r" (tmp)             \
        : "r" (VAL), "r" (PTR)   \
        );                       \
        tmp;                     \
})

struct spinLock {
    int lock;
};

void spinLock_ini(struct spinLock * lock);
void spinLock_lock(struct spinLock * lock);
void spinLock_unlock(struct spinLock * lock);
int spinLock_isLocked(struct spinLock * lock);
int spinLock_testAndLock(struct spinLock * lock);

#endif
