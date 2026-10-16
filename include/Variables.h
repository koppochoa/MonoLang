#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <stdbool.h>

typedef struct 
{
    char* name;
    int offset;
    bool created;
}Variable;

#define MAX_VARIABLES 100

typedef struct
{
    Variable symbol_table[MAX_VARIABLES];
    int variable_count;
    int offset;
}Table_Manager;

void init_table_manager(Table_Manager* tm);

void set_variable(Table_Manager* tm, const char* name,int offset);
Variable* lookupvariable(Table_Manager* tm,char* name);
void print_variables(Table_Manager* tm);