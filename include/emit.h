#pragma once

#include <stdio.h>

#include "Variables.h"
#include "register.h"
typedef enum
{
    OPERAND_IMMEDIATE,
    OPERAND_REGISTER,
    OPERAND_MEMORY
} OperandType;

struct Operand
{
    OperandType type;
    union
    {
        const Register reg;
        long immediate;
        struct
        {
            const Register reg;
            int offset;
        } memory;
    } value;
};

typedef enum
{
    MOV,
    ADD,
    SUB,
    MULT,
    NEG,
    MUL,
    DIV,
    LEAVE,
    RET,

    PUSH,
    SYSCALL
} Mnem;

#define REG(a)                                                                 \
    ((struct Operand){ .type = OPERAND_REGISTER, .value = { .reg = a } })

#define IMMEDIATE(a)                                                           \
    ((struct Operand){ .type = OPERAND_IMMEDIATE, .value = { .immediate = a } })


void emit_mem(FILE *out, Register reg, int offset);
void emit_register(FILE *out, const Register reg);
void emit_operand(FILE *out, const struct Operand op);
void emit_0(FILE *out, Mnem mn, VarSize type);
void emit_1(FILE *out, Mnem mn, VarSize type, const struct Operand op1);
void emit_2(FILE *out, Mnem mn, VarSize type, const struct Operand op1,
            const struct Operand op2);
void emit_label(FILE *out, const char *label);

char *get_mnem(Mnem mn, VarSize type);
char *mnemonic(const char *name, VarSize type);

char *syscall(VarSize type);
char *mov(VarSize type);
char *add(VarSize type);
char *sub(VarSize type);
char *mul(VarSize type);
char *divi(VarSize type);
char *neg(VarSize type);

char *push(VarSize type);

char *leave(VarSize type);
char *ret(VarSize type);
