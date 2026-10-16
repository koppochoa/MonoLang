#pragma once
#include "AST.h"
#include "lexer.h"

ASTNode *parse_factor(Token *current_token, Lexer *lexer, Table_Manager *tm);

ASTNode *parse_term(Token *current_token, Lexer *lexer, Table_Manager *tm);

ASTNode *parse_expr(Token *current_token, Lexer *lexer, Table_Manager *tm);

ASTNode *parse_power(Token *current_token, Lexer *lexer, Table_Manager *tm);

ASTNode *parse_program(Token *current_token, Lexer *lexer);

ASTList *parse_argument_list(Token *current_token, Lexer *lexer, Table_Manager *tm);