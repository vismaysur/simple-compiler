global _main
section .text

_main:
  mov rax, 2
  push rax
  mov rax, 4
  push rax
  mov rax, 2
  push rax
  pop rbx
  pop rax
  sub rax, rbx
  push rax
  pop rbx
  pop rax
  mul rbx
  push rax
  mov rax, 2
  push rax
  pop rbx
  pop rax
  div rbx
  push rax
  mov rax, 1
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  push QWORD [rsp + 0]
  mov rax, 0x2000001
  pop rdi
  syscall
  mov rax, 0x2000001
  mov rdi, 0
  syscall