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
   subq $24, %rsp
#START ARGUMENTS INITIALIZATION
#END ARGUMENTS INITIALIZATION
   movq $2, %rax
   movq $3, %rbx
   addq %rbx, %rax
   movq $2, %rbx
   movq $1, %rcx
   addq %rcx, %rbx
   imulq %rax, %rbx
#VAR DEF
   movq %rbx, -8(%rbp)
#VAR USE
   movq -8(%rbp), %rax
   movq $1, %rbx
   negq %rbx
   addq %rbx, %rax
#VAR DEF
   movq %rax, -16(%rbp)
#VAR USE
   movq -16(%rbp), %rax
   movq %rax, %rax
   leave
   ret
