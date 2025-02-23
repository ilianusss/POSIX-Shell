#define _DEFAULT_SOURCE

#include "for_node.h"

static struct ScriptContext *context = NULL;
// CREATE
struct for_node *for_node_create(char *loop_var, char **items, int item_count,
                                 struct node *body)
{
    struct for_node *fln = malloc(sizeof(struct for_node));

    static struct vtable loop_node_vtable = {
        .exec = for_node_exec,
        .free = for_node_destroy,
    };

    fln->node.table = &loop_node_vtable;
    fln->var = loop_var;
    fln->items = items;
    fln->item_count = item_count;
    fln->body = body;

    return fln;
}

// EXEC LOOP NODE
int for_node_exec(void *param)
{
    struct for_node *fln = (struct for_node *)param;
    if (!fln || !fln->var || !fln->body)
    {
        fprintf(stderr,
                "for_node.c: for_node_exec: Invalid for_node structure.\n");
        return -1;
    }

    // Use either fln->items or the positional parameters
    char **items = expand_list(fln->items, fln->item_count, NULL);
    int item_count = fln->item_count;

    if (!items || item_count == 0)
    {
        items = context->positional_params;
        if (!items)
        {
            fprintf(stderr,
                    "for_node.c: for_node_exec: Failed to retrieve positional "
                    "parameters.\n");
            return -1;
        }
        item_count = context->positional_count;
    }

    // Iterate over the items
    for (int i = 0; i < item_count; i++)
    {
        if (!items[i])
        {
            fprintf(stderr,
                    "for_node.c: for_node_exec: Null item encountered in items "
                    "array.\n");
            continue; // Skip null items
        }

        // Set the loop variable in the environment
        if (set_shell_var(fln->var, items[i]) != 0)
        {
            fprintf(stderr,
                    "for_node.c: for_node_exec: Failed to set loop variable "
                    "'%s'.\n",
                    fln->var);
            return -1;
        }

        // Execute the loop body
        int ret = fln->body->table->exec(fln->body);
        if (ret != 0)
        {
            fprintf(stderr,
                    "for_node.c: for_node_exec: Loop body execution failed\n");
            return ret; // Exit with the error code from the body
        }
    }

    // Unset the loop variable after the loop completes
    if (unset_shell_var(fln->var) != 0)
    {
        fprintf(
            stderr,
            "for_node.c: for_node_exec: Failed to unset loop variable '%s'.\n",
            fln->var);
        return -1;
    }
    free_expanded_list(items);

    return 0; // Loop completed successfully
}

// DESTROY
void for_node_destroy(void *param)
{
    struct for_node *fln = (struct for_node *)param;
    if (!fln)
    {
        fprintf(stderr, "for_node.c: for_node_destroy: error\n");
        return;
    }

    fln->body->table->free(fln->body);
    for (int i = 0; i < fln->item_count; i++)
    {
        free(fln->items[i]);
    }
    free(fln->items);
    free(fln->var);
    free(fln);
}
