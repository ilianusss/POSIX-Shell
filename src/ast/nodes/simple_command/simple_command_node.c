#include "simple_command_node.h"

#include "../../../cmds/cd/cd.h"
#include "../../../cmds/dot/dot.h"
#include "../../../cmds/echo/echo.h"
#include "../../../cmds/export/export.h"
#include "../../../cmds/false/false.h"
#include "../../../cmds/true/true.h"
#include "../../../cmds/var/expand_var_utils.h"
#include "../redirections/redirect_node.h"

// CREATE
struct simple_command_node *
simple_command_node_create(char **argv, int argc, enum word_type *word_types)
{
    struct simple_command_node *cmd =
        malloc(sizeof(struct simple_command_node));

    static struct vtable simple_command_vtable = {
        .exec = simple_command_node_exec,
        .free = simple_command_node_destroy,
    };

    cmd->node.table = &simple_command_vtable;
    // here, may have to allocate and do a deep copy, but askip le parser alloue
    cmd->argv = argv;
    cmd->argc = argc;
    cmd->word_types = word_types;
    cmd->redirections = NULL; // Initialize to NULL
    cmd->redir_count = 0; // Initialize to 0

    return cmd;
}

// EXEC SIMPLE COMMAND NODE
// Execute builtin commands
static int run_builtin(char **argv, int argc, enum word_type *word_types)
{
    if (strcmp(argv[0], "echo") == 0)
    {
        return builtin_echo(argc, argv, word_types);
    }
    else if (strcmp(argv[0], "true") == 0)
    {
        return builtin_true();
    }
    else if (strcmp(argv[0], "export") == 0)
    {
        return builtin_export(argc, argv);
    }
    else if (strcmp(argv[0], "false") == 0)
    {
        return builtin_false();
    }
    else if (strcmp(argv[0], "cd") == 0)
    {
        return builtin_cd(argc, argv);
    }
    else if (strcmp(argv[0], ".") == 0)
    {
        return builtin_dot(argc, argv);
    }

    return -1;
}

// Execute external commands
static int run_external(char **argv)
{
    pid_t pid = fork();

    if (pid == -1)
    {
        fprintf(stderr, "simple_command_node.c: run external: execvp failed\n");
        return -1;
    }

    if (pid == 0)
    {
        // Child process: execute the external command
        execvp(argv[0], argv);
        fprintf(stderr, "simple_command_node.c: run external: execvp failed\n");
        _exit(127);
    }

    // Parent process: wait for the child to finish
    int status;
    waitpid(pid, &status, 0);
    if (WIFEXITED(status))
    {
        return WEXITSTATUS(status); // Return the child's exit status
    }
    return -1;
}

static int simple_command_node_exec_simple(void *param)
{
    struct simple_command_node *cmd = (struct simple_command_node *)param;

    if (!cmd->argv || !cmd->argv[0])
    {
        fprintf(stderr, "simple_command_node_exec: Command has no arguments\n");
        return -1;
    }
    struct ScriptContext *context = init_script_context(cmd->argc, cmd->argv);
    if (!context)
    {
        fprintf(stderr, "Failed to initialize script context\n");
        return -1;
    }

    char **copy = malloc((cmd->argc + 1) * sizeof(char *));
    for (int i = 0; i < cmd->argc; i++)
    {
        if (cmd->word_types[i] != SINGLE)
        {
            char *expanded = expand_inline(cmd->argv[i], context);
            copy[i] = expanded;
        }
        else
        {
            copy[i] = my_strdup(cmd->argv[i]);
        }
    }

    copy[cmd->argc] = NULL;

    // Execute the built-in command first
    int result = run_builtin(copy, cmd->argc, cmd->word_types);
    if (result != -1)
    {
        free_expanded_list(copy);
        free_script_context(context); // Free the context before exiting
        set_last_exit_code(result);
        return result; // Built-in command executed successfully
    }

    // If it's not a built-in, run as an external command
    result = run_external(copy);

    free_expanded_list(copy);
    free_script_context(context); // Free the context before exiting

    set_last_exit_code(result);
    return result;
}

