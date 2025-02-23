#define _POSIX_C_SOURCE 200809L

#include "io.h"

#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define CHUNK_SIZE 1024

// Initialise l'input depuis une commande donnée en utilisant fmemopen
int init_input_from_cmd(FILE **file, char *cmd)
{
    *file = fmemopen(cmd, strlen(cmd), "r");
    if (*file == NULL)
        return EXIT_FAILURE;
    return EXIT_SUCCESS;
}

// Ouvre un fichier et conserve le fd
int init_input_from_file(int *fd, char *path)
{
    int new_fd = open(path, O_RDONLY);
    if (new_fd == -1)
        return EXIT_FAILURE;
    *fd = new_fd;
    return EXIT_SUCCESS;
}

// Ferme l'input proprement
int close_input(FILE **file, int *fd)
{
    if (*fd >= 2)
    {
        if (close(*fd) == -1)
            return EXIT_FAILURE;
    }
    else if (*file != NULL)
    {
        if (fclose(*file) == EOF)
            return EXIT_FAILURE;
    }
    return EXIT_SUCCESS;
}

char *read_input(FILE **file, int *fd)
{
    size_t current_size = CHUNK_SIZE; // Taille initiale du buffer
    size_t total_read = 0; // Nombre total de caractères lus
    ssize_t bytes_read = 0;
    char *buffer = malloc(current_size);

    if (!buffer)
    {
        perror("malloc");
        return NULL;
    }

    while (1)
    {
        // S'assurer que le buffer est suffisamment grand
        if (total_read + CHUNK_SIZE > current_size)
        {
            current_size *= 2;
            char *new_buffer = realloc(buffer, current_size);
            if (!new_buffer)
            {
                perror("realloc");
                free(buffer);
                return NULL;
            }
            buffer = new_buffer;
        }

        // Lecture des données
        if (*fd != -1)
        {
            bytes_read = read(*fd, buffer + total_read, CHUNK_SIZE);
            if (bytes_read < 0)
            {
                perror("read");
                free(buffer);
                return NULL;
            }
        }
        else
        {
            bytes_read = fread(buffer + total_read, 1, CHUNK_SIZE, *file);
            if (bytes_read == 0 && ferror(*file))
            {
                perror("fread");
                free(buffer);
                return NULL;
            }
        }

        total_read += bytes_read;

        // Vérification de la fin du flux
        if (bytes_read == 0 || ((*fd != -1) && bytes_read < CHUNK_SIZE))
            break;
    }

    // Ajouter le caractère de fin de chaîne
    if (total_read + 1 > current_size)
    {
        char *new_buffer = realloc(buffer, total_read + 1);
        if (!new_buffer)
        {
            perror("realloc");
            free(buffer);
            return NULL;
        }
        buffer = new_buffer;
    }
    buffer[total_read] = '\0';

    return buffer;
}
