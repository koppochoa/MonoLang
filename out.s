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
   movl %edi, -8(%rbp)
#END ARGUMENTS INITIALIZATION
   movl $42, %eax
#VAR DEF
   movl %eax, -16(%rbp)
   movl $10, %eax
#VAR DEF
   movl %eax, -20(%rbp)
#VAR USE
   movl -16(%rbp), %eax
#VAR USE
   movl -20(%rbp), %ebx
   addl %ebx, %eax
   movl %eax, %eax
   leave
   ret
