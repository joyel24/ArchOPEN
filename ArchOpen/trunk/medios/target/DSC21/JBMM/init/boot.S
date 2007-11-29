/*
*   kernel/target/arch/boot.S
*   Av1XX boot
*   MediOS project
*   Copyright (c) 2005 by Christophe THOMAS (oxygen77 at free.fr)
*
*/
.section ".text.init.arch"
.global entry_code
entry_code:
    /* no specific init for av1xx*/
    b kernel_entry
.ifdef TARGET_TYPE_STD
    .asciz "MEDIOS0"
    .word 0
.endif
.ifdef TARGET_TYPE_AVLO
    .space 0x54, 0
    .asciz "AJBMVERSION2000"
    .space 0x20, 0
.endif