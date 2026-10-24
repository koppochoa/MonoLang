#include "Function.h"

void init_function_table(Function_Table *ft)
{
    ft->count = 0;
}

void set_function(Function_Table *ft, char *name)
{
    ft->symbol_table[ft->count] = name;
    ft->count++;
}

char *lookupfunction(Function_Table *tm, char *name)
{
    for (int i = 0; i < MAX_FUNCTIONS; i++)
    {
        if (strcmp(name, tm->symbol_table[i]) == 0)
        {
            return name;
        }
    }

    return NULL;
}