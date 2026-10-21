#include <math.h>
#include <stdio.h>

#include "AST.h"
#include "Variables.h"
#include "lexer.h"
#include "parser.h"
#include "std.h"

bool has_main = false;

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

    if(has_main == false)
    {
        printf("No Main Symbol\n");
        exit(1);
    }

    generate_code(out, root, NULL);


    // print_ast(root, 0);
    // print_variables(&tm);

    fclose(in);

    return 0;
}
