#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "AST.h"
#include "utils.h"

ASTNode *parse_factor(Token *current_token, Lexer *lexer, Table_Manager *tm)
{
    if (current_token->tokentype == MINUS || current_token->tokentype == PLUS)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *operand = parse_factor(current_token, lexer, tm);

        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = (op == MINUS) ? NODE_UNARY_MINUS : NODE_UNARY_PLUS;
        node->unary.operand = operand;
        return node;
    }
    else if (current_token->tokentype == NUMBER)
    {
        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_NUMBER;
        node->value_int = current_token->value_int;
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
        ASTNode *node = parse_expr(current_token, lexer, tm);

        if (current_token->tokentype != RPARENTHESIS)
        {
            printf("Erreur : Parenthese fermante attendue\n");
            exit(1);
        }

        *current_token = get_next_token(lexer); // consomme ')'
        return node;
    }
    // else if (current_token->tokentype == IDENTIFIER)
    // {
    //     char *name = strdup(current_token->identifier);
    //     *current_token = get_next_token(lexer);

    //     if (current_token->tokentype == ASSIGN)
    //     {
    //         *current_token = get_next_token(lexer);
    //         ASTNode *expr = parse_expr(current_token, lexer, tm);

    //         ASTNode *assign = malloc(sizeof(ASTNode));
    //         assign->type = NODE_VARIABLE;
    //         assign->var.offset = -tm->offset;

    //         assign->var.op.left = malloc(sizeof(ASTNode));
    //         assign->var.op.left->type = NODE_IDENTIFIER;
    //         assign->var.op.left->identifier = name;

    //         assign->var.op.right = expr;

    //         set_variable(tm, assign->op.left->identifier, -tm->offset);

    //         return assign;
    //     }
    //     else if (current_token->tokentype == LPARENTHESIS)
    //     {
    //         *current_token = get_next_token(lexer); // consomme '('
    //         struct ASTNode *func_call = malloc(sizeof(struct ASTNode));
    //         func_call->type = NODE_FUNCTION_CALL;
    //         func_call->function.name->identifier = name;
    //         // func_call->function.args = parse_argument_list(current_token,
    //         // lexer);

    //         if (current_token->tokentype != RPARENTHESIS)
    //         {
    //             printf("Erreur : ')' attendu après les arguments\n");
    //             exit(1);
    //         }
    //         *current_token = get_next_token(lexer); // consomme ')'
    //         return func_call;
    //     }
    //     else
    //     {
    //         Variable *var = lookupvariable(tm, name);
    //         if (var == NULL)
    //         {
    //             printf("Erreur : variable non existante\n");
    //             exit(1);
    //         }
    //         else
    //         {
    //             ASTNode *res = malloc(sizeof(ASTNode));
    //             res->type = NODE_VARIABLE_CALL;
    //             res->varcall.offset = var->offset;
    //             return res;
    //         }
    //     }
    // }
    else if (current_token->tokentype == IDENTIFIER)
    {
        char *name = strdup(current_token->identifier);

        *current_token = get_next_token(lexer);
        Variable *var = lookupvariable(tm, name);
        if (var == NULL)
        {
            printf("Erreur : variable non existante\n");
            exit(1);
        }
        else
        {
            ASTNode *res = malloc(sizeof(ASTNode));
            res->type = NODE_VARIABLE_CALL;
            res->varcall.offset = var->offset;
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
                ASTNode *expr = parse_expr(current_token, lexer, tm);

                ASTNode *assign = malloc(sizeof(ASTNode));
                assign->type = NODE_VARIABLE;

                assign->var.type = type;

                assign->var.offset = -tm->offset;
                assign->var.op.right = expr;

                assign->var.op.left = malloc(sizeof(ASTNode));
                assign->var.op.left->type = NODE_IDENTIFIER;
                assign->var.op.left->identifier = name;

                set_variable(tm, assign->op.left->identifier, -tm->offset);

                return assign;
            }
        }

        // else err
    }
    else if (current_token->tokentype == RETURN_STATMENT)
    {
        *current_token = get_next_token(lexer);
        struct ASTNode *retval = parse_expr(current_token, lexer, tm);

        struct ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_RETURN_STATMENT;
        node->retst.expr = retval;

        return node;
    }
    else
    {
        printf("Erreur : facteur inattendu : %d\n", current_token->tokentype);
        exit(1);
    }
}

ASTNode *parse_term(Token *current_token, Lexer *lexer, Table_Manager *tm)
{
    ASTNode *left = parse_power(current_token, lexer, tm);

    while (current_token->tokentype == MULTIPLY
           || current_token->tokentype == DIVIDE)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_power(current_token, lexer, tm);

        ASTNode *node = malloc(sizeof(ASTNode));

        node->type = (op == MULTIPLY) ? NODE_MUL : NODE_DIV;
        node->op.left = left;
        node->op.right = right;

        left = node;
    }

    return left;
}

