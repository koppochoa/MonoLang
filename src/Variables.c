#include "Variables.h"

#include "utils.h"
void init_table_manager(Table_Manager *tm)
{
    tm->variable_count = 0;
    tm->offset = 0;
}

void set_variable(Table_Manager *tm, const char *name, int offset, VarSize type)
{
    tm->symbol_table[tm->variable_count].name = strdup(name);
    tm->symbol_table[tm->variable_count].offset = offset;
    tm->symbol_table[tm->variable_count].created = false;
    tm->symbol_table[tm->variable_count].type = type;

    tm->variable_count++;
}



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

size_t sizeofvartype(VarSize type)
{
    switch (type)
    {
    case INT8:
        return 1;
    case INT16:
        return 2;
    case INT32:
        return 4;
    case INT64:
        return 8;
    case VOID:
        return 0;
    default:
        return 0;
        break;
    }
}

void print_vartype(VarSize type, int indent)
{
    for (int i = 0; i < indent; ++i)
        printf("  ");
    printf("TYPE: ");
    switch (type)
    {
    case INT8:
        printf("INT8\n");
        break;
    case INT16:
        printf("INT16\n");
        break;
    case INT32:
        printf("INT32\n");
        break;
    case INT64:
        printf("INT64\n");
        break;
    case VOID:
        printf("VOID\n");
        break;
    default:
        break;
    }
}