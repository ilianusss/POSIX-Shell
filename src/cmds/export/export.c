#define _DEFAULT_SOURCE

#include "export.h"

int builtin_export(int argc, char **argv)
{
    if (argc != 2)
    {
        fprintf(stderr, "Usage: export VAR[=VALUE]\n");
        return 1;
    }

    char var_name[1024];
    char value[1024];
    int is_assigned = 0;
    int i = 0;
    char *c = argv[1];

    while (*c != '=' && *c != '\0' && i < 1023)
    {
        var_name[i++] = *c++;
    }
    var_name[i] = '\0';

    if (*c == '=')
    {
        c++;
        i = 0;
        while (*c != '\0' && i < 1023)
        {
            value[i++] = *c++;
        }
        value[i] = '\0';
        is_assigned = 1;
    }
    else
    {
        value[0] = '\0';
    }

    // Remove if the variable already exists in the shell's local list
    const char *existing_val = get_shell_var(var_name);
    if (existing_val != NULL)
        unset_shell_var(var_name);

    if (!is_assigned)
    {
        // No '=' -> export the existing local var if found; otherwise empty.
        if (existing_val == NULL)
            setenv(var_name, "", 1);
        else
            setenv(var_name, existing_val, 1);
    }
    else
    {
        // 'VAR=VALUE' form -> export with given value. */
        setenv(var_name, value, 1);
    }

    return 0;
}
