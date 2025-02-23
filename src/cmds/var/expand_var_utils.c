#define _DEFAULT_SOURCE

#include "expand_var_utils.h"

// Setters for specific shell environment variables
void set_last_exit_code(int code)
{
    char code_str[20];
    snprintf(code_str, sizeof(code_str), "%d", code);
    setenv("LAST_EXIT_CODE", code_str, 1);
}

int get_last_exit_code(void)
{
    const char *value = getenv("LAST_EXIT_CODE");
    return value ? atoi(value) : 0;
}

void set_pwd(const char *pwd)
{
    const char *oldpwd = getenv("PWD");
    if (oldpwd)
    {
        setenv("OLDPWD", oldpwd, 1);
    }
    setenv("PWD", pwd, 1);
}

char *get_pwd(void)
{
    const char *pwd = getenv("PWD");
    return pwd ? my_strdup(pwd) : "";
}

char *get_oldpwd(void)
{
    const char *oldpwd = getenv("OLDPWD");
    return oldpwd ? my_strdup(oldpwd) : "";
}

void set_random_seed(unsigned int seed)
{
    char seed_str[20];
    snprintf(seed_str, sizeof(seed_str), "%u", seed);
    setenv("RANDOM_SEED", seed_str, 1);
    srand(seed);
}

unsigned int get_random_seed(void)
{
    const char *value = getenv("RANDOM_SEED");
    return value ? (unsigned int)atoi(value) : 0;
}

int get_random_number(void)
{
    return rand() % 32768; // Random number between 0 and 32767
}

char *get_positional_param(struct ScriptContext *context, int index)
{
    if (index < 1 || index > context->positional_count)
    {
        return ""; // Return empty string if out of bounds
    }
    return context->positional_params[index - 1];
}

int get_positional_count(struct ScriptContext *context)
{
    return context->positional_count;
}
