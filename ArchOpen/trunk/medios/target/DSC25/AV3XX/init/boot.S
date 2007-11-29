/*
*   kernel/target/arch/boot.S
*   Av3XX boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
b real_entry_code
.ifdef TARGET_TYPE_STD
    .asciz "MEDIOS0"
    .word 0
.endif
.ifdef TARGET_TYPE_AVLO
    .space 0x54, 0
    .asciz "CJBMVERSION2000"
    .space 0x20, 0
.endif
real_entry_code:
    /* av3 init */
    mov SP,#0x8000
    ldr r0,=0x30920
    ldrh r0,[r0]
    cmp r0,#8
    beq sdram_code_cpy
    bl  memCfg_code_cpy
    mov r0,#0x30
    mov r1,#0x92
    add r1,r1,r0,lsl #8
    mov r0,r1, lsl #4
    mov r1,#8
    strh r1,[r0]
    mov r1,#0x20
    strh r1,[r0,#0x22]
    mov r0,#9
    mov r0, r0, lsl #20
    mov pc, r0
memCfg_code_cpy:
    mov r2, pc
    mov r0, lr    
    sub r2,r2,r0
    sub SP,SP,r2
    mov r1, SP
memCfg_codeCpy_loop:
    ldr     r3, [r0],#4
    str     r3, [r1],#4
    subs    r2, r2, #4
    bne     memCfg_codeCpy_loop  
    mov pc,SP
sdram_code_cpy:
    mov r0,#9
    mov r0, r0, lsl #20
    ldr r1, __code_start_ptr
    cmp r0,r1
    beq kernel_entry
    ldr r2, __real_iram_start_ptr
    ldr r5, __iram_end_ptr
    add r2,r2,r5
    ldr r5, __iram_start_ptr
    sub r2,r2,r5
    ldr r5, __sdend_end_ptr
    add r2,r2,r5
    ldr r5, __sdend_start_ptr
    sub r2,r2,r5
            
    sub r2, r2, r1
mv_sdram_loop:        
    /* copy data (256b at a time = sdram line size) */
    ldmia r0!,{r5,r6,r7,r8,r9,r10,r11,r12}
    stmia r1!,{r5,r6,r7,r8,r9,r10,r11,r12}
    subs r2, r2, #32
    bhi mv_sdram_loop

    ldr pc,=kernel_entry        
            
__real_iram_start_ptr: .word _real_iram_start
__iram_start_ptr:      .word _iram_start
__iram_end_ptr:        .word _iram_end
__sdend_start_ptr:     .word _sdend_start
__sdend_end_ptr:       .word _sdend_end
__code_start_ptr:      .word _code_start
            
            