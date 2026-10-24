#include "Function.h"

void init_function_table(Function_Table *ft)
{
    ft->count = 0;
}

void set_function(Function_Table *ft, Function *func)
{
    ft->symbol_table[ft->count] = func;
    ft->count++;
}

Function *lookupfunction(Function_Table *tm, char *name)
{
    for (int i = 0; i < MAX_FUNCTIONS; i++)
    {
        if (strcmp(name, tm->symbol_table[i]->name) == 0)
        {
            return tm->symbol_table[i];
        }
    }

    return NULL;
}