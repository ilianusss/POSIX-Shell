#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>

#include "redirection_utils.h"

int file_to_stdout(const char *file)
{
    int fd = open(file, O_RDONLY);
    if (fd == -1)
    {
        perror("file_to_stdout: open failed");
        return -1;
    }

    char buffer[4096];
    ssize_t bytes_read;
    while ((bytes_read = read(fd, buffer, sizeof(buffer))) > 0)
    {
        if (write(STDOUT_FILENO, buffer, bytes_read) == -1)
        {
            perror("file_to_stdout: write failed");
            close(fd);
            return -1;
        }
    }

    if (bytes_read == -1)
    {
        perror("file_to_stdout: read failed");
    }

    close(fd);
    return (bytes_read == -1) ? -1 : 0;
}
