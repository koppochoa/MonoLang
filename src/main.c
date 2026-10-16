#include <math.h>
#include <stdio.h>

#include "AST.h"
#include "Variables.h"
#include "lexer.h"
#include "parser.h"
#include "std.h"

int func()
{
    char *input = "value = sqrt(4) + cos(pi/2) + log(e) + max(2/2, 5/3); "
                  "newvalue = 10 * value + 10 ";

    Table_Manager tm;
    init_table_manager(&tm);
    // set_variable(&tm, "pi", M_PI);
    // set_variable(&tm, "e", M_E);

    Lexer lexer;
    set_lexer(&lexer, input);

    Token current_token = get_next_token(&lexer);
    ASTNode *root = parse_program(&current_token, &lexer);

    double result = evaluate(root, &tm);
    print_variables(&tm);
    print_ast(root, 0);

    // printf("Result : %d\n", result);

    free_ast(root);
    return 0;
}

int main(int argc, char **argv)
{
    if (argc < 2)
    {
        puts("No path");
        return 0;
    }

    FILE *in = fopen(argv[1], "rw");

    if (!in)
    {
        puts("unable to open file");
        return -1;
    }

    FILE *out = fopen("out.s", "w");
    if (!out)
    {
        puts("Error while creating output file");
        fclose(in);
        return -1;
    }

    fseek(in, 0, SEEK_END);
    long size = ftell(in);
    rewind(in);

    char *buffer = malloc(size + 1);
    if (!buffer)
    {
        fclose(in);
        return 1;
    }

    size_t bytes_read = fread(buffer, 1, size, in);
    buffer[bytes_read] = '\0';

    fprintf(out,
            ".section .text\n"
            ".globl _start\n\n"
            "_start:\n"
            "   call main\n"

    );

    _sysexit(out);

    Lexer lexer;
    set_lexer(&lexer, buffer);

    Token current_token = get_next_token(&lexer);

    debug_lexer(&lexer);

    ASTNode *root = parse_program(&current_token, &lexer);

    generate_code(out, root, NULL);


    print_ast(root, 0);
    // print_variables(&tm);

    fclose(in);

    return 0;
}
