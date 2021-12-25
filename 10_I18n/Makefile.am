pkgdata_DATA=ru/LC_MESSAGES/guess.mo

ru/LC_MESSAGES/guess.mo: ru.po
	mkdir -p `dirname $@`
	msgfmt $< -o $@

CFLAGS = -Wall -Wextra

bin_PROGRAMS=guess

guess_SOURCES=src/guess.c

messages.pot: guess.pot src/guess.c
	xgettext -k_ -c src/guess.c -o messages.new.pot
	cp guess.pot messages.pot
	msgmerge -U messages.pot messages.new.pot

gitclean:
	git clean -df