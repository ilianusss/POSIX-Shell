#include "loop_node.h"

// CREATE
struct loop_node *loop_node_create(struct node *condition, struct node *body,
                                   int is_until)
{
    struct loop_node *ln = malloc(sizeof(struct loop_node));

    static struct vtable loop_node_vtable = {
        .exec = loop_node_exec,
        .free = loop_node_destroy,
    };

    ln->node.table = &loop_node_vtable;
    ln->condition = condition;
    ln->body = body;
    ln->is_until = is_until;

    return ln;
}

// EXEC LOOP NODE
int loop_node_exec(void *param)
{
    struct loop_node *ln = (struct loop_node *)param;
    if (!ln || !ln->condition || !ln->body)
    {
        fprintf(stderr, "loop_node.c: loop_node_exec: error\n");
        return -1;
    }

    while (1)
    {
        int condition_status = ln->condition->table->exec(ln->condition);

        if ((ln->is_until && condition_status == 0)
            || (!ln->is_until && condition_status != 0))
        {
            break; // Break the loop if the condition matches
        }

        int body_status = ln->body->table->exec(ln->body);
        if (body_status != 0)
        {
            return body_status; // Return if the body execution fails
        }
    }

    return 0; // Loop completed successfully
}

// DESTROY
void loop_node_destroy(void *param)
{
    struct loop_node *ln = (struct loop_node *)param;
    if (!ln)
    {
        fprintf(stderr, "loop_node.c: loop_node_destroy: error\n");
        return;
    }
    ln->condition->table->free(ln->condition);
    ln->body->table->free(ln->body);
    free(ln);
}
