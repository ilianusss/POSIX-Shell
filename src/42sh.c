#include <getopt.h>
#include <stdbool.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#include "ast/ast.h"
#include "cmds/var/context.h"
#include "cmds/var/expand_var.h"
#include "cmds/var/expand_var_utils.h"
#include "io/io.h"
#include "lexer/lexer.h"
#include "lexer/token.h"
#include "parser/parser.h"
#include "utils/utils.h"

struct main_context
{
    char *buffer;
    struct lexer *lex;
    struct node *ast;
    FILE *file;
    int fd;
};

void free_all_and_can_exit(struct main_context *main_c, bool is_interactive)
{
    if (main_c->ast != NULL)
    {
        main_c->ast->table->free(main_c->ast);
        main_c->ast = NULL;
    }
    if (main_c->lex != NULL)
    {
        lexer_free(main_c->lex);
        main_c->lex = NULL;
    }
    if (main_c->file != NULL || main_c->fd >= 2)
        if (close_input(&main_c->file, &main_c->fd) == EXIT_FAILURE)
            exit(EXIT_FAILURE);

    if (main_c->buffer != NULL)
        free(main_c->buffer);

    if (!is_interactive)
    {
        free(main_c);
        exit(get_last_exit_code());
    }
}

int parse_args(int *argc, char ***argv, FILE **file)
{
    int opt;
    while ((opt = getopt(*argc, *argv, "c:")) != -1)
    {
        switch (opt)
        {
        case 'c': {
            if (init_input_from_cmd(file, optarg) == EXIT_FAILURE)
            {
                set_last_exit_code(1);
                return 1;
            }
            break;
        }
        default: {
            set_last_exit_code(2);
            return 1;
        }
        }
    }
    return 0;
}

int parse_file(int *argc, char ***argv, FILE **file, int *fd)
{
    for (int i = optind; i < *argc; i++)
    {
        if (*file == NULL && *fd == -1)
        {
            if (init_input_from_file(fd, (*argv)[i]) == EXIT_FAILURE)
            {
                fprintf(stderr,
                        "src/42sh.c: parse_file: Not such file or directory\n");
                set_last_exit_code(1);
                return 1;
            }
        }
        else
        {
            set_last_exit_code(2);
            return 1;
        }
    }
    return 0;
}

int main(int argc, char *argv[])
{
    bool is_interactive = false;
    struct main_context *main_c = calloc(1, sizeof(struct main_context));
    main_c->fd = -1;

    // Analyse les options dans la commande
    if (parse_args(&argc, &argv, &main_c->file) == 1)
    {
        free_all_and_can_exit(main_c, false);
    }

    // Utilise le fichier optionnel renseigné pour l'input
    if (parse_file(&argc, &argv, &main_c->file, &main_c->fd) == 1)
    {
        free_all_and_can_exit(main_c, false);
    }

    if (main_c->file == NULL && main_c->fd == -1)
    {
        is_interactive = true;
        main_c->fd = 0;
    }

    init_shell_env();
    while (true)
    {
        main_c->buffer = read_input(&main_c->file, &main_c->fd);

        if (check_quote(main_c->buffer) == 0)
        {
            fprintf(stderr, "main: error matching quote\n");
            set_last_exit_code(2);
            free(main_c->buffer);
            continue;
        }

        main_c->lex = lexer_new(main_c->buffer);

        if (lexer_peek(main_c->lex).type == TOKEN_EOF)
        {
            free_all_and_can_exit(main_c, false);
        }

        main_c->ast = parse_tokens(main_c->lex);
        if (!main_c->ast)
        {
            fprintf(stderr, "main: failed to parse input\n");
            set_last_exit_code(2);
        }
        else
        {
            int exec_status = main_c->ast->table->exec(main_c->ast);
            set_last_exit_code(exec_status);
        }

        free_all_and_can_exit(main_c, is_interactive);
    }
}
