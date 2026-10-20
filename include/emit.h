#pragma once

#include <stdio.h>
#include "register.h"
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
        const Register reg;
        long immediate;
    } value;
};

// #define REG_RBX ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rbx" })
// #define REG_RAX ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rax" })
// #define REG_RDI ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rdi" })
// #define REG_RBP ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rbp" })
// #define REG_RSP ((struct Operand){ .type = OPERAND_REGISTER, .value.reg = "%rsp" })

#define REG(a)((struct Operand){ .type = OPERAND_REGISTER, .value.reg = a })


#define IMMEDIATE(a)((struct Operand){ .type = OPERAND_IMMEDIATE, .value.immediate = a })


void emit_operand(FILE *out, const struct Operand op);
void emit_0(FILE *out, const char *op);
void emit_1(FILE *out, const char *op, const struct Operand op1);
void emit_2(FILE *out, const char *op, const struct Operand op1, const struct Operand op2);
void emit_label(FILE *out, const char *label);