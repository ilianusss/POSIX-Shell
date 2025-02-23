#ifndef LEXER_H
#define LEXER_H

#include "token.h"

enum lexer_mode
{
    NORMAL_MODE, // Default: parse keywords and operators
    COMMAND_MODE // Parse everything as arguments (as TOKEN_WORD)
};

struct lexer
{
    char *input; // The input data
    size_t pos; // The current position in the input
    struct token current_tok; // The current token
    enum lexer_mode mode; // The current lexer mode
};

/**
 * \brief Creates a new lexer given an input string.
 */
struct lexer *lexer_new(char *input);

/**
 ** \brief Frees the given lexer, but not its input.
 */
void lexer_free(struct lexer *lexer);

/**
 * \brief Returns the next token, but doesn't move forward: calling lexer_peek
 * multiple times in a row always returns the same result.
 * This function is meant to help the parser check if the next token matches
 * some rule.
 */
struct token lexer_peek(struct lexer *lexer);

/**
 * \brief Returns the next token, and removes it from the stream:
 *   calling lexer_pop in a loop will iterate over all tokens until EOF.
 */
void lexer_pop(struct lexer *lexer);

void lexer_enter_command_mode(struct lexer *lexer);
void lexer_exit_command_mode(struct lexer *lexer);

struct token lex_keywords(struct lexer *lexer);
struct token lex_redirection(struct lexer *lexer);
struct token lex_pipe_and_grouping(struct lexer *lexer);
struct token lex_operators(struct lexer *lexer);
struct token lex_string_related(struct lexer *lexer);
struct token lex_alias_and_variable(struct lexer *lexer);
struct token lex_word(struct lexer *lexer, const char *delimiters);

#endif /* !LEXER_H */
