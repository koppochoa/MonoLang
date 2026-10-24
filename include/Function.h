#pragma once

#include <string.h>

#define MAX_FUNCTIONS 100

typedef struct
{
    char* symbol_table[MAX_FUNCTIONS];
    int count;
} Function_Table;

void init_function_table(Function_Table *ft);

void set_function(Function_Table *tm, char *name);
char *lookupfunction(Function_Table *tm, char *name);