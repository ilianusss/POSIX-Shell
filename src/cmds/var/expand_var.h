#ifndef EXPAND_VAR_H
#define EXPAND_VAR_H

// HEADERS
#include "../../utils/utils.h"
#include "context.h"
#include "var.h"

// LIBS
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

// STRUCTURES
//
// FUNCTIONS
char **expand_list(char **token, int nb_tok, struct ScriptContext *context);
char *expand_inline(const char *input, struct ScriptContext *context);
void free_expanded_list(char **expanded_tokens); // Added for memory management
struct ScriptContext *init_script_context(int argc, char **argv);
void free_script_context(struct ScriptContext *context);
void init_shell_env(void);

#endif /* ! EXPAND_VAR_H */
