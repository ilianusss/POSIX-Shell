#ifndef EXPAND_VAR_UTILS_H
#define EXPAND_VAR_UTILS_H

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
void set_last_exit_code(int code);
int get_last_exit_code(void);
void set_pwd(const char *pwd);
char *get_pwd(void);
char *get_oldpwd(void);
void set_random_seed(unsigned int seed);
unsigned int get_random_seed(void);
int get_random_number(void);
char *get_positional_param(struct ScriptContext *context, int index);
int get_positional_count(struct ScriptContext *context);

#endif /* ! EXPAND_VAR_UTILS_H */
