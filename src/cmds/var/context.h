#ifndef CONTEXT_H
#define CONTEXT_H

// HEADERS
#include "../../utils/utils.h"

// LIBS
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>

// STRUCTURES
// stack data
struct ScriptContext
{
    char **positional_params; // Array of arguments
    int positional_count; // Number of arguments
};

// stack node
struct StackContextNode
{
    struct ScriptContext *data;
    struct StackContextNode *next;
};

// stack structure
struct StackContext
{
    struct StackContextNode *top;
};

// FUNCTIONS
struct StackContext *init_stack(void);
void push(struct StackContext *stack, struct ScriptContext *value);
struct ScriptContext *pop(struct StackContext *stack);
int is_empty(struct StackContext *stack);
void free_stack(struct StackContext *stack);

struct ScriptContext *init_script_context(int argc, char **argv);
void free_script_context(struct ScriptContext *context);

#endif /* ! CONTEXT_H */
