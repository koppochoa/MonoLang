#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "AST.h"
#include "utils.h"

ASTNode *parse_factor(Token *current_token, Lexer *lexer, Function_Table *ft,
                      Table_Manager *tm)
{
    if (current_token->tokentype == MINUS || current_token->tokentype == PLUS)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *operand = parse_factor(current_token, lexer, ft, tm);

        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = (op == MINUS) ? NODE_UNARY_MINUS : NODE_UNARY_PLUS;
        node->unary.operand = operand;
        return node;
    }
    else if (current_token->tokentype == NUMBER)
    {
        ASTNode *node = create_number_node(current_token->value_int);
        *current_token = get_next_token(lexer);

        return node;
    }
    else if (current_token->tokentype == FLOAT)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_FLOAT;
        node->value_float = current_token->value_float;
        *current_token = get_next_token(lexer);
        return node;
    }
    else if (current_token->tokentype == LPARENTHESIS)
    {
        *current_token = get_next_token(lexer);
        ASTNode *node = parse_expr(current_token, lexer, ft, tm);

        if (current_token->tokentype != RPARENTHESIS)
        {
            printf("Erreur : Parenthese fermante attendue\n");
            exit(1);
        }

        *current_token = get_next_token(lexer); // consomme ')'
        return node;
    }
    else if (current_token->tokentype == IDENTIFIER)
    {
        char *name = strdup(current_token->identifier);

        *current_token = get_next_token(lexer);
        Variable *var = lookupvariable(tm, name);
        if (var == NULL)
        {
            char *func = lookupfunction(ft, name);
            if (func == NULL)
            {
                printf("Erreur : variable non existante\n");
                exit(1);
            }


            ASTNode *func_node = malloc(sizeof(ASTNode));
            func_node->type = NODE_FUNCTION_CALL;
            func_node->function_call.name = name;
            func_node->function_call.args = NULL;

            ASTList **args_current = &(func_node->function_call.args);

            *current_token = get_next_token(lexer);

            print_token(*current_token);

            while (current_token->tokentype != RPARENTHESIS)
            {
                ASTNode *var = parse_expr(current_token, lexer, ft, tm);

                ASTList *new = malloc(sizeof(ASTList));
                new->statement = var;
                new->next = NULL;

                *args_current = new;
                args_current = &(new->next);

                if (current_token->tokentype == COMMA)
                {
                    *current_token = get_next_token(lexer);
                }
            }

            *current_token = get_next_token(lexer);

            print_ast(func_node, 0);

            return func_node;
        }
        else
        {
            ASTNode *res = create_varcall_node(var->offset, var->type);

            return res;
        }
    }
    else if (current_token->tokentype
             == VARTYPE) // schem <vartype> <identifier> <assign> <value>
    {
        VarSize type = current_token->variable.type;

        *current_token = get_next_token(lexer);

        if (current_token->tokentype == IDENTIFIER)
        {
            char *name = strdup(current_token->identifier);
            *current_token = get_next_token(lexer);

            if (current_token->tokentype == ASSIGN)
            {
                *current_token = get_next_token(lexer);
                ASTNode *expr = parse_expr(current_token, lexer, ft, tm);

                print_ast(expr, 0);

                ASTNode *assign = malloc(sizeof(ASTNode));
                assign->type = NODE_VARIABLE;

                assign->var.type = type;

                assign->var.offset = -tm->offset;
                assign->var.op.right = expr;

                assign->var.op.left = malloc(sizeof(ASTNode));
                assign->var.op.left->type = NODE_IDENTIFIER;
                assign->var.op.left->identifier = name;

                set_variable(tm, assign->op.left->identifier, -tm->offset,
                             type);

                return assign;
            }
        }

        // else err
    }
    else if (current_token->tokentype == RETURN_STATMENT)
    {
        *current_token = get_next_token(lexer);
        struct ASTNode *retval = parse_expr(current_token, lexer, ft, tm);

        struct ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_RETURN_STATMENT;
        node->retst.expr = retval;

        return node;
    }
    else
    {
        printf("Erreur : facteur inattendu : %d\n", current_token->tokentype);
        print_token(*current_token);
        exit(1);
    }
}

ASTNode *parse_power(Token *current_token, Lexer *lexer, Function_Table *ft,
                     Table_Manager *tm)
{
    ASTNode *left = parse_factor(current_token, lexer, ft, tm);

    while (current_token->tokentype == POWER)
    {
        *current_token = get_next_token(lexer);
        ASTNode *right =
            parse_power(current_token, lexer, ft,
                        tm); // récursif pour associativité à droite

        ASTNode *node = create_op_node(NODE_POW, left, right);

        return node;
    }

    return left;
}

