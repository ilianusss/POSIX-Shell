#include <stdio.h>

#include "parser.h"

// STEP 1 - PARSE TOKENS: Entry point
struct node *parse_tokens(struct lexer *lexer)
{
    if (!lexer)
    {
        fprintf(stderr, "input.c: parse_tokens: Lexer is NULL.\n");
        return NULL;
    }

    struct token current = lexer_peek(lexer);
    if (current.type == TOKEN_EOF)
        return NULL;

    struct node *root = parse_input(lexer);
    if (!root)
        return NULL;

    current = lexer_peek(lexer);
    if (current.type == TOKEN_ENDLINE)
    {
        lexer_pop(lexer);
        current = lexer_peek(lexer);
    }

    if (current.type != TOKEN_EOF)
    {
        fprintf(stderr, "input.c: parse_tokens: Expected end of input\n");
        root->table->free(root);
        return NULL;
    }

    return root;
}

// STEP 2 - PARSE INPUT: Parse the overall input
struct node *parse_input(struct lexer *lexer)
{
    struct token current = lexer_peek(lexer);
    struct node **commands = NULL;
    size_t count = 0;
    size_t capacity = 0;

    // If input is empty (EOF or ENDLINE), create a root node with no child
    while (current.type != TOKEN_EOF)
    {
        current = lexer_peek(lexer);
        while (current.type == TOKEN_ENDLINE)
        {
            lexer_pop(lexer);
            current = lexer_peek(lexer);
        }
        if (current.type == TOKEN_EOF)
            break;

        // Otherwise, parse the list of commands
        struct node *list = parse_list(lexer);
        if (!list)
        {
            fprintf(stderr, "input.c: parse_input: Failed to parse list\n");
            return NULL;
        }

        if (count + 1 >= capacity)
        {
            size_t new_capacity = (capacity == 0) ? 4 : capacity * 2;
            struct node **new_commands =
                realloc(commands, new_capacity * sizeof(struct node *));
            commands = new_commands;
            capacity = new_capacity;
        }

        commands[count++] = list;
        commands[count] = NULL;

        if (lexer_peek(lexer).type != TOKEN_ENDLINE
            && lexer_peek(lexer).type != TOKEN_EOF)
        {
            fprintf(stderr, "input.c: parse_input: Expected end of input\n");
            goto error;
        }
    }

    struct node *root = (struct node *)root_node_create(commands, count);
    if (!root)
    {
        fprintf(stderr, "structures.c:parse_compound_l:Failed to create\n");
        goto error;
    }
    if (!root)
    {
        fprintf(stderr, "input.c:parse_input:Failed create root empty\n");
    }
    return root;

error:
    for (size_t i = 0; i < count; i++)
        if (commands[i])
            commands[i]->table->free(commands[i]);
    free(commands);
    return NULL;
}

// STEP 3 - PARSE LIST: Parse a list of commands
struct node *parse_list(struct lexer *lexer)
{
    struct node *first = parse_and_or(lexer);
    if (!first)
    {
        fprintf(stderr, "input.c: parse_list: Failed to parse and_or.\n");
        return NULL;
    }

    // Initialize commands array with the first command
    size_t capacity = 2;
    size_t count = 1;
    struct node **commands = malloc(capacity * sizeof(struct node *));
    commands[0] = first;
    commands[1] = NULL;

    // Parse additional commands separated by ';'
    struct token current = lexer_peek(lexer);
    while (current.type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);

        // If next token is EOF or ENDLINE, stop parsing additional commands
        if (lexer_peek(lexer).type == TOKEN_EOF
            || lexer_peek(lexer).type == TOKEN_ENDLINE)
        {
            break;
        }

        struct node *next = parse_and_or(lexer);
        if (!next)
        {
            fprintf(stderr, "input.c: parse_list: Fail after ';'.\n");
            goto error;
        }

        // Resize the commands array if necessary
        if (count + 1 >= capacity)
        {
            capacity *= 2;
            struct node **new_commands =
                realloc(commands, capacity * sizeof(struct node *));
            commands = new_commands;
        }

        // Add the new command
        commands[count++] = next;
        commands[count] = NULL;

        current = lexer_peek(lexer);
    }

    // Create a command_list_node with the collected commands
    struct node *command_list =
        (struct node *)command_list_node_create(commands);
    if (!command_list)
    {
        fprintf(stderr, "input.c: parse_list: Fail to command node.\n");
        goto error;
    }

    return command_list;

error:
    for (size_t i = 0; i < count; i++)
    {
        if (commands[i])
            commands[i]->table->free(commands[i]);
    }
    free(commands);
    return NULL;
}

// STEP 4 - PARSE AND_OR: Parse '&&' and '||'
struct node *parse_and_or(struct lexer *lexer)
{
    // Parse the left pipeline
    if (lexer_peek(lexer).type == TOKEN_AND
        || lexer_peek(lexer).type == TOKEN_OR
        || lexer_peek(lexer).type == TOKEN_EOF)
    {
        fprintf(stderr, "input.c:parse_and_or:Failed to parse or,and,pipe\n");
        return NULL;
    }

