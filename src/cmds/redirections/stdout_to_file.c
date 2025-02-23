#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "redirection_utils.h"

int stdout_to_file(const char *file)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd == -1)
    {
        perror("stdout_to_file: open failed");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("stdout_to_file: dup2 failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}

int stdout_to_file_append(const char *file)
{
    int fd = open(file, O_WRONLY | O_CREAT | O_APPEND, 0644);
    if (fd == -1)
    {
        perror("stdout_to_file_append: open failed");
        return -1;
    }

    if (dup2(fd, STDOUT_FILENO) == -1)
    {
        perror("stdout_to_file_append: dup2 failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
