#include "compound_list_node.h"

// CREATE
struct compound_list_node *compound_list_node_create(struct node **commands)
{
    struct compound_list_node *cln = malloc(sizeof(struct compound_list_node));

    static struct vtable compound_list_vtable = {
        .exec = compound_list_node_exec,
        .free = compound_list_node_destroy,
    };

    cln->node.table = &compound_list_vtable;
    cln->commands = commands;

    return cln;
}

// EXEC COMPOUND LIST NODE
int compound_list_node_exec(void *param)
{
    struct compound_list_node *cln = (struct compound_list_node *)param;
    if (!cln || !cln->commands)
    {
        fprintf(stderr,
                "compound_list_node.c: compound_list_node_exec: error\n");
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
void compound_list_node_destroy(void *param)
{
    struct compound_list_node *cln = (struct compound_list_node *)param;
    if (!cln)
    {
        fprintf(stderr,
                "compound_list_node.c: compound_list_node_destroy: error\n");
        return;
    }
    for (int i = 0; cln->commands[i] != NULL; i++)
    {
        cln->commands[i]->table->free(cln->commands[i]);
    }
    free(cln->commands);
    free(cln);
}
