global _main
section .text

_main:
    mov rax, 0x2000001  ; macOS exit system call
    mov rdi, 69         ; exit status 69
    syscall