lib_LIBRARIES = liblexer.a

liblexer_a_SOURCES =  \
	lexer.c \
	helpers/alias_and_variables.c \
	helpers/keywords_lex.c \
	helpers/operators_lex.c \
	helpers/pipe_grouping_lex.c \
	helpers/redirect_lex.c \
	helpers/string_lex.c

liblexer_a_CPPFLAGS = -I$(top_srcdir)/src

liblexer_a_CFLAGS = -Wall -Wextra -Werror -Wvla -pedantic -std=c99

noinst_LIBRARIES = liblexer.a
