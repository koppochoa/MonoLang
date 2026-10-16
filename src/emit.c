#include "emit.h"

void emit_operand(FILE *out, const struct Operand op)
{
    switch (op.type)
    {
    case OPERAND_IMMEDIATE:
        fprintf(out, "$%ld", op.value.immediate);
        break;
    case OPERAND_REGISTER:
        fprintf(out, "%s", op.value.reg);
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
void emit_2(FILE *out, const char *op, const struct Operand op1, const struct Operand op2)
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