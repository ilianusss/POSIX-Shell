#ifndef SIMPLE_COMMAND_NODE_H
#define SIMPLE_COMMAND_NODE_H
// DEFINITIONS
//
// HEADERS
#include "../../../cmds/var/expand_var.h"
#include "../../../cmds/var/expand_var_utils.h"
#include "../../../utils/utils.h"
#include "../../ast.h"
#include "word_types.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// STRUCTURES

struct simple_command_node
{
    struct node node;
    char **argv;
    int argc;
    enum word_type *word_types;
    struct node **redirections; // Array of redirection nodes
    size_t redir_count; // Number of redirections
};

// FUNCTIONS
struct simple_command_node *
simple_command_node_create(char **argv, int argc, enum word_type *word_types);
int simple_command_node_exec(void *param);
void simple_command_node_destroy(void *param);

#endif /* ! SIMPLE_COMMAND_NODE_H */
