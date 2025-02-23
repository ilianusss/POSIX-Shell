#ifndef ASSIGN_NODE_H
#define ASSIGN_NODE_H
// DEFINITIONS
//
// HEADERS
#include "../../../cmds/var/expand_var.h"
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// STRUCTURES
struct assign_node
{
    struct node node; // Base node structure for compatibility in your AST
    char *name; // Name of the variable being assigned
    char *arg; // Value assigned to the variable (as a string)
};

// FUNCTIONS
struct assign_node *assign_node_create(char *name, char *arg);
int assign_node_exec(void *param);
void assign_node_destroy(void *param);

#endif /* ! ASSIGN_NODE_H */
