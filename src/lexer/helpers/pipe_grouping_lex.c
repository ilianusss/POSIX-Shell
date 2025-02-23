#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

// PARSE PIPE AND GROUPING
struct token lex_pipe_and_grouping(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    if (strncmp(&lexer->input[lexer->pos], "|", 1) == 0)
    {
        new.type = TOKEN_PIPE;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "$(", 2) == 0)
    {
        new.type = TOKEN_COMMAND_SUBST_OPEN;
        new.len = 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "(", 1) == 0)
    {
        new.type = TOKEN_SUBSHELL_OPEN;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], ")", 1) == 0)
    {
        new.type = TOKEN_SUBSHELL_CLOSE;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "{", 1) == 0)
    {
        new.type = TOKEN_LEFT_BRACE;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "}", 1) == 0)
    {
        new.type = TOKEN_RIGHT_BRACE;
        new.len = 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "`", 1) == 0)
    {
        // Backticks (command substitution)
        size_t offset = strcspn(lexer->input + lexer->pos + 1, "`");
        if (lexer->input[lexer->pos + 1 + offset] != '`')
        {
            return new;
        }
        new.type = TOKEN_BACKTICK;
        new.len = offset + 2;
        new.word = malloc(new.len + 1);
        memcpy(new.word, lexer->input + lexer->pos, new.len);
        new.word[new.len] = '\0';
    }

    return new;
}
