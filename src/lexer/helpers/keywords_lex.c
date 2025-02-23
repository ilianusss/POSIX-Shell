#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

static int check_fi(struct lexer *lexer)
{
    char *character = lexer->input + lexer->pos;
    if (character[0] != 'f')
        return 1;
    if (character[1] != 'i')
        return 1;
    if (character[2] != ';' && character[2] != '\n' && character[2] != '#'
        && character[2] != 0 && character[2] != 32)
        return 1;
    return 0;
}

// LEX KEYWORDS
struct token lex_keywords(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    if (strncmp(&lexer->input[lexer->pos], "if ", 3) == 0
        || strncmp(&lexer->input[lexer->pos], "if\n", 3) == 0)
    {
        new.type = TOKEN_IF;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "then ", 5) == 0
             || strncmp(&lexer->input[lexer->pos], "then\n", 5) == 0)
    {
        new.type = TOKEN_THEN;
        new.len = 4;
    }
    else if (strncmp(&lexer->input[lexer->pos], "else ", 5) == 0
             || strncmp(&lexer->input[lexer->pos], "else\n", 5) == 0)
    {
        new.type = TOKEN_ELSE;
        new.len = 4;
    }
    else if (strncmp(&lexer->input[lexer->pos], "elif ", 5) == 0
             || strncmp(&lexer->input[lexer->pos], "elif\n", 5) == 0)
    {
        new.type = TOKEN_ELIF;
        new.len = 4;
    }
    else if (check_fi(lexer) == 0)
    {
        new.type = TOKEN_FI;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "while ", 6) == 0
             || strncmp(&lexer->input[lexer->pos], "while\n", 6) == 0)
    {
        new.type = TOKEN_WHILE;
        new.len = 5;
    }
    else if (strncmp(&lexer->input[lexer->pos], "until ", 6) == 0
             || strncmp(&lexer->input[lexer->pos], "until\n", 6) == 0)
    {
        new.type = TOKEN_UNTIL;
        new.len = 5;
    }
    else if (strncmp(&lexer->input[lexer->pos], "for ", 4) == 0)
    {
        new.type = TOKEN_FOR;
        new.len = 3;
    }
    else if (strncmp(&lexer->input[lexer->pos], "do ", 3) == 0
             || strncmp(&lexer->input[lexer->pos], "do\n", 3) == 0)
    {
        new.type = TOKEN_DO;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "done", 4) == 0)
    {
        new.type = TOKEN_DONE;
        new.len = 4;
    }

    return new;
}
