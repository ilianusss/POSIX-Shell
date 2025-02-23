AC_INIT([42sh], [1.0], [42sh@assistants.epita.fr])

AM_INIT_AUTOMAKE([subdir-objects] [foreign])

AM_SILENT_RULES([yes])

AM_PROG_AR

AC_PROG_RANLIB

AC_PROG_CC

AX_COMPILER_FLAGS([], [], [], [-Wall -Wextra -Werror -Wvla -pedantic -std=c99])

AC_SUBST([CFLAGS], [-Wall -Wextra -Werror -Wvla -pedantic -std=c99])

AC_CONFIG_FILES([
                    Makefile
                    src/Makefile
                    src/ast/Makefile
                    src/cmds/Makefile
                    src/io/Makefile
                    src/lexer/Makefile
                    src/parser/Makefile
                    src/utils/Makefile
                ])
AC_OUTPUT
