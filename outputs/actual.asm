global _main
section .text

_main:
  mov rax, 5
  push rax
  mov rax, 3
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  mov rax, 1
  push rax
  push QWORD [rsp + 8]
  mov rax, 1
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  pop rbx
  pop rax
  add rax, rbx
  push rax
  push QWORD [rsp + 8]
  push QWORD [rsp + 8]
  pop rbx
  pop rax
  add rax, rbx
  push rax
  mov rax, 0x2000001
  pop rdi
  syscall
  mov rax, 0x2000001
  mov rdi, 0
  syscall