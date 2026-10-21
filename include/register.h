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

#define REG_RAX ((Register){.offset = 0, .reg = RAX})

#define REG_EAX ((Register){.offset = 0, .reg = EAX})
#define REG_EBX ((Register){.offset = 0, .reg = EBX})
#define REG_ECX ((Register){.offset = 0, .reg = ECX})
#define REG_EDX ((Register){.offset = 0, .reg = EDX})






static const RegisterId allocatable_regs[] = { RBX, RCX, RDX, RSI, RDI, R8, R9, R10, R11, R12, R13, R14, R15 };
static const RegisterId arguments_regs[] = { RDI, RSI, RDX, RCX, R8, R9};


#define ALLOCATABLE_REG_COUNT (sizeof(allocatable_regs) / sizeof(allocatable_regs[0]))

#define ARGUMENTS_REG_COUNT (sizeof(arguments_regs) / sizeof(arguments_regs[0]))

typedef struct
{
    bool used[ALLOCATABLE_REG_COUNT];
} RegisterManager;

extern RegisterManager rm;

Register find_free_reg();
void use_reg(Register reg);
void free_reg(Register reg);

Register find_free_reg_from(bool* arr);
void use_reg_from(bool* arr, int i);
