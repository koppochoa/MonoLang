#include "std.h"

void _sysexit(FILE *out)
{
    struct Operand rdi = { .type = OPERAND_REGISTER,
                           .value = { .reg = { .reg = RDI, .size = INT64 } } };
    struct Operand rax = { .type = OPERAND_REGISTER,
                           .value = { .reg = { .reg = RAX, .size = INT64 } } };

    emit_2(out, MOV, INT64, rax, rdi);
    emit_2(out, MOV, INT64, IMMEDIATE(60), rax);
    emit_0(out, SYSCALL, INT64);
}