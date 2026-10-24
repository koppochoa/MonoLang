#pragma once

#include <stdbool.h>

#include "Variables.h"

typedef enum
{
    RAX,
    RBX,
    RCX,
    RDX,
    RSI,
    RDI,
    RBP,
    RSP,
    R8,
    R9,
    R10,
    R11,
    R12,
    R13,
    R14,
    R15,
    REG_ERR
} RegisterId;

typedef struct
{
    RegisterId reg;
    VarSize size;
} Register;

static const char *register_names[4][16] = {
    // 8 bits
    { "%al", "%bl", "%cl", "%dl", "%sil", "%dil", "%bpl", "%spl", "%r8b",
      "%r9b", "%r10b", "%r11b", "%r12b", "%r13b", "%r14b", "%r15b" },

    // 16 bits
    { "%ax", "%bx", "%cx", "%dx", "%si", "%di", "%bp", "%sp", "%r8w", "%r9w",
      "%r10w", "%r11w", "%r12w", "%r13w", "%r14w", "%r15w" },

    // 32 bits
    { "%eax", "%ebx", "%ecx", "%edx", "%esi", "%edi", "%ebp", "%esp", "%r8d",
      "%r9d", "%r10d", "%r11d", "%r12d", "%r13d", "%r14d", "%r15d" },

    // 64 bits
    { "%rax", "%rbx", "%rcx", "%rdx", "%rsi", "%rdi", "%rbp", "%rsp", "%r8",
      "%r9", "%r10", "%r11", "%r12", "%r13", "%r14", "%r15" }
};

static const RegisterId allocatable_regs[] = { RBX, RCX, RDX, RSI, RDI, R8, R9,
                                               R10, R11, R12, R13, R14, R15 };
static const RegisterId arguments_regs[] = { RDI, RSI, RDX, RCX, R8, R9 };

#define ALLOCATABLE_REG_COUNT                                                  \
    (sizeof(allocatable_regs) / sizeof(allocatable_regs[0]))

#define ARGUMENTS_REG_COUNT (sizeof(arguments_regs) / sizeof(arguments_regs[0]))

typedef struct
{
    bool used[ALLOCATABLE_REG_COUNT];
} RegisterManager;

#define REG_RAX ((Register){ .reg = RAX, .size = INT64})
#define REG_EAX ((Register){ .reg = RAX, .size = INT32})
#define REG_EBX ((Register){ .reg = RBX, .size = INT32})
#define REG_ECX ((Register){ .reg = RCX, .size = INT32})
#define REG_EDX ((Register){ .reg = RDX, .size = INT32})

extern RegisterManager rm;

Register find_free_reg();
void use_reg(Register reg);
void free_reg(Register reg);

Register find_free_reg_from(bool *arr);
void use_reg_from(bool *arr, int i);
