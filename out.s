.section .text
.globl _start

_start:
   call main
   movq %rax, %rdi
   movq $60, %rax
   syscall
.globl main
.type main, @function
main:
   pushq %rbp
   movq %rsp, %rbp
   subq $32, %rsp
   pushq $0
#VAR DEF
   popq -8(%rbp)
   pushq $0
#VAR DEF
   popq -16(%rbp)
   pushq $2
#VAR USE
   pushq -16(%rbp)
   popq %rbx
   popq %rax
   imulq %rbx, %rax
   pushq %rax
   pushq $1
   popq %rbx
   popq %rax
   add %rbx, %rax
   pushq %rax
#VAR DEF
   popq -24(%rbp)
#VAR USE
   pushq -24(%rbp)
   popq %rax
   leave
   ret
