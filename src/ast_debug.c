
#include "AST.h"

void print_ast(ASTNode *node, int indent)
{
    if (node == NULL)
        return;

    // Affiche l'indentation
    for (int i = 0; i < indent; ++i)
        printf("  ");

    switch (node->type)
    {
    case NODE_NUMBER:
        printf("NUMBER: %d\n", node->value_int);
        break;

    case NODE_FLOAT:
        printf("FLOAT: %f\n", node->value_float);
        break;

    case NODE_IDENTIFIER:
        printf("IDENTIFIER : %s\n", node->identifier);
        break;

    case NODE_ADD:
        printf("ADD\n");
        print_ast(node->op.left, indent + 1);
        print_ast(node->op.right, indent + 1);
        break;

    case NODE_SUB:
        printf("SUB\n");
        print_ast(node->op.left, indent + 1);
        print_ast(node->op.right, indent + 1);
        break;

    case NODE_MUL:
        printf("MUL\n");
        print_ast(node->op.left, indent + 1);
        print_ast(node->op.right, indent + 1);
        break;

    case NODE_DIV:
        printf("DIV\n");
        print_ast(node->op.left, indent + 1);
        print_ast(node->op.right, indent + 1);
        break;

    case NODE_POW:
        printf("POW\n");
        print_ast(node->op.left, indent + 1);
        print_ast(node->op.right, indent + 1);
        break;

    case NODE_UNARY_MINUS:
        printf("UNARY_MINUS\n");
        print_ast(node->unary.operand, indent + 1);
        break;

    case NODE_UNARY_PLUS:
        printf("UNARY_PLUS\n");
        print_ast(node->unary.operand, indent + 1);
        break;

    case NODE_ASSIGN:
        printf("ASSIGN\n");
        print_ast(node->op.left, indent + 1); // variable
        print_ast(node->op.right, indent + 1); // expression
        break;
    case NODE_VARIABLE:
        printf("VARIABLE : %d\n", node->var.offset);
        print_vartype(node->var.type, indent + 1);
        print_ast(node->var.op.left, indent + 1); // variable
        print_ast(node->var.op.right, indent + 1); // expression
        break;
    case NODE_VARIABLE_CALL:
        printf("NODE_VARIABLE_CALL\n");
        break;

    case NODE_FUNCTION_CALL:
        printf("FUNCTION_CALL(%s)\n", node->function_call.name);
        {
            ASTList *current = node->function_call.args;
            while (current)
            {
                print_ast(current->statement, indent + 1);
                current = current->next;
            }
        }
        break;

    case NODE_FUNCTION_DECL: {
        printf("FUNCTION DECL(%s)\n", node->function.name);
        print_vartype(node->function.rettype, indent + 1);

        ASTList *current = node->function.body;
        while (current)
        {
            print_ast(current->statement, indent + 1);
            current = current->next;
        }
        break;
    }

    case NODE_PROGRAM: {
        printf("PROGRAM\n");
        ASTList *current = node->program.list;
        while (current)
        {
            print_ast(current->statement, indent + 1);
            current = current->next;
        }
        break;
    }
    case NODE_RETURN_STATMENT: {
        printf("RETURN\n");
        print_vartype(node->retst.type, indent + 1);
        break;
    }


    default:
        printf("UNKNOWN NODE TYPE\n");
        break;
    }
}
