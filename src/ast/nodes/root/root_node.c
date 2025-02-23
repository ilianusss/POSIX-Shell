#include "root_node.h"

#include <stdio.h>

// CREATE
struct root_node *root_node_create(struct node **child, size_t count)
{
    struct root_node *root = malloc(sizeof(struct root_node));

    static struct vtable root_vtable = {
        .exec = root_node_exec,
        .free = root_node_destroy,
    };

    root->node.table = &root_vtable;
    root->commands = child;
    root->count = count;

    return root;
}

// EXEC ROOT NODE
int root_node_exec(void *param)
{
    struct root_node *root = (struct root_node *)param;
    if (root->count == 0)
    {
        return 0;
    }
    if (!root)
    {
        fprintf(stderr,
                "compound_list_node.c: compound_list_node_exec: error\n");
        return -1;
    }

    int status = 0;
    for (int i = 0; root->commands[i] != NULL; i++)
    {
        status = root->commands[i]->table->exec(root->commands[i]);
    }
    return status;
}

// DESTROY
void root_node_destroy(void *param)
{
    struct root_node *root = (struct root_node *)param;
    if (!root)
    {
        fprintf(stderr, "root_node.c: root_node_destroy: error\n");
        return;
    }

    if (root->count > 0)
    {
        for (int i = 0; root->commands[i] != NULL; i++)
        {
            root->commands[i]->table->free(root->commands[i]);
        }
    }
    free(root->commands);
    free(root);
}
