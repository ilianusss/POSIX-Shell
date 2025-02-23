#ifndef TOKEN_H
#define TOKEN_H

#include <unistd.h>

enum token_type
{
    // Keywords
    TOKEN_IF, // "if"
    TOKEN_THEN, // "then"
    TOKEN_ELIF, // "elif"
    TOKEN_ELSE, // "else"
    TOKEN_FI, // "fi"
    TOKEN_WHILE, // "while"
    TOKEN_UNTIL, // "until"
    TOKEN_FOR, // "for"
    TOKEN_DO, // "do"
    TOKEN_DONE, // "done"
    TOKEN_CASE, // "case"
    TOKEN_IN, // "in"
    TOKEN_ESAC, // "esac"
    TOKEN_FUNCTION, // "function"

    // Control characters and logical operators
    TOKEN_SEMICOLON, // ';'
    TOKEN_DOUBLE_SEMICOLON, // ";;"
    TOKEN_ENDLINE, // '\n'
    TOKEN_AND, // "&&"
    TOKEN_OR, // "||"
    TOKEN_NEGATION, // '!'

    // Redirection
    TOKEN_REDIRECT_OUT, // '>'
    TOKEN_REDIRECT_OUT_APPEND, // '>>'
    TOKEN_REDIRECT_IN, // '<'
    TOKEN_DUP_OUT_DESC, // '>&'
    TOKEN_DUP_IN_DESC, // '<&'
    TOKEN_REDIRECT_OUT_FORCE, // '>|'
    TOKEN_REDIRECT_READ_WRITE, // '<>'
    TOKEN_HEREDOC, // '<<'

    // Pipe and grouping
    TOKEN_PIPE, // '|'
    TOKEN_LEFT_BRACE, // '{'
    TOKEN_RIGHT_BRACE, // '}'
    TOKEN_SUBSHELL_OPEN, // '(' for subshell
    TOKEN_SUBSHELL_CLOSE, // ')' for subshell

    // Command Substitution
    TOKEN_COMMAND_SUBST_OPEN, // "$(" for command substitution
    TOKEN_BACKTICK, // '`' for legacy command substitution

    // String-related tokens
    TOKEN_WORD, // Generic word
    TOKEN_SINGLE_QUOTES, // Single quote
    TOKEN_DOUBLE_QUOTES, // Double quote

    // Variable and assignment
    TOKEN_VARIABLE, // '$'
    TOKEN_ASSIGNMENT, // '='

    // Alias
    TOKEN_ALIAS, // "alias"
    TOKEN_UNALIAS, // "unalias"

    // End of input and error handling
    TOKEN_EOF, // End of input
    TOKEN_ERROR // Invalid input
};

struct token
{
    enum token_type type;
    ssize_t len;
    char *word;
};

#endif /* !TOKEN_H */
