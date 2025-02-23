#include "lexer.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../utils/utils.h"
#include "token.h"

struct token lexer_next_token(struct lexer *lexer)
{
    while (lexer->input[lexer->pos] == ' ') // Skip whitespace
        lexer->pos++;

    struct token new = { .type = TOKEN_ERROR, .len = 0, .word = NULL };

    // End of input
    if (strncmp(&lexer->input[lexer->pos], "\0", 1) == 0)
    {
        new.type = TOKEN_EOF;
        new.len = 1;
    }
    // End of line
    else if (strncmp(&lexer->input[lexer->pos], "\n", 1) == 0)
    {
        new.type = TOKEN_ENDLINE;
        new.len = 1;
    }
    // Handle comments
    else if (strncmp(&lexer->input[lexer->pos], "#", 1) == 0)
    {
        lexer->pos += strcspn(lexer->input + lexer->pos, "\n");
        if (lexer->input[lexer->pos] == '\n')
        {
            lexer->pos++;
        }
        return lexer_next_token(lexer);
    }
    // Handle ';
    else if (strncmp(&lexer->input[lexer->pos], ";", 1) == 0)
    {
        new.type = TOKEN_SEMICOLON;
        new.len = 1;
    }
    else if ((new = lex_string_related(lexer)).type != TOKEN_ERROR)
        return new;
    else if ((new = lex_redirection(lexer)).type != TOKEN_ERROR)
        return new;
    else if ((new = lex_operators(lexer)).type != TOKEN_ERROR)
        return new;
    else if ((new = lex_pipe_and_grouping(lexer)).type != TOKEN_ERROR)
        return new;
    // If in COMMAND_MODE, treat everything as TOKEN_WORD
    else if (lexer->mode == COMMAND_MODE)
    {
        return lex_word(lexer, " \n;");
    }
    // Call specialized lexing functions in NORMAL_MODE
    else if ((new = lex_keywords(lexer)).type != TOKEN_ERROR)
        return new;

    else if ((new = lex_alias_and_variable(lexer)).type != TOKEN_ERROR)
        return new;
    else
    {
        // NORMAL_MODE word
        return lex_word(lexer, " \n;=");
    }

    return new;
}

void lexer_enter_command_mode(struct lexer *lexer)
{
    // printf("COMMAND MODE ON\n");
    lexer->mode = COMMAND_MODE;
}

void lexer_exit_command_mode(struct lexer *lexer)
{
    // printf("COMMAND MODE OFF\n");
    lexer->mode = NORMAL_MODE;
}

struct lexer *lexer_new(char *input)
{
    struct lexer *new = malloc(sizeof(struct lexer));
    new->input = input;
    new->pos = 0;
    new->current_tok = lexer_next_token(new);
    new->mode = NORMAL_MODE;
    return new;
}

void lexer_free(struct lexer *lexer)
{
    struct token next = lexer->current_tok;
    if (next.type == TOKEN_WORD || next.type == TOKEN_SINGLE_QUOTES
        || next.type == TOKEN_DOUBLE_QUOTES || next.type == TOKEN_BACKTICK)
        free(next.word);
    free(lexer);
}

struct token lexer_peek(struct lexer *lexer)
{
    return lexer->current_tok;
}

void lexer_pop(struct lexer *lexer)
{
    struct token next = lexer->current_tok;
    if (next.type == TOKEN_WORD || next.type == TOKEN_SINGLE_QUOTES
        || next.type == TOKEN_DOUBLE_QUOTES || next.type == TOKEN_BACKTICK)
        free(next.word);
    lexer->pos += next.len;
    lexer->current_tok = lexer_next_token(lexer);
}
