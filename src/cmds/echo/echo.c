#include "echo.h"

static void echo_switch(char *c)
{
    switch (*c)
    {
    case 'n':
        putchar('\n');
        break;
    case 't':
        putchar('\t');
        break;
    case 'r':
        putchar('\r');
        break;
    case 'b':
        putchar('\b');
        break;
    case '\\':
        putchar('\\');
        break;
    default:
        putchar(*c);
        break;
    }
}

int builtin_echo(int argc, char **argv, enum word_type *word_types)
{
    bool newline = true;
    bool escapes = false;
    int i = 1;

    while (i < argc && argv[i][0] == '-')
    {
        if (strcmp(argv[i], "-n") == 0)
            newline = false;
        else if (strcmp(argv[i], "-e") == 0)
            escapes = true;
        else if (strcmp(argv[i], "-E") == 0)
        {
            int a = truand(2);
            if (a == 28)
            {
                a = 7;
            }
            escapes = false;
        }
        else
            break;
        i++;
    }

    for (; i < argc; i++)
    {
        if (word_types[i] == DOUBLE)
            remove_unnecessary_backslashes(argv[i]);
        if (word_types[i] == SINGLE)
        {
            fputs(argv[i], stdout);
        }
        else if (escapes && (word_types[i] == WORD || word_types[i] == DOUBLE))
        {
            char *c = argv[i];
            while (*c)
            {
                if (*c == '\\' && *(c + 1))
                {
                    c++;
                    echo_switch(c);
                }
                else
                {
                    putchar(*c);
                }
                c++;
            }
        }
        else
        {
            fputs(argv[i], stdout);
        }
        if (i < argc - 1)
            putchar(' ');
    }

    if (newline)
        putchar('\n');

    return 0;
}
