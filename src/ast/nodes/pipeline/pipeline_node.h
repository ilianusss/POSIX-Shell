#ifndef PIPELINE_NODE_H
#define PIPELINE_NODE_H

// DEFINITIONS
//
// HEADERS
#include "../../../utils/utils.h"
#include "../../ast.h"

// LIBS
#include <errno.h> // errno
#include <stdio.h> // fprintf
#include <stdlib.h> // exit
#include <string.h> // strerror
#include <sys/types.h> // pid_t
#include <sys/wait.h> // wait, waitpid
#include <unistd.h> // fork, pipe, dup2

// STRUCTURES
struct pipeline_node
{
    struct node node;
    struct node **commands; // NULL-terminated array of command nodes
};

// FUNCTIONS
struct pipeline_node *pipeline_node_create(struct node **commands);
int pipeline_node_exec(void *param);
void pipeline_node_destroy(void *param);

#endif /* ! PIPELINE_NODE_H */
