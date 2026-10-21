#include "emit.h"

void emit_classic(FILE* out, const Register reg, VariableType type)
{
    
}

void emit_register(FILE *out, const Register reg)
{
    if (reg.offset != 0)
    {
        if (reg.offset < 0)
        {
            fprintf(out, "-%d(", -reg.offset);
        }
        else
            fprintf(out, "%d(", reg.offset);
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
    case RSI:
        fprintf(out, "%%rsi");
        break;
    case R8:
        fprintf(out, "%%r8");
        break;
    case R9:
        fprintf(out, "%%r9");
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

char *mov(VariableType type)
{
    char *res = malloc(sizeof(char) * 5); // XXX Y EOF
    if (!res)
        return NULL;

    switch (type)

    {
    case INT8:
        strcpy(res, "movb\0");
        break;
    case INT16:
        strcpy(res, "movw\0");

        break;

    case INT32:
        strcpy(res, "movl\0");

        break;

    case INT64:
        strcpy(res, "movq\0");
        break;
    }

    return res;
}

char* get_mnem(Mnem mn, VariableType type)
{
    switch (mn)
    {
    case MOV:
        return mov(type);
    }
}