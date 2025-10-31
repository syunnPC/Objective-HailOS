.intel_syntax noprefix
.global ArchSwitchContext
.type ArchSwitchContext, @function
ArchSwitchContext:
    push rbp
    push rbx
    push r12
    push r13
    push r14
    push r15
    mov QWORD PTR [rdi], rsp
    mov rsp, rsi
    pop r15
    pop r14
    pop r13
    pop r12
    pop rbx
    pop rbp
    ret
.size ArchSwitchContext, .-ArchSwitchContext