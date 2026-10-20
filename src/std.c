#include "std.h"

void _sysexit(FILE *out)
{
    emit_2(out, "movq", REG(RAX), REG(RDI));
    emit_2(out, "movq", IMMEDIATE(60), REG(RAX));
    emit_0(out, "syscall");
}