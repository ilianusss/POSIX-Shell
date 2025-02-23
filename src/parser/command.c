#include <stdio.h>
#include <string.h>

#include "../ast/nodes/simple_command/word_types.h"
#include "parser.h"

// STEP 6 - PARSE COMMAND: Parse a single command
struct node *parse_command(struct lexer *lexer)
{
    struct token current = lexer_peek(lexer);

    if (current.type == TOKEN_IF)
        return parse_if_command(lexer);
    else if (current.type == TOKEN_WHILE || current.type == TOKEN_UNTIL)
        return parse_loop(lexer);
    else if (current.type == TOKEN_FOR)
        return parse_for(lexer);
    else if (current.type == TOKEN_CASE)
        return parse_case(lexer);

    // PARSE FUNCTIONS

    // Check for shell command:
    if (current.type == TOKEN_LEFT_BRACE || current.type == TOKEN_SUBSHELL_OPEN)
        return parse_shell_command(lexer);

    if (current.type == TOKEN_WORD)
        return parse_simple_command(lexer);
    fprintf(stderr, "parser.c: parse_command: expected word.\n");
    return NULL;
}

// STEP 6.5 - PARSE FUNCDEC
struct node *parse_funcdec(struct lexer *lexer)
{
    // Expect WORD '(' ')' then a shell_command
    // Function name
    struct token fn_name = lexer_peek(lexer);
    if (fn_name.type != TOKEN_WORD)
    {
        fprintf(stderr, "command.c: parse_funcdec: Expected function name.\n");
        return NULL;
    }
    lexer_pop(lexer); // consume the name

    // '('
    struct token current = lexer_peek(lexer);
    if (current.type != TOKEN_SUBSHELL_OPEN)
    {
        fprintf(
            stderr,
            "command.c: parse_funcdec: Expected '(' after function name.\n");
        return NULL;
    }
    lexer_pop(lexer); // consume '('

    // ')'
    current = lexer_peek(lexer);
    if (current.type != TOKEN_SUBSHELL_CLOSE)
    {
        fprintf(stderr,
                "command.c: parse_funcdec: Expected ')' in function "
                "declaration.\n");
        return NULL;
    }
    lexer_pop(lexer); // consume ')'

    // Skip newlines
    while (lexer_peek(lexer).type == TOKEN_ENDLINE)
        lexer_pop(lexer);

    // Now parse the shell command as the function body
    struct node *body = parse_shell_command(lexer);
    if (!body)
    {
        fprintf(stderr,
                "command.c: parse_funcdec: Failed to parse function body.\n");
        return NULL;
    }

    // HERE, call function_node_create(fn_name.word, body);

    // Modify this, j'ai mis body pour pas casser
    return body;
}

// STEP 6.6 - PARSE SHELL COMMAND: '{ ... }' or '( ... )'
struct node *parse_shell_command(struct lexer *lexer)
{
    struct token current = lexer_peek(lexer);

    if (current.type != TOKEN_LEFT_BRACE && current.type != TOKEN_SUBSHELL_OPEN)
    {
        fprintf(stderr,
                "command.c: parse_shell_command: Expected '{' or '('.\n");
        return NULL;
    }

    int is_brace = (current.type == TOKEN_LEFT_BRACE);
    lexer_pop(lexer); // consume '{' or '('

    // Parse the compound list inside
    struct node *child = parse_compound_list(lexer);
    if (!child)
    {
        fprintf(
            stderr,
            "command.c: parse_shell_command: Failed to parse compound list.\n");
        return NULL;
    }

    // Expect the matching closing token
    current = lexer_peek(lexer);
    if ((is_brace && current.type != TOKEN_RIGHT_BRACE)
        || (!is_brace && current.type != TOKEN_SUBSHELL_CLOSE))
    {
        fprintf(
            stderr,
            "command.c: parse_shell_command: Missing matching '}' or ')'.\n");
        child->table->free(child);
        return NULL;
    }
    lexer_pop(lexer); // consume '}' or ')'

