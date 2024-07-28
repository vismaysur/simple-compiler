global _main
section .text

_main:
  mov rax, 6
  push rax
  mov rax, 11
  push rax
  mov rax, 9
  push rax
  push QWORD [rsp + 8]
  mov rax, 0x2000001
  pop rdi
  syscall
  mov rax, 0x2000001
  mov rdi, 0
  syscall