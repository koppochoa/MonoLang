#include "emit.h"

void emit_mem(FILE *out, Register reg, int offset)
{
    if (offset != 0)
    {
        if (offset < 0)
        {
            fprintf(out, "-%d(", -offset);
        }
        else
            fprintf(out, "%d(", offset);
    }
    reg.size = INT64;
    emit_register(out, reg, INT64);

    if (offset != 0)
    {
        fprintf(out, ")");
    }
}




void emit_register(FILE *out, const Register reg, VarSize size)
{
    fprintf(out, "%s", register_names[size][reg.reg]);
}

void emit_operand(FILE *out, const struct Operand op, VarSize size)
{
    switch (op.type)
    {
    case OPERAND_IMMEDIATE:
        fprintf(out, "$%ld", op.value.immediate);
        break;
    case OPERAND_REGISTER:
        emit_register(out, op.value.reg, size);
        break;
    case OPERAND_MEMORY:
        emit_mem(out, op.value.memory.reg, op.value.memory.offset);
        break;
    case OPERAND_LABEL:
        // emit_label();
        break;
    default:
        break;
    }
}

void emit_0(FILE *out, Mnem mn, VarSize type)
{
    char *op = get_mnem(mn, type);
    fprintf(out, "   %s\n", op);

    if (op)
        free(op);
}
void emit_1(FILE *out, Mnem mn, VarSize type, const struct Operand op1)
{
    char *op = get_mnem(mn, type);
    fprintf(out, "   %s ", op);
    emit_operand(out, op1, type);
    fprintf(out, "\n");

    if (op)
        free(op);
}
void emit_2(FILE *out, Mnem mn, VarSize type, const struct Operand op1,
            const struct Operand op2)
{
    char *op = get_mnem(mn, type);

    fprintf(out, "   %s ", op);
    emit_operand(out, op1,type);
    fprintf(out, ", ");

    emit_operand(out, op2,type);
    fprintf(out, "\n");

    if (op)
        free(op);
}

void emit_label(FILE *out, const char *label)
{
    fprintf(out, ".globl %s\n", label);
    fprintf(out, ".type %s, @function\n", label);
    fprintf(out, "%s:\n", label);
}

void totype(char *buff, int i, VarSize type)
{
    switch (type)
    {
    case INT8:
        buff[i] = 'b';
        break;
    case INT16:
        buff[i] = 'w';
        break;
    case INT32:
        buff[i] = 'l';
        break;
    case INT64:
        buff[i] = 'q';
        break;
    }
}

char *mnemonic(const char *name, VarSize type)
{
    size_t len = strlen(name);
    char *res = malloc(len + 2); // suffixe + '\0'
    if (!res)
        return NULL;

    memcpy(res, name, len + 1);
    totype(res, len, type);

    res[len + 1] = '\0';

    return res;
}

char *mov(VarSize type)
{
    return mnemonic("mov", type);
}

char *add(VarSize type)
{
    return mnemonic("add", type);
}

char *sub(VarSize type)
{
    return mnemonic("sub", type);
}

char *neg(VarSize type)
{
    return mnemonic("neg", type);
}

char *mul(VarSize type)
{
    return mnemonic("imul", type);
}

char *divi(VarSize type)
{
    return mnemonic("div", type);
}

char *push(VarSize type)
{
    return mnemonic("push", type);
}

char *call()
{
    size_t len = strlen("call");
    char *res = malloc(len + 1); // suffixe + '\0'
    if (!res)
        return NULL;

    memcpy(res, "call", len);
    res[len] = '\0';

    return res;
}

char *leave()
{
    size_t len = strlen("leave");
    char *res = malloc(len + 1); // suffixe + '\0'
    if (!res)
        return NULL;

    memcpy(res, "leave", len);
    res[len] = '\0';

    return res;
}

char *ret()
{
    size_t len = strlen("ret");
    char *res = malloc(len + 1); // suffixe + '\0'
    if (!res)
        return NULL;

    memcpy(res, "ret", len);
    res[len] = '\0';

    return res;
}

char *syscall()
{
    size_t len = strlen("syscall");
    char *res = malloc(len + 1); // suffixe + '\0'
    if (!res)
        return NULL;

    memcpy(res, "syscall", len);
    res[len] = '\0';

    return res;
}

char *get_mnem(Mnem mn, VarSize type)
{
    switch (mn)
    {
    case SYSCALL:
        return syscall(type);
    case MOV:
        return mov(type);
    case ADD:
        return add(type);
    case SUB:
        return sub(type);
    case MUL:
        return mul(type);
    case DIV:
        return divi(type);
    case NEG:
        return neg(type);
    case PUSH:
        return push(type);
    case LEAVE:
        return leave(type);
    case RET:
        return ret(type);
    case CALL:
        return call(type);
    }
}