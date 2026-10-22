#pragma once

#include <stdbool.h>
#include <stdio.h>

#include "Variables.h"

typedef enum
{
    NUMBER,
    PLUS,
    MINUS,
    MULTIPLY,
    DIVIDE,
    INVALID,
    TOKEN_EOF,
    RPARENTHESIS,
    LPARENTHESIS,
    POWER,
    FLOAT,
    IDENTIFIER,
    ASSIGN,
    SEMICOLON,
    COMMA,
    COLON,

    FUNCTION_DECL,
    FUNCTION_NAME,

    LBRACE,
    RBRACE,

    COMPARE_EQ,
    GREATER_THAN,
    GREATER_EQUAL,
    SMALLER_THAN,
    SMALLER_EQUAL,

    IF_STATEMENT,
    WHILE_STATEMENT,

    VARTYPE,

    TYPE_INT8,
    TYPE_INT16,
    TYPE_INT32,
    TYPE_INT64,

    TYPE_FLOAT,
    TYPE_BOOL,
    TYPE_CHAR,
    TYPE_STRING,
    TYPE_VOID,

    BODY_START,
    BODY_END,
    RETURN_STATMENT,

    MAIN,

} TokenType;

typedef struct Function
{
    char *name;
} Function;

typedef struct
{
    TokenType tokentype;
    union
    {
        int value_int;

        union variable {
            VarSize type;   
        }variable;

        double value_float;
        char *identifier;
        struct Function function;
    };
} Token;

typedef struct
{
    int position;
    char *data;
} Lexer;

void print_token(Token token);
void set_lexer(Lexer *lexer, char *input);

void debug_lexer(Lexer* lexer);

bool is_digit(const char input);
Token get_next_token(Lexer *lexer);
