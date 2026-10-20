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
//         if (strcmp(node->function.function_name, function_table[i].name) ==
//         0)
//         {
//             double result = function_table[i].func(args, count);
//             free(args);
//             return result;
//         }
//     }

//     printf("Erreur : fonction inconnue '%s'\n",
//     node->function.function_name); free(args); exit(1);
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
        emit_label(out, node->function.name->identifier);

        emit_1(out, "pushq", REG(RBP));
        emit_2(out, "movq", REG(RSP), REG(RBP));
        emit_2(out, "subq", IMMEDIATE(node->function.tm->offset), REG(RSP));

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

    break;

    case NODE_POW:
        break;

    case NODE_UNARY_PLUS: {
        generate_code(out, node->unary.operand, tm);
    }
    break;

    case NODE_UNARY_MINUS: {
        generate_code(out, node->unary.operand, tm);

        emit_1(out, "popq", REG(RAX));
        emit_1(out, "negq", REG(RAX));
        emit_1(out, "pushq", REG(RAX));

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
            // printf("var : [%s] doesnt exists for now, creating
            // variable...\n", node->var.op.left->identifier);

            // generate_code(out, node->var.op.right, tm);
            Register reg = generate_expr(out, node->var.op.right);

            char buff[32];
            snprintf(buff, sizeof(buff), "-%d(%%rbp)", var->offset);

            Register dreg = { .offset = var->offset, .reg = RBP };

            const struct Operand dest = { .type = OPERAND_REGISTER,
                                          .value.reg = dreg };

            fprintf(out, "#VAR DEF\n");
            // emit_1(out, "popq", dest);

            emit_2(out, "movq", REG(reg), dest);

            free_reg(reg);

            var->created = true;

            break;
        }

        break;
    }
    case NODE_RETURN_STATMENT: {
        Register reg = generate_expr(out, node->retst.expr);

        // emit_1(out, "popq", REG(RAX));
        emit_2(out, "movq", REG(reg), REG(RAX));

        emit_0(out, "leave");
        emit_0(out, "ret");

        break;
    }

    default:
        break;
    }
}

Register generate_expr(FILE *out, ASTNode *node)
{
    switch (node->type)
    {
    case NODE_NUMBER: {
        Register free_reg = find_free_reg();

        struct Operand op = { .type = OPERAND_REGISTER, .value = free_reg };
        emit_2(out, "movq", IMMEDIATE(node->value_int), op);

        use_reg(free_reg);

        return free_reg;
    }

    break;

    case NODE_ADD: {
        Register r1 = generate_expr(out, node->op.right);
        Register r2 = generate_expr(out, node->op.left);

        emit_2(out, "addq", REG(r2), REG(r1));

        free_reg(r2);

        return r1;
    }

    case NODE_SUB: {
        Register r1 = generate_expr(out, node->op.right);
        Register r2 = generate_expr(out, node->op.left);

        emit_2(out, "subq", REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_MUL: {
        Register r1 = generate_expr(out, node->op.right);
        Register r2 = generate_expr(out, node->op.left);

        emit_2(out, "imulq", REG(r1), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_DIV: {
        Register r1 = generate_expr(out, node->op.right);
        Register r2 = generate_expr(out, node->op.left);

        emit_2(out, "movq", REG(r1), REG(EAX));
        emit_2(out, "movq", IMMEDIATE(0), REG(EDX));
        emit_2(out, "movq", REG(r2), REG(EBX));

        emit_1(out, "divl", REG(EBX));

        emit_2(out, "movq", REG(EAX), REG(r2));

        free_reg(r1);

        return r2;
    }

    case NODE_VARIABLE_CALL: {

        // opti : Register should be able to be an offseted register so I can
        // return directly the register containing the direct value

        char buff[32];
        snprintf(buff, sizeof(buff), "-%d(%%rbp)", node->varcall.offset);

        Register reg = { .offset = node->varcall.offset, .reg = RBP };

        const struct Operand src = { .type = OPERAND_REGISTER,
                                     .value.reg = reg };
        fprintf(out, "#VAR USE\n");

        Register freereg = find_free_reg();

        emit_2(out, "movq", src, REG(freereg));

        return freereg;
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
    case NODE_RETURN_STATMENT: {
        printf("RETURN\n");
        break;
    }

    default:
        printf("UNKNOWN NODE TYPE\n");
        break;
    }
}
