#pragma once

#include "AST.h"

void generate_code(FILE *out, ASTNode *node, Table_Manager *tm);
Register generate_expr(FILE *out, ASTNode *node, VarSize type);
void generate_args(FILE *out, ASTNode *node, bool *used,
                   unsigned long int *varcount, int *varoffset);
