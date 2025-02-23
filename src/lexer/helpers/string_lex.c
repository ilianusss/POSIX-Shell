#include <complex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

static size_t get_offset(struct lexer *lexer)
{
    char *character = lexer->input + lexer->pos + 1;
    size_t offset = 0;
    while (*character != '\0' && *character != '\"')
    {
        if (*character == '\\')
        {
            character += 2;
            offset += 2;
        }
        else
        {
            character += 1;
            offset += 1;
        }
    }
    return offset;
}

// PARSE STRING-RELATED
struct token lex_string_related(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    if (strncmp(&lexer->input[lexer->pos], "'", 1) == 0)
    {
        size_t offset = strcspn(lexer->input + lexer->pos + 1, "'");
        if (lexer->input[lexer->pos + 1 + offset] != '\'')
        {
            return new;
        }
        new.type = TOKEN_SINGLE_QUOTES;
        new.len = offset + 2;
        new.word = malloc(offset + 1);
        memcpy(new.word, lexer->input + lexer->pos + 1, offset);
        new.word[offset] = '\0';
    }
    else if (strncmp(&lexer->input[lexer->pos], "\"", 1) == 0)
    {
        size_t offset = get_offset(lexer);
        if (lexer->input[lexer->pos + 1 + offset] != '"')
        {
            return new;
        }
        new.type = TOKEN_DOUBLE_QUOTES;
        new.len = offset + 2;
        new.word = malloc(offset + 1);
        memcpy(new.word, lexer->input + lexer->pos + 1, offset);
        new.word[offset] = '\0';
    }

    return new;
}

struct token lex_word(struct lexer *lexer, const char *delimiters)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    size_t offset = strcspn(lexer->input + lexer->pos, delimiters);
    size_t len = offset;
    new.type = TOKEN_WORD;
    new.len = len;
    new.word = malloc(len + 1);
    memcpy(new.word, lexer->input + lexer->pos, len);
    new.word[len] = '\0';

    return new;
}
