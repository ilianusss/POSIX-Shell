#include "negation_node.h"

// CREATE
struct negation_node *negation_node_create(struct node *command)
{
    struct negation_node *nn = malloc(sizeof(struct negation_node));

    static struct vtable negation_node_vtable = {
        .exec = negation_node_exec,
        .free = negation_node_destroy,
    };

    nn->node.table = &negation_node_vtable;
    nn->command = command;

    return nn;
}

// EXEC NEGATION NODE
int negation_node_exec(void *param)
{
    struct negation_node *nn = (struct negation_node *)param;
    if (!nn || !nn->command)
    {
        fprintf(stderr, "negation_node.c: negation_node_exec: error\n");
        return -1;
    }

    int command_status = nn->command->table->exec(nn->command);
    return command_status == 0 ? 1 : 0; // Negate the command status
}

// DESTROY
void negation_node_destroy(void *param)
{
    struct negation_node *nn = (struct negation_node *)param;
    if (!nn)
    {
        fprintf(stderr, "negation_node.c: negation_node_destroy: error\n");
        return;
    }
    nn->command->table->free(nn->command);
    free(nn);
}
