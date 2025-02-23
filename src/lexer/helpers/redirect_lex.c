#include <ctype.h> // For isdigit
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../lexer.h"
#include "../token.h"

static int lex_redirection_switch(struct lexer *lexer, struct token *new,
                                  size_t number_len, size_t start_pos)
{
    // Ensure the next character is a redirection operator
    if (strncmp(&lexer->input[lexer->pos], ">&", 2) == 0)
    {
        new->type = TOKEN_DUP_OUT_DESC;
        new->len = number_len + 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "<&", 2) == 0)
    {
        new->type = TOKEN_DUP_IN_DESC;
        new->len = number_len + 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], ">>", 2) == 0)
    {
        new->type = TOKEN_REDIRECT_OUT_APPEND;
        new->len = number_len + 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], ">|", 2) == 0)
    {
        new->type = TOKEN_REDIRECT_OUT_FORCE;
        new->len = number_len + 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], "<>", 2) == 0)
    {
        new->type = TOKEN_REDIRECT_READ_WRITE;
        new->len = number_len + 2;
    }
    else if (strncmp(&lexer->input[lexer->pos], ">", 1) == 0)
    {
        new->type = TOKEN_REDIRECT_OUT;
        new->len = number_len + 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "<", 1) == 0)
    {
        new->type = TOKEN_REDIRECT_IN;
        new->len = number_len + 1;
    }
    else if (strncmp(&lexer->input[lexer->pos], "<<", 2) == 0)
    {
        new->type = TOKEN_HEREDOC;
        new->len = number_len + 2;
    }
    else
    {
        lexer->pos = start_pos; // Reset position if not a valid redirection
        return 1;
    }
    return 0;
}

struct token lex_redirection(struct lexer *lexer)
{
    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };
    size_t start_pos = lexer->pos;

    // Detect optional IONUMBER (e.g., 1>, 2>>)
    while (isdigit(lexer->input[lexer->pos]))
    {
        lexer->pos++;
    }

    size_t number_len = lexer->pos - start_pos;

    if (number_len > 0)
    {
        if (lex_redirection_switch(lexer, &new, number_len, start_pos) == 1)
            return new;
    }
    else
    {
        // No numeric prefix, proceed with normal redirection
        if (strncmp(&lexer->input[lexer->pos], ">&", 2) == 0)
        {
            new.type = TOKEN_DUP_OUT_DESC;
            new.len = 2;
        }
        else if (strncmp(&lexer->input[lexer->pos], "<&", 2) == 0)
        {
            new.type = TOKEN_DUP_IN_DESC;
            new.len = 2;
        }
        else if (strncmp(&lexer->input[lexer->pos], ">>", 2) == 0)
        {
            new.type = TOKEN_REDIRECT_OUT_APPEND;
            new.len = 2;
        }
        else if (strncmp(&lexer->input[lexer->pos], ">|", 2) == 0)
        {
            new.type = TOKEN_REDIRECT_OUT_FORCE;
            new.len = 2;
        }
        else if (strncmp(&lexer->input[lexer->pos], "<>", 2) == 0)
        {
            new.type = TOKEN_REDIRECT_READ_WRITE;
            new.len = 2;
        }
        else if (strncmp(&lexer->input[lexer->pos], ">", 1) == 0)
        {
            new.type = TOKEN_REDIRECT_OUT;
            new.len = 1;
        }
        else if (strncmp(&lexer->input[lexer->pos], "<", 1) == 0)
        {
            new.type = TOKEN_REDIRECT_IN;
            new.len = 1;
        }
        else if (strncmp(&lexer->input[lexer->pos], "<<", 2) == 0)
        {
            new.type = TOKEN_HEREDOC;
            new.len = 2;
        }
    }

    if (new.type != TOKEN_ERROR)
    {
        new.word = &lexer->input[start_pos]; // Reference input string
    }
    else
    {
        lexer->pos = start_pos; // Reset on failure
    }

    return new;
}
