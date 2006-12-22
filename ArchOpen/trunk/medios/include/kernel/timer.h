/*
*   include/kernel/timer.h
*
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/


#ifndef __TIMER_H
#define __TIMER_H

#include <kernel/io.h>
#include <sys_def/types.h>
#include <driver/timer.h>

struct tmr_s {
    unsigned long expires;
    unsigned long stdDelay;
    void (*action)(void);
    struct tmr_s * nxt;
    struct tmr_s * prev;
    int trigger;
    int freeRun;
    char * name;
};

extern void tmr_intAction(int irq,struct pt_regs * regs);

extern void tmr_setup(struct tmr_s * tmr_data,char * name);
extern void tmr_remove(struct tmr_s * tmr_data);
extern void tmr_start(struct tmr_s * tmr_data);
extern void tmr_stop(struct tmr_s * tmr_data);

void tmr_init(void);
void tmr_print(void);

unsigned int tmr_getTick(void);
unsigned int tmr_getMicroTick(void);

void arch_tmr_init(void);
unsigned int arch_tmr_getMicroTick(void);

#endif
