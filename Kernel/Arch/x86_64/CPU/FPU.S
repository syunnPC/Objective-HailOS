.intel_syntax noprefix
.text
.global FPUInitState
.global FPUSave
.global FPURestore

FPUInitState:
    fninit
    fxsave64 [rdi]
    ret

FPUSave:
    fxsave64 [rdi]
    ret

FPURestore:
    fxrstor64 [rdi]
    ret
    