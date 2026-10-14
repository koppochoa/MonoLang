.section .text
.globl _start

_start:
   movq $60, %rax 
   popq %rdi
   syscall
