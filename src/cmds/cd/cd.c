#include "cd.h"

#include <unistd.h>

#include "../var/expand_var_utils.h"

int builtin_cd(int argc, char **argv)
{
    if (argc > 2)
        return 1;
    if (strcmp(argv[1], "-") == 0)
    {
        char *old_path = get_oldpwd();
        if (chdir(old_path) == -1)
            return 1;
        set_pwd(old_path);
        printf("%s\n", old_path);
        free(old_path);
    }
    else
    {
        if (chdir(argv[1]) == -1)
            return 1;
        char path[1000];
        char *real_path = getcwd(path, 1000);
        if (real_path == NULL)
            return 1;
        set_pwd(real_path);
    }
    return 0;
}
