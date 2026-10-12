#include "AST.h"

#include <math.h>

#include "Variables.h"

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

typedef double (*MathFunc)(double *args, int arg_count);

typedef struct
{
    const char *name;
    MathFunc func;
} FunctionEntry;

double builtin_log(double *args, int arg_count)
{
    if (arg_count != 1)
    {
        printf("Erreur : log attend 1 argument\n");
        exit(1);
    }
    return log(args[0]);
}

double builtin_cos(double *args, int arg_count)
{
    if (arg_count != 1)
    {
        printf("Erreur : log attend 1 argument\n");
        exit(1);
    }
    return cos(args[0]);
}

double builtin_sin(double *args, int arg_count)
{
    if (arg_count != 1)
    {
        printf("Erreur : log attend 1 argument\n");
        exit(1);
    }
    return sin(args[0]);
}

double builtin_tan(double *args, int arg_count)
{
    if (arg_count != 1)
    {
        printf("Erreur : log attend 1 argument\n");
        exit(1);
    }
    return tan(args[0]);
}

double builtin_sqrt(double *args, int arg_count)
{
    if (arg_count != 1)
    {
        printf("Erreur : log attend 1 argument\n");
        exit(1);
    }
    return sqrt(args[0]);
}

double builtin_max(double *args, int arg_count)
{
    if (arg_count != 2)
    {
        printf("Erreur : max attend 2 arguments\n");
        exit(1);
    }
    return (args[0] > args[1]) ? args[0] : args[1];
}

double my_max(double a, double b)
{
    if (a > b)
        return a;
    return b;
}

FunctionEntry function_table[] = {
    { "log", builtin_log }, { "sin", builtin_sin },   { "cos", builtin_cos },
    { "tan", builtin_tan }, { "sqrt", builtin_sqrt }, { "max", builtin_max },
    { NULL, NULL }
};

double evaluate_function(ASTNode *node, Table_Manager *tm)
{
    // Compter le nombre d'arguments
    int count = 0;
    ASTList *current = node->function.args;
    while (current)
    {
        count++;
        current = current->next;
    }

    // Allouer un tableau de doubles
    double *args = malloc(sizeof(double) * count);

    // Évaluer les arguments
    current = node->function.args;
    for (int i = 0; i < count; ++i)
    {
        args[i] = evaluate(current->statement, tm);
        current = current->next;
    }

    // Chercher la fonction dans le tableau
    for (int i = 0; function_table[i].name != NULL; ++i)
    {
        if (strcmp(node->function.function_name, function_table[i].name) == 0)
        {
            double result = function_table[i].func(args, count);
            free(args);
            return result;
        }
    }

    printf("Erreur : fonction inconnue '%s'\n", node->function.function_name);
    free(args);
    exit(1);
}

double evaluate(ASTNode *node, Table_Manager *tm)
{
    switch (node->type)
    {
    case NODE_PROGRAM: {
        double result = 0;
        ASTList *current = node->program.list;
        while (current)
        {
            result = evaluate(current->statement, tm);
            current = current->next;
        }

        return result;
    }

    case NODE_FUNCTION_CALL: {
        // if(strcmp(node->function.function_name, "log") == 0)
        // {
        //     return log(evaluate(node->function.args, tm));
        // }

        return evaluate_function(node, tm);
    }

    case NODE_FLOAT:
        return node->value_float;
    case NODE_NUMBER:
        return (double)node->value_int;
    case NODE_ADD:
        return evaluate(node->op.left, tm) + evaluate(node->op.right, tm);
    case NODE_SUB:
        return evaluate(node->op.left, tm) - evaluate(node->op.right, tm);
    case NODE_MUL:
        return evaluate(node->op.left, tm) * evaluate(node->op.right, tm);
    case NODE_DIV:
        return evaluate(node->op.left, tm) / evaluate(node->op.right, tm);
    case NODE_POW:
        return pow(evaluate(node->op.left, tm), evaluate(node->op.right, tm));
    case NODE_UNARY_PLUS:
        return evaluate(node->unary.operand, tm);
    case NODE_UNARY_MINUS:
        return -evaluate(node->unary.operand, tm);
    case NODE_ASSIGN: {
        double val = evaluate(node->op.right, tm);
        set_variable(tm, node->op.left->identifier, val);
        return val;
    }
    case NODE_IDENTIFIER:
        return get_variable(tm, node->identifier);
    default:
        break;
    }

    return 0;
}

void generate_code(FILE *out, ASTNode *node, Table_Manager *tm)
{
    switch (node->type)
    {
    case NODE_PROGRAM: {
        ASTList *current = node->program.list;
        while (current)
        {
            generate_code(out, current->statement, tm);
            current = current->next;
        }

        break;
    }

    case NODE_FUNCTION_CALL: {
        // if(strcmp(node->function.function_name, "log") == 0)
        // {
        //     return log(evaluate(node->function.args, tm));
        // }
        break;
    }

    case NODE_FLOAT: {
        break;
    }
    case NODE_NUMBER: {
        fprintf(out,
                "   # Pushing Number\n"
                "   movq $%d, %%rax\n"
                "   pushq %%rax\n\n",
                node->value_int);
        break;
    }
    case NODE_ADD: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        fprintf(out,
                "   # Sum\n"
                "   popq %%rbx\n"
                "   popq %%rax\n"
                "   add  %%rbx, %%rax\n"
                "   pushq %%rax\n\n");
        break;
    }
    case NODE_SUB: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        fprintf(out,
                "   # Sub\n"
                "   popq %%rbx\n"
                "   popq %%rax\n"
                "   sub  %%rbx, %%rax\n"
                "   pushq %%rax\n\n");
    }
    break;
    case NODE_MUL: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        fprintf(out,
                "   # Sub\n"
                "   popq %%rbx\n"
                "   popq %%rax\n"
                "   imulq  %%rbx, %%rax\n"
                "   pushq %%rax\n\n");
    }
    break;

    case NODE_DIV: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        fprintf(out,
                "   # Sub\n"
                "   popq %%rbx\n"
                "   popq %%rax\n"
                "   idivq  %%rbx, %%rax\n"
                "   pushq %%rax\n\n");
    }
    break;

    case NODE_POW:
        break;

    case NODE_UNARY_PLUS: {
        // fprintf(out,
        //         "   # Pushing Number\n"
        //         "   movq $%d, %%rax\n"
        //         "   pushq %%rax\n\n",
        //         node->unary.operand);
        generate_code(out, node->unary.operand, tm);
    }
    break;

    case NODE_UNARY_MINUS: {
        // fprintf(out,
        //         "   # Pushing Number\n"
        //         "   movq $%d, %%rax\n"
        //         "   negq %%rax\n"
        //         "   pushq %%rax\n\n",
        //         node->unary);
        generate_code(out, node->unary.operand, tm);
        fprintf(out,
                "   # Unary minus\n"
                "   popq %%rax\n"
                "   negq %%rax\n"
                "   pushq %%rax\n\n");
        break;
    }

    case NODE_ASSIGN: {
        break;
    }
    case NODE_IDENTIFIER:
        break;

    default:
        break;
    }
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
        printf("IDENTIFIER: %s\n", node->identifier);
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

    case NODE_FUNCTION_CALL:
        printf("FUNCTION_CALL: %s\n", node->function.function_name);
        {
            ASTList *current = node->function.args;
            while (current)
            {
                print_ast(current->statement, indent + 1);
                current = current->next;
            }
        }
        break;

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

    default:
        printf("UNKNOWN NODE TYPE\n");
        break;
    }
}
