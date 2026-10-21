#include "std.h"

void _sysexit(FILE *out)
{
    struct Operand rdi = { .type = OPERAND_REGISTER,
                           .value = { .reg = { .offset = 0, .reg = RDI } } };
    struct Operand rax = { .type = OPERAND_REGISTER,
                           .value = { .reg = { .offset = 0, .reg = RAX } } };

    emit_2(out, "movq", rax, rdi);
    emit_2(out, "movq", IMMEDIATE(60), rax);
    emit_0(out, "syscall");
}