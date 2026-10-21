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
   movq $42, %rax
#VAR DEF
   movq %rax, -8(%rbp)
   
   movq $10, %rax
#VAR DEF
   movq %rax, -16(%rbp)
