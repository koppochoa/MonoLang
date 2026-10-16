#pragma once

#include <stdio.h>
#include "emit.h"

#define EXIT(out)({emit_2(out, "movq",IMMEDIATE(60), REG_RAX); emit_0(out, "syscall");})

void _sysexit(FILE* out);