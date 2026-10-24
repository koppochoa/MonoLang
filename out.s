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
   subq $28, %rsp
#START ARGUMENTS INITIALIZATION
   movl %edi, -8(%rbp)
#END ARGUMENTS INITIALIZATION
   movl $52, %ebx
#VAR DEF
   movl %ebx, -16(%rbp)
   movl $10, %ebx
#VAR DEF
   movl %ebx, -20(%rbp)
#FUNC CALL
   movl $10, %edi
   movl $1, %esi
   call add
#VAR DEF
   movl %eax, -24(%rbp)
#VAR USE
   movl -24(%rbp), %ebx
   movl %ebx, %eax
   leave
   ret
