.section .boot
.global _start
.global KernelStackTop
.global KernelStackBottom

_start:
    cli
    movabs $KernelStackTop, %rsp
    sub $8, %rsp
    cld
    call main
hang_loop:
    cli
    hlt
    jmp hang_loop

.section .bss
.align 16
KernelStackBottom:
    .skip 4096*4
KernelStackTop: