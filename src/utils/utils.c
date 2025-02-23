#include "utils.h"

static int is_escapable_char(char c)
{
    return (c == '"' || c == '\\');
}

void remove_unnecessary_backslashes(char *str)
{
    if (!str)
        return;

    char *src = str;
    char *dest = str;
    while (*src)
    {
        if (*src == '\\' && is_escapable_char(*(src + 1)))
        {
            src++; // Skip the backslash if followed by an escapable character
        }
        *dest++ = *src++;
    }
    *dest = '\0'; // Null terminate the modified string
}

char *my_strdup(const char *src)
{
    if (!src)
        return NULL;
    size_t len = strlen(src);
    char *dup = malloc(len + 1);
    if (dup)
        memcpy(dup, src, len + 1);
    return dup;
}

int is_redirection_token(int token_type)
{
    return token_type == TOKEN_REDIRECT_OUT || // >
        token_type == TOKEN_REDIRECT_OUT_APPEND || // >>
        token_type == TOKEN_REDIRECT_IN || // <
        token_type == TOKEN_DUP_OUT_DESC || // >&
        token_type == TOKEN_DUP_IN_DESC || // <&
        token_type == TOKEN_REDIRECT_OUT_FORCE || // >|
        token_type == TOKEN_REDIRECT_READ_WRITE || // <>
        token_type == TOKEN_HEREDOC; // <<
}

int is_numeric(const char *str)
{
    if (!str || *str == '\0')
    {
        return 0;
    }
    for (const char *ptr = str; *ptr != '\0'; ptr++)
    {
        if (!isdigit((unsigned char)*ptr))
        {
            return 0;
        }
    }
    return 1;
}

int check_quote(const char *str)
{
    int in_single = 0; // 1 if inside single quotes, 0 otherwise
    int in_double = 0; // 1 if inside double quotes, 0 otherwise
    int escaped = 0; // 1 if current character is escaped by '\', 0 otherwise

    for (; *str; str++)
    {
        if (escaped)
        {
            // Current character is treated literally, so reset escaped
            escaped = 0;
        }
        else
        {
            // Check if current character is the escape backslash
            if (*str == '\\')
            {
                escaped = 1;
                continue;
            }
            // Toggle single quotes if we see a ' and we're not inside double
            // quotes
            if (*str == '\'' && !in_double)
            {
                in_single = !in_single;
                continue;
            }
            // Toggle double quotes if we see a " and we're not inside single
            // quotes
            if (*str == '\"' && !in_single)
            {
                in_double = !in_double;
                continue;
            }
        }
    }

    // If we're still inside single or double quotes, they're not properly
    // closed
    if (in_single || in_double)
    {
        return 0;
    }
    return 1;
}
