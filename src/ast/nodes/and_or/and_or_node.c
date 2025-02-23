#include "and_or_node.h"

// CREATE
struct and_or_node *and_or_node_create(struct node *left, struct node *right,
                                       int is_and)
{
    struct and_or_node *aon = malloc(sizeof(struct and_or_node));

    static struct vtable and_or_node_vtable = {
        .exec = and_or_node_exec,
        .free = and_or_node_destroy,
    };

    aon->node.table = &and_or_node_vtable;
    aon->left = left;
    aon->right = right;
    aon->is_and = is_and;

    return aon;
}

// EXEC AND_OR NODE
int and_or_node_exec(void *param)
{
    struct and_or_node *aon = (struct and_or_node *)param;
    if (!aon || !aon->left || !aon->right)
    {
        fprintf(stderr, "and_or_node.c: and_or_node_exec: error\n");
        return -1;
    }

    int left_status = aon->left->table->exec(aon->left);

    if (aon->is_and)
    {
        // `&&`
        return (left_status == 0) ? aon->right->table->exec(aon->right)
                                  : left_status;
    }
    else
    {
        // `||`
        return (left_status != 0) ? aon->right->table->exec(aon->right) : 0;
    }
}

// DESTROY
void and_or_node_destroy(void *param)
{
    struct and_or_node *aon = (struct and_or_node *)param;
    if (!aon)
    {
        fprintf(stderr, "and_or_node.c: and_or_node_destroy: error\n");
        return;
    }
    aon->left->table->free(aon->left);
    aon->right->table->free(aon->right);
    free(aon);
}
