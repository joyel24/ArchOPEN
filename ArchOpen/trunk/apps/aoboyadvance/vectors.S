.section ".core"

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global vector_SWI
vector_SWI:
    stmfd sp!,{r4-r12,lr}

    mrs r5,spsr       @ if not user mode, branch to medios SWI handler
    and r5,r5,#0x1f
    cmp r5,#0x10
    beq aoba_SWI

    ldmfd sp!,{r4-r12,lr}
    b trap_swi

aoba_SWI:
    ldr r5,=registers
    stmia r5,{r0-r3}        @ store params

	mrs r0,spsr
	tst r0,#0x20              @ thumb?
    bne swi_is_thumb

    ldr r0,[lr,#-4]
    bic r0,r0,#0xff000000   @ get SWI #
    b swi_ist_end

swi_is_thumb:
    ldrh r0,[lr,#-2]
    bic r0,r0,#0xffffff00       @ get SWI #

swi_ist_end:
    cmp r0,#5
    beq swi_VBlankIntrWait

    ldr r1,=0xffff
    cmp r1,r0

    mrseq r0,spsr
    biceq r0,r0,#0x80
    msreq spsr,r0
    blne swiHandler

    ldmia r5,{r0-r3}        @ restore modified params

    ldmfd sp!,{r4-r12,lr}
    movs pc,lr

swi_VBlankIntrWait:
    ldr r0,=VBlank
    mov r1,#0
    str r1,[r0]

/*
    ldr r0,=0x30300
    mov r1,#'S'
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
*/

    ldmia r5,{r0-r3}

    mrs r4,spsr
    tst r4,#0x20
    addne lr,lr,#1
    ldr r4,=swi_saved_PC
    str lr,[r4]            @ store return address

    ldr r4,=swi_saved_CPSR
    mrs r5,spsr
    str r5,[r4]            @ store status flags

    ldmfd sp!,{r4-r12,lr}

    msr cpsr,#00
    stmfd sp!,{r0-r2,pc}
/*
    ldr r0,=0x4000006
    mov r1,#160

swi_VBlankLoop:
    ldrh r2,[r0]
    cmp r2,r1
    bne swi_VBlankLoop
*/

    ldr r0,=VBlank

swi_VBlankLoop:
    ldr r1,[r0]
    cmp r1,#0
    beq swi_VBlankLoop

    mov r1,#0
    str r1,[r0]

    ldr r0,=swi_saved_PC
    ldr r0,[r0]
    str r0,[sp,#0xc]      @ overwrite PC position in stack with return address

    ldr r0,=swi_saved_CPSR
    ldr r0,[r0]
    msr cpsr_f,r0          @ restore ststus flags

/*
    ldr r0,=0x30300
    mov r1,#'T'
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
*/

    ldmfd sp!,{r0-r2,pc}

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global vector_PrefetchAbort
vector_PrefetchAbort:
    ldr sp,=stack_Abort
    stmfd sp!,{r0-r12,lr}
    mov r0,lr
	mrc	p15, 0, r1, c6, c0, 0 @ get Fault Address Register
    bl prefetchAbortHandler
    ldmfd sp!,{r0-r12,lr}
    movs pc,lr

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global vector_DataAbort
vector_DataAbort:
    ldr sp,=registers
    stmia sp,{r0-r12,lr}

    ldr sp,=stack_Abort
	mrc	p15, 0, r1, c6, c0, 0 @ get Fault Address Register

	mrs r0,spsr
	tst r0,#0x20              @ thumb?
    bne da_is_thumb

    ldr r0,[lr,#-8]
/*
    ldr r3,=0xfff00000
    ldr r4,=da_arm_return
    str r4,[r3,#8]
    str r0,[r3]            @ prepare the execution of the aborted opcode @ 0xfff00000

    stmfd sp!,{r0-r2}      @ store state

    ldr r0,=registers
    ldmia r0,{r0-r4}
    msr cpsr,#0x9f         @ set everything as it must be seen by the GBA ROM

    ldr pc,=0xfff00000     @ let's go :)
da_arm_return:

    msr cpsr,#0x97         @ back in abort mode

    ldr lr,=registers
    stmia lr,{r0-r4}       @ store modified registers

    ldmfd sp!,{r0-r2}      @ restore state
*/
    bl dataAbortHandler

    ldr sp,=registers
    ldmia sp,{r0-r12,lr}

    subs pc,lr,#4

da_is_thumb:
    ldrh r0,[lr,#-8]
/*
    ldr r3,=0xfff00000
    strh r0,[r3,#0x10]     @ prepare the execution of the aborted opcode @ 0xfff00010

    stmfd sp!,{r0-r2}      @ store state

    ldr r0,=registers
    ldmia r0,{r0-r4}       @ set everything as it must be seen by the GBA ROM

    ldr r8,=0xfff00011     @ let's go :)
    blx r8

    ldr lr,=registers
    stmia lr,{r0-r4}       @ store modified registers

    ldmfd sp!,{r0-r2}      @ restore state
*/
    bl dataAbortHandler

    ldr sp,=registers
    ldmia sp,{r0-r12,lr}

    subs pc,lr,#6

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global vector_IRQ
vector_IRQ:
    ldr sp,=0x30508
    ldrh sp,[sp]
    tst sp,#0x2            @ TMR1 irq?
    bne trap_irq

    ldr sp,=stack_IRQ
    stmfd sp!,{r0-r12,lr}

    msr cpsr,#0x9f
    mov r0,sp
    mov r1,lr
    msr cpsr,#0x92
    stmfd sp!,{r0-r1}      @ save true lr & sp

    ldr r0,=0x30508
    mov r1,#0x2
    strh r1,[r0]           @ ack irq

    mov r0,lr
    bl lineTimerInterrupt

    ldmfd sp!,{r0-r1}
    msr cpsr,#0x9f
    mov sp,r0
    mov lr,r1
    msr cpsr,#0x92         @ restore true lr & sp

    ldr r2,=GBAInterrupt
    ldr r3,[r2]
    cmp r3,#0
    bne vi_do_GBA_interrupt

    ldmfd sp!,{r0-r12,lr}
    subs pc,lr,#4

/*
vi_do_GBA_interrupt:
    mov r3,#0
    str r3,[r2]            @ set interrupt flag as serviced

    ldmfd sp!,{r0-r12,lr}

    stmfd sp!,{r0-r3,r12,lr}

    ldr r0,=0x3007ffc
    ldr r0,[r0]            @ run interrupt code
    blx r0

    ldmfd sp!,{r0-r3,r12,lr}

    subs pc,lr,#4
*/

vi_do_GBA_interrupt:
    mov r3,#0
    str r3,[r2]            @ set interrupt flag as serviced

    ldmfd sp!,{r0-r12,lr}

    mrs sp,spsr
    sub lr,lr,#4
    tst sp,#0x20
    addne lr,lr,#1
    ldr sp,=vi_saved_PC
    str lr,[sp]            @ store return address

    ldr sp,=vi_saved_CPSR
    mrs lr,spsr
    str lr,[sp]            @ store status flags

    msr cpsr,#0x90
@    msr cpsr,#0x10

    stmfd sp!,{r0-r3,r12,lr,pc}

/*
    ldr r0,=0x30300
    mov r1,#'I'
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
*/

    ldr r0,=0x3007ffc
    ldr r0,[r0]            @ run interrupt code
    blx r0

/*
    ldr r0,=0x30300
    mov r1,#'J'
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
    strh r1,[r0]
*/

    ldr r0,=vi_saved_PC
    ldr r0,[r0]
    str r0,[sp,#0x18]      @ overwrite PC position in stack with return address

    ldr r0,=vi_saved_CPSR
    ldr r0,[r0]
    msr cpsr_f,r0          @ restore ststus flags

    swi #0xffff            @ will reenable interrupts

    ldmfd sp!,{r0-r3,r12,lr,pc}

@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@
@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@@

.global registers
registers: .space 16*4

.space 508
stack_Abort:
.space 4

.space 508
.global stack_IRQ
stack_IRQ:
.space 4

vi_saved_PC:
.space 4
vi_saved_CPSR:
.space 4
swi_saved_PC:
.space 4
swi_saved_CPSR:
.space 4
