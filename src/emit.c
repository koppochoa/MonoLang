#include "emit.h"

void emit_register(FILE *out, const Register reg)
{
    if (reg.offset != 0)
    {
        fprintf(out, "-%d(", reg.offset);
    }

    switch (reg.reg)
    {
    case RAX:
        fprintf(out, "%%rax");
        break;
    case RBX:
        fprintf(out, "%%rbx");
        break;
    case RCX:
        fprintf(out, "%%rcx");
        break;
    case RDX:
        fprintf(out, "%%rdx");
        break;
    case RBP:
        fprintf(out, "%%rbp");
        break;
    case RSP:
        fprintf(out, "%%rsp");
        break;
     case RDI:
        fprintf(out, "%%rdi");
        break;
    default:
        break;
    }

    if (reg.offset != 0)
    {
        fprintf(out, ")");
    }
}

void emit_operand(FILE *out, const struct Operand op)
{
    switch (op.type)
    {
    case OPERAND_IMMEDIATE:
        fprintf(out, "$%ld", op.value.immediate);
        break;
    case OPERAND_REGISTER:
        emit_register(out, op.value.reg);
        break;
    default:
        break;
    }
}

void emit_0(FILE *out, const char *op)
{
    fprintf(out, "   %s\n", op);
}
void emit_1(FILE *out, const char *op, const struct Operand op1)
{
    fprintf(out, "   %s ", op);
    emit_operand(out, op1);
    fprintf(out, "\n");
}
void emit_2(FILE *out, const char *op, const struct Operand op1,
            const struct Operand op2)
{
    fprintf(out, "   %s ", op);
    emit_operand(out, op1);
    fprintf(out, ", ");

    emit_operand(out, op2);
    fprintf(out, "\n");
}

void emit_label(FILE *out, const char *label)
{
    fprintf(out, ".globl %s\n", label);
    fprintf(out, ".type %s, @function\n", label);
    fprintf(out, "%s:\n", label);
}