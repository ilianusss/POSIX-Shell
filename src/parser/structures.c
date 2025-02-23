#include <stdlib.h>

#include "parser.h"

// STEP 6.1.1 - PARSE IF COMMAND: Parse if statements
struct node *parse_if_command(struct lexer *lexer)
{
    lexer_pop(lexer); // Pop the if

    // Parse condition
    struct node *condition = parse_compound_list(lexer);
    if (!condition)
    {
        fprintf(stderr, "parser.c:if: Fail to parse condition\n");
        return NULL;
    }

    // Check for then token
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_THEN)
    {
        fprintf(stderr, "parser.c: if: expected then\n");
        condition->table->free(condition);
        return NULL;
    }
    lexer_pop(lexer); // Pop `then`

    // Parse then branch
    struct node *then_branch = parse_compound_list(lexer);
    if (!then_branch)
    {
        fprintf(stderr, "parser.c: if: Fail to parse then\n");
        condition->table->free(condition);
        return NULL;
    }

    struct node *else_branch =
        parse_if_else_branch(lexer, then_branch, condition);
    if (!else_branch && lexer_peek(lexer).type == TOKEN_ELSE)
    {
        fprintf(stderr, "parser.c: if: jsp\n");
        return NULL;
    }

    // Check for fi
    tok = lexer_peek(lexer);
    if (tok.type != TOKEN_FI)
    {
        fprintf(stderr, "parser.c: if: expected fi\n");
        goto error;
    }
    lexer_pop(lexer); // Pop `fi`

    // Create node
    struct if_node *ifn = if_node_create(condition, then_branch, else_branch);
    if (!ifn)
    {
        fprintf(stderr, "parser.c: if: fail to create if node\n");
        goto error;
    }

    return (struct node *)ifn;

error:
    if (else_branch)
        else_branch->table->free(else_branch);
    return NULL;
}

// STEP 6.1.2 - PARSE IF ELSE: Parse elif and else branches
static int handle_elif(struct lexer *lexer, struct node **else_branch,
                       struct if_node **last_elif)
{
    lexer_pop(lexer);
    struct node *elif_condition = parse_compound_list(lexer);
    if (!elif_condition)
    {
        fprintf(stderr, "parser.c: ifelse: elif cond\n");
        return 0;
    }

    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_THEN)
    {
        fprintf(stderr, "parser.c: ifelse: expexted then\n");
        elif_condition->table->free(elif_condition);
        return 0;
    }
    lexer_pop(lexer);

    struct node *elif_then = parse_compound_list(lexer);
    if (!elif_then)
    {
        fprintf(stderr, "parser.c: ifelse: else branch\n");
        elif_condition->table->free(elif_condition);
        return 0;
    }

    struct if_node *elif_ifn = if_node_create(elif_condition, elif_then, NULL);
    if (!elif_ifn)
    {
        fprintf(stderr, "parser.c: ifelse: creating node\n");
        elif_condition->table->free(elif_condition);
        elif_then->table->free(elif_then);
        return 0;
    }

    if (!*else_branch)
    {
        *else_branch = (struct node *)elif_ifn;
    }
    else
    {
        ((struct if_node *)(*last_elif))->else_branch = (struct node *)elif_ifn;
    }
    *last_elif = elif_ifn;
    return 1;
}

static int handle_else(struct lexer *lexer, struct node **else_branch,
                       struct if_node *last_elif)
{
    lexer_pop(lexer);
    struct node *else_compound = parse_compound_list(lexer);
    if (!else_compound)
    {
        fprintf(stderr, "parser.c: ifelse: else branch\n");
        return 0;
    }

    if (!last_elif)
    {
        *else_branch = else_compound;
    }
    else
    {
        ((struct if_node *)last_elif)->else_branch = else_compound;
    }
    return 1;
}

static void cleanup_on_error(struct node *then_branch, struct node *condition)
{
    then_branch->table->free(then_branch);
    condition->table->free(condition);
}

struct node *parse_if_else_branch(struct lexer *lexer, struct node *then_branch,
                                  struct node *condition)
{
    struct node *else_branch = NULL;
    struct if_node *last_elif = NULL;
    struct token tok;

    while (1)
    {
        tok = lexer_peek(lexer);
        if (tok.type == TOKEN_ELIF)
        {
            if (!handle_elif(lexer, &else_branch, &last_elif))
                goto error_then;
        }
        else if (tok.type == TOKEN_ELSE)
        {
            if (!handle_else(lexer, &else_branch, last_elif))
                goto error_then;
            break;
        }
        else
        {
            break;
        }
    }

    return else_branch;

error_then:
    cleanup_on_error(then_branch, condition);
    return NULL;
}

// STEP 6.2 - PARSE RULE WHILE: Parse 'while' loops
struct node *parse_loop(struct lexer *lexer)
{
    struct node *body = NULL;
    struct token tok = lexer_peek(lexer);
    int is_until = tok.type == TOKEN_UNTIL;
    lexer_pop(lexer); // Pop the 'while' or 'until' keyword

    // Parse the condition
    struct node *condition = parse_compound_list(lexer);
    if (!condition)
    {
        fprintf(stderr, "parser.c: loop: condition in while\n");
        return NULL;
    }