    // HERE, call shell_command_node_create or block_node_create en fonction de
    // is_brace

    // return the node not this
    return child;
}

static enum word_type get_word_type(enum token_type token_type)
{
    switch (token_type)
    {
    case TOKEN_WORD:
        return WORD;
    case TOKEN_SINGLE_QUOTES:
        return SINGLE;
    case TOKEN_DOUBLE_QUOTES:
        return DOUBLE;
    default:
        return NOTAWORD;
    }
}

// STEP 6.7.1 - PARSE SIMPLE COMMAND: Parse a simple command
static int check_and_enter_command_mode(struct lexer *lexer,
                                        struct token current, int *is_echo)
{
    if (strcmp(current.word, "echo") == 0
        || strcmp(current.word, "export") == 0)
    {
        *is_echo = 0;
        lexer_enter_command_mode(lexer);
        return 1;
    }
    return 0;
}

static int collect_word(struct lexer *lexer, char ***argv,
                        enum word_type **word_types, int *argc)
{
    struct token current = lexer_peek(lexer);
    if (current.type == TOKEN_WORD || current.type == TOKEN_SINGLE_QUOTES
        || current.type == TOKEN_DOUBLE_QUOTES)
    {
        *argv = realloc(*argv, (*argc + 2) * sizeof(char *));
        (*argv)[*argc] = my_strdup(current.word);
        *word_types =
            realloc(*word_types, (*argc + 1) * sizeof(enum word_type));
        (*word_types)[*argc] = get_word_type(current.type);
        (*argc)++;
        lexer_pop(lexer);
        return 1;
    }
    return 0;
}

static struct node *handle_redirections(struct lexer *lexer,
                                        struct node ***redirections,
                                        size_t *redir_count,
                                        size_t *redir_capacity)
{
    struct node *redir_node = parse_redirection(lexer);
    if (!redir_node)
    {
        fprintf(stderr, "command.c: simple: parsing redirection\n");
        return NULL;
    }
    if (*redir_count == *redir_capacity)
    {
        *redir_capacity = (*redir_capacity == 0) ? 4 : (*redir_capacity) * 2;
        *redirections =
            realloc(*redirections, *redir_capacity * sizeof(struct node *));
    }
    (*redirections)[(*redir_count)++] = redir_node;
    return redir_node;
}

static struct node *handle_assignment(struct lexer *lexer, char **argv,
                                      enum word_type *word_types, int argc)
{
    struct assign_node *an = parse_assignment(lexer, argv[0]);
    if (an)
    {
        for (int i = 0; argv[i] != NULL && i < argc; i++)
            free(argv[i]);
        free(argv);
        if (word_types)
            free(word_types);
        return (struct node *)an;
    }
    else
    {
        fprintf(stderr, "command.c: simple: parsing assign\n");
        return NULL;
    }
}

static void free_argv_and_word_types(char **argv, enum word_type *word_types,
                                     int argc)
{
    if (argv)
    {
        for (int i = 0; i < argc; i++)
            free(argv[i]);
        free(argv);
    }
    if (word_types)
        free(word_types);
}

static void free_redirections(struct node **redirections, size_t redir_count)
{
    if (redirections)
    {
        for (size_t i = 0; i < redir_count; i++)
            redirections[i]->table->free(redirections[i]);
        free(redirections);
    }
}

static struct node *parse_simple_cmd_good(struct simple_command_node **scn,
                                          struct node ***redirections,
                                          size_t *redir_count)
{
    (*scn)->redirections = *redirections;
    (*scn)->redir_count = *redir_count;

    return (struct node *)(*scn);
}

static int parse_simple_command_condition(struct token current)
{
    return ((current.type == TOKEN_WORD || current.type == TOKEN_SINGLE_QUOTES
             || current.type == TOKEN_DOUBLE_QUOTES)
            || is_redirection_token(current.type));
}

struct node *parse_simple_command(struct lexer *lexer)
{
    int argc = 0;
    char **argv = NULL;
    enum word_type *word_types = NULL;
    int is_echo = 1;