ASTNode *parse_term(Token *current_token, Lexer *lexer, Function_Table *ft,
                    Table_Manager *tm)
{
    ASTNode *left = parse_power(current_token, lexer, ft, tm);

    while (current_token->tokentype == MULTIPLY
           || current_token->tokentype == DIVIDE)
    {
        TokenType op = current_token->tokentype;
        NodeType nt = (op == MULTIPLY) ? NODE_MUL : NODE_DIV;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_power(current_token, lexer, ft, tm);

        ASTNode *node = create_op_node(nt, left, right);

        left = node;
    }

    return left;
}

ASTNode *parse_expr(Token *current_token, Lexer *lexer, Function_Table *ft,
                    Table_Manager *tm)
{
    ASTNode *left = parse_term(current_token, lexer, ft, tm);

    while (current_token->tokentype == PLUS
           || current_token->tokentype == MINUS)
    {
        TokenType op = current_token->tokentype;
        NodeType nt = (op == PLUS) ? NODE_ADD : NODE_SUB;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_term(current_token, lexer, ft, tm);

        ASTNode *node = create_op_node(nt, left, right);

        left = node;
    }

    return left;
}

ASTNode *parse_argument(Token *current_token, Lexer *lexer, Table_Manager *tm)
{
    ASTNode *node = NULL;
    if (current_token->tokentype == VARTYPE)
    {
        VarSize vs = current_token->variable.type;
        *current_token = get_next_token(lexer);

        if (current_token->tokentype == IDENTIFIER)
        {
            ASTNode *assign = malloc(sizeof(ASTNode));
            assign->type = NODE_VARIABLE;

            assign->var.offset = -tm->offset;
            assign->var.op.left = malloc(sizeof(ASTNode));
            assign->var.op.left->type = NODE_IDENTIFIER;
            assign->var.op.left->identifier = current_token->identifier;
            assign->var.type = vs; // to change

            assign->var.op.right = NULL;

            set_variable(tm, assign->op.left->identifier, -tm->offset, vs);

            tm->offset += sizeof(long);

            node = assign;
        }
    }

    return node;
}

ASTNode *parse_function(Token *current_token, Lexer *lexer, Function_Table *ft)
{
    ASTNode *res = NULL;
    if (current_token->tokentype == FUNCTION_DECL)
    {
        // on sattend a avoir les tokens dans le bon ordre
        *current_token = get_next_token(lexer);

        char *fname = current_token->function.name;

        ASTNode *func = create_func_node(fname);

        ASTList **current = &(func->function.body);
        ASTList **args_current = &(func->function.args);

        *current_token = get_next_token(lexer);

        while (current_token->tokentype != RPARENTHESIS)
        {
            *current_token = get_next_token(lexer);

            ASTNode *var =
                parse_argument(current_token, lexer, func->function.tm);

            if (var)
            {
                ASTList *new = malloc(sizeof(ASTList));
                new->statement = var;
                new->next = NULL;

                *args_current = new;
                args_current = &(new->next);
            }
        }

        print_variables(func->function.tm);

        // void is default return type
        func->function.rettype = VOID;

        while (current_token->tokentype != LBRACE)
        {
            if (current_token->tokentype == COLON)
            {
                *current_token = get_next_token(lexer);

                if (current_token->tokentype == VARTYPE)
                {
                    print_vartype(current_token->variable.type, 0);
                    func->function.rettype = current_token->variable.type;
                }
            }

            *current_token = get_next_token(lexer);
        }

        if (current_token->tokentype != LBRACE)
        {
            // err no opening brace
        }

        *current_token = get_next_token(lexer);

        if (strcmp(func->function.name, "main") == 0)
        {
            has_main = true;
        }

        set_function(ft, fname);

        while (current_token->tokentype != RBRACE)
        {
            ASTNode *stmt =
                parse_expr(current_token, lexer, ft, func->function.tm);

            if (stmt->type == NODE_VARIABLE)
            {
                func->function.tm->offset += sizeofvartype(stmt->var.type);
            }

            if (stmt->type == NODE_RETURN_STATMENT)
            {
                stmt->retst.type = func->function.rettype;
            }

            ASTList *new = malloc(sizeof(ASTList));
            new->statement = stmt;
            new->next = NULL;

            *current = new;
            current = &(new->next);

            if (current_token->tokentype != SEMICOLON)
            {
                printf("Erreur : ';' attendu\n");
                exit(1);
            }

            *current_token = get_next_token(lexer);
        }

        res = func;
    }

    return res;
}

ASTNode *parse_program(Token *current_token, Lexer *lexer)
{
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->program.list = NULL;
    program->program.ft = malloc(sizeof(Function_Table));

    ASTList **current = &(program->program.list);

    while (current_token->tokentype != TOKEN_EOF)
    {
        ASTNode *stmt =
            parse_function(current_token, lexer, program->program.ft);

        ASTList *new_node = malloc(sizeof(ASTList));
        new_node->statement = stmt;
        new_node->next = NULL;

        *current = new_node;
        current = &(new_node->next);

        *current_token = get_next_token(lexer);
    }

    return program;
}
