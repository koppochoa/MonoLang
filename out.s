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
   subq $80, %rsp
#START ARGUMENTS INITIALIZATION
   movq %rdi, -8(%rbp)
   movq %rsi, -16(%rbp)
   movq %rdx, -24(%rbp)
   movq %rcx, -32(%rbp)
   movq %r8, -40(%rbp)
   movq %r9, -48(%rbp)
   movq 16(%rbp), %rax
   movq %rax, -56(%rbp)
#END ARGUMENTS INITIALIZATION
   movq $2, %rax
   movq $3, %rbx
   addq %rbx, %rax
   movq $2, %rbx
   movq $1, %rcx
   addq %rcx, %rbx
   imulq %rax, %rbx
#VAR DEF
   movq %rbx, -64(%rbp)
   movq $1, %rax
#VAR USE
   movq -64(%rbp), %rbx
   addq %rbx, %rax
#VAR DEF
   movq %rax, -72(%rbp)
#VAR USE
   movq -72(%rbp), %rax
   movq %rax, %rax
   leave
   ret
