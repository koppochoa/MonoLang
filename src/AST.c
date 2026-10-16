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

FunctionEntry function_table[] = { { "log", builtin_log }, { "sin", builtin_sin },   { "cos", builtin_cos },
                                   { "tan", builtin_tan }, { "sqrt", builtin_sqrt }, { "max", builtin_max },
                                   { NULL, NULL } };

// double evaluate_function(ASTNode *node, Table_Manager *tm)
// {
//     // Compter le nombre d'arguments
//     int count = 0;
//     ASTList *current = node->function.args;
//     while (current)
//     {
//         count++;
//         current = current->next;
//     }

//     // Allouer un tableau de doubles
//     double *args = malloc(sizeof(double) * count);

//     // Évaluer les arguments
//     current = node->function.args;
//     for (int i = 0; i < count; ++i)
//     {
//         args[i] = evaluate(current->statement, tm);
//         current = current->next;
//     }

//     // Chercher la fonction dans le tableau
//     for (int i = 0; function_table[i].name != NULL; ++i)
//     {
//         if (strcmp(node->function.function_name, function_table[i].name) == 0)
//         {
//             double result = function_table[i].func(args, count);
//             free(args);
//             return result;
//         }
//     }

//     printf("Erreur : fonction inconnue '%s'\n", node->function.function_name);
//     free(args);
//     exit(1);
// }

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
        // return evaluate_function(node, tm);
        return 0;
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
        // set_variable(tm, node->op.left->identifier.ident, val);
        return val;
    }
    case NODE_IDENTIFIER:
        // return get_variable(tm, node->identifier.ident);
        return 0;
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

    case NODE_FUNCTION_DECL: {
        // Ecrire Label
        // (future) recuperer args
        // write...

        emit_label(out, node->function.name->identifier);

        emit_1(out, "pushq", REG_RBP);
        emit_2(out, "movq", REG_RSP, REG_RBP);
        emit_2(out, "subq", IMMEDIATE(node->function.tm->offset), REG_RSP);

        ASTList *current = node->function.body;
        while (current)
        {
            generate_code(out, current->statement, node->function.tm);

            current = current->next;
        }

        break;
    }

    case NODE_FLOAT: {
        break;
    }
    case NODE_NUMBER: {
        emit_1(out, "pushq", IMMEDIATE(node->value_int));

        // emit_1(out, "pushq", REG_RAX);

        break;
    }
    case NODE_ADD: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        emit_1(out, "popq", REG_RBX);
        emit_1(out, "popq", REG_RAX);
        emit_2(out, "add", REG_RBX, REG_RAX);
        emit_1(out, "pushq", REG_RAX);

        break;
    }
    case NODE_SUB: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        emit_1(out, "popq", REG_RBX);
        emit_1(out, "popq", REG_RAX);
        emit_2(out, "sub", REG_RBX, REG_RAX);
        emit_1(out, "pushq", REG_RAX);
    }
    break;
    case NODE_MUL: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        emit_1(out, "popq", REG_RBX);
        emit_1(out, "popq", REG_RAX);
        emit_2(out, "imulq", REG_RBX, REG_RAX);
        emit_1(out, "pushq", REG_RAX);
    }
    break;

    case NODE_DIV: {
        generate_code(out, node->op.left, tm);
        generate_code(out, node->op.right, tm);

        emit_1(out, "popq", REG_RBX); // Diviseur
        emit_1(out, "popq", REG_RAX); // Dividende
        emit_0(out, "cqo"); // À ajouter à ton API
        emit_1(out, "idivq", REG_RBX);
        emit_1(out, "pushq", REG_RAX);
    }
    break;

    case NODE_POW:
        break;

    case NODE_UNARY_PLUS: {
        generate_code(out, node->unary.operand, tm);
    }
    break;

    case NODE_UNARY_MINUS: {
        generate_code(out, node->unary.operand, tm);

        emit_1(out, "popq", REG_RAX);
        emit_1(out, "negq", REG_RAX);
        emit_1(out, "pushq", REG_RAX);

        break;
    }

    case NODE_VARIABLE: {
        if (!tm)
        {
            printf("No Table Manager\n");
            break;
        }

        // Check si la variable existe
        Variable *var = lookupvariable(tm, node->var.op.left->identifier);
        if (var == NULL)
        {
            fprintf(stderr, "Variable inconnue\n");
            return;
        }
        if (var->created == false)
        {
            // printf("var : [%s] doesnt exists for now, creating variable...\n", node->var.op.left->identifier);

            generate_code(out, node->var.op.right, tm); // pushed into stack

            char buff[32];
            snprintf(buff, sizeof(buff), "-%d(%%rbp)", var->offset);

            const struct Operand dest = { .type = OPERAND_REGISTER, .value.reg = buff };

            fprintf(out, "#VAR DEF\n");
            emit_1(out, "popq", dest);
            var->created = true;

            break;
        }

        break;
    }
    case NODE_VARIABLE_CALL: {
        // printf("using variable offset: [%d]\n", node->varcall.offset);

        char buff[32];
        snprintf(buff, sizeof(buff), "-%d(%%rbp)", node->varcall.offset);

        const struct Operand src = { .type = OPERAND_REGISTER, .value.reg = buff };
        fprintf(out, "#VAR USE\n");

        emit_1(out, "pushq", src);

        break;
    }
    case NODE_RETURN_STATMENT: {
        generate_code(out, node->retst.expr, tm);

        emit_1(out, "popq", REG_RAX);
        emit_0(out, "leave");
        emit_0(out, "ret");

        break;
    }

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
        printf("IDENTIFIER\n");
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
        printf("VARIABLE\n");
        print_ast(node->var.op.left, indent + 1); // variable
        print_ast(node->var.op.right, indent + 1); // expression
        break;
    case NODE_VARIABLE_CALL:
        printf("NODE_VARIABLE_CALL\n");
        break;

    case NODE_FUNCTION_CALL:
        printf("FUNCTION_CALL\n");
        {
            ASTList *current = node->function.body;
            while (current)
            {
                print_ast(current->statement, indent + 1);
                current = current->next;
            }
        }
        break;

    case NODE_FUNCTION_DECL: {
        printf("FUNCTION DECL\n");

        print_ast(node->function.name, indent + 1);

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
    case NODE_RETURN_STATMENT: 
    {
        printf("RETURN\n");
        break;
    }

    default:
        printf("UNKNOWN NODE TYPE\n");
        break;
    }
}
