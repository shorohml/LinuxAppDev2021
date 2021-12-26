bin_PROGRAMS=guess

pkgdata_DATA=guess.1

guess_SOURCES=guess.c

gitclean:
	git clean -df

%.1: %
	$(HELP2MAN) --no-info --output=$@ --name='@PACKAGE_NAME@' ./$<