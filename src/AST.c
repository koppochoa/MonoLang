#include "AST.h"

#include <math.h>

#include "Variables.h"
#include "register.h"

ASTNode *create_number_node(int value)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_NUMBER;
    node->value_int = value;
    return node;
}

ASTNode *create_op_node(NodeType type, ASTNode *left, ASTNode *right)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = type;
    node->op.left = left;
    node->op.right = right;
    return node;
}

ASTNode *create_varcall_node(int offset, VarSize type)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_VARIABLE_CALL;
    node->varcall.offset = offset;
    node->varcall.type = type;

    return node;
}

ASTNode *create_func_node(char *fname)
{
    ASTNode *node = malloc(sizeof(ASTNode));
    node->type = NODE_FUNCTION_DECL;
    node->function.name = fname;
    node->function.body = NULL;
    node->function.args_size = 0;
    node->function.tm = malloc(sizeof(Table_Manager));

    if (!node->function.tm)
    {
        // er
    }

    node->function.tm->offset = sizeof(long);
    node->function.tm->variable_count = 0;
    for (long unsigned int i = 0; i < ARGUMENTS_REG_COUNT; i++)
    {
        node->function.used[i] = false;
    }

    return node;
}

void free_ast(ASTNode *node)
{
    if (node == NULL)
        return;

    if (node->type != NODE_NUMBER)
    {
        free_ast(node->op.left);
        free_ast(node->op.right);
    }

    free(node);
}
