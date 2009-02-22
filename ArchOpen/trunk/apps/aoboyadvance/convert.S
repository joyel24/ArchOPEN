.arm
.global convert_line

convert_line:
    stmfd sp!,{r0-r12,lr}

    ldr r12,=0xffff
    mov r3,#30
    cvt_loop:
/*
        ldmia r1!,{r4-r11}

        and r4,r4,r12
        and r5,r5,r12
        and r6,r6,r12
        and r7,r7,r12
        and r8,r8,r12
        and r9,r9,r12
        and r10,r10,r12
        and r11,r11,r12
*/
        ldrh r4,[r1],#4
        ldrh r5,[r1],#4
        ldrh r6,[r1],#4
        ldrh r7,[r1],#4
        ldrh r8,[r1],#4
        ldrh r9,[r1],#4
        ldrh r10,[r1],#4
        ldrh r11,[r1],#4

        ldr r4,[r2,r4,lsl #2]
        ldr r5,[r2,r5,lsl #2]
        ldr r6,[r2,r6,lsl #2]
        ldr r7,[r2,r7,lsl #2]
        ldr r8,[r2,r8,lsl #2]
        ldr r9,[r2,r9,lsl #2]
        ldr r10,[r2,r10,lsl #2]
        ldr r11,[r2,r11,lsl #2]

        stmia r0!,{r4-r11}

        subs r3,r3,#1
        bne cvt_loop

    ldmfd sp!,{r0-r12,pc}