    struct node **redirections = NULL;
    size_t redir_count = 0;
    size_t redir_capacity = 0;

    struct token current = lexer_peek(lexer);

    check_and_enter_command_mode(lexer, current, &is_echo);

    if (collect_word(lexer, &argv, &word_types, &argc))
    {
        current = lexer_peek(lexer);
    }

    if (argc == 1 && current.type == TOKEN_ASSIGNMENT)
    {
        struct node *node = handle_assignment(lexer, argv, word_types, argc);
        if (node)
            return node;
        else
            goto error;
    }

    while (parse_simple_command_condition(current))
    {
        if (is_redirection_token(current.type))
        {
            if (!handle_redirections(lexer, &redirections, &redir_count,
                                     &redir_capacity))
            {
                goto error;
            }
        }
        else
        {
            if (!collect_word(lexer, &argv, &word_types, &argc))
            {
                break;
            }
        }
        current = lexer_peek(lexer);
    }

    if (is_echo == 0)
        lexer_exit_command_mode(lexer);

    if (argc == 0)
    {
        goto error;
    }

    argv[argc] = NULL;

    struct simple_command_node *scn =
        simple_command_node_create(argv, argc, word_types);
    if (!scn)
    {
        goto error;
    }

    return parse_simple_cmd_good(&scn, &redirections, &redir_count);

error:
    free_argv_and_word_types(argv, word_types, argc);
    free_redirections(redirections, redir_count);
    return NULL;
}

struct assign_node *parse_assignment(struct lexer *lexer, char *name)
{
    struct token current = lexer_peek(lexer);

    // Ensure the next token is TOKEN_ASSIGNMENT
    if (current.type != TOKEN_ASSIGNMENT)
    {
        fprintf(stderr,
                "command.c: parse_assignment: Expected assignment token, got "
                "'%s'.\n",
                current.word);
        return NULL;
    }

    // Consume the assignment token
    lexer_pop(lexer);
    current = lexer_peek(lexer);

    // Ensure there is exactly one word after the '='
    if (current.type != TOKEN_WORD && current.type != TOKEN_SINGLE_QUOTES
        && current.type != TOKEN_DOUBLE_QUOTES)
    {
        fprintf(stderr,
                "command.c: parse_assignment: Expected a single word after "
                "'=', got '%s'.\n",
                current.word);
        return NULL;
    }

    // Duplicate the value (argument)
    char *argument = my_strdup(current.word);

    // Consume the value token
    lexer_pop(lexer);

    // Duplicate the name to ensure ownership
    char *name_dup = my_strdup(name);

    // Create and return the assign_node
    struct assign_node *an = assign_node_create(name_dup, argument);
    if (!an)
    {
        fprintf(stderr,
                "command.c: parse_assignment: Failed to create assign_node.\n");
        free(argument);
        free(name_dup);
        return NULL;
    }

    return an;
}

// STEP 7 - PARSE COMPOUND LIST: Parse commands grouped
static void skip_initial_newlines(struct lexer *lexer, struct token *current)
{
    while (current->type == TOKEN_ENDLINE)
    {
        lexer_pop(lexer);
        *current = lexer_peek(lexer);
    }
}

static int ensure_capacity(struct node ***commands, size_t *count,
                           size_t *capacity)
{
    if (*count + 1 >= *capacity)
    {
        size_t new_capacity = (*capacity == 0) ? 4 : (*capacity) * 2;
        struct node **new_commands =
            realloc(*commands, new_capacity * sizeof(struct node *));
        if (!new_commands)
            return 0;
        *commands = new_commands;
        *capacity = new_capacity;
    }
    return 1;
}

static void handle_separators(struct lexer *lexer, struct token *current)
{
    if (current->type == TOKEN_SEMICOLON || current->type == TOKEN_ENDLINE)
    {
        lexer_pop(lexer);
        while ((*current = lexer_peek(lexer)).type == TOKEN_ENDLINE)
            lexer_pop(lexer);
    }
}

