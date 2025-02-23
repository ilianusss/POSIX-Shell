#ifndef IO_H
#define IO_H

#include <stdio.h>
#include <unistd.h>

int init_input_from_cmd(FILE **file, char *cmd);
int init_input_from_file(int *fd, char *path);
int close_input(FILE **file, int *fd);
char *read_input(FILE **file, int *fd);

#endif /* ! IO_H */
