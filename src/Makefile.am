SUBDIRS = \
	utils \
	io \
	lexer \
	parser \
	ast \
	cmds

bin_PROGRAMS = 42sh

42sh_SOURCES = 42sh.c

42sh_CPPFLAGS = -I%D%
42sh_CFLAGS = -Wall -Wextra -Werror -Wvla -pedantic -std=c99

42sh_LDADD =  \
	io/libio.a \
	lexer/liblexer.a \
	parser/libparser.a \
	ast/libast.a \
	cmds/libcmds.a \
	utils/libutils.a
