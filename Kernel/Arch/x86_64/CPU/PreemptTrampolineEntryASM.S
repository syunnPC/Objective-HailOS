.intel_syntax noprefix
.text
.global PreemptTrampolineEntry
.global PreemptTrampolineBegin
.global PreemptTrampolineEnd

.extern SchedYieldTrampoline
.extern PreemptDisableForTrampoline
.extern PreemptEnableForTrampoline
.extern GetPreemptResumeRIP

PreemptTrampolineEntry:
PreemptTrampolineBegin:
    push rax
    push rcx
    push rdx
    push rsi
    push rdi
    push r8
    push r9
    push r10
    push r11
    push rbx
    push rbp
    push r12
    push r13
    push r14
    push r15

    mov  rbx, rsp 

    and  rsp, -16
    sub  rsp, 8 

    call PreemptDisableForTrampoline
    call SchedYieldTrampoline
    call PreemptEnableForTrampoline

    add  rsp, 8
    mov  rsp, rbx

    pop  r15
    pop  r14
    pop  r13
    pop  r12
    pop  rbp
    pop  rbx
    pop  r11
    pop  r10
    pop  r9
    pop  r8
    pop  rdi
    pop  rsi
    pop  rdx
    pop  rcx
    pop  rax

    call GetPreemptResumeRIP
    jmp  rax
PreemptTrampolineEnd: