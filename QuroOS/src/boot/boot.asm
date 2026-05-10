; Multiboot header
section .multiboot
align 4
    dd 0x1BADB002              ; magic
    dd 0x03                    ; flags (align + meminfo)
    dd -(0x1BADB002 + 0x03)   ; checksum

section .text
global _start
extern kernel_main

_start:
    mov esp, stack_top
    push eax                   ; multiboot magic
    push ebx                   ; multiboot info
    call kernel_main
    cli
    hlt
    jmp $

section .bss
align 16
stack_bottom:
    resb 16384                 ; 16KB stack
stack_top:
