#include "redirect_node.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

// CREATE
struct redirection_node *redirection_node_create(int type, int fd_target,
                                                 const char *target)
{
    struct redirection_node *rn = malloc(sizeof(struct redirection_node));
    if (!rn)
    {
        fprintf(stderr, "redirection_node_create: Failed to allocate memory\n");
        return NULL;
    }

    static struct vtable redirection_node_vtable = {
        .exec = redirection_node_exec,
        .free = redirection_node_destroy,
    };

    rn->node.table = &redirection_node_vtable;
    rn->type = type;
    rn->fd_target = (fd_target != -1) ? fd_target : -1; // Target FD
    rn->fd = -1; // Initialize to invalid FD; will be set in exec
    rn->target = target ? my_strdup(target) : NULL;

    if (rn->fd_target == -1)
    {
        // Default target FD based on redirection type
        switch (type)
        {
        case TOKEN_REDIRECT_OUT:
        case TOKEN_REDIRECT_OUT_APPEND:
        case TOKEN_REDIRECT_OUT_FORCE:
            rn->fd_target = STDOUT_FILENO;
            break;

        case TOKEN_REDIRECT_IN:
        case TOKEN_REDIRECT_READ_WRITE:
            rn->fd_target = STDIN_FILENO;
            break;
        case TOKEN_DUP_OUT_DESC:
            rn->fd_target = STDOUT_FILENO;
            break;
        case TOKEN_DUP_IN_DESC:
            rn->fd_target = STDIN_FILENO;
            break;

        default:
            break;
        }
    }

    if (target && !rn->target)
    {
        fprintf(stderr, "redirection_node_create: strdup failed\n");
        free(rn);
        return NULL;
    }

    return rn;
}

// EXECUTE REDIRECTION
int redirection_node_exec(void *param)
{
    struct redirection_node *rn = (struct redirection_node *)param;
    if (!rn || !rn->target)
    {
        fprintf(stderr, "redirection_node_exec: Invalid node or target\n");
        return -1;
    }

    // Open or validate FD
    switch (rn->type)
    {
    case TOKEN_REDIRECT_OUT: // >
        rn->fd = open(rn->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        break;

    case TOKEN_REDIRECT_OUT_APPEND: // >>
        rn->fd = open(rn->target, O_WRONLY | O_CREAT | O_APPEND, 0644);
        break;

    case TOKEN_REDIRECT_IN: // <
        rn->fd = open(rn->target, O_RDONLY);
        break;

    case TOKEN_REDIRECT_OUT_FORCE: // >|
        rn->fd = open(rn->target, O_WRONLY | O_CREAT | O_TRUNC, 0644);
        break;

    case TOKEN_REDIRECT_READ_WRITE: // <>
        rn->fd = open(rn->target, O_RDWR | O_CREAT, 0644);
        break;

    case TOKEN_DUP_OUT_DESC: // >&N
    case TOKEN_DUP_IN_DESC: // <&N
        rn->fd = atoi(rn->target); // Parse the source FD from the target string
        if (rn->fd < 0)
        {
            fprintf(stderr, "redirection_node_exec: Invalid source FD: %s\n",
                    rn->target);
            return -1;
        }
        break;

    default:
        fprintf(stderr, "redirect_node_exec: Unsupported redirection type\n");
        return -1;
    }

    // Check if FD is valid for non-dup cases
    if ((rn->type != TOKEN_DUP_OUT_DESC && rn->type != TOKEN_DUP_IN_DESC)
        && rn->fd == -1)
    {
        perror("redirection_node_exec: open failed");
        return -1;
    }
    if (rn->type == TOKEN_DUP_IN_DESC)
    {
        int a = rn->fd;
        rn->fd = rn->fd_target;
        rn->fd_target = a;
    }

    return 0;
}

// DESTROY
void redirection_node_destroy(void *param)
{
    struct redirection_node *rn = (struct redirection_node *)param;
    if (!rn)
        return;

    if (rn->target)
    {
        free(rn->target);
    }
    free(rn);
}
