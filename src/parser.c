#include "parser.h"

#include <stdlib.h>
#include <string.h>

#include "AST.h"

ASTNode *parse_factor(Token *current_token, Lexer *lexer)
{
    if (current_token->tokentype == MINUS || current_token->tokentype == PLUS)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *operand = parse_factor(current_token, lexer);

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
        ASTNode *node = parse_expr(current_token, lexer);

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

        if (current_token->tokentype == ASSIGN)
        {
            *current_token = get_next_token(lexer);
            ASTNode *expr = parse_expr(current_token, lexer);

            ASTNode *assign = malloc(sizeof(ASTNode));
            assign->type = NODE_ASSIGN;
            assign->op.left = malloc(sizeof(ASTNode));
            assign->op.left->type = NODE_IDENTIFIER;
            assign->op.left->identifier = name;
            assign->op.right = expr;
            return assign;
        }
        else if (current_token->tokentype == LPARENTHESIS)
        {
            *current_token = get_next_token(lexer); // consomme '('
            ASTNode *func_call = malloc(sizeof(ASTNode));
            func_call->type = NODE_FUNCTION_CALL;
            func_call->function.function_name = name;
            func_call->function.args =
                parse_argument_list(current_token, lexer);

            if (current_token->tokentype != RPARENTHESIS)
            {
                printf("Erreur : ')' attendu après les arguments\n");
                exit(1);
            }
            *current_token = get_next_token(lexer); // consomme ')'
            return func_call;
        }
        else
        {
            ASTNode *var = malloc(sizeof(ASTNode));
            var->type = NODE_IDENTIFIER;
            var->identifier = name;
            return var;
        }
    }
    else
    {
        printf("Erreur : facteur inattendu\n");
        exit(1);
    }
}

ASTNode *parse_term(Token *current_token, Lexer *lexer)
{
    ASTNode *left = parse_power(current_token, lexer);

    while (current_token->tokentype == MULTIPLY
           || current_token->tokentype == DIVIDE)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_power(current_token, lexer);

        ASTNode *node = malloc(sizeof(ASTNode));

        node->type = (op == MULTIPLY) ? NODE_MUL : NODE_DIV;
        node->op.left = left;
        node->op.right = right;

        left = node;
    }

    return left;
}

ASTNode *parse_expr(Token *current_token, Lexer *lexer)
{
    ASTNode *left = parse_term(current_token, lexer);

    while (current_token->tokentype == PLUS
           || current_token->tokentype == MINUS)
    {
        TokenType op = current_token->tokentype;
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_term(current_token, lexer);

        ASTNode *node = malloc(sizeof(ASTNode));

        node->type = (op == PLUS) ? NODE_ADD : NODE_SUB;
        node->op.left = left;
        node->op.right = right;

        left = node;
    }

    return left;
}

ASTNode *parse_power(Token *current_token, Lexer *lexer)
{
    ASTNode *left = parse_factor(current_token, lexer);

    while (current_token->tokentype == POWER)
    {
        *current_token = get_next_token(lexer);
        ASTNode *right = parse_power(
            current_token, lexer); // récursif pour associativité à droite

        ASTNode *node = malloc(sizeof(ASTNode));
        node->type = NODE_POW;
        node->op.left = left;
        node->op.right = right;

        return node;
    }

    return left;
}

ASTList *parse_argument_list(Token *current_token, Lexer *lexer)
{
    ASTList *head = malloc(sizeof(ASTList));
    head->statement = parse_expr(current_token, lexer);
    head->next = NULL;

    ASTList *current = head;

    while (current_token->tokentype == COMMA)
    {
        *current_token = get_next_token(lexer); // consomme la virgule
        ASTList *next_arg = malloc(sizeof(ASTList));
        next_arg->statement = parse_expr(current_token, lexer);
        next_arg->next = NULL;
        current->next = next_arg;
        current = next_arg;
    }

    return head;
}

ASTNode *parse_program(Token *current_token, Lexer *lexer)
{
    ASTNode *program = malloc(sizeof(ASTNode));
    program->type = NODE_PROGRAM;
    program->program.list = NULL;

    ASTList **current = &(program->program.list);

    while (current_token->tokentype != TOKEN_EOF)
    {
        ASTNode *stmt = parse_expr(current_token, lexer);

        ASTList *new_node = malloc(sizeof(ASTList));
        new_node->statement = stmt;
        new_node->next = NULL;

        *current = new_node;
        current = &(new_node->next);

        if (current_token->tokentype != SEMICOLON)
        {
            printf("Erreur : ';' attendu\n");
            exit(1);
        }

        *current_token = get_next_token(lexer);

    }

    return program;
}
