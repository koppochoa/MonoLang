#pragma once

#include <stdio.h>

typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    // OPERAND_MEMORY
} OperandType;

struct Operand
{
    OperandType type;
    union
    {
        const char *reg;
        long immediate;
    } value;
};

#define REG_RBX ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rbx" })
#define REG_RAX ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rax" })

#define IMMEDIATE(a) ((struct Operand){ .type = OPERAND_IMMEDIATE, .value.immediate = a })

void emit_operand(FILE *out, const struct Operand op);
void emit_0(FILE *out, const char *op);
void emit_1(FILE *out, const char *op, const struct Operand op1);
void emit_2(FILE *out, const char *op, const struct Operand op1, const struct Operand op2);