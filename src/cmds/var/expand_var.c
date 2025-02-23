#define _DEFAULT_SOURCE

#include "expand_var.h"

#include "expand_var_utils.h"

static char *format_to_string(const char *fmt, ...)
{
    va_list args;
    va_list tmp;
    va_start(args, fmt);
    va_copy(tmp, args);

    // Determine length needed (not counting null terminator).
    int needed = vsnprintf(NULL, 0, fmt, tmp);
    va_end(tmp);

    // Allocate buffer (+1 for null terminator).
    char *buf = malloc(needed + 1);
    if (!buf)
    {
        va_end(args);
        return NULL; // malloc failed
    }

    // Actually write the formatted data.
    vsnprintf(buf, needed + 1, fmt, args);
    va_end(args);

    return buf;
}

// Initialize the shell environment variables
void init_shell_env(void)
{
    // Set the random seed
    unsigned int seed = (unsigned int)time(NULL);
    char seed_str[20];
    snprintf(seed_str, sizeof(seed_str), "%u", seed);
    setenv("RANDOM_SEED", seed_str, 1);
    srand(seed);

    // Set PWD and OLDPWD
    char cwd[1024];
    if (getcwd(cwd, sizeof(cwd)) != NULL)
    {
        setenv("PWD", cwd, 1);
        setenv("OLDPWD", cwd, 1);
    }
    else
    {
        setenv("PWD", "/", 1);
        setenv("OLDPWD", "/", 1);
    }

    // Set PATH if not already set
    if (getenv("PATH") == NULL)
    {
        setenv("PATH", "/usr/bin:/bin", 1);
    }

    // Set HOME if not already set
    if (getenv("HOME") == NULL)
    {
        const char *home = getenv("USER") ? "/home/USER" : "/";
        setenv("HOME", home, 1);
    }
}

static char *expand_positional_all(struct ScriptContext *context)
{
    int count = get_positional_count(context);
    if (count == 0)
        return my_strdup("");

    /* Calcul de la longueur totale nécessaire */
    size_t total_len = 0;
    for (int i = 0; i < count; i++)
    {
        const char *param = get_positional_param(context, i + 1);
        total_len += strlen(param);
        if (i < count - 1)
            total_len++; // espace entre les paramètres
    }

    char *result = malloc(total_len + 1);
    if (!result)
        return NULL;

    result[0] = '\0';
    for (int i = 0; i < count; i++)
    {
        if (i > 0)
            strcat(result, " ");
        strcat(result, get_positional_param(context, i + 1));
    }
    return result;
}

static char *expand_positional_quoted(struct ScriptContext *context)
{
    int count = get_positional_count(context);
    if (count == 0)
        return my_strdup("");

    /* Calcul de la longueur totale avec les guillemets */
    size_t total_len = 0;
    for (int i = 0; i < count; i++)
    {
        const char *param = get_positional_param(context, i + 1);
        total_len += strlen(param) + 2; // deux guillemets
        if (i < count - 1)
            total_len++; // espace
    }

    char *result = malloc(total_len + 1);
    if (!result)
        return NULL;

    result[0] = '\0';
    for (int i = 0; i < count; i++)
    {
        strcat(result, "\"");
        strcat(result, get_positional_param(context, i + 1));
        strcat(result, "\"");
        if (i < count - 1)
            strcat(result, " ");
    }
    return result;
}

