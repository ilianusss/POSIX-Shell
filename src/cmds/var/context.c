#include "context.h"

// Initialize the stack
struct StackContext *init_stack(void)
{
    struct StackContext *stack = malloc(sizeof(struct StackContext));
    if (!stack)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return NULL;
    }
    stack->top = NULL;
    return stack;
}

// Push a new ScriptContext onto the stack
void push(struct StackContext *stack, struct ScriptContext *value)
{
    struct StackContextNode *new_node = malloc(sizeof(struct StackContextNode));
    if (!new_node)
    {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }
    new_node->data = value; // Shallow copy
    new_node->next = stack->top;
    stack->top = new_node;
}

// Pop a ScriptContext from the stack
struct ScriptContext *pop(struct StackContext *stack)
{
    if (stack->top == NULL)
        return NULL;
    struct StackContextNode *temp = stack->top;
    struct ScriptContext *popped_data = temp->data; // Shallow copy
    stack->top = stack->top->next;
    free(temp);
    return popped_data;
}

// Check if stack is empty
int is_empty(struct StackContext *stack)
{
    return stack->top == NULL;
}

// Free the entire stack
void free_stack(struct StackContext *stack)
{
    while (!is_empty(stack))
    {
        struct ScriptContext *sc = pop(stack);
        free(sc);
    }
}

struct ScriptContext *init_script_context(int argc, char **argv)
{
    struct ScriptContext *context = malloc(sizeof(struct ScriptContext));
    if (!context)
    {
        perror("Failed to allocate ScriptContext");
        exit(EXIT_FAILURE);
    }

    context->positional_count = argc - 1; // Exclude the script name
    context->positional_params =
        malloc(context->positional_count * sizeof(char *));
    if (!context->positional_params)
    {
        perror("Failed to allocate positional_params");
        free(context);
        exit(EXIT_FAILURE);
    }

    // Copy positional arguments
    for (int i = 1; i < argc; i++)
    {
        context->positional_params[i - 1] = my_strdup(argv[i]);
    }

    return context;
}

void free_script_context(struct ScriptContext *context)
{
    if (context)
    {
        for (int i = 0; i < context->positional_count; i++)
        {
            free(context->positional_params[i]);
        }
        free(context->positional_params);
        free(context);
    }
}
