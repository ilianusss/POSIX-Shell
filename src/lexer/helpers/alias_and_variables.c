#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

// PARSE ALIASES AND VARIABLES
struct token lex_alias_and_variable(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    if (strncmp(&lexer->input[lexer->pos], "alias ", 6) == 0)
    {
        new.type = TOKEN_ALIAS;
        new.len = 5;
    }
    else if (strncmp(&lexer->input[lexer->pos], "unalias ", 8) == 0)
    {
        new.type = TOKEN_UNALIAS;
        new.len = 7;
    }
    else if (lexer->input[lexer->pos] == '=')
    {
        new.type = TOKEN_ASSIGNMENT;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "case ", 5) == 0)
    {
        new.type = TOKEN_CASE;
        new.len = 4;
    }
    else if (strncmp(&lexer->input[lexer->pos], "in ", 3) == 0)
    {
        new.type = TOKEN_IN;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "in\n", 3) == 0)
    {
        new.type = TOKEN_IN;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "esac", 4) == 0)
    {
        new.type = TOKEN_ESAC;
        new.len = 4;
    }
    else if (strncmp(&lexer->input[lexer->pos], "function ", 9) == 0)
    {
        new.type = TOKEN_FUNCTION;
        new.len = 8;
    }

    return new;
}
