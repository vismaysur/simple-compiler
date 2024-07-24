global _main
section .text

_main:
    mov rax, 0x2000001
    mov rdi, 69
    syscall