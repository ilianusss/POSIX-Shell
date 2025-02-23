#ifndef REDIRECT_NODE_H
#define REDIRECT_NODE_H

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#include "../../../utils/utils.h"
#include "../../ast.h"

struct redirection_node
{
    struct node node; // Base node structure
    int type; // Type of redirection (e.g., >, >>, <, etc.)
    int fd; // Source file descriptor (e.g., opened file or existing FD)
    int fd_target; // Target file descriptor for dup2
    char *target; // Target file or FD as a string
};

struct redirection_node *redirection_node_create(int type, int fd,
                                                 const char *target);
int redirection_node_exec(void *param);
void redirection_node_destroy(void *param);

#endif /* REDIRECT_NODE_H */
