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
   subq $40, %rsp
   movq $2, %rax
   movq $3, %rbx
   addq %rbx, %rax
   movq $2, %rbx
   movq $1, %rcx
   addq %rcx, %rbx
   imulq %rax, %rbx
#VAR DEF
   movq %rbx, -24(%rbp)
   movq $1, %rax
#VAR USE
   movq -24(%rbp), %rbx
   addq %rbx, %rax
#VAR DEF
   movq %rax, -32(%rbp)
#VAR USE
   movq -32(%rbp), %rax
   movq %rax, %rax
   leave
   ret
