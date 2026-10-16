#include "Variables.h"

void init_table_manager(Table_Manager *tm)
{
    tm->variable_count = 0;
    tm->offset = 0;
}

void set_variable(Table_Manager *tm, const char *name, int offset)
{
    tm->symbol_table[tm->variable_count].name = strdup(name);
    tm->symbol_table[tm->variable_count].offset = offset;
    tm->symbol_table[tm->variable_count].created = false;
    tm->variable_count++;
}

// double get_variable(Table_Manager *tm, const char *name)
// {
//     for (int i = 0; i < tm->variable_count; i++)
//     {
//         if (strcmp(tm->symbol_table[i].name, name) == 0)
//         {
//             return tm->symbol_table[i].value;
//         }
//     }
//     printf("Erreur : variable '%s' non définie\n", name);
//     exit(1);
// }

Variable *lookupvariable(Table_Manager *tm, char *name)
{
    for (int i = 0; i < tm->variable_count; i++)
    {
        if (strcmp(tm->symbol_table[i].name, name) == 0)
            return tm->symbol_table + i;
    }

    return NULL;
}

void print_variables(Table_Manager *tm)
{
    printf("Variables:\n");
    for (int i = 0; i < tm->variable_count; i++)
    {
        printf("%s\n", tm->symbol_table[i].name);
    }
}