// EXEC SIMPLE COMMAND NODE
static char **prepare_copy(struct simple_command_node *cmd,
                           struct ScriptContext *context)
{
    char **copy = malloc((cmd->argc + 1) * sizeof(char *));
    for (int i = 0; i < cmd->argc; i++)
    {
        if (cmd->word_types[i] != SINGLE)
        {
            char *expanded = expand_inline(cmd->argv[i], context);
            copy[i] = expanded;
        }
        else
        {
            copy[i] = my_strdup(cmd->argv[i]);
        }
    }
    copy[cmd->argc] = NULL;
    return copy;
}

static void cleanup_dup2(int saved_stdin, int saved_stdout, int saved_stderr)
{
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    dup2(saved_stderr, STDERR_FILENO);
    close(saved_stdin);
    close(saved_stdout);
    close(saved_stderr);
}

static int handle_redirections_and_execute(struct simple_command_node *cmd,
                                           char **copy)
{
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    int saved_stderr = dup(STDERR_FILENO);
    if (saved_stdin == -1 || saved_stdout == -1 || saved_stderr == -1)
    {
        perror("simple_command_node_exec: Failed to save file descriptors");
        return -1;
    }
    fflush(stdout);
    fflush(stderr);
    int result = -1;
    for (size_t i = 0; i < cmd->redir_count; i++)
    {
        struct redirection_node *rn =
            (struct redirection_node *)cmd->redirections[i];
        if (rn->fd == -1 && ((struct node *)rn)->table->exec(rn) == -1)
        {
            fprintf(
                stderr,
                "simple_command_node_exec: Failed to prepare redirection\n");
            goto cleanup;
        }
        if (dup2(rn->fd, rn->fd_target) == -1)
        {
            perror("simple_command_node_exec: dup2 failed");
            goto cleanup;
        }
        {
            int result = run_builtin(copy, cmd->argc, cmd->word_types);
            if (result == -1)
            {
                result = run_external(cmd->argv);
                if (result == -1)
                    goto cleanup;
            }
        }
        fflush(stdout);
        fflush(stderr);
    }
    result = 0;
cleanup:
    cleanup_dup2(saved_stdin, saved_stdout, saved_stderr);
    for (size_t i = 0; i < cmd->redir_count; i++)
    {
        struct redirection_node *rn =
            (struct redirection_node *)cmd->redirections[i];
        if (rn->type != TOKEN_DUP_OUT_DESC && rn->type != TOKEN_DUP_IN_DESC
            && rn->fd != -1)
        {
            close(rn->fd);
        }
    }
    return result;
}

int simple_command_node_exec(void *param)
{
    struct simple_command_node *cmd = (struct simple_command_node *)param;
    if (!cmd->argv || !cmd->argv[0])
    {
        fprintf(stderr, "simple_command_node_exec: Command has no arguments\n");
        return -1;
    }
    if (cmd->redir_count == 0)
        return simple_command_node_exec_simple(param);
    struct ScriptContext *context = init_script_context(cmd->argc, cmd->argv);
    if (!context)
    {
        fprintf(stderr, "Failed to initialize script context\n");
        return -1;
    }
    char **copy = prepare_copy(cmd, context);
    int result = handle_redirections_and_execute(cmd, copy);
    free_script_context(context);
    free_expanded_list(copy);
    set_last_exit_code(result);
    return result;
}

// DESTROY
void simple_command_node_destroy(void *param)
{
    struct simple_command_node *cmd = (struct simple_command_node *)param;
    if (!cmd)
    {
        fprintf(stderr, "simple_command_node_destroy: NULL parameter\n");
        return;
    }

    for (int i = 0; i < cmd->argc; i++)
    {
        free(cmd->argv[i]);
    }
    free(cmd->argv);
    free(cmd->word_types);

    if (cmd->redirections)
    {
        for (size_t i = 0; i < cmd->redir_count; i++)
        {
            if (cmd->redirections[i])
            {
                cmd->redirections[i]->table->free(cmd->redirections[i]);
            }
        }
        free(cmd->redirections);
    }

    free(cmd);
}
