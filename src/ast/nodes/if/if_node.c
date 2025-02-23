#include "if_node.h"

// CREATE
struct if_node *if_node_create(struct node *condition, struct node *then_branch,
                               struct node *else_branch)
{
    struct if_node *ifn = malloc(sizeof(struct if_node));

    static struct vtable if_node_vtable = {
        .exec = if_node_exec,
        .free = if_node_destroy,
    };

    ifn->node.table = &if_node_vtable;
    ifn->condition = condition;
    ifn->then_branch = then_branch;
    ifn->else_branch = else_branch;

    return ifn;
}

// EXEC IF NODE
int if_node_exec(void *param)
{
    struct if_node *ifn = (struct if_node *)param;
    if (!ifn->condition || !ifn->then_branch)
    {
        fprintf(stderr, "if_node.c: if_node_exec: error\n");
        return -1;
    }

    int condition_status = ifn->condition->table->exec(ifn->condition);
    if (condition_status == 0)
    {
        return ifn->then_branch->table->exec(ifn->then_branch);
    }
    if (ifn->else_branch)
    {
        return ifn->else_branch->table->exec(ifn->else_branch);
    }
    return 0;
}

// DESTROY
void if_node_destroy(void *param)
{
    struct if_node *ifn = (struct if_node *)param;
    if (!ifn)
    {
        fprintf(stderr, "if_node.c: if_node_destroy: error\n");
        return;
    }
    ifn->condition->table->free(ifn->condition);
    ifn->then_branch->table->free(ifn->then_branch);
    if (ifn->else_branch)
    {
        ifn->else_branch->table->free(ifn->else_branch);
    }
    free(ifn);
}