    // Validate 'do' token
    tok = lexer_peek(lexer);
    if (tok.type != TOKEN_DO)
    {
        fprintf(stderr, "parser.c: loop: Expected do\n");
        goto error;
    }
    lexer_pop(lexer);

    // Parse the body of the while loop
    body = parse_compound_list(lexer);
    if (!body)
    {
        fprintf(stderr, "parser.c: loop: body\n");
        goto error;
    }

    // Validate 'done' token
    tok = lexer_peek(lexer);
    if (tok.type != TOKEN_DONE)
    {
        fprintf(stderr, "parser.c: loop: Expected done.\n");
        goto error;
    }
    lexer_pop(lexer);

    // Create the while node
    struct loop_node *loop_node = loop_node_create(condition, body, is_until);
    if (!loop_node)
    {
        fprintf(stderr, "parser.c: loop: creating loop node\n");
        goto error;
    }

    return (struct node *)loop_node;

error:
    if (body)
        body->table->free(body);
    if (condition)
        condition->table->free(condition);
    return NULL;
}

// STEP 6.3 - PARSE FOR COMMAND: Parse for loop
static char *parse_loop_var(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_WORD)
    {
        fprintf(stderr, "parser.c: for: word after for.\n");
        return NULL;
    }

    char *loop_var = my_strdup(tok.word);
    lexer_pop(lexer);
    return loop_var;
}

static int parse_in_or_semicolon(struct lexer *lexer, char ***items,
                                 int *item_count)
{
    struct token tok = lexer_peek(lexer);
    int item_capacity = 4;

    if (tok.type == TOKEN_SEMICOLON)
    {
        lexer_pop(lexer);
        return 1;
    }
    else if (tok.type == TOKEN_IN)
    {
        lexer_pop(lexer);
        *items = malloc(item_capacity * sizeof(char *));
        if (!*items)
            return 0;

        while ((tok = lexer_peek(lexer)).type == TOKEN_WORD)
        {
            if (*item_count + 1 >= item_capacity)
            {
                item_capacity *= 2;
                char **new_items =
                    realloc(*items, item_capacity * sizeof(char *));
                if (!new_items)
                    return 0;
                *items = new_items;
            }

            (*items)[*item_count] = my_strdup(tok.word);
            (*item_count)++;
            lexer_pop(lexer);
        }

        if (*item_count == 0)
        {
            fprintf(stderr, "parser.c: for: expect word after in\n");
            return 0;
        }

        (*items)[*item_count] = NULL;

        tok = lexer_peek(lexer);
        if (tok.type != TOKEN_SEMICOLON && tok.type != TOKEN_ENDLINE)
        {
            fprintf(stderr, "parser.c: for: Expect ; or endline\n");
            return 0;
        }
        lexer_pop(lexer);
        return 1;
    }
    else
    {
        fprintf(stderr, "parser.c: parse_for: Expect ; or in\n");
        return 0;
    }
}

static void skip_endlines(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    while (tok.type == TOKEN_ENDLINE)
    {
        lexer_pop(lexer);
        tok = lexer_peek(lexer);
    }
}

static int validate_do(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_DO)
    {
        fprintf(stderr, "parser.c: for: Expected do\n");
        return 0;
    }
    lexer_pop(lexer);
    return 1;
}

static struct node *parse_body(struct lexer *lexer)
{
    struct node *body = parse_compound_list(lexer);
    if (!body)
    {
        fprintf(stderr, "parser.c: for: Failed to parse body\n");
    }
    return body;
}

static int validate_done(struct lexer *lexer)
{
    struct token tok = lexer_peek(lexer);
    if (tok.type != TOKEN_DONE)
    {
        fprintf(stderr, "parser.c: for: Expected done\n");
        return 0;
    }
    lexer_pop(lexer);
    return 1;
}

static struct node *create_for_node(char *loop_var, char **items,
                                    int item_count, struct node *body)
{
    struct for_node *for_node =
        for_node_create(loop_var, items, item_count, body);
    if (!for_node)
    {
        fprintf(stderr, "parser.c: for: Fail to create 'for_node'.\n");
    }
    return (struct node *)for_node;
}

static void cleanup_items(char **items, int item_count)
{
    for (int i = 0; i < item_count; i++)
    {
        free(items[i]);
    }
    free(items);
}

struct node *parse_for(struct lexer *lexer)
{
    lexer_pop(lexer);
    char *loop_var = parse_loop_var(lexer);
    if (!loop_var)
        return NULL;

    char **items = NULL;
    int item_count = 0;
    if (!parse_in_or_semicolon(lexer, &items, &item_count))
        goto error_loop_var;

    skip_endlines(lexer);

    if (!validate_do(lexer))
        goto error_items;

    struct node *body = parse_body(lexer);
    if (!body)
        goto error_items;

    if (!validate_done(lexer))
        goto error_body;

    struct node *for_node = create_for_node(loop_var, items, item_count, body);
    if (!for_node)
        goto error_body;

    return for_node;

error_body:
    body->table->free(body);
error_items:
    cleanup_items(items, item_count);
error_loop_var:
    free(loop_var);
    return NULL;
}

// STEP 6.4 - PARSE RULE CASE: Parse case statements
struct node *parse_case(struct lexer *lexer)
{
    if (!lexer)
        return NULL;
    return NULL;
}
