.section .text
.globl _start

_start:
   # Pushing Number
   movq $2, %rax
   pushq %rax

   # Pushing Number
   movq $1, %rax
   pushq %rax

   # Sum
   popq %rbx
   popq %rax
   add  %rbx, %rax
   pushq %rax

   movq $60, %rax 
   popq %rdi
   syscall
