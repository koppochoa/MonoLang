#pragma once

#include <stdbool.h>

typedef enum
{
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,

    EAX,
    EBX,
    EDX,

    RBP,
    RSP,
    REG_ERR
} RegisterId;

typedef struct
{
    RegisterId reg;
    int offset;
} Register;

static const RegisterId allocatable_regs[] = { RBX, RCX, RDX, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, R15 };

#define ALLOCATABLE_REG_COUNT (sizeof(allocatable_regs) / sizeof(allocatable_regs[0]))

typedef struct
{
    bool used[ALLOCATABLE_REG_COUNT];
} RegisterManager;

extern RegisterManager rm;

Register find_free_reg();
void use_reg(Register reg);
void free_reg(Register reg);