    struct node *left = parse_pipeline(lexer);
    if (!left)
    {
        fprintf(stderr,
                "input.c:parse_and_or:Failed to parse left pipeline.\n");
        return NULL;
    }

    // Check for '&&' or '||'
    struct token current = lexer_peek(lexer);
    while (current.type == TOKEN_AND || current.type == TOKEN_OR)
    {
        int is_and = (current.type == TOKEN_AND);
        lexer_pop(lexer); // Consume && or ||
        if (lexer_peek(lexer).type == TOKEN_AND
            || lexer_peek(lexer).type == TOKEN_OR
            || lexer_peek(lexer).type == TOKEN_EOF)
        {
            fprintf(stderr, "input.c: parse_and_or: Failed to parse secondary");
            left->table->free(left);
            return NULL;
        }
        // Skip initial newlines
        current = lexer_peek(lexer);
        while (current.type == TOKEN_ENDLINE)
        {
            lexer_pop(lexer);
            current = lexer_peek(lexer);
        }
        // Parse the right pipeline
        struct node *right = parse_pipeline(lexer);
        if (!right)
        {
            fprintf(stderr,
                    "input.c: parse_and_or: Failed to parse right pipeline\n");
            left->table->free(left);
            return NULL;
        }

        // Create and_or_node
        struct and_or_node *aon = and_or_node_create(left, right, is_and);
        if (!aon)
        {
            fprintf(stderr,
                    "input.c: parse_and_or: Failed to create and_or_node.\n");
            left->table->free(left);
            right->table->free(right);
            return NULL;
        }

        // The new node becomes the 'left' for the next iteration
        left = (struct node *)aon;
        current = lexer_peek(lexer);
    }

    return left;
}

// STEP 5 - PARSE PIPELINE: Parse a pipeline of commands
struct node *wrap_in_negation(struct node *node)
{
    struct negation_node *neg_node = negation_node_create(node);
    if (!neg_node)
    {
        fprintf(stderr, "input.c: pipe: nega_node.\n");
        node->table->free(node);
        return NULL;
    }
    return (struct node *)neg_node;
}

struct node **resize_commands_array(struct node **commands, size_t *capacity)
{
    *capacity *= 2;
    struct node **new_commands =
        realloc(commands, *capacity * sizeof(struct node *));
    return new_commands;
}

struct node **parse_piped_commands(struct lexer *lexer, struct node *first,
                                   size_t *count)
{
    size_t capacity = 2;
    struct node **commands = malloc(capacity * sizeof(struct node *));
    commands[0] = first;
    commands[1] = NULL;
    struct token current = lexer_peek(lexer);

    while (current.type == TOKEN_PIPE)
    {
        lexer_pop(lexer);
        if (lexer_peek(lexer).type == TOKEN_PIPE
            || lexer_peek(lexer).type == TOKEN_EOF)
        {
            fprintf(stderr, "input.c: pipe: nothing after |\n");
            goto error;
        }
        struct node *next = parse_command(lexer);
        if (!next)
        {
            fprintf(stderr, "input.c: pipe: Fail after |\n");
            goto error;
        }

        if (*count + 1 >= capacity)
        {
            struct node **new_commands =
                resize_commands_array(commands, &capacity);
            commands = new_commands;
        }

        commands[(*count)++] = next;
        commands[*count] = NULL;
        current = lexer_peek(lexer);
    }

    return commands;

error:
    for (size_t i = 0; i < *count; i++)
    {
        if (commands[i])
            commands[i]->table->free(commands[i]);
    }
    free(commands);
    return NULL;
}

struct node *parse_pipeline(struct lexer *lexer)
{
    struct token current = lexer_peek(lexer);
    int negate = 0;

    if (current.type == TOKEN_PIPE)
    {
        fprintf(stderr, "input.c: pipeline: Fail nothing before\n");
        return NULL;
    }

    if (current.type == TOKEN_NEGATION)
    {
        negate = 1;
        lexer_pop(lexer);
    }

    struct node *first = parse_command(lexer);
    if (!first)
    {
        fprintf(stderr, "input.c: pipeline: Fail first command\n");
        return NULL;
    }

    current = lexer_peek(lexer);
    if (!(current.type == TOKEN_PIPE))
        return negate ? wrap_in_negation(first) : first;

    size_t count = 1;
    struct node **commands = parse_piped_commands(lexer, first, &count);
    if (!commands)
        return NULL;

    struct node *pipeline = (struct node *)pipeline_node_create(commands);
    if (!pipeline)
    {
        fprintf(stderr, "input.c: pipe: Fail to create node.\n");
        goto error;
    }

    return negate ? wrap_in_negation(pipeline) : pipeline;

error:
    for (size_t i = 0; i < count; i++)
    {
        if (commands[i])
            commands[i]->table->free(commands[i]);
    }
    free(commands);
    return NULL;
}
