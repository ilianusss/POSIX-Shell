#ifndef REDIRECTION_UTILS_H
#define REDIRECTION_UTILS_H

int file_to_stdout(const char *file);
int stdout_to_file(const char *file);
int stdout_to_file_append(const char *file);
int stdout_to_file_force(const char *file); // For `>|`
int file_read_write(const char *file); // For `<>`

#endif /* REDIRECTION_UTILS_H */
