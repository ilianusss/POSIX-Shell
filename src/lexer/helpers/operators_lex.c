#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

// LEX CONTROL CHAR & OPERATORS
struct token lex_operators(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    if (strncmp(&lexer->input[lexer->pos], "&&", 2) == 0)
    {
        new.type = TOKEN_AND;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "||", 2) == 0)
    {
        new.type = TOKEN_OR;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "! ", 2) == 0)
    {
        new.type = TOKEN_NEGATION;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], ";;", 2) == 0)
    {
        new.type = TOKEN_DOUBLE_SEMICOLON;
        new.len = 2;
    }

    return new;
}
