#include "lexer.h"

#include <ctype.h>
#include <stdlib.h>
#include <string.h>

void print_error_at_position(const char *input, int position, const char *message)
{
    fprintf(stderr, "%s\n", input);

    // Affiche des espaces jusqu'à la position
    for (int i = 0; i < position - 1; ++i)
    {
        if (input[i] == '\t') // support tabulation
            fputc('\t', stderr);
        else
            fputc(' ', stderr);
    }

    // Affiche la flèche
    fprintf(stderr, "^\n");
    fprintf(stderr, "Erreur : %s\n", message);
}

void print_token(Token token)
{
    switch (token.tokentype)
    {
    case NUMBER:
        printf("NUMBER: %d\n", token.value_int);
        break;
    case FLOAT:
        printf("FLOAT: %f\n", token.value_float);
        break;
    case IDENTIFIER:
        printf("IDENTIFIER: %s\n", token.identifier);
        break;
    case ASSIGN:
        printf("ASSIGN (=)\n");
        break;
    case PLUS:
        printf("PLUS (+)\n");
        break;
    case MINUS:
        printf("MINUS (-)\n");
        break;
    case MULTIPLY:
        printf("MULTIPLY (*)\n");
        break;
    case DIVIDE:
        printf("DIVIDE (/)\n");
        break;
    case POWER:
        printf("POWER (^)\n");
        break;
    case SEMICOLON:
        printf("SEMICOLON (;)\n");
        break;
    case LPARENTHESIS:
        printf("LPARENTHESIS: (\n");
        break;
    case RPARENTHESIS:
        printf("RPARENTHESIS: )\n");
        break;
    case FUNCTION_DECL:
        printf("FUNCTION_DECL\n");
        break;
    case LBRACE:
        printf("LBRACE\n");
        break;
    case RBRACE:
        printf("RBRACE\n");
        break;
    case TOKEN_EOF:
        printf("EOF\n");
        break;
    case RETURN_STATMENT:
        printf("RETURN_STATMENT\n");
        break;

    case INVALID:
        break;
    default:
        printf("INVALID TOKEN\n");
        break;
    }
}

void set_lexer(Lexer *lexer, char *input)
{
    lexer->data = input;
    lexer->position = 0;
}

bool is_digit(char input)
{
    return isdigit((unsigned char)input);
}

void skip_spaces(Lexer *lexer)
{
    while (lexer->data[lexer->position] == ' ')
    {
        lexer->position++;
    }
}

Token lex_eof(const char current_char)
{
    Token token;
    token.tokentype = TOKEN_EOF;
    return token;
}

Token lex_operators(Lexer *lexer)
{
    Token token;

    switch (lexer->data[lexer->position])
    {
    case '\0':
        token.tokentype = TOKEN_EOF;
        return token;
    case '+':
        token.tokentype = PLUS;
        lexer->position++;
        return token;
    case '-':
        token.tokentype = MINUS;
        lexer->position++;
        return token;
    case '*':
        token.tokentype = MULTIPLY;
        lexer->position++;
        return token;
    case '/':
        token.tokentype = DIVIDE;
        lexer->position++;
        return token;
    case '(':
        token.tokentype = LPARENTHESIS;
        lexer->position++;
        return token;
    case ')':
        token.tokentype = RPARENTHESIS;
        lexer->position++;
        return token;
    case '^':
        token.tokentype = POWER;
        lexer->position++;
        return token;
    case '=':
        token.tokentype = ASSIGN;
        lexer->position++;
        return token;
    case ';':
        token.tokentype = SEMICOLON;
        lexer->position++;
        return token;
    case ',':
        token.tokentype = COMMA;
        lexer->position++;
        return token;
    case '{':
        token.tokentype = LBRACE;
        lexer->position++;
        return token;
    case '}':
        token.tokentype = RBRACE;
        lexer->position++;
        return token;
    default:
        lexer->position++;
        print_error_at_position(lexer->data, lexer->position, "Caractère non reconnu");
        exit(1);
    }
}

Token lex_identifier(Lexer *lexer)
{
    Token token;

    int start = lexer->position;
    while (isalnum(lexer->data[lexer->position]))
    {
        lexer->position++;
    }

    int length = lexer->position - start;
    char *ident = malloc(length + 1);
    strncpy(ident, lexer->data + start, length);
    ident[length] = '\0';

    if (strcmp(ident, "fn") == 0)
    {
        token.tokentype = FUNCTION_DECL;
        // potentiel free
        return token;
    }

    if (strcmp(ident, "return") == 0)
    {
        token.tokentype = RETURN_STATMENT;
        // potentiel free
        return token;
    }

    token.tokentype = IDENTIFIER;
    token.identifier = ident;
    return token;
}

Token lex_number(Lexer *lexer)
{
    Token token;

    char buffer[64];
    int i = 0;
    int point_count = 0;

    while (is_digit(lexer->data[lexer->position]) || lexer->data[lexer->position] == '.')
    {
        if (lexer->data[lexer->position] == '.')
        {
            point_count++;
            if (point_count > 1)
                break; // plus d'un point = fin du nombre
        }
        buffer[i++] = lexer->data[lexer->position];
        lexer->position++;
    }
    buffer[i] = '\0';

    if (point_count == 0)
    {
        // nombre entier
        int val = atoi(buffer);
        token.tokentype = NUMBER; // ou NUMBER_INT
        token.value_int = val;
    }
    else
    {
        // nombre flottant
        double val = strtod(buffer, NULL);
        token.tokentype = FLOAT;
        token.value_float = val;
    }

    return token;
}

Token get_next_token(Lexer *lexer)
{
    Token token;
    token.tokentype = INVALID;

    skip_spaces(lexer);

    char current_char = lexer->data[lexer->position];

    if (current_char == '\n')
    {
        lexer->position++;
        return get_next_token(lexer);
    }

    if (current_char == '\0')
        return lex_eof(current_char);

    if (isalpha(current_char))
        return lex_identifier(lexer);

    if (is_digit(current_char) || current_char == '.')
        return lex_number(lexer);

    return lex_operators(lexer);
}

void debug_lexer(Lexer *lexer)
{
    Lexer dl = { .data = lexer->data, .position = 0 };
    Token tk = get_next_token(&dl);

    while (tk.tokentype != TOKEN_EOF)
    {
        print_token(tk);
        tk = get_next_token(&dl);
    }

    if (tk.tokentype == TOKEN_EOF)
    {
        print_token(tk);
    }
}