ASTNode *parse_expr(Token *current_token, Lexer *lexer, Table_Manager *tm)
{
    ASTNode *left = parse_term(current_token, lexer, tm);

    while (current_token->tokentype == PLUS
           || current_token->tokentype == MINUS)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_term(current_token, lexer, tm);

        ASTNode *node = malloc(sizeof(ASTNode));

        node->type = (op == PLUS) ? NODE_ADD : NODE_SUB;
        node->op.left = left;
        node->op.right = right;

        left = node;
    }

    return left;
}

ASTNode *parse_power(Token *current_token, Lexer *lexer, Table_Manager *tm)
{
    ASTNode *left = parse_factor(current_token, lexer, tm);

    while (current_token->tokentype == POWER)
    {
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_power(
            current_token, lexer, tm); // récursif pour associativité à droite

        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_POW;
        node->op.left = left;
        node->op.right = right;

        return node;
    }

    return left;
}

ASTList *parse_argument_list(Token *current_token, Lexer *lexer,
                             Table_Manager *tm)
{
    ASTList *head = malloc(sizeof(ASTList));
    head->statement = parse_expr(current_token, lexer, tm);
    head->next = NULL;

    ASTList *current = head;

    while (current_token->tokentype == COMMA)
    {
        *current_token = get_next_token(lexer); // consomme la virgule
        ASTList *next_arg = malloc(sizeof(ASTList));
        next_arg->statement = parse_expr(current_token, lexer, tm);
        next_arg->next = NULL;
        current->next = next_arg;
        current = next_arg;
    }

    return head;
}

ASTNode *parse_function(Token *current_token, Lexer *lexer)
{
    ASTNode *res = NULL;
    if (current_token->tokentype == FUNCTION_DECL)
    {
        ASTNode *func = malloc(sizeof(ASTNode));
        func->type = NODE_FUNCTION_DECL;

        func->function.body = NULL;
        func->function.tm = malloc(sizeof(Table_Manager));
        func->function.tm->offset = sizeof(long);
        func->function.tm->variable_count = 0;
        for (long unsigned int i = 0; i < ARGUMENTS_REG_COUNT; i++)
        {
            func->function.used[i] = false;
        }

        ASTList **current = &(func->function.body);
        ASTList **args_current = &(func->function.args);

        // on sattend a avoir les tokens dans le bon ordre
        *current_token = get_next_token(lexer);

        func->function.name = malloc(sizeof(struct ASTNode));
        func->function.name->type = NODE_IDENTIFIER;
        func->function.name->identifier = current_token->function.name;
        func->function.args_size = 0;

        *current_token = get_next_token(lexer);
        printf("tokens fun\n");
        while (current_token->tokentype != LPARENTHESIS)
        {
            print_token(*current_token);
            *current_token = get_next_token(lexer);

            if (current_token->tokentype == IDENTIFIER)
            {
                ASTNode *assign = malloc(sizeof(ASTNode));
                assign->type = NODE_VARIABLE;
                assign->var.offset = -func->function.tm->offset;

                assign->var.op.left = malloc(sizeof(ASTNode));
                assign->var.op.left->type = NODE_IDENTIFIER;
                assign->var.op.left->identifier = current_token->identifier;

                assign->var.op.right = NULL;

                ASTList *new = malloc(sizeof(ASTList));
                new->statement = assign;
                new->next = NULL;

                *args_current = new;
                args_current = &(new->next);

                set_variable(func->function.tm, assign->op.left->identifier,
                             func->function.tm->offset);

                func->function.tm->offset += sizeof(long);
            }
        }

        print_variables(func->function.tm);

        while (current_token->tokentype != LBRACE)
        {
            if(current_token->tokentype == COLON)
            {
                *current_token = get_next_token(lexer);

                if(current_token->tokentype == VARTYPE)
                {
                    func->function.type = current_token->variable.type;
                }
            }

            *current_token = get_next_token(lexer);
        }
        

        if (current_token->tokentype != LBRACE)
        {
            // err no opening brace
        }

        *current_token = get_next_token(lexer);

        if (strcmp(func->function.name->identifier, "main") == 0)
        {
            has_main = true;
        }

        while (current_token->tokentype != RBRACE)
        {
            ASTNode *stmt = parse_expr(current_token, lexer, func->function.tm);

            if (stmt->type == NODE_VARIABLE)
            {
                // printf("ident offset : %d\n", func->function.tm->offset);

                // func->function.tm->offset += sizeof(long);
                func->function.tm->offset += sizeofvartype(stmt->var.type);
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

        // print_variables(func->function.tm);

        res = func;
    }

    return res;
}

ASTNode *parse_program(Token *current_token, Lexer *lexer)
{
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->program.list = NULL;

    ASTList **current = &(program->program.list);

    while (current_token->tokentype != TOKEN_EOF)
    {
        // ASTNode *stmt = parse_expr(current_token, lexer);
        ASTNode *stmt = parse_function(current_token, lexer);

        ASTList *new_node = malloc(sizeof(ASTList));
        new_node->statement = stmt;
        new_node->next = NULL;

        *current = new_node;
        current = &(new_node->next);

        *current_token = get_next_token(lexer);
    }

    return program;
}
