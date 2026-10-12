#pragma once

#include <string.h>
#include <stdlib.h>
#include <stdio.h>

typedef struct 
{
    char* name;
    double value;
}Variable;

#define MAX_VARIABLES 100

typedef struct
{
    Variable symbol_table[MAX_VARIABLES];
    int variable_count;
}Table_Manager;

void init_table_manager(Table_Manager* tm);

void set_variable(Table_Manager* tm, const char* name, double value);
double get_variable(Table_Manager* tm, const char* name);
void print_variables(Table_Manager* tm);