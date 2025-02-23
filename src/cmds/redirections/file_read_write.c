#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "redirection_utils.h"

int file_read_write(const char *file)
{
    int fd = open(file, O_RDWR | O_CREAT, 0644);
    if (fd == -1)
    {
        perror("file_read_write: open failed");
        return -1;
    }

    if (dup2(fd, STDIN_FILENO) == -1)
    {
        perror("file_read_write: dup2 failed");
        close(fd);
        return -1;
    }

    close(fd);
    return 0;
}
