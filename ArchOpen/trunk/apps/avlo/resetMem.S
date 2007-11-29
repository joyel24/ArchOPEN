.section ".core"
.arm
.global resetMem
resetMem:
    MRS R2, CPSR
    BIC R2, R2, #0x1F
    ORR R2, R2, #0xD3
    MSR CPSR_cf, R2
    ldr r2,=0x30920
    strh r1,[r2]
    bkpt
    mov pc, r0
    
            