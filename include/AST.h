#pragma once

#include <stdlib.h>
#include <stdbool.h>

#include "Variables.h"
#include "emit.h"
#include "lexer.h"
#include "std.h"
#include "register.h"

extern bool has_main;

typedef enum
{
    NODE_NUMBER,
    NODE_ADD,
    NODE_SUB,
    NODE_MUL,
    NODE_DIV,
    NODE_POW,
    NODE_UNARY_MINUS,
    NODE_UNARY_PLUS,
    NODE_FLOAT,
    NODE_IDENTIFIER,
    NODE_ASSIGN,
    NODE_PROGRAM,

    NODE_FUNCTION_DECL,
    NODE_FUNCTION_NAME,
    NODE_FUNCTION_CALL,
    NODE_VARIABLE,
    NODE_VARIABLE_CALL,

    NODE_COMPARE_EQ,
    NODE_GREATER_THAN,
    NODE_GREATER_EQUAL,
    NODE_SMALLER_THAN,
    NODE_SMALLER_EQUAL,

    NODE_IF_STATMENT,
    NODE_WHILE_STATMENT,

    NODE_TYPE_INT,
    NODE_TYPE_FLOAT,
    NODE_TYPE_BOOL,
    NODE_TYPE_CHAR,
    NODE_TYPE_STRING,
    NODE_TYPE_VOID,

    NODE_BODY_START,
    NODE_BODY_END,
    NODE_RETURN_STATMENT,

    NODE_MAIN,

} NodeType;

typedef struct ASTList
{
    struct ASTNode *statement;
    struct ASTList *next;
} ASTList;

typedef struct ASTNode
{
    NodeType type;

    union
    {
        int value_int;
        double value_float;
        char *identifier;

        struct op
        {
            struct ASTNode *left;
            struct ASTNode *right;
        } op;

        struct var
        {
            struct op op;
            int offset;
            VarSize type;
        } var;

        struct varcall
        {
            int offset;
            VarSize type;
        } varcall;

        struct retst
        {
            struct ASTNode *expr;
            VarSize type;
        } retst;

        struct unary
        {
            struct ASTNode *operand;
        } unary;
        struct program
        {
            struct ASTList *list;
        } program;
        struct function
        {
            struct ASTNode *name;
            struct ASTList *body;
            struct ASTList *args;
            int args_size;
            Table_Manager *tm;
            bool used[ARGUMENTS_REG_COUNT];
            VarSize rettype;
    } function;
    };

} ASTNode;

ASTNode *create_number_node(int value);
ASTNode *create_op_node(NodeType type, ASTNode *left, ASTNode *right);

void free_ast(ASTNode *node);
void print_ast(ASTNode *node, int indent);
