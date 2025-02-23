#ifndef VAR_H
#define VAR_H
// DEFINITIONS

// HEADERS
#include "../../utils/utils.h"
// LIBS
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES
struct ShellVar
{
    char *name;
    char *value;
    struct ShellVar *next;
};

// FUNCTIONS
const char *get_shell_var(const char *name);
int set_shell_var(const char *name, char *value);
int unset_shell_var(const char *name);
void free_shell_vars(void);

#endif /* ! VAR_H */
