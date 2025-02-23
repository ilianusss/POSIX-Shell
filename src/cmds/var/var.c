#include "var.h"

struct ShellVar *g_shell_vars = NULL;

const char *get_shell_var(const char *name)
{
    struct ShellVar *current = g_shell_vars;
    while (current)
    {
        if (strcmp(current->name, name) == 0)
            return current->value;
        current = current->next;
    }
    return NULL;
}

int set_shell_var(const char *name, char *value)
{
    remove_unnecessary_backslashes(value);
    if (!name)
        return 1;

    struct ShellVar *current = g_shell_vars;
    while (current)
    {
        if (strcmp(current->name, name) == 0)
        {
            free(current->value);
            current->value = my_strdup(value ? value : "");
            return 0;
        }
        current = current->next;
    }

    // Not found, create a new node
    struct ShellVar *new_var = malloc(sizeof(*new_var));
    if (!new_var)
        return 1;
    new_var->name = my_strdup(name);
    new_var->value = my_strdup(value ? value : "");
    new_var->next = g_shell_vars;
    g_shell_vars = new_var;
    return 0;
}

int unset_shell_var(const char *name)
{
    struct ShellVar **pp = &g_shell_vars;
    while (*pp)
    {
        struct ShellVar *entry = *pp;
        if (strcmp(entry->name, name) == 0)
        {
            *pp = entry->next;
            free(entry->name);
            free(entry->value);
            free(entry);
            return 0;
        }
        pp = &entry->next;
    }
    return 1;
}

void free_shell_vars(void)
{
    struct ShellVar *current = g_shell_vars;
    while (current)
    {
        struct ShellVar *tmp = current;
        current = current->next;
        free(tmp->name);
        free(tmp->value);
        free(tmp);
    }
    g_shell_vars = NULL;
}
