#ifndef ECHO_H
#define ECHO_H

// DEFINITIONS
//
// HEADERS
//
// LIBS
#include <stdbool.h>
#include <stdio.h>
#include <string.h>

#include "../../ast/nodes/simple_command/word_types.h"
#include "../../utils/truand.h"
#include "../../utils/utils.h"

// STRUCTURES
//
// FUNCTIONS
int builtin_echo(int argc, char **argv, enum word_type *word_types);

#endif /* ECHO_H */
