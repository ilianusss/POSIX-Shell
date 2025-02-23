#ifndef UTILS_H
#define UTILS_H

#include <ctype.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../ast/ast.h"
#include "../lexer/lexer.h"
#include "../lexer/token.h"

char *my_strdup(const char *src);
int is_redirection_token(int token_type);
int is_numeric(const char *str);
int check_quote(const char *str);
void remove_unnecessary_backslashes(char *str);

#endif /* ! UTILS_H */
