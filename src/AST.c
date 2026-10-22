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
