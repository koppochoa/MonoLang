#pragma once
#include "lexer.h"
#include "AST.h"

ASTNode* parse_factor(Token* current_token, Lexer* lexer);

ASTNode* parse_term(Token* current_token, Lexer* lexer);

ASTNode* parse_expr(Token* current_token, Lexer* lexer);

ASTNode* parse_power(Token* current_token, Lexer* lexer);

ASTNode* parse_program(Token* current_token, Lexer* lexer);

ASTList* parse_argument_list(Token* current_token, Lexer* lexer);