// Expand Function
char *expand(const char *name, struct ScriptContext *context)
{
    if (!name)
        return NULL;

    if (strcmp(name, "?") == 0)
    {
        int code = get_last_exit_code();
        return format_to_string("%d", code);
    }
    else if (strcmp(name, "$") == 0)
    {
        //    pid_t pid = get_pid();
        //    return format_to_string("%d", (int)pid);
        return my_strdup("");
    }
    else if (strcmp(name, "UID") == 0)
    {
        uid_t uid = getuid();
        return format_to_string("%d", (int)uid);
    }
    else if (strcmp(name, "RANDOM") == 0 && context != NULL)
    {
        int random_val = get_random_number();
        return format_to_string("%d", random_val);
    }
    else if (strcmp(name, "PWD") == 0)
    {
        return get_pwd();
    }
    else if (strcmp(name, "OLDPWD") == 0)
    {
        return get_oldpwd();
    }
    // $# -> number of positional parameters
    else if (strcmp(name, "#") == 0 && context != NULL)
    {
        int count = get_positional_count(context);
        return format_to_string("%d", count);
    }

    // $* -> All positional parameters separated by a space
    else if (strcmp(name, "*") == 0 && context != NULL)
        return expand_positional_all(context);

    // $@ -> All positional parameters, each quoted, separated by space
    else if (strcmp(name, "@") == 0 && context != NULL)
        return expand_positional_quoted(context);

    // Single-digit positional parameters: $1, $2, ..., $9
    else if (name[0] >= '1' && name[0] <= '9' && name[1] == '\0'
             && context != NULL)
    {
        int index = name[0] - '0'; // '1' -> 1
        return my_strdup(get_positional_param(context, index));
    }

    const char *shell_val = get_shell_var(name);
    if (shell_val)
        return my_strdup(shell_val);

    const char *env_val = getenv(name);
    if (env_val)
        return my_strdup(env_val);

    // If none found, return empty string
    return my_strdup("");
}

int append_string(char **result, size_t *length, size_t *capacity,
                  const char *str)
{
    size_t str_len = strlen(str);
    while (*length + str_len + 1 >= *capacity)
    {
        *capacity *= 2;
        char *new_res = realloc(*result, *capacity);
        if (!new_res)
            return 0;
        *result = new_res;
    }
    memcpy((*result) + *length, str, str_len + 1);
    *length += str_len;
    return 1;
}

static void extract_varname(const char *input, size_t *i, char *varname,
                            size_t varname_size)
{
    size_t var_len = 0;
    memset(varname, 0, varname_size);

    if (strchr("?@*#", input[*i]) || isdigit((unsigned char)input[*i]))
    {
        varname[var_len++] = input[(*i)++];
    }
    else if (isalpha((unsigned char)input[*i]) || input[*i] == '_')
    {
        varname[var_len++] = input[(*i)++];
        while ((isalnum((unsigned char)input[*i]) || input[*i] == '_')
               && var_len < varname_size - 1)
        {
            varname[var_len++] = input[(*i)++];
        }
    }
    (*i)--;
    varname[var_len] = '\0';
}

char *expand_inline(const char *input, struct ScriptContext *context)
{
    if (!input)
        return my_strdup("");

    size_t capacity = 128;
    size_t length = 0;
    char *result = malloc(capacity);
    if (!result)
        return NULL;
    result[0] = '\0';

    size_t input_len = strlen(input);
    for (size_t i = 0; i < input_len; i++)
    {
        if (input[i] == '$')
        {
            i++;
            if (input[i] == '\0')
                break;

            char varname[128];
            extract_varname(input, &i, varname, sizeof(varname));

            if (strlen(varname) == 0)
            {
                if (!append_string(&result, &length, &capacity, "$"))
                    goto error;
                continue;
            }

            char *expanded = expand(varname, context);
            if (!expanded)
                expanded = my_strdup("");
            if (!expanded
                || !append_string(&result, &length, &capacity, expanded))
            {
                free(expanded);
                goto error;
            }
            free(expanded);
        }
        else
        {
            char temp[2] = { input[i], '\0' };
            if (!append_string(&result, &length, &capacity, temp))
                goto error;
        }
    }
    return result;
error:
    free(result);
    return NULL;
}

char **expand_list(char **token, int nb_tok, struct ScriptContext *context)
{
    if (!token)
        return NULL;

    char **expanded_tokens = malloc((nb_tok + 1) * sizeof(char *));
    if (!expanded_tokens)
        return NULL;

    for (int i = 0; i < nb_tok; i++)
    {
        char *expanded = expand_inline(token[i], context);
        if (!expanded)
        {
            for (int j = 0; j < i; j++)
                free(expanded_tokens[j]);
            free(expanded_tokens);
            return NULL;
        }

        expanded_tokens[i] = expanded;
    }

    expanded_tokens[nb_tok] = NULL;
    return expanded_tokens;
}

void free_expanded_list(char **expanded_tokens)
{
    if (expanded_tokens)
    {
        for (int i = 0; expanded_tokens[i]; i++)
        {
            free(expanded_tokens[i]);
        }
        free(expanded_tokens);
    }
}
