lib_LIBRARIES = libast.a

libast_a_SOURCES =  \
	ast.h \
	nodes/and_or/and_or_node.c \
	nodes/assignment/assign_node.c \
	nodes/command_list/command_list_node.c \
	nodes/compound_list/compound_list_node.c \
	nodes/for/for_node.c \
	nodes/if/if_node.c \
	nodes/loop/loop_node.c \
	nodes/negation/negation_node.c \
	nodes/pipeline/pipeline_node.c \
	nodes/redirections/redirect_node.c \
	nodes/root/root_node.c \
	nodes/simple_command/simple_command_node.c

libast_a_CPPFLAGS = -I$(top_srcdir)/src

libast_a_CFLAGS = -Wall -Wextra -Werror -Wvla -pedantic -std=c99

noinst_LIBRARIES = libast.a
