#ifndef LOOP_NODE_H
#define LOOP_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES
struct loop_node
{
    struct node node;
    struct node *condition;
    struct node *body;
    int is_until; // 0 for `while`, 1 for `until`
};

// FUNCTIONS
struct loop_node *loop_node_create(struct node *condition, struct node *body,
                                   int is_until);
int loop_node_exec(void *param);
void loop_node_destroy(void *param);

#endif /* ! LOOP_NODE_H */
