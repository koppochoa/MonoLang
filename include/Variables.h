#pragma once

#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef enum
{
    INT8,
    INT16,
    INT32,
    INT64,
    VOID,

} VarSize;

typedef struct
{
    char *name;
    int offset;
    bool created;

} Variable;

#define MAX_VARIABLES 100

typedef struct
{
    Variable symbol_table[MAX_VARIABLES];
    int variable_count;
    int offset;
} Table_Manager;

void init_table_manager(Table_Manager *tm);

void set_variable(Table_Manager *tm, const char *name, int offset);
Variable *lookupvariable(Table_Manager *tm, char *name);
void print_variables(Table_Manager *tm);
void print_vartype(VarSize type, int indent);
size_t sizeofvartype(VarSize type);
