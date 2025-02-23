#include "command_list_node.h"

// CREATE
struct command_list_node *command_list_node_create(struct node **commands)
{
    struct command_list_node *cln = malloc(sizeof(struct command_list_node));

    static struct vtable command_list_vtable = {
        .exec = command_list_node_exec,
        .free = command_list_node_destroy,
    };

    cln->node.table = &command_list_vtable;
    cln->commands = commands;

    return cln;
}

// EXEC COMMAND LIST NODE
int command_list_node_exec(void *param)
{
    struct command_list_node *cln = (struct command_list_node *)param;
    if (!cln || !cln->commands)
    {
        fprintf(stderr, "command_list_node.c: command_list_node_exec: error\n");
        return -1;
    }

    int status = 0;
    for (int i = 0; cln->commands[i] != NULL; i++)
    {
        status = cln->commands[i]->table->exec(cln->commands[i]);
    }
    return status;
}

// DESTROY
void command_list_node_destroy(void *param)
{
    struct command_list_node *cln = (struct command_list_node *)param;
    if (!cln)
    {
        fprintf(stderr,
                "command_list_node.c: command_list_node_destroy: error\n");
        return;
    }
    for (int i = 0; cln->commands[i] != NULL; i++)
    {
        cln->commands[i]->table->free(cln->commands[i]);
    }
    free(cln->commands);
    free(cln);
}