struct node *parse_compound_list(struct lexer *lexer)
{
    struct node **commands = NULL;
    size_t count = 0;
    size_t capacity = 0;

    struct token current = lexer_peek(lexer);
    skip_initial_newlines(lexer, &current);

    while (current.type != TOKEN_THEN && current.type != TOKEN_ELSE
           && current.type != TOKEN_ELIF && current.type != TOKEN_FI
           && current.type != TOKEN_DONE && current.type != TOKEN_ESAC
           && current.type != TOKEN_EOF && current.type != TOKEN_DO)
    {
        struct node *and_or = parse_and_or(lexer);
        if (!and_or)
        {
            fprintf(stderr, "command.c: compound: parsing andor \n");
            goto error;
        }

        if (!ensure_capacity(&commands, &count, &capacity))
        {
            goto error;
        }

        commands[count++] = and_or;
        commands[count] = NULL;

        current = lexer_peek(lexer);
        if (current.type == TOKEN_SEMICOLON || current.type == TOKEN_ENDLINE)
        {
            handle_separators(lexer, &current);
        }
        else
        {
            break;
        }
    }

    if (count == 0)
    {
        fprintf(stderr, "structures.c: compound: Empty compound list.\n");
        free(commands);
        return NULL;
    }

    struct compound_list_node *cln = compound_list_node_create(commands);
    if (!cln)
    {
        fprintf(stderr, "structures.c: compound: creating node\n");
        goto error;
    }

    return (struct node *)cln;

error:
    for (size_t i = 0; i < count; i++)
        if (commands[i])
            commands[i]->table->free(commands[i]);
    free(commands);
    return NULL;
}

// STEP 8 - PARSE REDIRECTION: Parse redirections
struct node *parse_redirection(struct lexer *lexer)
{
    struct token current = lexer_peek(lexer);
    int fd = -1;

    // Handle optional numeric file descriptor (e.g., "2>", "3>>")
    if (current.type == TOKEN_WORD && is_numeric(current.word))
    {
        fd = atoi(current.word); // Convert file descriptor to integer
        if (fd < 0 || fd > 9) // Basic validation
        {
            fprintf(stderr, "parse_redirect: Invalid fd '%d'\n", fd);
            return NULL;
        }
        lexer_pop(lexer); // Consume the numeric token
        current = lexer_peek(lexer); // Move to redirection token
    }

    // Redirection type (e.g., >, >>, <)
    if (!is_redirection_token(current.type))
    {
        fprintf(stderr, "parse_redirect: found '%s'\n", current.word);
        return NULL;
    }
    int redir_type = current.type;
    lexer_pop(lexer); // Consume the redirection token

    struct redirection_node *redir_node;
    if (current.type == TOKEN_DUP_OUT_DESC)
    {
        redir_node = redirection_node_create(redir_type, fd, "a");
        current = lexer_peek(lexer);
        if (current.type == TOKEN_WORD && is_numeric(current.word))
            lexer_pop(lexer);
    }
    else
    {
        // Target (file name or HEREDOC content)

        current = lexer_peek(lexer);
        if (current.type != TOKEN_WORD && current.type != TOKEN_HEREDOC)
        {
            fprintf(stderr, "parse_redirect:'%s'\n", current.word);
            return NULL;
        }

        char *target = my_strdup(current.word);
        if (!target)
        {
            fprintf(stderr, "parse_redirection: strdup failed for target\n");
            return NULL;
        }
        lexer_pop(lexer); // Consume the target token

        // Create and return the redirection node
        redir_node = redirection_node_create(redir_type, fd, target);
        if (redir_type != TOKEN_DUP_IN_DESC && redir_type != TOKEN_DUP_OUT_DESC)
            free(target); // Cleanup local copy
    }
    if (!redir_node)
    {
        fprintf(stderr, "parse_redirect: Failed to create node\n");
        return NULL;
    }

    return (struct node *)redir_node;
}
