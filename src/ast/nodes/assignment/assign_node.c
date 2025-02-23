#define _DEFAULT_SOURCE

#include "assign_node.h"

struct assign_node *assign_node_create(char *name, char *arg)
{
    struct assign_node *an = malloc(sizeof(struct assign_node));

    static struct vtable assign_vtable = {
        .exec = assign_node_exec,
        .free = assign_node_destroy,
    };

    an->node.table = &assign_vtable;
    an->name = name;
    an->arg = arg;

    return an;
}

int assign_node_exec(void *param)
{
    struct assign_node *an = (struct assign_node *)param;

    // Ensure name and arg are not NULL
    if (!an->name)
    {
        fprintf(stderr,
                "assign_node.c: assign_node_exec: Variable name is NULL.\n");
        return -1;
    }

    // If arg is NULL or empty, decide to unset or set to empty string
    if (!an->arg || an->arg[0] == '\0')
    {
        if (unset_shell_var(an->name) != 0)
        {
            fprintf(stderr,
                    "assign_node.c: assign_node_exec: Failed to unset variable "
                    "'%s'\n",
                    an->name);
            return -1;
        }
    }
    else
    {
        if (set_shell_var(an->name, an->arg) != 0)
        {
            fprintf(stderr,
                    "assign_node.c: assign_node_exec: Failed to set variable "
                    "'%s' to '%s'\n",
                    an->name, an->arg);
            return -1;
        }
    }

    return 0;
}

void assign_node_destroy(void *param)
{
    struct assign_node *an = (struct assign_node *)param;
    if (!an)
    {
        fprintf(stderr, "assign_node.c: assign_node_destroy: error");
        return;
    }

    /*
    // Unset pas on sait jms
    if (unsetenv() != 0)
    {
        fprintf(stderr, "for_node.c: for_node_exec: Failed to unset loop
    variable '%s'.\n", fln->var); return -1;
    }*/
    free(an->name);
    free(an->arg);
    free(an);
}
