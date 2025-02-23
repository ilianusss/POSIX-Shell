AUTOMAKE_OPTIONS = foreign

SUBDIRS = src

check: all
	./tests/testsuite.sh

leak: all
	./tests/memoryleak.sh

clang-format:
	find -iname "*.[ch]" -exec clang-format -i {} \;

clean-all:
	find . -name 'Makefile.in' -exec rm -f {} +
	find . -name 'Makefile' -exec rm -f {} +
	find . -name '.deps' -exec rm -rf {} +
	find . -name 'Makefile' -exec rm -f {} +
	find . -name 'Makefile.in' -exec rm -f {} +
	find . -name '*.a' -exec rm -f {} +
	find . -name '*.o' -exec rm -f {} +
	find . -type d -name "printed_ast" -exec rm -rf {} +
	rm -f config.log
	rm -rf autom4te.cache
	rm -f config.status
	rm -f aclocal.m4
	rm -f ar-lib
	rm -f compile
	rm -f configure
	rm -f configure~
	rm -f depcomp
	rm -f install-sh
	rm -f missing
	rm -f src/42sh
	rm -f tests/test
	rm -f tests/input
	rm -f tests/output
	rm -f tests/filename1
	rm -f tests/filename2
	rm -f valgrind.log
