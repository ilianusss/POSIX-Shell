#ifndef PARSER_H
#define PARSER_H
// DEFINITIONS
//
// HEADERS
#include "../ast/ast.h"
#include "../ast/nodes/and_or/and_or_node.h"
#include "../ast/nodes/assignment/assign_node.h"
// #include "../ast/nodes/case/case_node.h"
#include "../ast/nodes/command_list/command_list_node.h"
#include "../ast/nodes/compound_list/compound_list_node.h"
#include "../ast/nodes/for/for_node.h"
#include "../ast/nodes/if/if_node.h"
#include "../ast/nodes/loop/loop_node.h"
#include "../ast/nodes/negation/negation_node.h"
#include "../ast/nodes/pipeline/pipeline_node.h"
#include "../ast/nodes/redirections/redirect_node.h"
#include "../ast/nodes/root/root_node.h"
#include "../ast/nodes/simple_command/simple_command_node.h"
#include "../lexer/lexer.h"
#include "../utils/utils.h"

// LIBS
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// STRUCTURES
//
// FUNCTIONS
// input.c
struct node *parse_tokens(struct lexer *lexer);
struct node *parse_input(struct lexer *lexer);
struct node *parse_list(struct lexer *lexer);
struct node *parse_and_or(struct lexer *lexer);
struct node *parse_pipeline(struct lexer *lexer);

// command.c
struct node *parse_command(struct lexer *lexer);
struct node *parse_funcdec(struct lexer *lexer);
struct node *parse_shell_command(struct lexer *lexer);
struct node *parse_simple_command(struct lexer *lexer);
struct assign_node *parse_assignment(struct lexer *lexer, char *name);
struct node *parse_compound_list(struct lexer *lexer);
struct node *parse_redirection(struct lexer *lexer);

// structures.c
struct node *parse_if_command(struct lexer *lexer);
struct node *parse_if_else_branch(struct lexer *lexer, struct node *then_branch,
                                  struct node *condition);
struct node *parse_loop(struct lexer *lexer);
struct node *parse_for(struct lexer *lexer);
struct node *parse_case(struct lexer *lexer);

#endif /* ! PARSER_H */
