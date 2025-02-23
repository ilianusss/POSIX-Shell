lib_LIBRARIES = libcmds.a

libcmds_a_SOURCES =  \
	var/var.c \
	true/true.c \
	export/export.c \
	false/false.c \
	echo/echo.c \
	cd/cd.c \
	dot/dot.c \
	redirections/file_read_write.c \
	redirections/file_to_stdout.c \
	redirections/redirection_utils.h \
	redirections/stdout_to_file.c \
	redirections/stdout_to_file_force.c \
	var/expand_var.c \
	var/expand_var_utils.c \
	var/context.c

libcmds_a_CPPFLAGS = -I$(top_srcdir)/src

libcmds_a_CFLAGS = -Wall -Wextra -Werror -Wvla -pedantic -std=c99

noinst_LIBRARIES = libcmds.a
