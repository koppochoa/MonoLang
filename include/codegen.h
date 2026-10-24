#pragma once

#include "AST.h"

void generate_code(FILE *out, ASTNode *node, Function_Table *ft);
void generate_function(FILE *out, ASTNode *node, Function_Table *ft,
                       Table_Manager* tm);
Register generate_expr(FILE *out, ASTNode *node, Function_Table *ft,
                       VarSize type);
void generate_args(FILE *out, ASTNode *node, bool *used,
                   unsigned long int *varcount, int *varoffset);
void generate_params(FILE *out, ASTNode *node, bool *used, Function_Table* ft, VarSize type, int* param_idx);
