.section .text
.globl _start

_start:
   call main
   movq %rax, %rdi
   movq $60, %rax
   syscall
.globl add
.type add, @function
add:
   pushq %rbp
   movq %rsp, %rbp
   subq $24, %rsp
#START ARGUMENTS INITIALIZATION
   movl %edi, -8(%rbp)
   movl %esi, -16(%rbp)
#END ARGUMENTS INITIALIZATION
#VAR USE
   movl -16(%rbp), %eax
#VAR USE
   movl -8(%rbp), %ebx
   addl %ebx, %eax
   movl %eax, %eax
   leave
   ret
.globl main
.type main, @function
main:
   pushq %rbp
   movq %rsp, %rbp
   subq $32, %rsp
#START ARGUMENTS INITIALIZATION
   movl %edi, -8(%rbp)
#END ARGUMENTS INITIALIZATION
#VAR DEF
   movl $52, %ebx
   movl %ebx, -16(%rbp)
#VAR DEF
   movl $10, %ebx
   movl %ebx, -20(%rbp)
#VAR DEF
#FUNC CALL
   movl $5, %ebx
   movl %ebx, %edi
   movl $10, %ecx
   movl %ecx, %esi
   call add
   movl %eax, -24(%rbp)
#VAR DEF
#FUNC CALL
#VAR USE
   movl -16(%rbp), %eax
   movl %eax, %edi
#VAR USE
   movl -20(%rbp), %edx
   movl %edx, %esi
   call add
   movl %eax, -28(%rbp)
#VAR USE
   movl -28(%rbp), %eax
#VAR USE
   movl -24(%rbp), %esi
   addl %esi, %eax
   movl %eax, %eax
   leave
   ret
