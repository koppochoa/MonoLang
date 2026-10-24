#pragma once

#include <string.h>

#include "Variables.h"
#include "register.h"

#define MAX_FUNCTIONS 100

typedef struct
{
    char *name;
    struct ASTList *body;
    struct ASTList *args;
    int args_size;
    Table_Manager *tm;
    bool used[ARGUMENTS_REG_COUNT];
    VarSize rettype;
} Function;

typedef struct
{
    Function *symbol_table[MAX_FUNCTIONS];
    int count;
} Function_Table;

void init_function_table(Function_Table *ft);

void set_function(Function_Table *tm, Function *func);
Function *lookupfunction(Function_Table *tm, char* name);