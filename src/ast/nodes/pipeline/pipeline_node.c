#include "pipeline_node.h"

// CREATE
struct pipeline_node *pipeline_node_create(struct node **commands)
{
    struct pipeline_node *pn = malloc(sizeof(struct pipeline_node));

    static struct vtable pipeline_node_vtable = {
        .exec = pipeline_node_exec,
        .free = pipeline_node_destroy,
    };

    pn->node.table = &pipeline_node_vtable;
    pn->commands = commands;

    return pn;
}

// EXEC PIPELINE NODE
static int create_pipe(int pipefd[2])
{
    if (pipe(pipefd) == -1)
    {
        fprintf(stderr, "create_pipe: Failed to create pipe: %s\n",
                strerror(errno));
        return -1;
    }
    return 0;
}

static pid_t safe_fork(void)
{
    pid_t pid = fork();
    if (pid == -1)
    {
        fprintf(stderr, "safe_fork: Failed to fork process: %s\n",
                strerror(errno));
    }
    return pid;
}

static int redirect_input(int pipefd[2])
{
    if (dup2(pipefd[0], STDIN_FILENO) == -1)
    {
        fprintf(stderr, "redirect_input: Failed to redirect input: %s\n",
                strerror(errno));
        return -1;
    }
    return 0;
}

static int redirect_output(int pipefd[2])
{
    if (dup2(pipefd[1], STDOUT_FILENO) == -1)
    {
        fprintf(stderr, "redirect_output: Failed to redirect output: %s\n",
                strerror(errno));
        return -1;
    }
    return 0;
}

static void close_unused_pipes(int pipes[][2], size_t num_pipes)
{
    for (size_t i = 0; i < num_pipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static void execute_command(struct node *command)
{
    int status = command->table->exec(command);
    exit(status);
}

static void close_all_pipes(int pipes[][2], size_t num_pipes)
{
    for (size_t i = 0; i < num_pipes; i++)
    {
        close(pipes[i][0]);
        close(pipes[i][1]);
    }
}

static int wait_for_children(size_t num_children)
{
    int status = 0;
    for (size_t i = 0; i < num_children; i++)
    {
        if (wait(&status) == -1)
        {
            fprintf(stderr,
                    "wait_for_children: Failed to wait for child process: %s\n",
                    strerror(errno));
            return -1;
        }
    }
    return status;
}

// Main function
int pipeline_node_exec(void *param)
{
    struct pipeline_node *pn = (struct pipeline_node *)param;
    if (!pn || !pn->commands)
    {
        fprintf(stderr, "pipeline_node_exec: Invalid pipeline_node.\n");
        return -1;
    }

    size_t num_commands = 0;
    while (pn->commands[num_commands] != NULL)
        num_commands++;

    if (num_commands == 0)
    {
        fprintf(stderr, "pipeline_node_exec: No commands in pipeline.\n");
        return -1;
    }

    int(*pipes)[2] = NULL;
    if (num_commands > 1)
    {
        pipes = malloc((num_commands - 1) * sizeof(*pipes));
        if (!pipes)
        {
            fprintf(stderr, "Failed to allocate memory for pipes.\n");
            return -1;
        }
    }

    for (size_t i = 0; i < num_commands; i++)
    {
        if (i < num_commands - 1)
        {
            if (create_pipe(pipes[i]) == -1)
                goto error_parent;
        }

        pid_t pid = safe_fork();
        if (pid == -1)
            goto error_parent;
        else if (pid == 0)
        {
            if (i > 0 && redirect_input(pipes[i - 1]) == -1)
                goto error_child;

            if (i < num_commands - 1 && redirect_output(pipes[i]) == -1)
                goto error_child;

            close_unused_pipes(pipes, num_commands - 1);

            execute_command(pn->commands[i]);

        error_child:
            _exit(EXIT_FAILURE);
        }
    }

    close_all_pipes(pipes, num_commands - 1);
    free(pipes);

    return wait_for_children(num_commands);

error_parent:
    close_all_pipes(pipes, num_commands - 1);
    free(pipes);
    return -1;
}

// DESTROY
void pipeline_node_destroy(void *param)
{
    struct pipeline_node *pn = (struct pipeline_node *)param;
    if (!pn)
    {
        fprintf(stderr, "pipeline_node_destroy: invalid pointer\n");
        return;
    }

    for (int i = 0; pn->commands[i] != NULL; i++)
    {
        pn->commands[i]->table->free(pn->commands[i]);
    }

    free(pn->commands);

    free(pn);
}
