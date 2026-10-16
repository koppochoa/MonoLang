#include "std.h"

void _sysexit(FILE *out)
{
    emit_2(out, "movq", REG_RAX, REG_RDI);
    EXIT(out);
}