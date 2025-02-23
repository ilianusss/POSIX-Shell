#ifndef CASE_NODE_H
#define CASE_NODE_H
// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/wait.h>
#include <unistd.h>

// STRUCTURES
// FAUT REVIEW CA
struct case_clause
{
    char **patterns; // Patterns to match
    struct node *commands; // Commands to execute for matched patterns
};

struct case_node
{
    struct node node;
    struct node *expression; // Expression to match against
    struct case_clause **clauses; // Array of case clauses
};

// FUNCTIONS
struct case_node *case_node_create(void);
int case_node_exec(void *param);
void case_node_destroy(void *param);

#endif /* ! CASE_NODE_H */
